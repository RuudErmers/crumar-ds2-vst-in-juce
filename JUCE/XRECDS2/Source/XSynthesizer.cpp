/*
  ==============================================================================

    XSynthesizer.cpp
    Created: 14 Sep 2017 7:17:54pm
    Author:  Ruud

  ==============================================================================
*/

#include "XSynthesizer.h"

XSynthesizer::XSynthesizer()
	: sampleRate(0),
	FNoteCounter(0),
	minimumSubBlockSize(32),
	subBlockSubdivisionIsStrict(false)
{
	for (int i = 0; i < numElementsInArray(lastPitchWheelValues); ++i)
		lastPitchWheelValues[i] = 0x2000;
	FLastPitch = 0;
	FNoteCounter = 0;
	FSustain = false;
	for (int i=0;i<128;i++) FNotes[i]=0;
}

XSynthesizer::~XSynthesizer()
{
}

//==============================================================================
void XSynthesizer::setCurrentPlaybackSampleRate(const double newRate)
{
	if (sampleRate != newRate)
	{
		const ScopedLock sl(lock);

		allNotesOff(0, false);

		sampleRate = newRate;

	}
}

template <typename floatType>
void XSynthesizer::processNextBlock(AudioBuffer<floatType>& outputAudio,
	const MidiBuffer& midiData,
	int startSample,
	int numSamples)
{
	// must set the sample rate before using this!
	jassert(sampleRate != 0);
	const int targetChannels = outputAudio.getNumChannels();

	MidiBuffer::Iterator midiIterator(midiData);
	midiIterator.setNextSamplePosition(startSample);

	bool firstEvent = true;
	int midiEventPos;
	MidiMessage m;

	const ScopedLock sl(lock);

	while (numSamples > 0)
	{
		if (!midiIterator.getNextEvent(m, midiEventPos))
		{
			if (targetChannels > 0)
				renderVoices(outputAudio, startSample, numSamples);

			return;
		}

		const int samplesToNextMidiMessage = midiEventPos - startSample;

		if (samplesToNextMidiMessage >= numSamples)
		{
			if (targetChannels > 0)
				renderVoices(outputAudio, startSample, numSamples);

			handleMidiEvent(m);
			break;
		}

		if (samplesToNextMidiMessage < ((firstEvent ) ? 1 : minimumSubBlockSize))
		{
			handleMidiEvent(m);
			continue;
		}

		firstEvent = false;

		if (targetChannels > 0)
			renderVoices(outputAudio, startSample, samplesToNextMidiMessage);

		handleMidiEvent(m);
		startSample += samplesToNextMidiMessage;
		numSamples -= samplesToNextMidiMessage;
	}

	while (midiIterator.getNextEvent(m, midiEventPos))
		handleMidiEvent(m);
}

// explicit template instantiation
template void XSynthesizer::processNextBlock<float>(AudioBuffer<float>& outputAudio,
	const MidiBuffer& midiData,
	int startSample,
	int numSamples);
template void XSynthesizer::processNextBlock<double>(AudioBuffer<double>& outputAudio,
	const MidiBuffer& midiData,
	int startSample,
	int numSamples);

template <typename FloatType>
void XSynthesizer::renderVoices(AudioBuffer<FloatType>& buffer, int startSample, int numSamples)
{
	while (--numSamples >= 0)
		{
			onRenderSample();
		    double sample = 0;
			try {
				  for (int voice = 0; voice < VOICECOUNT; ++voice)
					if (voices[voice].isPlaying)
						sample = sample + voices[voice].process(); 
			}
			catch (...) {}
			if (sample>1) sample = 1;
			if (sample<-1) sample = -1;
			for (int i = buffer.getNumChannels(); --i >= 0;)
				buffer.addSample(i, startSample, static_cast<FloatType>(sample));
			++startSample;
		}
}

void XSynthesizer::handleMidiEvent(const MidiMessage& m)
{
	const int channel = m.getChannel();

	if (m.isNoteOn())
	{
		noteOn(channel, m.getNoteNumber(), m.getFloatVelocity());
	}
	else if (m.isNoteOff())
	{
		noteOff(channel, m.getNoteNumber(), m.getFloatVelocity());
	}
	else if (m.isAllNotesOff() || m.isAllSoundOff())
	{
		allNotesOff(channel, true);
	}
	else if (m.isPitchWheel())
	{
		const int wheelPos = m.getPitchWheelValue();
		lastPitchWheelValues[channel - 1] = wheelPos;
		handlePitchWheel(channel, wheelPos);
	}
	else if (m.isAftertouch())
	{
		handleAftertouch(channel, m.getNoteNumber(), m.getAfterTouchValue());
	}
	else if (m.isChannelPressure())
	{
		handleChannelPressure(channel, m.getChannelPressureValue());
	}
	else if (m.isController())
	{
		handleController(channel, m.getControllerNumber(), m.getControllerValue());
	}
	else if (m.isProgramChange())
	{
		handleProgramChange(channel, m.getProgramChangeNumber());
	}
}

//==============================================================================

void XSynthesizer::handleController(int /*midiChannel*/,	int controllerNumber,	int controllerValue)
{

	switch (controllerNumber) {
	case 64:  Sustain(controllerValue>64);
            break;
    }
}

int XSynthesizer::PreviousNote()
{
	int max; int pmax;
	max = 0;
	pmax = 0;
	for (int i = 0; i < 128; ++i) 
      if (FNotes[i]>max) { pmax = i; max = FNotes[i]; }
	return pmax;
}

void XSynthesizer::Sustain(bool _on)
{
	int i;
	FSustain = _on;
	if (!FSustain)
		for (i = 0; i < VOICECOUNT; ++i)
			if ((voices[i].sustain) && (voices[i].isPlaying))
				voices[i].NoteOff();
}

bool XSynthesizer::CheckVoicePlaying(int pitch)
{
	bool result; 
    int i;

	result = false;
	for (i = 0; i < VOICECOUNT; ++i)
	if ((voices[i].pitch == pitch) && (voices[i].isPlaying))
		{
			voices[i].releaseQuick();
			result = true;
		}
	return result;
}
int XSynthesizer::EarliestPlayingIndex()
{
	int result; int i; int lowNoteCounter;

	result = -1;
	lowNoteCounter = FNoteCounter + 10;
	for (i = 0; i < VOICECOUNT; ++i)
		if ((voices[i].isPlaying) && (!voices[i].isQuickReleasing) && (voices[i].noteCounter < lowNoteCounter))
		{
			result = i;
			lowNoteCounter = voices[i].noteCounter;
		}
	return result;
}
void XSynthesizer::RemoveVoice()
{
	int index;

	index = EarliestPlayingIndex();
	if (index >= 0)
		voices[index].releaseQuick();
}
int XSynthesizer::ActiveVoicesCount()
{
	int result; int i;

	result = 0;
	for (i = 0; i < VOICECOUNT; ++i)
		if ((voices[i].isPlaying) && !voices[i].isQuickReleasing) ++result; 
	return result;
}
int XSynthesizer::GetFreeIndex()
{
	int i;
	for (i = 0; i < VOICECOUNT; ++i)
		if ((!voices[i].isPlaying)) return i; 
	return 0;
}

void XSynthesizer::noteOn(int /*midiChannel*/, int pitch, double velocity)  // careful: velocity in range 0..1 instead of 0..127 
{

	int av; int index; int VoiceMax;

	CheckVoicePlaying(pitch);
	av = ActiveVoicesCount();
	if (FPoly) VoiceMax = 8; else VoiceMax = 1;
	if (av >= VoiceMax) RemoveVoice();
	RegisterNote(pitch);
	index = GetFreeIndex();
	voices[index].noteCounter = FNoteCounter++;
	voices[index].isPlaying = true;
   	voices[index].NoteOn(pitch, FLastPitch, velocity);
	FLastVelocity = velocity;
	FLastPitch = pitch;
}

void XSynthesizer::ReleaseNote(int pitch)
{
	int i;
	for (i = 0; i < VOICECOUNT; ++i)
		if ((voices[i].isPlaying) && (voices[i].pitch == pitch))
		{
			if (FSustain) voices[i].sustain = true;
			else
				voices[i].NoteOff();
	}
}

void XSynthesizer::noteOff(const int midiChannel, int pitch, double /*velocity*/)
{
	UnregisterNote(pitch);
	if (!FPoly)
	{
		int prev = PreviousNote();
		if ((prev != 0) && (pitch == FLastPitch))
            noteOn(midiChannel,prev, FLastVelocity);
		ReleaseNote(pitch);
	}
	else ReleaseNote(pitch);
}

void XSynthesizer::onVoiceDone(XSynthVoice *Sender)
{
  for (int i = 0; i < VOICECOUNT; ++i)
		if ((voices[i].isPlaying) && (&voices[i] == Sender))
		{
			voices[i].isPlaying = false;
			return;
		}
}


void XSynthesizer::allNotesOff(const int /*midiChannel*/, const bool /*allowTailOff*/)
{
	for (int i=0;i<VOICECOUNT;i++)
		voices[i].isPlaying = false;
}


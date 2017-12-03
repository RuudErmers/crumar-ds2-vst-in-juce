/*
  ==============================================================================

    XSynthesizer.h
    Created: 14 Sep 2017 7:17:54pm
    Author:  Ruud

  ==============================================================================
*/

#pragma once

#include "XsynthVoice.h"
const int VOICECOUNT = 16;
class XSynthesizer
{

public:
	//==============================================================================
	/** Creates a new synthesiser.
      We could have used a baseclass for the voices and let a descendant class do the real creation, but we don't do that 
      We also could associate a XSynthesizer but again, we don't do that and neither do we rename this class to XSynthesizerBase
	*/
	XSynthesizer();

	/** Destructor. */
	virtual ~XSynthesizer();

	//==============================================================================
	//==============================================================================
	/** Triggers a note-on event.

	The default method here will find all the sounds that want to be triggered by
	this note/channel. For each sound, it'll try to find a free voice, and use the
	voice to start playing the sound.

	Subclasses might want to override this if they need a more complex algorithm.

	This method will be called automatically according to the midi data passed into
	renderNextBlock(), but may be called explicitly too.

	The midiChannel parameter is the channel, between 1 and 16 inclusive.
	*/
	virtual void noteOn(int midiChannel,int pitch,	double velocity);

	void ReleaseNote(int pitch);

	/** Triggers a note-off event.


	This will turn off any voices that are playing a sound for the given note/channel.

	If allowTailOff is true, the voices will be allowed to fade out the notes gracefully
	(if they can do). If this is false, the notes will all be cut off immediately.

	This method will be called automatically according to the midi data passed into
	renderNextBlock(), but may be called explicitly too.

	The midiChannel parameter is the channel, between 1 and 16 inclusive.
	*/
	virtual void noteOff(int midiChannel,int pitch,	double velocity);

	void onVoiceDone(XSynthVoice * Sender);

	/** Turns off all notes.

	This will turn off any voices that are playing a sound on the given midi channel.

	If midiChannel is 0 or less, then all voices will be turned off, regardless of
	which channel they're playing. Otherwise it represents a valid midi channel, from
	1 to 16 inclusive.

	If allowTailOff is true, the voices will be allowed to fade out the notes gracefully
	(if they can do). If this is false, the notes will all be cut off immediately.

	This method will be called automatically according to the midi data passed into
	renderNextBlock(), but may be called explicitly too.
	*/
	virtual void allNotesOff(int midiChannel,
		bool allowTailOff);

	/** Sends a pitch-wheel message to any active voices.

	This will send a pitch-wheel message to any voices that are playing sounds on
	the given midi channel.

	This method will be called automatically according to the midi data passed into
	renderNextBlock(), but may be called explicitly too.

	@param midiChannel          the midi channel, from 1 to 16 inclusive
	@param wheelValue           the wheel position, from 0 to 0x3fff, as returned by MidiMessage::getPitchWheelValue()
	*/
	virtual void handlePitchWheel(int /*midiChannel*/,	int /*wheelValue*/) {}

	/** Sends a midi controller message to any active voices.

	This will send a midi controller message to any voices that are playing sounds on
	the given midi channel.

	This method will be called automatically according to the midi data passed into
	renderNextBlock(), but may be called explicitly too.

	@param midiChannel          the midi channel, from 1 to 16 inclusive
	@param controllerNumber     the midi controller type, as returned by MidiMessage::getControllerNumber()
	@param controllerValue      the midi controller value, between 0 and 127, as returned by MidiMessage::getControllerValue()
	*/
	virtual void handleController(int midiChannel,
		int controllerNumber,
		int controllerValue);

	/** Sends an aftertouch message.

	This will send an aftertouch message to any voices that are playing sounds on
	the given midi channel and note number.

	This method will be called automatically according to the midi data passed into
	renderNextBlock(), but may be called explicitly too.

	@param midiChannel          the midi channel, from 1 to 16 inclusive
	@param midiNoteNumber       the midi note number, 0 to 127
	@param aftertouchValue      the aftertouch value, between 0 and 127,
	as returned by MidiMessage::getAftertouchValue()
	*/
	virtual void handleAftertouch(int /*midiChannel*/, int /*midiNoteNumber*/, int /*aftertouchValue*/) {}

	/** Sends a channel pressure message.

	This will send a channel pressure message to any voices that are playing sounds on
	the given midi channel.

	This method will be called automatically according to the midi data passed into
	renderNextBlock(), but may be called explicitly too.

	@param midiChannel              the midi channel, from 1 to 16 inclusive
	@param channelPressureValue     the pressure value, between 0 and 127, as returned
	by MidiMessage::getChannelPressureValue()
	*/
	virtual void handleChannelPressure(int /*midiChannel*/, int /*channelPressureValue*/) {}

	/** Can be overridden to handle an incoming program change message.
	The base class implementation of this has no effect, but you may want to make your
	own synth react to program changes.
	*/
	virtual void handleProgramChange(int /*midiChannel*/,	int /*programNumber*/) {}

	//==============================================================================
	/** Tells the synthesiser what the sample rate is for the audio it's being used to render.

	This value is propagated to the voices so that they can use it to render the correct
	pitches.
	*/
	virtual void setCurrentPlaybackSampleRate(double sampleRate);

	/** Creates the next block of audio output.

	This will process the next numSamples of data from all the voices, and add that output
	to the audio block supplied, starting from the offset specified. Note that the
	data will be added to the current contents of the buffer, so you should clear it
	before calling this method if necessary.

	The midi events in the inputMidi buffer are parsed for note and controller events,
	and these are used to trigger the voices. Note that the startSample offset applies
	both to the audio output buffer and the midi input buffer, so any midi events
	with timestamps outside the specified region will be ignored.
	*/
	inline void renderNextBlock(AudioBuffer<float>& outputAudio,
		const MidiBuffer& inputMidi,
		int startSample,
		int numSamples)
	{
		processNextBlock(outputAudio, inputMidi, startSample, numSamples);
	}

	inline void renderNextBlock(AudioBuffer<double>& outputAudio,
		const MidiBuffer& inputMidi,
		int startSample,
		int numSamples)
	{
		processNextBlock(outputAudio, inputMidi, startSample, numSamples);
	}

	/** Returns the current target sample rate at which rendering is being done.
	Subclasses may need to know this so that they can pitch things correctly.
	*/
	double getSampleRate() const noexcept { return sampleRate; }

	/** Sets a minimum limit on the size to which audio sub-blocks will be divided when rendering.

	When rendering, the audio blocks that are passed into renderNextBlock() will be split up
	into smaller blocks that lie between all the incoming midi messages, and it is these smaller
	sub-blocks that are rendered with multiple calls to renderVoices().

	Obviously in a pathological case where there are midi messages on every sample, then
	renderVoices() could be called once per sample and lead to poor performance, so this
	setting allows you to set a lower limit on the block size.

	The default setting is 32, which means that midi messages are accurate to about < 1ms
	accuracy, which is probably fine for most purposes, but you may want to increase or
	decrease this value for your synth.

	If shouldBeStrict is true, the audio sub-blocks will strictly never be smaller than numSamples.

	If shouldBeStrict is false (default), the first audio sub-block in the buffer is allowed
	to be smaller, to make sure that the first MIDI event in a buffer will always be sample-accurate
	(this can sometimes help to avoid quantisation or phasing issues).
	*/

protected:
	//==============================================================================
	/** This is used to control access to the rendering callback and the note trigger methods. */
	CriticalSection lock;

	XSynthVoice voices[VOICECOUNT]; 

	/** The last pitch-wheel values for each midi channel. */
	int lastPitchWheelValues[16];
	virtual void onRenderSample() {}

	/** Renders the voices for the given range.
	By default this just calls renderNextBlock() on each voice, but you may need
	to override it to handle custom cases.
	*/
	template <typename FloatType>
	void renderVoices(AudioBuffer<FloatType>& outputAudio,
		int startSample, int numSamples);

	/** Can be overridden to do custom handling of incoming midi events. */
	virtual void handleMidiEvent(const MidiMessage&);
	int PreviousNote();
	void Sustain(bool _on);
	bool CheckVoicePlaying(int pitch);
	bool FPoly = false;
	int EarliestPlayingIndex();
	void RemoveVoice();
	int ActiveVoicesCount();
	int GetFreeIndex();
	void RegisterNote(int pitch) { FNotes[pitch] = FNoteCounter; }
	void UnregisterNote(int pitch) { FNotes[pitch] = 0; }
	int FNoteCounter;
	double sampleRate;
private:
	//==============================================================================
	template <typename floatType>
	void processNextBlock(AudioBuffer<floatType>& outputAudio,
		const MidiBuffer& inputMidi,
		int startSample,
		int numSamples);
	//==============================================================================

	double FLastVelocity; 
    int FLastPitch;
	int FNotes[128];

	bool FSustain;
	int minimumSubBlockSize;
	bool subBlockSubdivisionIsStrict;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(XSynthesizer)
};


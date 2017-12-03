#include "XSynthVoice.h"
#include "XSynthModel.h"

XSynthVoice::XSynthVoice() // XSynthModel *_model)
{
  isPlaying = false;
  sustain = false;
  for (int i=0;i<3;i++) FOscillators[i].setDigital(false);
}

XSynthVoice::~XSynthVoice()
{

}

void XSynthVoice::setSampleRate(double const& Value)
{
	if (Value != FSampleRate)
	{
		FSampleRate = Value;
		sampleRateChanged();
	}
}

void XSynthVoice::sampleRateChanged()
{
	int osc;
	FSampleReci = 1 * 1.0 / FSampleRate;
	for (osc = 0; osc < 3; ++osc)
		FOscillators[osc].setSampleRate(FSampleRate); 
	FFilter.setSampleRate(FSampleRate);  
}

void XSynthVoice::ProcessADSR(TADSRStage & FADSRStage, double & FADSRGain, double A, double D, double S, double R)
{
	if (FReleased) FADSRStage = adsrRelease;
	switch (FADSRStage) {
	case adsrAttack: {
		FADSRGain = FADSRGain + A * (1 - FADSRGain);
		if (FADSRGain > 0.999) FADSRStage = adsrDecay;
	}
	break;
	case adsrDecay: {
		FADSRGain = FADSRGain - D * (FADSRGain - S);
		if (FADSRGain < S)
		{
			FADSRStage = adsrSustain;
			FADSRGain = S;
		}
	}
	break;
	case adsrSustain: {
		FADSRGain = S;
		// make sure ADSRGAin <> 0, because the note will be ended, even if you slide sustain up
	}
    break;
	case adsrRelease: {
		FADSRGain = FADSRGain - R * FADSRGain;
		if (FADSRGain < 0.001) FADSRGain = 0;
	}
    break;

	}
}

double XSynthVoice::LFODelay(bool enabled, double Value, double delay)
{
	double result; 
    double t;
	if ((!enabled) || (delay<0.01)) result = Value; else
	{
		// maximum delay time = 5 seconds
		t = FTicks * FSampleReci * 0.2 *1.0 / delay;
		if (t>1) t = 1;
		result = t*Value;
	}
	return result;
}

double XSynthVoice::GetOscFrequency(int osc, double & currPitch)
{
	double depth; 
    double ratio;
	double spitch;  
	if (!model->GetGlide(this, depth)) ratio=1;  
	else
	{
		double timeToGlide = abs(pitch - Fstartpitch) * depth *1.0 / 12; // in Seconds...
		ratio = FTicks *1.0 / (timeToGlide * FSampleRate);
		if (ratio > 1) ratio=1;
    }
	spitch = pitch*ratio + Fstartpitch*(1 - ratio);
	currPitch = spitch;
	return model->GetOscFrequency(osc, spitch);
}

double XSynthVoice::process()
{
	double result;
	bool enabled;
	double lfo[2];
	double gain; double t; double vcf; double vca; double A; double D; double S; double R; double rate; double currPitch;

	// LFOs
	++FTicks;
	for (int ilfo = 0; ilfo < 2; ++ilfo)
	{
		lfo[ilfo] = model->GetLFOValue(ilfo);
		model->GetLFODelay(ilfo, enabled, rate);
		lfo[ilfo] = LFODelay(enabled, lfo[ilfo], rate);
	}
	// Oscillators

	result = 0;
	for (int iosc = 0; iosc < 3; ++iosc)
	{
		// OSC LFO modulation
		t = 0;
		for (int ilfo = 0; ilfo < 2; ++ilfo)
			t = t + lfo[ilfo] *model->GetOscModDepth(iosc, ilfo) *1.0 / 4;

		// t ligt tussen -0.5 en 0.5 en dat betekent 1 octaaf max...
		FOscillators[iosc].setWaveShape(model->GetOscWaveshape(iosc));
		FOscillators[iosc].setPulseWidth(model->GetOscPulseWidth(iosc));
		FOscillators[iosc].setFrequency(GetOscFrequency(iosc, currPitch)*(1 + t));
		result = result + FOscillators[iosc].process()*model->GetOscLevel(iosc);
	}
	// VCA
	if (isQuickReleasing)
	{
		FReleaseQuickADSRRange = FReleaseQuickADSRRange - 0.0003;
		FADSRGainVCA = FReleaseQuickADSRRange;
		gain = 1;
	}
	else 
	{
		model->GetConvertedVCA(A, D, S, R, gain);
		ProcessADSR(FADSRStageVCA, FADSRGainVCA, A, D, S, R);
	}
	vca = FADSRGainVCA*gain;
	if ((FADSRGainVCA <= 0))
	{
		result = 0;
		model->onVoiceDone(this);
		return result;
	}
	// VCF
	model->GetConvertedVCF(A, D, S, R, gain);
	ProcessADSR(FADSRStageVCF, FADSRGainVCF, A, D, S, R);
	vcf = FADSRGainVCF*gain;
	// VCF LFO & Filter
	t = 0;
	for (int ilfo = 0; ilfo < 2; ++ilfo)
		t = t + lfo[ilfo]*model->GetVCFModDepth(ilfo) *1.0 / 4; 
	FFilter.setCutoff( model->GetCutoff(currPitch)*vcf*(1 + t));
	FFilter.setResonance(model->GetResonance());
	result = FFilter.process(result);  

	// VCA LFO & Amp & Volume
	t = 0;
	for (int ilfo = 0; ilfo < 2; ++ilfo)
		t = t + lfo[ilfo]*model->GetVCAModDepth(ilfo) *1.0 / 4; 
	result = result*vca*(1 + t)*model->GetLevel()*FNoteLevel;
	return result;
}

void XSynthVoice::NoteOn(int notePitch, int startPitch, double amplitude)
{
	FReleased = false;
	FFilter.reset();
	FADSRStageVCA = adsrAttack;
	FADSRGainVCA = 0;
	FADSRStageVCF = adsrAttack;
	FADSRGainVCF = 0;
	FTicks = 0;
	pitch = notePitch;
	Fstartpitch = startPitch;
	FNoteLevel = amplitude;
    isQuickReleasing = false;
}

void XSynthVoice::NoteOff()
{
	FReleased = true;
}

void XSynthVoice::releaseQuick()
{
  isQuickReleasing = true;
  FReleaseQuickADSRRange = FADSRGainVCA;
  NoteOff();
}






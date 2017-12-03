/*
  ==============================================================================

    XSynthModel.cpp
    Created: 14 Sep 2017 1:07:57pm
    Author:  Ruud

  ==============================================================================
*/

#include "XSynthModel.h"
#include "XSynthVoice.h"
#include "PluginEditor.h"
#include "XSynthConstants.h"

// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	return new XSynthModel();
}

XSynthModel::XSynthModel()
{
  for (int i = 0; i<VOICECOUNT; i++) voices[i].setModel(this);
  FLevel = 1;
  for (int lfo = 0; lfo < 2; ++lfo)
  {
	  FLFOOscs[lfo].setWaveShape (wsNone);
	  FLFOOscs[lfo].setSampleRate(sampleRate);
  }
  FModWheel = 0;
  FPitchBend = 0.5;

  XAddParameter(PARMOSC1TYPE, "PARMOSC1TYPE", "OSC1 Wave", 0, 3, 0, "", "Square PW;Square;Triangle;Saw");
  XAddParameter(PARMVCAATTACK, "PARMVCAATTACK", "VCA Attack", 0, 100, 10, "%");
  XAddParameter(PARMVCADECAY, "PARMVCADECAY", "VCA Decay", 0, 100, 80, "%");
  XAddParameter(PARMVCASUSTAIN, "PARMVCASUSTAIN", "VCA Sustain", 0, 100, 80, "%");
  XAddParameter(PARMVCARELEASE, "PARMVCARELEASE", "VCA Release", 0, 100, 70, "%");
  XAddParameter(PARMOSC1LEVEL, "PARMOSC1LEVEL", "OSC1 Level", 0, 100, 80, "%");
  XAddParameter(PARMOSC2TYPE, "PARMOSC2TYPE", "OSC2 Wave", 0, 3, 0, "", "Sine;Square;Triangle;Saw");
  XAddParameter(PARMVCFATTACK, "PARMVCFATTACK", "VCF Attack", 0, 100, 10, "%");
  XAddParameter(PARMVCFDECAY, "PARMVCFDECAY", "VCF Decay", 0, 100, 80, "%");
  XAddParameter(PARMVCFSUSTAIN, "PARMVCFSUSTAIN", "VCF Sustain", 0, 100, 80, "%");
  XAddParameter(PARMVCFRELEASE, "PARMVCFRELEASE", "VCF Release", 0, 100, 70, "%");
  XAddParameter(PARMOSC2LEVEL, "PARMOSC2LEVEL", "OSC2 Level", 0, 100, 40, "%");
  XAddParameter(PARMCUTOFF, "PARMCUTOFF", "Cutoff", 20, 20000, 10000, "Hz", "__CUTOFF");
  XAddParameter(PARMRESONANCE, "PARMRESONANCE", "Resonance", 0, 100, 0, "");
  XAddParameter(PARMOUTPUTLEVEL, "PARMOUTPUTLEVEL", "Output Level", 0, 100, 100, "%");
  XAddParameter(PARMLFO1TYPE, "PARMLFO1TYPE", "LFO1 Wave", 0, 3, 0, "", "Saw Up;Square;Triangle;Saw Down");
  XAddParameter(PARMLFO1RATE, "PARMLFO1RATE", "LFO1 Speed", 0, 127, 10, "Hz", "__LFO");
  XAddParameter(PARMOSC1MODTYPE, "PARMOSC1MODTYPE", "OSC1 Mod Type", 0, 3, 0, "", "None;LFO1;LFO2;LFO1+2");
  XAddParameter(PARMOSC1MODLEVEL, "PARMOSC1MODLEVEL", "OSC1 Mod Depth", 0, 100, 0, "%");
  XAddParameter(PARMNOISECOLOR, "PARMNOISECOLOR", "Noise Color", 0, 1, 0, "", "Pink;White");
  XAddParameter(PARMNOISELEVEL, "PARMNOISELEVEL", "Noise Level", 0, 100, 0, "%");
  XAddParameter(PARMOSC1FOOT, "PARMOSC1FOOT", "OSC1 Foot", 0, 3, 2, "", "32;16;8;4");
  XAddParameter(PARMOSC1SEMI, "PARMOSC1SEMI", "OSC1 Semi", -12, 12, 0, "");
  XAddParameter(PARMOSC2FOOT, "PARMOSC2FOOT", "OSC2 Foot", 0, 3, 2, "", "32;16;8;4");
  XAddParameter(PARMOSC2SEMI, "PARMOSC2SEMI", "OSC2 Semi", -12, 12, 0, "");
  XAddParameter(PARMOSC2DETUNE, "PARMOSC2DETUNE", "OSC2 Detune", 0, 100, 10, "%");
  XAddParameter(PARMOSC2MODTYPE, "PARMOSC2MODTYPE", "OSC2 Mod Type", 0, 3, 0, "", "None;LFO1;LFO2;LFO1+2");
  XAddParameter(PARMOSC2MODLEVEL, "PARMOSC2MODLEVEL", "OSC2 Mod Depth", 0, 100, 0, "%");
  XAddParameter(PARMVCFMODTYPE, "PARMVCFMODTYPE", "VCF Mod Type", 0, 3, 0, "", "None;LFO1;LFO2;LFO1+2");
  XAddParameter(PARMVCFMODLEVEL, "PARMVCFMODLEVEL", "VCF Mod Depth", 0, 100, 0, "%");
  XAddParameter(PARMVCAMODTYPE, "PARMVCAMODTYPE", "VCA Mod Type", 0, 3, 0, "", "None;LFO1;LFO2;LFO1+2");
  XAddParameter(PARMVCAMODLEVEL, "PARMVCAMODLEVEL", "VCA Mod Depth", 0, 100, 0, "%");
  XAddParameter(PARMVCFLEVEL, "PARMVCFLEVEL", "VCF ADSR", 0, 100, 80, "%");
  XAddParameter(PARMVCALEVEL, "PARMVCALEVEL", "VCA ADSR", 0, 100, 80, "%");
  XAddParameter(PARMVCAMODLEVEL, "PARMVCAMODLEVEL", "VCA Mod Depth", 0, 100, 0, "%");
  XAddParameter(PARMLFO1DELAY, "PARMLFO1DELAY", "LFO1 Delay", 0, 1, 0, "", "Off;On");
  XAddParameter(PARMLFO2TYPE, "PARMLFO2TYPE", "LFO1 Wave", 0, 3, 0, "", "Stairway;Square;Triangle;Sample+Hold");
  XAddParameter(PARMLFO2RATE, "PARMLFO2RATE", "LFO2 Speed", 0, 127, 5, "Hz", "__LFO");
  XAddParameter(PARMLFO2DELAY, "PARMLFO2DELAY", "LFO2 Delay", 0, 1, 0, "", "Off;On");
  XAddParameter(PARMLFODELAY, "PARMLFODELAY", "LFOs Delay", 0, 100, 30, "%");
  XAddParameter(PARMPWMMODTYPE, "PARMPWMMODTYPE", "PWM Mod Type", 0, 3, 0, "", "Manual;LFO1;LFO2;LFO1+2");
  XAddParameter(PARMPWMMODLEVEL, "PARMPWMMODLEVEL", "PWM Mod Depth", 0, 100, 50, "%");
  XAddParameter(PARMGLIDE, "PARMGLIDE", "Glide", 0, 100, 30, "%");
  XAddParameter(PARMVCFKEYFOLLOW, "PARMVCFKEYFOLLOW", "Key Follow", 0, 100, 0, "%");
  XAddParameter(PARMPOLY, "PARMPOLY", "Poly", 0, 1, 0, "", "Mono;Poly");
  XAddParameter(PARMGLIDEENABLE, "PARMGLIDEENABLE", "Glide Enable", 0, 1, 0, "", "Off;On");
  resendParameters();
}

double XSynthModel::calcLFO(double value)
{
   return 0.01*exp(0.0598*value);
}

void XSynthModel::OnParameterChanged(int index, int value)
{
	const int ADSR_MUL_A = 4;
	const int ADSR_MUL_D = 4;
	const int ADSR_MUL_R = 8;

	switch (index) {
	case PARMOSC1TYPE:     FOscs[0].OType = value;
		break;
	case PARMVCAATTACK:    FVCA.Attack = ConvertADSR(value/100.0,ADSR_MUL_A);
		break;
	case PARMVCADECAY:     FVCA.Decay = ConvertADSR(value/100.0, ADSR_MUL_D);
		break;
	case PARMVCARELEASE:   FVCA.Release = ConvertADSR(value/100.0, ADSR_MUL_R);
		break;
	case PARMVCASUSTAIN:   FVCA.Sustain = value/100.0;
		break;
	case PARMOSC1LEVEL:    FOscs[0].Level = value/100.0;
		break;
	case PARMOSC2TYPE:     FOscs[1].OType = value;
		break;
	case PARMVCFATTACK:    FVCF.Attack = ConvertADSR(value/100.0, ADSR_MUL_A);
		break;
	case PARMVCFDECAY:     FVCF.Decay = ConvertADSR(value/100.0, ADSR_MUL_D);
		break;
	case PARMVCFRELEASE:   FVCF.Release = ConvertADSR(value/100.0, ADSR_MUL_R);
		break;
	case PARMVCFSUSTAIN:   FVCF.Sustain = value/100.0;
		break;
	case PARMOSC2LEVEL:    FOscs[1].Level = value/100.0;
		break;
	case PARMCUTOFF:       FVCF.Cutoff =value;
		break;
	case PARMRESONANCE:    FVCF.Resonance = value;
		break;
	case PARMOUTPUTLEVEL:  FLevel = value/100.0;
		break;
	case PARMLFO1TYPE:     FLFOs[0].OType = value;
		break;
	case PARMLFO1RATE:     FLFOs[0].speed = calcLFO(value);
		break;
	case PARMOSC1MODTYPE:  FOscs[0].ModTarget = value;
		break;
	case PARMOSC1MODLEVEL: FOscs[0].ModDepth = value/100.0;
		break;
	case PARMNOISECOLOR:   FOscs[2].OType = value;
		break;
	case PARMNOISELEVEL:   FOscs[2].Level = value/100.0;
		break;
	case PARMOSC1FOOT:     FOscs[0].Foot = value;
		break;
	case PARMOSC1SEMI:     FOscs[0].Semi = value;
		break;
	case PARMOSC2FOOT:     FOscs[1].Foot = value;
		break;
	case PARMOSC2SEMI:     FOscs[1].Semi = value;
		break;
	case PARMOSC2DETUNE:   FDetune = value/100.0;
		break;
	case PARMOSC2MODTYPE:  FOscs[1].ModTarget = value;
		break;
	case PARMOSC2MODLEVEL: FOscs[1].ModDepth = value/100.0;
		break;
	case PARMVCFMODTYPE:   FVCF.ModTarget = value;
		break;
	case PARMVCFMODLEVEL:  FVCF.ModDepth = value/100.0;
		break;
	case PARMVCAMODTYPE:   FVCA.ModTarget = value;
		break;
	case PARMVCAMODLEVEL:  FVCA.ModDepth = value/100.0;
		break;
	case PARMVCFLEVEL:     FVCF.Level = value/100.0;
		break;
	case PARMVCALEVEL:     FVCA.Level = value/100.0;
		break;
	case PARMLFO1DELAY:    FLFOs[0].delay = value!=0;
		break;
	case PARMLFO2TYPE:     FLFOs[1].OType = value;
		break;
	case PARMLFO2RATE:     FLFOs[1].speed = calcLFO(value);
		break;
	case PARMLFO2DELAY:    FLFOs[1].delay = value!=0;
		break;
	case PARMLFODELAY:     FDelay = value/100.0;
		break;
	case PARMPWMMODTYPE:   FPWM.ModTarget = value;
		break;
	case PARMPWMMODLEVEL:  FPWM.ModDepth = value/100.0;
		break;
	case PARMGLIDE:        FGlide = value/100.0;
		break;
	case PARMGLIDEENABLE:  FGlideEnable = value!=0;
		break;
	case PARMVCFKEYFOLLOW: FVCFKeyFollow = value/100.0;
		break;
	case PARMPOLY:         FPoly = value!=0;
		break;
	}
}

double XSynthModel::GetCutoff(double pitch)
{
	double result; 
    double t;
	result = FVCF.Cutoff;
	// pitch bepaalt bij FVCFKeyFollow de uiteindelijke cutoff
	t = FVCFKeyFollow*(84 - pitch) *1.0 / 48;  // ongeveer tussen 0 en 1, komt niet zo nauw
	if (t>1) t = 1;
	result = result*(1 - 0.7*t);
	return result;
}


bool XSynthModel::GetGlide(XSynthVoice *voice, double & depth)
{
	bool result;
	result = (voice->noteCounter == FNoteCounter - 1) && (voice->noteCounter != 0) && FGlideEnable;
	depth = FGlide;
	return result;
} 

double XSynthModel::GetResonance()
{
	return FVCF.Resonance; 
}

double XSynthModel::ConvertADSR(double x, double MAXSECONDS)
{
	double result; 
    double t;

	static double const VMIN = 0.005;

	t = x * MAXSECONDS * sampleRate;
    result = 1 - pow(VMIN, 1 * 1.0 / t);
	return result;
}

void XSynthModel::GetConvertedVCA(double & A, double & D, double & S, double & R, double & gain)
{
	A = FVCA.Attack;
	D = FVCA.Decay;
	S = FVCA.Sustain;
	R = FVCA.Release;
	gain = FVCA.Level;
}

void XSynthModel::GetConvertedVCF(double & A, double & D, double & S, double & R, double & gain)
{
	A = FVCF.Attack;
	D = FVCF.Decay;
	S = FVCF.Sustain;
	R = FVCF.Release;
	gain = FVCF.Level;
}

double XSynthModel::GetLevel()
{
	return FLevel;
}

void XSynthModel::GetLFODelay(int lfo, bool & enabled, double & rate)
{
	enabled = FLFOs[lfo].delay;
	rate = FDelay;
}

double XSynthModel::GetLFOSpeed(int lfo)
{
	return  FLFOs[lfo].speed;
}

double XSynthModel::GetLFOValue(int lfo)
{
	return FLFOs[lfo].value;
}

double XSynthModel::GetOscModDepth(int osc, int lfo)
{
	double result;
	if ((FOscs[osc].ModTarget & (1 << lfo)) != 0)
		result = std::max(FOscs[osc].ModDepth, FModWheel); 
    else result = 0;
	return result;
}

double XSynthModel::GetOscPulseWidth(int osc)
{
	double result; 
	double t;

	result = 0.5;
	if ((osc == 0) && (FOscs[0].OType == 0))
	{
		if (FPWM.ModTarget == 0)
			result = 0.5 - 0.45*FPWM.ModDepth; 
        else
		{
			t = 0;
			{ for (int lfo = 0; lfo < 2; ++lfo)
				if ((FPWM.ModTarget & (1 << lfo)) != 0)
					t = t + FLFOOscs[lfo].value *1.0 / 2; }
			result = 0.5 + 0.45*FPWM.ModDepth*t;
		}
	}
	return result;
}

double XSynthModel::GetVCFModDepth(int lfo)
{
	double result;
	if ((FVCF.ModTarget & (1 << lfo)) != 0)
		result = std::max(FVCF.ModDepth, FModWheel);
    else result = 0;
	return result;
}

double XSynthModel::GetVCAModDepth(int lfo)
{
	double result;
	if ((FVCA.ModTarget & (1 << lfo)) != 0)
		result = std::max(FVCA.ModDepth, FModWheel);
    else result = 0;
	return result;
}

double XSynthModel::GetOscLevel(int osc)
{
	return FOscs[osc].Level; 
}

double XSynthModel::pitch2Hertz(double pitch)
{
	const float frequencyOfA = 440.0;
	return frequencyOfA * pow(2.0, (pitch - 69) / 12.0);
}

double XSynthModel::GetOscFrequency(int osc, double pitch)
{
	if (osc == 2) return 10000;
	pitch = pitch + (FOscs[osc].Foot - 2) * 12 + FOscs[osc].Semi;
	if (pitch<12) pitch = 12;
	if (pitch>108) pitch = 108;
	pitch = pitch + 16 * (FPitchBend - 0.5);
	if (osc == 0) pitch = pitch - FDetune*1.0 / 2; else pitch = pitch + FDetune*1.0 / 2;
	return pitch2Hertz(pitch);
}

EWaveShape XSynthModel::GetOscWaveshape(int osc)
{
	EWaveShape result = wsSine;
	switch (FOscs[osc].OType) {
	case 0:
		switch (osc) {
		case 0:  result = wsSquare;
			break;
		case 1:  result = wsSine;
			break;
		case 2:  result = wsPinkNoise;
			break;
		}
		break;
	case 1:  if (osc != 2) result = wsSquare; else result = wsWhiteNoise;
		break;
	case 2:  result = wsTriangle;
		break;
	case 3:  result = wsSawUp;
		break;
	}
	return result;
}

EWaveShape XSynthModel::GetLFOWaveshape(int osc)
{
	EWaveShape result = wsSine;
	switch (FLFOs[osc].OType) {
	case 0:   if (osc == 0) result = wsSawUp; else result = wsStairCase;
		break;
	case 1:   result = wsSquare;
		break;
	case 2:   result = wsSine;
		break;
	case 3:   if (osc == 0) result = wsSawDown; else result = wsSAndH;
		break;
	}
	return result;
}

void XSynthModel::SetSampleRate(double rate)
{
	int i; int lfo;
	setCurrentPlaybackSampleRate(rate);

	for (lfo = 0; lfo < 2; ++lfo)
		FLFOOscs[lfo].setSampleRate(sampleRate); 
	for (i = 0; i < VOICECOUNT; ++i)
		voices[i].setSampleRate(sampleRate); 
}

void XSynthModel::prepareToPlay(double newSampleRate, int /*samplesPerBlock*/)
{
	// Use this method as the place to do any pre-playback
	// initialisation that you need..
    inherited::prepareToPlay(newSampleRate, 0);
	SetSampleRate(newSampleRate);
}

void XSynthModel::renderSynth(AudioBuffer<float>& buffer, MidiBuffer& midiMessages, int /*startSample*/, int numSamples)
{
	renderNextBlock(buffer, midiMessages, 0, numSamples);
}
void XSynthModel::renderSynth(AudioBuffer<double>& buffer, MidiBuffer& midiMessages, int /*startSample*/, int numSamples)
{
	renderNextBlock(buffer, midiMessages, 0, numSamples);
}

void XSynthModel::onRenderSample()
{
  for (int lfo = 0; lfo<2; lfo++)
  {
	FLFOOscs[lfo].setWaveShape(GetLFOWaveshape(lfo));
	FLFOOscs[lfo].setFrequency(GetLFOSpeed(lfo));
	FLFOs[lfo].value = FLFOOscs[lfo].process();
  }
}

void XSynthModel::handleController(int midiChannel, int controllerNumber, int controllerValue)
{
	XSynthesizer::handleController(midiChannel, controllerNumber, controllerValue);
    if (controllerNumber == 1)
	  FModWheel = controllerValue *1.0 / 127;
    else
    {
		int index = cctoparam(controllerNumber);
		if (index == -1) return;
		setValueFrom0__127(index, controllerValue);
    }
}

void XSynthModel::handlePitchWheel(int /*midiChannel*/, int wheelValue)
{
	FPitchBend = (float)wheelValue / (128 * 128);
}


void XSynthModel::handleProgramChange(int /*midiChannel*/, int programNumber)
{
	setCurrentProgram(programNumber); 
}

//==============================================================================
AudioProcessorEditor* XSynthModel::createEditor()
{
	return new XSynthModelEditor(*this);
}

int XSynthModel::cctoparam(int cc)
// this code was generated with a tool...
{
	const int PhysToParam[128] = {-1, -1, -1, -1, -1, 42, -1, -1, -1, 44, 1, 2, -1, 0, 3, -1, -1, -1, -1, -1, 5, 6, -1, 4, 7, -1, 12, 15, 16, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 10, -1, -1, 11, -1, 13, -1, -1, 14, -1, 17, 18, 19, 20, -1, 43, 21, -1, -1, -1, -1, 28, 26, -1, 27, 22, 23, 24, 25, 29, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 41, -1, -1, -1, 8, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
	/////////////////////////////////////////
	return PhysToParam[cc];
}


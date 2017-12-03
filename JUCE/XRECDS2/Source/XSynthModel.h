/*
  ==============================================================================

    XSynthModel.h
    Created: 14 Sep 2017 1:07:57pm
    Author:  Ruud

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "XOscillator.h"
//#include "XSynthVoice.h"
#include "PluginProcessor.h"
#include "XSynthesizer.h"

#pragma once

struct TOsc {
	int OType;
	double Level;
	double ModDepth;
	int ModTarget;
	int Foot;
	int Semi;
};
struct TVCF {
	double Attack;
	double Decay;
	double Release;
	double Sustain;
	double ModDepth;
	int ModTarget;
	double Cutoff; 
    double Resonance;
	double Level;
};
struct TVCA {
	double Attack;
	double Decay;
	double Release;
	double Sustain;
	double ModDepth;
	int ModTarget;
	double Level;
};
struct TLFO {
	int OType;
	double speed;
	bool delay;
	double value; // current lfo value
};
struct TPWM {
	double ModDepth;
	int ModTarget;
};

class XSynthModel : public XSynthModule, public XSynthesizer
{
typedef XSynthModule  inherited;
public:
    XSynthModel();
	void OnParameterChanged(int index, int value) override;
	void prepareToPlay(double sampleRate, int samplesPerBlock) override;
	AudioProcessorEditor* createEditor() override;
	EWaveShape GetOscWaveshape(int osc);
	double GetOscFrequency(int osc, double pitch)/*overload*/;

	double GetCutoff(double pitch);
	double GetResonance();
	double GetOscModDepth(int osc, int lfo);
	double GetVCFModDepth(int lfo);
	double GetVCAModDepth(int lfo);
	double GetOscLevel(int osc);
	void GetConvertedVCA(double & A, double & D, double & S, double & R, double & gain);
	void GetConvertedVCF(double & A, double & D, double & S, double & R, double & gain);
	double GetLevel();
	double GetLFOValue(int lfo);
	void GetLFODelay(int lfo, bool & enabled, double & rate);
	double GetOscPulseWidth(int osc);
	bool GetGlide(XSynthVoice *voice, double & depth);

protected:

	void renderSynth(AudioBuffer<float>& buffer, MidiBuffer& midiMessages, int startSample, int numSamples) override;
	void renderSynth(AudioBuffer<double>& buffer, MidiBuffer& midiMessages, int startSample, int numSamples) override;
	void onRenderSample() override;
	void handleController(int midiChannel, int controllerNumber, int controllerValue) override;
	void handlePitchWheel(int midiChannel, int wheelValue);
	void handleProgramChange(int midiChannel, int programNumber) override;
private:
	double FLevel;
	double pitch2Hertz(double pitch) ;
	TOsc FOscs[3];
	TVCF FVCF; 
	TVCA FVCA; 
	TPWM FPWM;
	double FDetune; double FDelay;
	TLFO FLFOs[2];
	XOscillator FLFOOscs[2];
	double FLastFrequency[2];
	double FVCFKeyFollow; double FGlide;
    bool FGlideEnable;
	double FModWheel, FPitchBend;

	double GetLFOSpeed(int lfo);
	EWaveShape GetLFOWaveshape(int osc);
	void SetSampleRate(double rate);
	void RegisterNote(int pitch);
	void UnregisterNote(int pitch);

	int PreviousNote();
	void Sustain(bool _on);
	int cctoparam(int cc);
	double ConvertADSR(double x, double MAXSECONDS);

	int waveShapeParam = 0;
	void initialiseSynth();
	double calcLFO(double value);
};


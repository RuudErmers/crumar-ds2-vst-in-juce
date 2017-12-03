#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
//#include "XSynthModel.h"
#include "XOscillator.h"
#include "XMoogFilter.h"

class XSynthModel;
//==============================================================================
enum TADSRStage { adsrAttack, adsrDecay, adsrSustain, adsrRelease };
class XSynthVoice 
{
public:
	XSynthVoice(); 
	~XSynthVoice();
	void setSampleRate(double const & Value);
    void setModel(XSynthModel *amodel) { model = amodel; };
	double process();
	/* property */ int noteCounter;
	/* property */ bool isPlaying;
    /* property */ int pitch;
	/* property */ bool sustain; 
    /* property */ bool isQuickReleasing; 
	void NoteOn(int notePitch, int startPitch, double amplitude);
	void NoteOff();
	void releaseQuick();
private:
	double FNoteLevel, FReleaseQuickADSRRange;
	bool FReleased;

	XOscillator FOscillators[3];
	TADSRStage FADSRStageVCA;
	double FADSRGainVCA;
	TADSRStage FADSRStageVCF;
	double FADSRGainVCF;
	XMoogFilter FFilter;
	int Fstartpitch;
	int FTicks;
	void ProcessADSR(TADSRStage & FADSRStage, double & FADSRGain, double A, double D, double S, double R);
	double LFODelay(bool enabled, double Value, double delay);
	double GetOscFrequency(int osc, double & currPitch);
	void sampleRateChanged();
	XSynthModel *model = NULL;
	double level = 0, tailOff = 0;
	double FSampleRate, FSampleReci;
};


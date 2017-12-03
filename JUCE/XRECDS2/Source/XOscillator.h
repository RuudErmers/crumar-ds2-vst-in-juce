/*
  ==============================================================================

    XOscillator.h
    Created: 13 Sep 2017 11:51:36am
    Author:  Ruud

  ==============================================================================
*/

#pragma once
enum  EWaveShape { wsNone, wsSine, wsSquare, wsWhiteNoise, wsPinkNoise, wsTriangle, wsSawUp, wsSawDown, wsStairCase, wsSAndH };

class XPinky {
public:
  XPinky() { for (int i=0;i<5;i++)  FContribution[i]=0; }
  double process();
private:
	double FContribution[5];
};

class XBaseOscillator 
{
public:
  XBaseOscillator(); 
  void setWaveShape(EWaveShape _waveShape);
  void setFrequency(double _frequency);
  void setSampleRate(double _sampleRate);
  void setDigital(bool digital) { fDigital = digital; }
  void setPulseWidth(double _pulseWidth) { pulseWidth =_pulseWidth;} 
  virtual double process();
  double value;
protected:
  EWaveShape waveShape;
  XPinky pinky;
  bool fDigital;
  double frequency,sampleRate,sampleReci, positionRe, positionIm, xPos, SAndHLevel, stairCaseLevel, pulseWidth, angleRe, angleIm;
};

class XOscillator : public XBaseOscillator
{
typedef XBaseOscillator inherited;
public:
	double process() override;
private:
	double polyBlep(double t);

};


/*
  ==============================================================================

    XOscillator.cpp
    Created: 13 Sep 2017 11:51:36am
    Author:  Ruud

  ==============================================================================
*/

#include "XOscillator.h"
#define _USE_MATH_DEFINES
#include <cmath>
#include "../JuceLibraryCode/JuceHeader.h"

double XPinky::process()
{   
	double result;
	double ur1;

	static double const pA[5] = { 0.23980, 0.18727, 0.1638, 0.194685, 0.214463 };
	static double const pSUM[5] = { 0.00198, 0.01478, 0.06378, 0.23378, 0.91578 };

	ur1 = Random::getSystemRandom().nextDouble();
	double FastRandom = 2 * Random::getSystemRandom().nextDouble() - 1;
	if ((ur1 <= pSUM[0])) FContribution[0] = FastRandom * pA[0]; else
		if ((ur1 <= pSUM[1])) FContribution[1] = FastRandom * pA[1]; else
			if ((ur1 <= pSUM[2])) FContribution[2] = FastRandom * pA[2]; else
				if ((ur1 <= pSUM[3])) FContribution[3] = FastRandom * pA[3]; else
					if ((ur1 <= pSUM[4])) FContribution[4] = FastRandom * pA[4];

	result = (FContribution[0] + FContribution[1] +
		FContribution[2] + FContribution[3] + FContribution[4]);
	return result;
}

XBaseOscillator::XBaseOscillator()
{
  frequency= 1000;
  positionRe = 0;      
  positionIm = -1;      
  angleRe = 0;
  angleIm=-1;
  xPos = 0;
  SAndHLevel = 0;
  stairCaseLevel = 0;
  pulseWidth = 0.5;
  waveShape = wsNone;
  fDigital = true;
}

void XBaseOscillator::setWaveShape(EWaveShape _waveShape)
{
  waveShape=_waveShape;
}

void XBaseOscillator::setFrequency(double _frequency)
{
  frequency = _frequency;
  angleIm = sin(2 * M_PI *frequency * sampleReci);
  angleRe = cos(2 * M_PI *frequency * sampleReci);
}

void XBaseOscillator::setSampleRate(double _sampleRate)
{
   sampleRate = _sampleRate;
   if (sampleRate==0) sampleReci = 1;
   else sampleReci = 1 / sampleRate; 
}

double XBaseOscillator::process()
{
  double pos,result=0;
  pos  = positionRe*angleRe - positionIm*angleIm;
  positionIm = positionIm*angleRe + positionRe*angleIm;
  positionRe = pos;
  xPos = xPos + sampleReci* frequency;
  if (xPos>=1)
  { 
	xPos = xPos - 1;
    SAndHLevel = 1-2* Random::getSystemRandom().nextDouble();
    stairCaseLevel = stairCaseLevel - 1.0 / 6;
	if (stairCaseLevel<=-1) stairCaseLevel  = 1;
  }
  switch (waveShape)
  {
	case wsNone : result = 0;
                  break;
    case wsPinkNoise: result=pinky.process(); 
						break;
	case wsWhiteNoise: result = (2 * Random::getSystemRandom().nextDouble() - 1);
                       break;
  case wsSAndH: result = SAndHLevel;
					break;
  case wsStairCase: result = stairCaseLevel;
                    break;
  case wsSine: result = pos;
                    break;
  case wsSquare: result = xPos<pulseWidth ? 1 : -1;
                    break; 
  case wsTriangle: 
				   if (xPos<0.25) result  = 4 * xPos;
				   else if (xPos<0.75) result = 2 - 4 * xPos;
				   else result = -4 + 4 * xPos;
				   break;
  case wsSawUp:    if (xPos<0.5) result = 2 * xPos; else result  = -2 + 2 * xPos;
                   break;
  case wsSawDown:  if (xPos<0.5) result = -2 * xPos; else result  = 2 - 2 * xPos;
                   break;
  }
  value = result;
  return result; 
}

/* TXOscillator */

// http://www.martin-finke.de/blog/articles/audio-plugins-018-polyblep-oscillator/

double XOscillator::process()
{
	double result = inherited::process();
    if (fDigital) return result; // No smoothing...
	switch (waveShape) {
	case wsSawUp:result = result - polyBlep(xPos+0.5);
						break;
	case wsSawDown:  result = result + polyBlep(xPos+0.5);
						break;  
	case wsSquare: {
		result = result + polyBlep(xPos);
		result = result - polyBlep(fmod(xPos + 1 - pulseWidth,1));
	}
	}
	return result;
}


double XOscillator::polyBlep(double t)
{

	double dt;

	dt = sampleReci* frequency;
	// 0 <= t < 1
	if ((t < dt))
	{
		t = t *1.0 / dt;
		return 2 * t - t*t - 1.0;
	}
	// -1 < t < 0
	else if ((t > 1.0 - dt))
	{
		t = (t - 1.0) *1.0 / dt;
		return t*t + 2 * t + 1.0;
	}
	// 0 otherwise
	else return 0.0;
}



//END

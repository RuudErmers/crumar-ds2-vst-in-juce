/*
  ==============================================================================

    XMoogFilter.h
    Created: 15 Sep 2017 3:29:36pm
    Author:  Ruud

  ==============================================================================
*/

#pragma once

class XMoogFilter 
{
public:
	XMoogFilter();
	double process(double input);
	void reset();
	void setCutoff(double cutoff);
	void setResonance(double resonance);
	void setSampleRate(double value);

private:
	double FCutoff; double FResonance;
	double FSampleRate;
	double y1; double y2; double y3; double y4; double oldx; double oldy1; double oldy2; double oldy3; double x; double f; double p; double k; double t; double t2; double r;
	void calc();

};

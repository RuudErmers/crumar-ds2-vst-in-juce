/*
  ==============================================================================

    RMCLookAndFeel.cpp
    Created: 16 Sep 2017 2:34:10pm
    Author:  Ruud

  ==============================================================================
*/

#include "RMCLookAndFeel.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

int dx(int  r, int alpha) 
{
  return (int)(r*cos(M_PI*alpha / 180));
}

int dy(int  r, int alpha)
{
	return (int)(r*sin(M_PI*alpha / 180));
}
const int angles[4] = {270 - 36, 270 - 2 * 36, 270 - 3 * 36, 270 - 4 * 36};

void RMCLookAndFeel::drawLinearSliderThumb(Graphics& g, int x, int y, int width, int height, float sliderPos, float /*minSliderPos*/, float /*maxSliderPos*/, const Slider::SliderStyle /*style*/, Slider& slider)
{
	RMCSlider *rmcSlider = dynamic_cast<RMCSlider *>(&slider);
	if ((rmcSlider != nullptr) && (rmcSlider->knob == tkSliderMulti))
	{
		Image *im;
		Rectangle<float> r;
		int centerX, centerY;		if (width < height)
		{
			im = &verticalKnobThumb;
			centerX = x + width / 2;
			centerY = (int)sliderPos;  // Hmmm....
		}
		else
		{
			centerX = (int)sliderPos;
			centerY = y+height / 2;
			im = &horizontalKnobThumb;
		}
		g.setOpacity(1.0);
        float c = 1.0;
		r = Rectangle<float>(centerX - c * im->getWidth() , centerY - c *im->getHeight() , 2*c* im->getWidth(),2*c*im->getHeight());
		g.drawImage(*im, r);
	}
}

void RMCLookAndFeel::drawLinearSliderBackground(Graphics& g, int x, int y, int width, int height, float /*sliderPos*/, float /*minSliderPos*/, float /*maxSliderPos*/, const Slider::SliderStyle /*style*/, Slider& slider)
{
	RMCSlider *rmcSlider = dynamic_cast<RMCSlider *>(&slider);
	Image *im = nullptr;
	if ((rmcSlider != nullptr) && (rmcSlider->knob==tkSliderMulti))
	{
		if (rmcSlider->getWidth() < rmcSlider->getHeight())
             im = &verticalSlider;
        else
             im = &horizontalSlider; 
		g.setOpacity(1.0);
		g.drawImage(*im, Rectangle<float>((float)x, (float)y, (float)width, (float)height));
   }
}


void RMCLookAndFeel::drawRotarySlider(Graphics& g, int x, int y, int width, int height, float sliderPos,
	const float /*rotaryStartAngle*/, const float /*rotaryEndAngle*/, Slider& slider)
{
	RMCSlider *rmcSlider = dynamic_cast<RMCSlider *>(&slider);
	Image *im = nullptr;
	if (rmcSlider != nullptr)
		switch (rmcSlider->knob)
		{
		case tvLOF1: im = &KnobtvLOF1;
			break;
		case tvKnob: im = &KnobtvKnob;
			break;
		case tvWaveFoot4: im = &KnobtvWaveFoot4;
			break;
		case tvWaveShape4: im = &KnobtvWaveShape4;
			break;
		case tvLFOSel: im = &KnobtvLFOSel;
			break;
		case tvLFO2: im = &KnobtvLFO2;
			break;
		}
	if (im != nullptr)
	{
		// its ours !
		g.setOpacity(1.0);
		g.drawImage(*im, Rectangle<float>((float)x, (float)y, (float)width, (float)height));
	}
	switch (rmcSlider->knob)
	{
		case tvLOF1:
		case tvWaveFoot4:
		case tvWaveShape4:
		case tvLFOSel:
		case tvLFO2:
		{
			int mx = 32;
			int my = 28;
			int r1 = 0;
			int r2 = 16;
			int pos = (int) (3 * sliderPos+0.49);
			int alpha = angles[pos];
			int x1 = dx(r1, alpha);
			int x2 = dx(r2, alpha);
			int y1 = -dy(r1, alpha);
			int y2 = -dy(r2, alpha);
			g.setColour(Colours::white);
			g.drawLine(mx + x1, my + y1, mx + x2, my + y2, 2);
		}
		break;
		case tvKnob:
		{
			int mx = 34;
			int my = 30;
			int r1 = 13;
			int r2 = 16;
			int alpha = (int)(225 - 270 * sliderPos);
			int x1 = dx(r1, alpha);
			int x2 = dx(r2, alpha);
			int y1 = -dy(r1, alpha);
			int y2 = -dy(r2, alpha);
			g.setColour(Colours::black);
			g.drawLine(mx, my, mx + x1, my + y1, 2);
			g.setColour(Colours::white);
			g.drawLine(mx + x1, my + y1, mx + x2, my + y2, 2);
		}
		break;
	}
}


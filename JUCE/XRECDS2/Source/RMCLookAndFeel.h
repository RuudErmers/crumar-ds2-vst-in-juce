/*
  ==============================================================================

    RMCLookAndFeel.h
    Created: 16 Sep 2017 2:34:10pm
    Author:  Ruud

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "XViewItem.h"
#include "RMCResources.h"

class RMCSlider : public Slider
{
public:
	RMCSlider(const String& componentName, ERMCKnobShape aknob) : Slider(componentName),knob(aknob) {}
	ERMCKnobShape knob;
};

class RMCLookAndFeel : public LookAndFeel_V2
{
typedef LookAndFeel_V2 inherited;
public:
	RMCLookAndFeel() 
	{
		verticalKnobThumb = ImageCache::getFromMemory(RMCResources::verticalKnobThumb_png, RMCResources::verticalKnobThumb_pngSize);
		verticalSlider = ImageCache::getFromMemory(RMCResources::verticalSlider_png, RMCResources::verticalSlider_pngSize);
		horizontalKnobThumb = ImageCache::getFromMemory(RMCResources::horizontalKnobThumb_png, RMCResources::horizontalKnobThumb_pngSize);
        horizontalSlider = ImageCache::getFromMemory(RMCResources::horizontalSlider_png, RMCResources::horizontalSlider_pngSize);
		KnobtvLOF1= ImageCache::getFromMemory(RMCResources::knobtvLof1_png, RMCResources::knobtvLof1_pngSize);
        KnobtvKnob= ImageCache::getFromMemory(RMCResources::knobtvKnob_png, RMCResources::knobtvKnob_pngSize);
		KnobtvWaveFoot4 = ImageCache::getFromMemory(RMCResources::knobtvWaveFoot4_png, RMCResources::knobtvWaveFoot4_pngSize);
        KnobtvWaveShape4 = ImageCache::getFromMemory(RMCResources::knobtvWaveShape4_png, RMCResources::knobtvWaveShape4_pngSize);
        KnobtvLFOSel = ImageCache::getFromMemory(RMCResources::knobtvLfosel_png, RMCResources::knobtvLfosel_pngSize);
        KnobtvLFO2 = ImageCache::getFromMemory(RMCResources::knobtvLfo2_png, RMCResources::knobtvLfo2_pngSize);

	}
	void drawLinearSliderThumb(Graphics& g,	int x, int y,int width, int height,	float sliderPos,float minSliderPos,	float maxSliderPos,	const Slider::SliderStyle style,Slider& slider) override;
    void drawLinearSliderBackground(Graphics& g, int x, int y, int width, int height, float sliderPos, float minSliderPos, float maxSliderPos, const Slider::SliderStyle style, Slider& slider) override;
	void drawRotarySlider(Graphics& g, int x, int y, int width, int height, float sliderPos,
		const float rotaryStartAngle, const float rotaryEndAngle, Slider& slider) override;
private:
	Image verticalKnobThumb,verticalSlider,	horizontalKnobThumb, horizontalSlider,	KnobtvLOF1,	KnobtvKnob,
		  KnobtvWaveFoot4,	KnobtvWaveShape4,	KnobtkSliderMulti,	KnobtvLFOSel,	KnobtvLFO2;

};


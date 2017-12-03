/*
  ==============================================================================

    XViewItem.h
    Created: 15 Sep 2017 8:21:45pm
    Author:  Ruud

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

enum EViewItemStyle { visSlider,visRotator, visButton,visUnknown};
enum ERMCKnobShape  {trKnob, trButton, trSlider, trPitchKnob, trNone, trMidi4, trLedButton, trTextButton, trText, trLed, trTwinkle, trTextLine, tkVCOWave, tkLFOWave, tkFoot, tkNoise, tkValue, tkReserved, tkNone, tkText, tkLFO, tkSlider, tkSliderMulti, tsKnob, tsButton, tsSlider, tvKnob, tvWave, tvFoot,
	tvLFO, tvLFOSel, tvWaveFoot4, tvWaveShape4, tvLOF1, tvLFO2};
const int VIEWITEMCOUNT = 50;

class XViewItem 
{
public:
	ERMCKnobShape knob;
	int x,y,w,h,min,max,value;
	ScopedPointer<Component> control;
	XViewItem () { control = nullptr; value = max+1;}
	EViewItemStyle knobToViewStyle()
	{
       switch (knob)
		{ 
           case trKnob: case  trPitchKnob: case  tkVCOWave: case tkLFOWave: case tkFoot: case tkLFO: case  tsKnob: case   tvKnob: case tvWave: case tvFoot: case
			   tvLFO: case tvLFOSel: case tvWaveFoot4: case tvWaveShape4: case tvLOF1: case tvLFO2:  return visRotator;
           break;
		   case trSlider: case tkSlider: case tkSliderMulti: case tsSlider:  return visSlider;
			break;
		   case  trButton: case  trLedButton: case  trTextButton: case  trLed:  case  tsButton: return visButton;
		default: return visUnknown;
	   };
	}
};


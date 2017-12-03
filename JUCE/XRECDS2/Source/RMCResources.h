/*
  ==============================================================================

  This is an automatically generated GUI class created by the Projucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Projucer version: 5.1.1

  ------------------------------------------------------------------------------

  The Projucer is part of the JUCE library - "Jules' Utility Class Extensions"
  Copyright (c) 2015 - ROLI Ltd.

  ==============================================================================
*/

#pragma once

//[Headers]     -- You can add your own extra header files here --
#include "../JuceLibraryCode/JuceHeader.h"
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Projucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class RMCResources  : public Component
{
public:
    //==============================================================================
    RMCResources ();
    ~RMCResources();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;

    // Binary resources:
    static const char* verticalKnobThumb_png;
    static const int verticalKnobThumb_pngSize;
    static const char* verticalSlider_png;
    static const int verticalSlider_pngSize;
    static const char* horizontalSlider_png;
    static const int horizontalSlider_pngSize;
    static const char* knobtvKnob_png;
    static const int knobtvKnob_pngSize;
    static const char* knobtvLfo2_png;
    static const int knobtvLfo2_pngSize;
    static const char* knobtvLfosel_png;
    static const int knobtvLfosel_pngSize;
    static const char* knobtvLof1_png;
    static const int knobtvLof1_pngSize;
    static const char* knobtvWaveFoot4_png;
    static const int knobtvWaveFoot4_pngSize;
    static const char* knobtvWaveShape4_png;
    static const int knobtvWaveShape4_pngSize;
    static const char* horizontalKnobThumb_png;
    static const int horizontalKnobThumb_pngSize;


private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    //[/UserVariables]

    //==============================================================================


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RMCResources)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

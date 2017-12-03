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
#include "XSynthModel.h"
#include "XViewItem.h"
#include "RMCLookAndFeel.h"
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Projucer.

    Describe your class and how it works here!
	public  Slider::Listener,
                                                                    //[/Comments]
*/
class MainView  : public Component,
                  private Timer,
                  public Slider::Listener,
                  public ButtonListener
{
public:
    //==============================================================================
    MainView ();
    ~MainView();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
	void ModelUpdateParameter(int id, int value);
	void setModel(XSynthModel *_model) { model = _model; }
	void timerCallback() override;
	void addViewItem(int aid, ERMCKnobShape knob, int ax, int ay, int aw, int ah, int amin, int amax, int arange);
	void sliderValueChanged(Slider* sliderThatWasMoved) override;
	void buttonClicked(Button * buttonThatWasClicked);
    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;

    // Binary resources:
    static const char* background_png;
    static const int background_pngSize;


private:
    //[UserVariables]   -- You can add your own custom variables in this section.
	RMCLookAndFeel rmcLookAndFeel;
	XSynthModel *model = NULL;
	XViewItem viewItems[VIEWITEMCOUNT];
    //[/UserVariables]

    //==============================================================================
    Image cachedImage_background_png_1;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainView)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

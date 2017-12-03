/*
  ==============================================================================

   This file is part of the JUCE library.
   Copyright (c) 2017 - ROLI Ltd.

   JUCE is an open source library subject to commercial or open-source
   licensing.

   By using JUCE, you agree to the terms of both the JUCE 5 End-User License
   Agreement and JUCE 5 Privacy Policy (both updated and effective as of the
   27th April 2017).

   End User License Agreement: www.juce.com/juce-5-licence
   Privacy Policy: www.juce.com/juce-5-privacy-policy

   Or: You may also use this code under the terms of the GPL v3 (see
   www.gnu.org/licenses).

   JUCE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES, WHETHER
   EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE, ARE
   DISCLAIMED.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "XAudioParameter.h"
#include "XSynthConstants.h"

const int PROGRAMSCOUNT = 32;
class ProgramDefinition
{
public:
   int parameterValues[PARMCOUNT];
   String name;
};


//==============================================================================
/**
    As the name suggest, this class does the actual audio processing.
*/
class XSynthModule  : public AudioProcessor, XAudioOnChangeListener
{
typedef AudioProcessor inherited;
public:
    //==============================================================================
    XSynthModule();
    ~XSynthModule();
	//====== My Stuff ========================================================================
	void resendParameters();
	virtual void OnParameterChanged(int /*index*/, int /*value*/) {};
	void setValueFrom0__127(int id, int value);
	void HostParameterChanged(int id) override;
	int getXParameter(int index);
    void setXParameter(int index, int value);
    //==============================================================================
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
	void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    //==============================================================================
    void processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages) override
    {
        jassert (! isUsingDoublePrecision());
        process (buffer, midiMessages);
    }

    void processBlock (AudioBuffer<double>& buffer, MidiBuffer& midiMessages) override
    {
        jassert (isUsingDoublePrecision());
        process (buffer, midiMessages);
    }

    //==============================================================================
    bool hasEditor() const override                                             { return true; }

    //==============================================================================
    const String getName() const override                                       { return JucePlugin_Name; }

    bool acceptsMidi() const override                                           { return true; }
    bool producesMidi() const override                                          { return true; }

    double getTailLengthSeconds() const override                                { return 0.0; }

    //==============================================================================
    int getNumPrograms() override                                               { return PROGRAMSCOUNT; }
    int getCurrentProgram() override                                            { return currentProgram; }
    void setCurrentProgram (int index) override;
	void CopyParamsToProgram(int program);
	void CopyProgramToParams(int program);
    const String getProgramName (int index) override                        { return Programs[index].name; }
    void changeProgramName (int index, const String& name) override         { Programs[index].name = name; }

    //==============================================================================
    void getStateInformation (MemoryBlock&) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //==============================================================================
    // These properties are public so that our editor component can access them
    // A bit of a hacky way to do it, but it's only a demo! Obviously in your own
    // code you'll do this much more neatly..

    // this is kept up to date with the midi messages that arrive, and the UI component
    // registers with it so it can represent the incoming messages
    MidiKeyboardState keyboardState;

    // these are used to persist the UI's size - the values are stored along with the
    // filter's other parameters, and the UI component will update them when it gets
    // resized.
    int lastUIWidth = 800, lastUIHeight = 600;

    // Our parameters
protected:
	virtual void renderSynth(AudioBuffer<double>& buffer, MidiBuffer& midiMessages, int startSample, int numSamples) = 0;
	virtual void renderSynth(AudioBuffer<float>& buffer, MidiBuffer& midiMessages, int startSample, int numSamples) = 0;
	void XAddParameter(int id, String idName, String DisplayName, int min, int max, int def, String units, String extText="");
private:
    //==============================================================================
    template <typename FloatType>
    void process (AudioBuffer<FloatType>& buffer, MidiBuffer& midiMessages);
	XAudioParameter* XParams[PARMCOUNT];
	ProgramDefinition Programs[PROGRAMSCOUNT];
    int currentProgram = 0;
    static BusesProperties getBusesProperties();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (XSynthModule)
};

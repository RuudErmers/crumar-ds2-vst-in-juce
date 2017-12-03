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

#include "PluginProcessor.h"

AudioProcessor* JUCE_CALLTYPE createPluginFilter();


void XSynthModule::XAddParameter(int id, String idName,String DisplayName, int min, int max, int def, String units, String extText)
{
	XParams[id]->init(this,id, idName,DisplayName,min,max,def,units,extText);
	for (int i=0;i<PROGRAMSCOUNT;i++)
      Programs[i].parameterValues[id]=def;
}

//==============================================================================
XSynthModule::XSynthModule()
    : AudioProcessor (getBusesProperties())
{
    // This creates our parameters. We'll keep some raw pointers to them in this class,
    // so that we can easily access them later, but the base class will take care of
    // deleting them for us.
    for (int i=0;i<PARMCOUNT;i++) 
		addParameter(XParams[i]= new XAudioParameter(i));
	for (int i=0;i<PROGRAMSCOUNT;i++)
		changeProgramName(i,String::formatted("Preset %02d",i+1));
}

XSynthModule::~XSynthModule()
{
}

void XSynthModule::resendParameters()
{
	for (int i = 0; i< PARMCOUNT; i++)
		HostParameterChanged(i);
}

void XSynthModule::setValueFrom0__127(int id, int value)
{
	if (id >= 0 && id<PARMCOUNT)
		XParams[id]->setValueFrom0__127(value);
}

void XSynthModule::HostParameterChanged(int id)
{
	if (id>=0 && id<PARMCOUNT)
    {
		OnParameterChanged(id, XParams[id]->getXValue());
	}
}

int XSynthModule::getXParameter(int index)
{
    if (index>=0 && index<PARMCOUNT)
      return XParams[index]->getXValue();
	return 0;
}

void XSynthModule::setXParameter(int index, int value)
{
	if (index >= 0 && index<PARMCOUNT)
		XParams[index]->setXValue(value);
}



//==============================================================================
bool XSynthModule::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    // Only mono/stereo and input/output must have same layout
    const AudioChannelSet& mainOutput = layouts.getMainOutputChannelSet();
    const AudioChannelSet& mainInput  = layouts.getMainInputChannelSet();

    // input and output layout must either be the same or the input must be disabled altogether
    if (! mainInput.isDisabled() && mainInput != mainOutput)
        return false;

    // do not allow disabling the main buses
    if (mainOutput.isDisabled())
        return false;

    // only allow stereo and mono
    if (mainOutput.size() > 2)
        return false;

    return true;
}

void XSynthModule::prepareToPlay(double /*sampleRate*/, int /*samplesPerBlock*/)
{
	keyboardState.reset();
	reset();
}

AudioProcessor::BusesProperties XSynthModule::getBusesProperties()
{
    return BusesProperties().withInput  ("Input",  AudioChannelSet::stereo(), true)
                            .withOutput ("Output", AudioChannelSet::stereo(), true);
}

//==============================================================================


void XSynthModule::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
    keyboardState.reset();
}


template <typename FloatType>
void XSynthModule::process (AudioBuffer<FloatType>& buffer,
                                            MidiBuffer& midiMessages)
{
    const int numSamples = buffer.getNumSamples();

    // Now pass any incoming midi messages to our keyboard state object, and let it
    // add messages to the buffer if the user is clicking on the on-screen keys
    keyboardState.processNextMidiBuffer (midiMessages, 0, numSamples, true);

    // and now get our synth to process these midi events and generate its output.
	renderSynth(buffer, midiMessages, 0, numSamples);

    // In case we have more outputs than inputs, we'll clear any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    for (int i = getTotalNumInputChannels(); i < getTotalNumOutputChannels(); ++i)
        buffer.clear (i, 0, numSamples);

}

void XSynthModule::setCurrentProgram(int index)
{
	if (currentProgram == index)
		return;
    CopyParamsToProgram(currentProgram);
	currentProgram = index;
	CopyProgramToParams(currentProgram);
	updateHostDisplay();
	resendParameters();
}

void XSynthModule::CopyParamsToProgram(int program)
{
  for (int i=0;i<PARMCOUNT;i++)
	  Programs[program].parameterValues[i]= getXParameter(i);
}

void XSynthModule::CopyProgramToParams(int program)
{
	for (int i = 0; i<PARMCOUNT; i++)
		setXParameter(i,Programs[program].parameterValues[i]);
}

//==============================================================================
void XSynthModule::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // Here's an example of how you can use XML to make it easy and more robust:

	CopyParamsToProgram(currentProgram);
	// Create an outer XML element..
	XmlElement xml("XSynthModule");

    // add some attributes to it..
    xml.setAttribute ("uiWidth", lastUIWidth);
    xml.setAttribute ("uiHeight", lastUIHeight);
	XAudioParameter *q = XParams[0]; 
	String s = q->getIdName();
    // Store the values of all our parameters, using their param ID as the XML attribute

	for (int program = 0; program<PROGRAMSCOUNT; program++)
	{
		String id = String::formatted("PRGN%02d", program);
		xml.setAttribute(id, Programs[program].name);

		for (int param = 0; param<PARMCOUNT; param++)
		{
			String sid = String::formatted("PRGP%02d", program);
			sid+=XParams[param]->getIdName();
			xml.setAttribute(sid, Programs[program].parameterValues[param]);
		}
	}
    // then use this helper function to stuff it into the binary blob and return it..
    copyXmlToBinary (xml, destData);
}

void XSynthModule::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.

    // This getXmlFromBinary() helper function retrieves our XML from the binary blob..
    ScopedPointer<XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));

	if (xmlState != nullptr)
	{
		// make sure that it's actually our type of XML object..
		if (xmlState->hasTagName("XSynthModule"))
		{
			// ok, now pull out our last window size..
			lastUIWidth = jmax(xmlState->getIntAttribute("uiWidth", lastUIWidth), 800);
			lastUIHeight = jmax(xmlState->getIntAttribute("uiHeight", lastUIHeight), 600);
			for (int program = 0; program<PROGRAMSCOUNT; program++)
			{
				String id = String::formatted("PRGN%02d", program);
				String def = String::formatted("Preset %2d",program+1);
				Programs[program].name = xmlState->getStringAttribute(id, def);
				for (int param = 0; param<PARMCOUNT; param++)
				{
					String sid = String::formatted("PRGP%02d", program);
					sid += XParams[param]->getIdName();
					int &p = Programs[program].parameterValues[param];
					p = xmlState->getIntAttribute(sid, p);
				}
			}
		}
		CopyProgramToParams(currentProgram);
	}
}

//==============================================================================

/*
  ==============================================================================

    XAudioParameter.h
    Created: 13 Sep 2017 6:29:35pm
    Author:  Ruud

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"


class XAudioOnChangeListener 
{
public:
   virtual void HostParameterChanged(int id) = 0;
};

class XAudioParameter : public AudioProcessorParameter//WithID
{
public:
	/** Creates an AudioParameterInt with an ID, name, and range.
	Note that the min and max range values are inclusive.
	On creation, its value is set to the default value.
	*/
	XAudioParameter(int aid);
	void init(XAudioOnChangeListener *alistener, int aid, const String &aidName,const String& aDisplayName, int min, int max, int def, const String &aUnits, const String& extText);

	/** Destructor. */
	~XAudioParameter();

	/** Returns the parameter's current value as an integer. */
	int getXValue() const noexcept { return roundToInt(value); }
    void setXValue(int _value, bool callHost=true); 
	void setValueFrom0__127(int _value) { setValue (_value/127.0f); }
	String getIdName() const { return idName; }
	/** Provides access to the parameter's ID string. */
	const String paramID;

	/** Provides access to the parameter's name. */
	String name;

	/** Provides access to the parameter's label. */
	String label;

	/** Provides access to the parameter's category. */
	const Category category;

protected:
	void setValue(float newValue) override;
	String getText(float, int) const override;

	String getName(int maximumStringLength) const override { return name.substring(0, maximumStringLength); }
	String getLabel() const override {return label; }
	Category getCategory() const override {return category;}
private:
	//==============================================================================
	int value, defaultValue;  
	XAudioOnChangeListener *listener;
	float getValue() const override;
	int minValue, maxValue;
    int id;
    String textDef,idName;
	float getDefaultValue() const override;

	float getValueForText(const String&) const override;

	int limitRange(int) const noexcept;
	float convertTo0to1(int) const noexcept;
	int convertFrom0to1(float) const noexcept;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(XAudioParameter)
};




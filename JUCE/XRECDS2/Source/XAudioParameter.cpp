/*
  ==============================================================================

    XAudioParameter.cpp
    Created: 13 Sep 2017 6:29:35pm
    Author:  Ruud

  ==============================================================================
*/

#include "XAudioParameter.h"

XAudioParameter::XAudioParameter(int aid) : //: AudioProcessorParameterWithID(String::formatted("PARM%03d",aid),"",""),
    listener(nullptr),
	minValue(0), maxValue(127),
	value(10),
	defaultValue(10),
	id(aid),  textDef(""),
    category(AudioProcessorParameter::genericParameter)
{}

void XAudioParameter::init(XAudioOnChangeListener * alistener, int aid, const String &aidName,const String & aDisplayName, int min, int max, int def, const String & aUnits, const String & extText)
{
	listener=alistener;
	minValue=min;
    maxValue=max,
	defaultValue=value=def,
	id=aid;
    textDef=extText;
	name=aDisplayName;
	idName = aidName;
    label=aUnits;
}

XAudioParameter::~XAudioParameter() {}


void XAudioParameter::setXValue(int _value, bool callHost)
{
   int newValue = limitRange(_value);
   if (newValue!=value)
   {
      value = newValue;
      if (callHost) 
	    setValueNotifyingHost(convertTo0to1(value));
	  if (listener != nullptr)
		  listener->HostParameterChanged(id);
   }
} 

int XAudioParameter::limitRange(int v) const noexcept { return jlimit(minValue, maxValue, v); }
float XAudioParameter::convertTo0to1(int v) const noexcept { return (limitRange(v) - minValue) / (float)(maxValue - minValue); }
int XAudioParameter::convertFrom0to1(float v) const noexcept { return limitRange(roundToInt((v * (float)(maxValue - minValue)) + minValue)); }

float XAudioParameter::getValue() const { return convertTo0to1(roundToInt(value)); }
float XAudioParameter::getDefaultValue() const { return convertTo0to1(defaultValue); }
float XAudioParameter::getValueForText(const String& text) const { return convertTo0to1(text.getIntValue()); }

void XAudioParameter::setValue(float newValue) // Called from Host
{
   int inewValue = convertFrom0to1(newValue);
   if (value!=inewValue)
   {
	 value = inewValue;
     if (listener!=nullptr)
		 listener->HostParameterChanged(id);
   }
}

String XAudioParameter::getText(float avalue, int) const
{
	int n = convertFrom0to1(avalue);
    if (textDef == "")
	  return String(n);
    else if (textDef == "__LFO")
	{
		float sv = (float)(0.1*exp(0.0598f*n));
		return(String(sv).substring(0,4));
	}
	else if (textDef == "__CUTOFF")
    {
		return String(n);
    }
	else // 'Stairway;Square;Triangle;Sample+Hold'
	{
		String s = textDef;
		for (int i=0;i<s.length();i++)
        { 
			if (n == 0)
			{
				s = s.substring(i);
				int p = s.indexOfChar(';');
				if (p == -1) return s;
				return s.substring(0,p);
			}
			if (s[i]==';') n--;
        }
	}
	return "Error";
}



This is the build directory for the Juce version of the Crumar VST.
I am not an expert in JUCE, but I think you can copy this directory anywhere in your system.
It assumes Juce to be present at C:\Juce. The version used is 5.1.0.
Output will be written to C:\Midi\Vst or some subdirectory of it. That is where I keep my VSTs.

Some remarks on JUCE:

I used the sine example as a base for my development.
That's great, since you immediately have a working example.
There are a few I did not like so well.
First, there is no mechanism to update your 'model' when you change parameter in your DAW.
You do have AudioParameterInt but unfortunately, your Model can't listen to this if changed by the Host.
It seems to be a design decision, but it did not work for me.
Moreover the PluginEditor polls the parameters instead of notification. In my Delphi variant this is all
implemented in the framework.
Now, there a few other things, but overall, it is an amazing piece of software. Where do you get software
that helps you to create all important target Operating Sytems... And it works.
Debugging is a breeze with Visual Studio and Reaper. 

Some remarks on the code:

Some parts from the code were ported from Delphi. That is true for the 'Model' , the UI and the Voice processing.
I used a tool Delphi2Cpp to do some translation. Not perfect but a fine start.

I split up the work in different parts: 
the PluginProcessor does the communication with the host. It also implements parameter handling and Preset handling.
Here the famous 'process' callback is forwarded to my Model.
the Model build up the Processor and stores all data needed by the synth, like oscillator settings etc. 
It also builds up on 'Synthesizer', where the midi handling is done and where the voices are being handled.
An XSynthVoice is a voice with all capabilities of the Crumar DS2. So, two oscillators, VCF, VCA, 2x EG etc.
Note that the LFOs are the same for all voices (in sync) so these are handled in the Model itself. 
(This caused quite a headacke because the original code did not support 'global' things. Therefor the process loop 
had to be changed).

The XSynthVoice and the model use Oscillators. Ported from Delphi these are very naive oscillators, with one 
touch of extra, namely a PolyBleb. 

The LookAndFeel (very nicely implemented by JUCE!) was copied from my Delphi code and I just added a few Resources. 
In my lazyness I also copied the main background as a png, that is different from the Delphi version.


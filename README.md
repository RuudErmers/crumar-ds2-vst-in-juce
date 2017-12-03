XRECDS2...

This is set of a few examples on how I created a VST simulation for a Crumar DS2.
First off, it was not meant to sound exactly the same as a Crumar DS2, but the functionality is almost the same.
So, why this?
Firs, if you look at my website www.ermers.org, you can see I have build a large MIDI system.
One of the nice synths I use(d) is a Crumar DS2. Eh, that is, it is a stripped Crumar DS2 and I only use 
the controllers. There's also feedback with various light sources. This is a complete Total Recall solution.
Read more on that on my website. 
I always wanted to create a reproduction of the DS2 so when I got some spare time the last weeks I dedcided to go for it.

A short description.

The VST implements an almost exact copy from a Crumar DS2, omitting the Poly Ensemble (instead just being polyphonic).
It has two VCO's, two LFO's, an VCA, VCF, Noise generator and a great modulation matrix, for the day.
In the Juce directory you will find some more details on the exact implementation.
Suffices to say, that using this package, it almost comes down to creating a UI and 
do the Audio processing (I never did that). There are a few other things to handle with, like
Patchhandling in JUCE
Better algorithm for voice selection and last note priority
Parameter handling so that it is nice to see all parameter correctly in e.g. Reaper.

On that, I only run the VST in Reaper, no other DAWs. (With the rest of my MIDI system, this VST is in complete sync with my real controller:
Any change you make in the VST is visible on the synth and vice versa)

Like my other repositories, I will not update this. There are still a few things to be made, but those are pretty personal wishes.
If you have any questions on these, don't hesitate to mail me...






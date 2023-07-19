/*
  ==============================================================================

    SynthSound.h
    Created: 4 Apr 2023 4:11:45pm
    Author:  PistaPapa

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class SynthSound : public juce::SynthesiserSound
{
public:
    bool appliesToNote(int midiNoteNumber) override { return true; }

    bool appliesToChannel(int midiChannel) override { return true; }
};
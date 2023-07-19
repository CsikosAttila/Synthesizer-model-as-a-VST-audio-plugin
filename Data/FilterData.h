/*
  ==============================================================================

    FilterData.h
    Created: 17 May 2023 6:34:40pm
    Author:  Csati

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class FilterData
{
public:
    void prepareToPLay(double sampleRate, int samplesPerBlock, int numChannels);
    void process(juce::AudioBuffer<float>& buffer);
    void updateParameters(const int filterType, const float frequency, const float resonance, const float modulator = 1.0f);
    void reset();


private:
    juce::dsp::StateVariableTPTFilter<float> filter;
    bool isPrepared{ false };



};

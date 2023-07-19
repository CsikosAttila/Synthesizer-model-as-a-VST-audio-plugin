/*
  ==============================================================================

    FilterData.cpp
    Created: 17 May 2023 6:34:40pm
    Author:  Csati

  ==============================================================================
*/

#include "FilterData.h"

void FilterData::prepareToPLay(double sampleRate, int samplesPerBlock, int numChannels)
{
    filter.reset();

    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.sampleRate = sampleRate;
    spec.numChannels = numChannels;
    
    filter.prepare(spec);

    isPrepared = true;
}

void FilterData::process(juce::AudioBuffer<float>& buffer)
{
    jassert(isPrepared);

    juce::dsp::AudioBlock<float> block{ buffer };

    filter.process(juce::dsp::ProcessContextReplacing<float> { block });
}

void FilterData::updateParameters(const int filterType, const float frequency, const float resonance, const float modulator)
{
    switch (filterType)
    {
    case 0:
        filter.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
        break;
    case 1:
        filter.setType(juce::dsp::StateVariableTPTFilterType::bandpass);
        break;
    case 2:
        filter.setType(juce::dsp::StateVariableTPTFilterType::highpass);
        break;
    }

    float modFreq = frequency * modulator;
    modFreq = std::fmax(modFreq, 20.0f);//ez biztositja hogy nem lehet kisebb a modulalt toresponti frekvencia az eredetileg meghatarozott hataererteknel (nem mehet 20 ala)
    modFreq = std::fmin(modFreq, 20000.0f);//ez biztositja hogy nem lehet nagyobb a modulalt toresponti frekvencia az eredetileg meghatarozott hataererteknel (nem mehet 20000 fele)

    filter.setCutoffFrequency(modFreq);
    filter.setResonance(resonance);
}

void FilterData::reset()
{
    filter.reset();
}

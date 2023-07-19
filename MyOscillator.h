/*
  ==============================================================================

    MyOscillator.h
    Created: 28 Apr 2023 11:10:59pm
    Author:  Csati

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class MyOscillator : public juce::dsp::Oscillator<float>
{
public:

    MyOscillator();


    void my_initialise(bool isSaw, const std::function<float(float)>& function);
    void test();
    void my_process(juce::dsp::ProcessContextReplacing<float>(context));

    void my_process_v2(juce::dsp::ProcessContextReplacing<float>(context));

    void my_setFrequency(NumericType newFrequency, bool force = false);

    void make_z1_and_phase_zero();

    void setWaveType(const int choice);
    
    float my_getFrequency();

    float dpw();


private:
    std::function<float(float)> my_generator;
    juce::Array<NumericType> rampBuffer;
    juce::SmoothedValue<float> frequency{ static_cast<float> (2000.0) };
    NumericType sampleRate = 44100.0;
    juce::dsp::Phase<NumericType> phase;
    float z1{ 0.0f };
    float my_phase{ 0.0f };
};
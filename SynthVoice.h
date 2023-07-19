/*
  ==============================================================================

    SynthVoice.h
    Created: 4 Apr 2023 4:11:22pm
    Author:  PistaPapa

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "SynthSound.h"
#include "MyOscillator.h"
#include "Data/AdsrData.h"
#include "Data/FilterData.h"






class SynthVoice : public ::juce::SynthesiserVoice
{
public:
    bool canPlaySound(juce::SynthesiserSound* sound) override;
    
    void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound* sound, int currentPitchWheelPosition) override;
    
    void stopNote(float velocity, bool allowTailOff) override;

    void pitchWheelMoved(int newPitchWheelValue) override;
    
    void controllerMoved(int controllerNumber, int newControllerValue) override;

    //Az oscillatorhoz megírt prepare to play
    void prepareToPlay(double sampleRate, int samplesPerBlock, int outputChannels);

    void updateADSR(const float attack, const float decay, const float sustain, const float release);

    void updateFilter(const int filterType, const float cutoff, const float resonance);

    void updateModAdsr(const float attack, const float decay, const float sustain, const float release);

    void renderNextBlock(juce::AudioBuffer< float >& outputBuffer, int startSample, int numSamples) override;

    MyOscillator& getOscillator();

private:
    juce::AudioBuffer<float>synthBuffer;

    MyOscillator my_oscillator;
    AdsrData adsr;
    FilterData filter;
    AdsrData modAdsr;

    juce::dsp::Gain<float> gain;

    bool isPrepared{ false };

};
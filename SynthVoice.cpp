/*
  ==============================================================================

    SynthVoice.cpp
    Created: 4 Apr 2023 4:11:22pm
    Author:  PistaPapa

  ==============================================================================
*/

#include "SynthVoice.h"
#include <functional>

//float DPW_wave_function() // at kell alakitani h bevegye a spec ben adott parametereket
//{
//    float fx = 2000.0f;
//    float samplingRate = 44100.0f;
//    float N = 44100.0f;
//
//    float delta = fx / samplingRate;
//    float c = samplingRate / (4 * fx);
//
//    float phase = 0;
//    float z1 = 0;
//
//    float bphase = 0;
//    float sq = 0;
//    float dsq = 0;
//    float dpw_s[44100] = { 0 };
//
//    static unsigned int count = 0;
//
//    for (int i = 0; i < N; i++)
//    {
//        phase = fmod(phase + delta, 1);
//        bphase = 2 * phase - 1;
//        sq = bphase * bphase;
//        dsq = sq - z1;
//        z1 = sq;
//        dpw_s[i] = c * dsq;
//
//    }
//    count++;
//    if (count >= 44100)
//    {
//        count = 0;
//    }
//
//    return dpw_s[count]; // idaig jo kitermeli a megfelelo szamokat
//};

std::vector<float> DPW_wave_function(float my_freq, double my_sampleRate, int my_samplesPerBlock)
{

    float delta = my_freq / my_sampleRate;
    float c = my_sampleRate / (4 * my_freq);

    float phase = 0;
    float z1 = 0;

    float bphase = 0;
    float sq = 0;
    float dsq = 0;
    std::vector<float> dpw_s(my_samplesPerBlock);

    for (int i = 0; i < my_samplesPerBlock; i++)
    {
        phase = fmod(phase + delta, 1);
        bphase = 2 * phase - 1;
        sq = bphase * bphase;
        dsq = sq - z1;
        z1 = sq;
        dpw_s[i] = c * dsq;
    }

    return dpw_s; // idaig jo kitermeli a megfelelo szamokat
};

//int count = 0;
//
//float DPW_wave_function(float my_freq, double my_sampleRate, int my_samplesPerBlock/*ide beraktam a countot ,int count*/)
//{
//
//    float delta = my_freq / my_sampleRate;
//    float c = my_sampleRate / (4 * my_freq);
//
//    float phase = 0;
//    float z1 = 0;
//
//    float bphase = 0;
//    float sq = 0;
//    float dsq = 0;
//    std::vector<float> dpw_s(my_samplesPerBlock);
//
//    for (int i = 0; i < my_samplesPerBlock; i++)
//    {
//        phase = fmod(phase + delta, 1);
//        bphase = 2 * phase - 1;
//        sq = bphase * bphase;
//        dsq = sq - z1;
//        z1 = sq;
//        dpw_s[i] = c * dsq;
//    }
//    count++;
//    if (count >= my_samplesPerBlock)
//    {
//        count = 0;
//    };
//    return dpw_s[count];
//};

//float test2(float x)
//{
//    return x / juce::MathConstants<float>::pi;
//}

bool SynthVoice::canPlaySound(juce::SynthesiserSound* sound) 
{
    return dynamic_cast<juce::SynthesiserSound*>(sound) != nullptr;
};
void  SynthVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound* sound, int currentPitchWheelPosition)
{
    my_oscillator.my_setFrequency(juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber)); // ez adja meg a midi noteot frekinek az oscillatornak
    std::string MIDI_data = std::to_string(midiNoteNumber);
    DBG("The Note that is pressed is: " << MIDI_data << + " frequency:" + juce::String(my_oscillator.my_getFrequency()));
    
    adsr.noteOn(); /// itt aplikalja az adsrt 
    
    modAdsr.noteOn();
};
void  SynthVoice::stopNote(float velocity, bool allowTailOff) 
{
    DBG("Note is OFF");
    adsr.noteOff(); // itt kapcsolja ki az adsrt

    modAdsr.noteOff();

    //clearCurrentNote(); /////////// EZT RAKTAM IDE H AZ ADSR MENTES BEALLITASBAN IS ELHALLGASON AMIKOR ELKELL
    //my_oscillator.make_z1_and_phase_zero();

    if( !allowTailOff || !adsr.isActive()) //////////EZT VISSZA KELL RAKNI H MUKODJON AZ ADSR
    {
        clearCurrentNote();
        juce::Logger::writeToLog(juce::String("stopNote if agaban vagyunk"));
    }
    my_oscillator.make_z1_and_phase_zero();

};
void SynthVoice::pitchWheelMoved(int newPitchWheelValue)
{
    
};
void  SynthVoice::controllerMoved(int controllerNumber, int newControllerValue) 
{

};

///OSCZILLATOR KODJA////
void SynthVoice::prepareToPlay(double sampleRate, int samplesPerBlock, int outputChannels)
{
    

    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.sampleRate = sampleRate;
    spec.numChannels = outputChannels;

    my_oscillator.prepare(spec);
    juce::Logger::writeToLog(juce::String("prepare ben vagyunk"));
    
    adsr.setSampleRate(sampleRate);
    
    //my_oscillator.test();

    filter.prepareToPLay(sampleRate, samplesPerBlock, outputChannels);

    modAdsr.setSampleRate(sampleRate);

    gain.prepare(spec);

    gain.setGainLinear(0.05f);

    isPrepared = true; /// ezzel a bealitassal tudjuk elleonirni h tenyleg prepare el- e a rendszer

};

void SynthVoice::updateADSR(const float attack, const float decay, const float sustain, const float release)
{
    adsr.updateADSR(attack, decay, sustain, release);
};

MyOscillator& SynthVoice::getOscillator()
{
    return my_oscillator;
};

void  SynthVoice::renderNextBlock(juce::AudioBuffer< float >& outputBuffer, int startSample, int numSamples) 
{
    jassert(isPrepared); // ez megallitja a futast ha a isPrepared nem igaz

    if(! isVoiceActive())
    {
        return;
    }

    synthBuffer.setSize(outputBuffer.getNumChannels(),numSamples, false, false, true);

    modAdsr.applyEnvelopeToBuffer(synthBuffer, 0, /*synthBuffer.getNumSamples()*/numSamples);

    synthBuffer.clear();

    juce::dsp::AudioBlock<float> audioBlock{ synthBuffer };

    my_oscillator.my_process(juce::dsp::ProcessContextReplacing<float>(audioBlock));

    gain.process(juce::dsp::ProcessContextReplacing<float>(audioBlock));

    adsr.applyEnvelopeToBuffer(synthBuffer, 0, synthBuffer.getNumSamples()); ////// EZT VISSZA KELL RAKNI H MUKODJON AZ ADSR

    filter.process(synthBuffer);

    for(int channel = 0; channel < outputBuffer.getNumChannels(); ++channel)
    {
        outputBuffer.addFrom(channel, startSample, synthBuffer, channel, 0, numSamples);

        if(! adsr.isActive())
        {
            clearCurrentNote();
        }
    }


};

void SynthVoice::updateFilter(const int filterType, const float cutoff, const float resonance)
{
    float modulator = modAdsr.getNextSample();
    filter.updateParameters(filterType, cutoff, resonance, modulator);
};

void SynthVoice::updateModAdsr(const float attack, const float decay, const float sustain, const float release)
{
    modAdsr.updateADSR(attack, decay, sustain, release);
}
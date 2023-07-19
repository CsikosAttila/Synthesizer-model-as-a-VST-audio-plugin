/*
  ==============================================================================

    MyOscillator.cpp
    Created: 28 Apr 2023 11:10:59pm
    Author:  Csati

  ==============================================================================
*/

#include "MyOscillator.h"





MyOscillator::MyOscillator()
{
    my_generator = std::bind(&MyOscillator::dpw, this);
}

void MyOscillator::my_initialise(bool isSaw, const std::function<float(float)>& function)
{
    //my_generator = function;
    //my_generator = std::bind(&MyOscillator::dpw, this, std::placeholders::_1, std::placeholders::_2);
    
    if(isSaw)
    {
        my_generator = std::bind(&MyOscillator::dpw, this);
    }
    else
    {
        my_generator = function;
    }
    

}

float MyOscillator::dpw()
{

    float delta = frequency.getCurrentValue() / sampleRate;
    float c = sampleRate / (4 * frequency.getCurrentValue());

    //float phase = 0.0f;
    //float z1 = 0.0f;

    float bphase = 0.0f;
    float sq = 0.0f;
    float dsq = 0.0f;

    my_phase = fmod(my_phase + delta, 1);
        bphase = 2.0f * my_phase - 1.0f;
        sq = bphase * bphase;
        dsq = sq - z1;
        z1 = sq;


    return c * dsq; // idaig jo kitermeli a megfelelo szamokat

    //return p_gen < 0.0f ? -1.0f : 1.0f;
    //return std::sin(p_gen);
}

void MyOscillator::my_process_v2(juce::dsp::ProcessContextReplacing<float>(context))
{
    auto&& outBlock = context.getOutputBlock();
    auto&& inBlock = context.getInputBlock();

    auto len = outBlock.getNumSamples();
    auto numChannels = outBlock.getNumChannels();
    auto inputChannels = inBlock.getNumChannels();

    //juce::Logger::writeToLog(juce::String(sampleRate));

    size_t ch;

    for (ch = 0; ch < juce::jmin(numChannels, inputChannels); ++ch)
    {
        auto* dst = outBlock.getChannelPointer(ch);

        for (size_t i = 0; i < len; ++i)
        {
            //return;
            //dst[i] += my_generator(p.advance(freq) - juce::MathConstants<NumericType>::pi, 0.0f);
            dst[i] = my_generator(0.0f);
            //std::string data = std::to_string(dst[i]);
            //DBG(juce::String(dst[i]));
            // 
            //juce::Logger::writeToLog(juce::String(i) + "data: " + juce::String(dst[i]) + " frequency: " + juce::String(frequency.getCurrentValue()) + " phase: " + juce::String(my_phase));
            //juce::Logger::writeToLog(juce::String(dst[i]));
            //std::cout << dst[i] << "\n";
        }
    }
    ////Make z1 and phase 0
    //z1 = 0.0f;
    //my_phase = 0.0f;

}

float MyOscillator::my_getFrequency()
{
    return frequency.getCurrentValue();
}

void MyOscillator::make_z1_and_phase_zero()
{
    z1 = 0.0f;
    my_phase = 0.0f;
    juce::Logger::writeToLog("0-zzuk a fazist es z1-t\n");
}

void MyOscillator::my_process(juce::dsp::ProcessContextReplacing<float>(context))
{
//    jassert(isInitialised());
    auto&& outBlock = context.getOutputBlock();
    auto&& inBlock = context.getInputBlock();

    // this is an output-only processor
    //jassert(outBlock.getNumSamples() <= static_cast<size_t> (rampBuffer.size()));

    auto len = outBlock.getNumSamples();
    auto numChannels = outBlock.getNumChannels();
    auto inputChannels = inBlock.getNumChannels();
    auto baseIncrement = juce::MathConstants<NumericType>::twoPi / sampleRate;

    if (context.isBypassed)
        context.getOutputBlock().clear();

    if (frequency.isSmoothing())
    {
        auto* buffer = rampBuffer.getRawDataPointer();

        for (size_t i = 0; i < len; ++i)
            buffer[i] = phase.advance(baseIncrement * frequency.getNextValue())
            - juce::MathConstants<NumericType>::pi;

        if (!context.isBypassed)
        {
            size_t ch;

            if (context.usesSeparateInputAndOutputBlocks())
            {
                for (ch = 0; ch < juce::jmin(numChannels, inputChannels); ++ch)
                {
                    auto* dst = outBlock.getChannelPointer(ch);
                    auto* src = inBlock.getChannelPointer(ch);

                    for (size_t i = 0; i < len; ++i)
                        dst[i] = src[i] + my_generator(buffer[i]);
                }
            }
            else
            {
                for (ch = 0; ch < juce::jmin(numChannels, inputChannels); ++ch)
                {
                    auto* dst = outBlock.getChannelPointer(ch);

                    for (size_t i = 0; i < len; ++i)
                        dst[i] += my_generator(buffer[i]);
                }
            }

            for (; ch < numChannels; ++ch)
            {
                auto* dst = outBlock.getChannelPointer(ch);

                for (size_t i = 0; i < len; ++i)
                    dst[i] = my_generator(buffer[i]);
            }
        }
    }
    else
    {
        // Here we are!
        auto freq = baseIncrement * frequency.getNextValue();
        auto p = phase;

        if (context.isBypassed)
        {
            frequency.skip(static_cast<int> (len));
            p.advance(freq * static_cast<NumericType> (len));
        }
        else
        {
            size_t ch;

            if (context.usesSeparateInputAndOutputBlocks())
            {
                for (ch = 0; ch < juce::jmin(numChannels, inputChannels); ++ch)
                {
                    p = phase;
                    auto* dst = outBlock.getChannelPointer(ch);
                    auto* src = inBlock.getChannelPointer(ch);

                    for (size_t i = 0; i < len; ++i)
                        dst[i] = src[i] + my_generator(p.advance(freq) - juce::MathConstants<NumericType>::pi);
                }
            }
            else
            {
                // Here we are!
                for (ch = 0; ch < juce::jmin(numChannels, inputChannels); ++ch)
                {
                    p = phase;
                    auto* dst = outBlock.getChannelPointer(ch);

                    for (size_t i = 0; i < len; ++i)
                    {
                        //dst[i] += my_generator(p.advance(freq) - juce::MathConstants<NumericType>::pi, 0.0f);
                        dst[i] = my_generator(p.advance(freq) - juce::MathConstants<NumericType>::pi);
                        ///juce::Logger::writeToLog(juce::String(dst[i]));

                        if (dst[i] > 1)
                        {
                            dst[i] = 0.999f;
                        }
                    }
                }
            }

            for (; ch < numChannels; ++ch)
            {
                p = phase;
                auto* dst = outBlock.getChannelPointer(ch);

                for (size_t i = 0; i < len; ++i)
                {
                   dst[i] = my_generator(p.advance(freq) - juce::MathConstants<NumericType>::pi);
                }
            }
            //Make z1 and phase 0
            /*z1 = 0.0f;
            my_phase = 0.0f;*/
        }

        phase = p;
    }
}

void MyOscillator::my_setFrequency(NumericType newFrequency, bool force)
{
    if (force)
    {
        frequency.setCurrentAndTargetValue(newFrequency);
        return;
    }

    frequency.setTargetValue(newFrequency);
}

void MyOscillator::setWaveType(const int choice)
{
    switch (choice)
    {
    case 0: //SinWave
       my_initialise(false, [](float x) { return std::sin(x); });
       break;

    case 1: //MY_SawWave
        my_initialise(true, [](float x) { return std::sin(x);/*EZ ITT FELESLEGES DE KELL EGY FUNCTION*/ });
        break;

    case 2: //SquareWave
        my_initialise(false, [](float x) { return x < 0.0f ? -1.0f : 1.0f; });
        break;
    
    default:
        //jassertfalse;
        my_initialise(true, [](float x) { return std::sin(x);/*EZ ITT FELESLEGES DE KELL EGY FUNCTION*/ });
        break;
    }
}

void MyOscillator::test()
{
    int x = 1;
    x++;
}

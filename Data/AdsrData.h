/*
  ==============================================================================

    AdsrData.h
    Created: 9 May 2023 8:17:47pm
    Author:  Csati

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class AdsrData : public juce::ADSR
{
public:

    void AdsrData::updateADSR(const float attack, const float decay, const float sustain, const float release);

private:
    juce::ADSR::Parameters adsrParams;


};

/*
  ==============================================================================

    HRTFManager.h
    Created: 19 Nov 2025 11:12:50pm
    Author:  nicky_hgjk9m6

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class HRTFManager {
public:
    HRTFManager();
    void loadBinaryData();
    void setSampleRate(double newSampleRate);
    const double getIRSampleRate();

    juce::AudioBuffer<float>& getIR(float elevation, float azimuth, int channel);
private:
    struct IRPair {
        juce::AudioBuffer<float> left;
        juce::AudioBuffer<float> right;
    };

    // irMap[el][az] -> IR Pair
    std::map<int, std::map<int, IRPair>> irMap;
    
    double sampleRate;
    double irSampleRate = 48000.0;

    bool loadIR(const juce::String& name, juce::AudioBuffer<float>& dest);

    juce::AudioFormatManager audioFormatManager;

    void resampleBuffer(juce::AudioBuffer<float>& buffer, double srcRate, double destRate);

    juce::LagrangeInterpolator interpolator;
};
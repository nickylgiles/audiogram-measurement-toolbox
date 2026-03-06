/*
  ==============================================================================

    CalibrationFilter.h
    Created: 20 Feb 2026 2:22:14pm
    Author:  nicky_hgjk9m6

  ==============================================================================
*/

#pragma once

#include "CalibrationFilterParams.h"

class CalibrationFilter {
public:
    struct Metadata {
        juce::String calibrationId = "";
        juce::String calibrationDate = "";
        juce::String deviceModel = "";

        float targetSPL = 0.0f;
    };

    CalibrationFilter(double sampleRate, int maximumBlockSize);

    void setSampleRate(double newSampleRate);
    void loadCalibrationProfile(const juce::File& file);
    void makeFilters();

    void processBlock(juce::AudioBuffer<float>& buffer);

    const Metadata& getMetadata();

private:
    juce::dsp::IIR::Coefficients<float>::Ptr 
    makeFilter(const CalibrationFilterParams::BiquadParams& biquadParams);

    std::pair<CalibrationFilterParams, CalibrationFilterParams> leftRightParams;

    std::vector<juce::dsp::IIR::Filter<float>> filtersLeft;
    std::vector<juce::dsp::IIR::Filter<float>> filtersRight;

    double sampleRate;
    int maximumBlockSize;

    Metadata metadata;
};
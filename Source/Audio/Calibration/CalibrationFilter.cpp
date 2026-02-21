/*
  ==============================================================================

    CalibrationFilter.cpp
    Created: 20 Feb 2026 2:22:14pm
    Author:  nicky_hgjk9m6

  ==============================================================================
*/

#include "CalibrationFilter.h"

CalibrationFilter::CalibrationFilter(double sampleRate, int maximumBlockSize)
    : sampleRate(sampleRate), maximumBlockSize(maximumBlockSize) {
}

void CalibrationFilter::setSampleRate(double newSampleRate) {
    if (sampleRate == newSampleRate)
        return;

    sampleRate = newSampleRate;

    makeFilters();
}

void CalibrationFilter::loadCalibrationProfile(const juce::File& file) {
    // TODO: Load metadata from file here

    leftRightParams = CalibrationFilterParams::loadFromFile(file);

    makeFilters();
}

void CalibrationFilter::makeFilters() {
    filtersLeft.clear();
    filtersRight.clear();

    auto& leftParams = leftRightParams.first.getBiquadParams();
    auto& rightParams = leftRightParams.second.getBiquadParams();

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = maximumBlockSize;
    spec.numChannels = 1;

    for (const CalibrationFilterParams::BiquadParams& params : leftParams) {
        filtersLeft.emplace_back();
        juce::dsp::IIR::Filter<float>& filter = filtersLeft.back();

        filter.prepare(spec);
        filter.reset();
        filter.coefficients = makeFilter(params);
    }

    for (const CalibrationFilterParams::BiquadParams& params : rightParams) {
        filtersRight.emplace_back();
        juce::dsp::IIR::Filter<float>& filter = filtersRight.back();

        filter.prepare(spec);
        filter.reset();
        filter.coefficients = makeFilter(params);
    }
}

void CalibrationFilter::processBlock(juce::AudioBuffer<float>& buffer) {

}

juce::dsp::IIR::Coefficients<float>::Ptr
CalibrationFilter::makeFilter(const CalibrationFilterParams::BiquadParams& biquadParams) {
    using FilterType = CalibrationFilterParams::BiquadParams::Type;

    switch (biquadParams.type) {
        case FilterType::HP:
            return juce::dsp::IIR::Coefficients<float>::makeHighPass(
                sampleRate, biquadParams.fc, biquadParams.q);

        case FilterType::LP:
            return juce::dsp::IIR::Coefficients<float>::makeLowPass(
                sampleRate, biquadParams.fc, biquadParams.q);

        case FilterType::PK:
            return juce::dsp::IIR::Coefficients<float>::makePeakFilter(
                sampleRate, biquadParams.fc, biquadParams.q, 
                juce::Decibels::decibelsToGain(biquadParams.gain)
            );

        case FilterType::None:
        default:
            return new juce::dsp::IIR::Coefficients<float>(
                1.0f, 0.0f, 0.0f,
                1.0f, 0.0f, 0.0f
            ); // identity filter
        }
}

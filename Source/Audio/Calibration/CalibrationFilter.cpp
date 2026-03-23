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
    // Load metadata from file here
    if (!file.existsAsFile())
        return;

    juce::var json = juce::JSON::parse(file);

    if (auto* root = json.getDynamicObject()) {
        metadata.calibrationId = root->getProperty("calibration_id").toString();
        metadata.deviceModel = root->getProperty("device_name").toString();
        metadata.calibrationDate = root->getProperty("date").toString();
        metadata.targetSPL = root->getProperty("target_spl").toString().getFloatValue();
    }

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
        DBG("Filter added: fc=" << params.fc << ", q=" << params.q << ", gain=" << params.gain);
    }

    for (const CalibrationFilterParams::BiquadParams& params : rightParams) {
        filtersRight.emplace_back();
        juce::dsp::IIR::Filter<float>& filter = filtersRight.back();

        filter.prepare(spec);
        filter.reset();
        filter.coefficients = makeFilter(params);
        DBG("Filter added: fc=" << params.fc << ", q=" << params.q << ", gain=" << params.gain);
    }
}

void CalibrationFilter::processBlock(juce::AudioBuffer<float>& buffer) {
    int numSamples = buffer.getNumSamples();

    float* left = buffer.getWritePointer(0);
    float* right = buffer.getWritePointer(1);

    for (int i = 0; i < numSamples; ++i) {
        float sampleL = left[i];

        for (auto& filter : filtersLeft) {
            sampleL = filter.processSample(sampleL);
        }
        left[i] = sampleL;


        float sampleR = right[i];

        for (auto& filter : filtersRight) {
            sampleR = filter.processSample(sampleR);
        }
        right[i] = sampleR;
    }
}

const CalibrationFilter::Metadata& CalibrationFilter::getMetadata() {
    return metadata;
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

        case FilterType::HS:
            return juce::dsp::IIR::Coefficients<float>::makeHighShelf(
                sampleRate, biquadParams.fc, biquadParams.q,
                juce::Decibels::decibelsToGain(biquadParams.gain)
            );

        case FilterType::LS:
            return juce::dsp::IIR::Coefficients<float>::makeLowShelf(
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

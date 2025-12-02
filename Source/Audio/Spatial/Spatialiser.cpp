/*
  ==============================================================================

    Spatialiser.cpp
    Created: 1 Nov 2025 7:00:41pm
    Author:  nicky_hgjk9m6

  ==============================================================================
*/

#include "Spatialiser.h"
#include <JuceHeader.h>
#include <BinaryData.h>

Spatialiser::Spatialiser(HRTFManager& hrtfManagerRef)
    : hrtfManager(hrtfManagerRef),
    convolverLeft(64),
    convolverRight(64)
{
}

void Spatialiser::setSampleRate(double newSampleRate) {
    sampleRate = newSampleRate;

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = 4096;
    spec.numChannels = 1;

    firLeft.prepare(spec);
    firRight.prepare(spec);

    DBG("Convolutions prepared. Sample rate = " << sampleRate);
}

void Spatialiser::setDirection(float newElevation, float newAzimuth) {
    azimuth = newAzimuth;
    elevation = newElevation;

    // Load IRs
    juce::AudioBuffer<float> hrirL = hrtfManager.getIR(elevation, azimuth, 0);
    juce::AudioBuffer<float> hrirR = hrtfManager.getIR(elevation, azimuth, 1);

    const float* samplesL = hrirL.getReadPointer(0);
    size_t numTapsL = (size_t)hrirL.getNumSamples();

    auto coeffsL = new juce::dsp::FIR::Coefficients<float>(samplesL, numTapsL);
    firLeft.coefficients = coeffsL;
    firLeft.reset();

    const float* samplesR = hrirR.getReadPointer(0);
    size_t numTapsR = (size_t)hrirR.getNumSamples();

    auto coeffsR = new juce::dsp::FIR::Coefficients<float>(samplesR, numTapsR);
    firRight.coefficients = coeffsR;
    firRight.reset();

    
    convolverLeft.loadIR(hrirL);
    convolverRight.loadIR(hrirR);


    irLoaded = true;
}

void Spatialiser::reset() {
    return;
}

void Spatialiser::setFFTBlockSize(int newBlockSize) {
    return;
}


void Spatialiser::processBlock(const float* input, float* outputL, float* outputR, int numSamples) {
    if(!irLoaded) {
        float pan = (azimuth + 90.0f) / 180.0f;
        float gainLeft = std::cos(pan * juce::MathConstants<float>::halfPi);
        float gainRight = std::sin(pan * juce::MathConstants<float>::halfPi);

        for (int i = 0; i < numSamples; ++i) {
            outputL[i] = input[i] * gainLeft;
            outputR[i] = input[i] * gainRight;
        }
        return;
    }

    /*
    for (int i = 0; i < numSamples; ++i) {
        float inSample = input[i];

        outputL[i] = firLeft.processSample(inSample);
        outputR[i] = firRight.processSample(inSample);
    }
    */
    
    convolverLeft.processBlock(input, outputL, numSamples);
    convolverRight.processBlock(input, outputR, numSamples);
    

}


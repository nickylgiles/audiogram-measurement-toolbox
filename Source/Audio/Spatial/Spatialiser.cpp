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

Spatialiser::Spatialiser(HRTFManager& hrtfManagerRef, int fftBlockSize)
    : hrtfManager(hrtfManagerRef),
    convolverLeft(fftBlockSize),
    convolverRight(fftBlockSize),
    azimuth(0.0f), elevation(0.0f),sampleRate(0.0)
{
}

void Spatialiser::setSampleRate(double newSampleRate) {
    sampleRate = newSampleRate;
}

void Spatialiser::setDirection(float newElevation, float newAzimuth) {
    azimuth = newAzimuth;
    elevation = newElevation;

    // Load IRs
    juce::AudioBuffer<float> hrirL = hrtfManager.getIR(elevation, azimuth, 0);
    juce::AudioBuffer<float> hrirR = hrtfManager.getIR(elevation, azimuth, 1);

    convolverLeft.loadIR(hrirL);
    convolverRight.loadIR(hrirR);

    irLoaded = true;
}

void Spatialiser::reset() {
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
    
    convolverLeft.processBlock(input, outputL, numSamples);
    convolverRight.processBlock(input, outputR, numSamples);
}

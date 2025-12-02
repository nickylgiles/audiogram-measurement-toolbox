/*
  ==============================================================================

    Spatialiser.h
    Created: 1 Nov 2025 7:00:41pm
    Author:  nicky_hgjk9m6

  ==============================================================================
*/

#pragma once

#include "HRTFManager.h"
#include <JuceHeader.h>
#include "PartitionedConvolver.h"

class Spatialiser {
public:
    Spatialiser(HRTFManager& hrtfManagerRef, int fftBlockSize = 64);
    void setSampleRate(double newSampleRate);
    void setDirection(float newElevation, float newAzimuth);
    void reset();
    void processBlock(const float* input, float* outputL, float* outputR, int numSamples);
private:
    float azimuth;
    float elevation;
    double sampleRate;
    HRTFManager& hrtfManager;

    PartitionedConvolver convolverLeft;
    PartitionedConvolver convolverRight;

    bool irLoaded = false;
};
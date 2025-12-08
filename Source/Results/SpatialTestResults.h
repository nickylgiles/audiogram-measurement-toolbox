/*
  ==============================================================================

    SpatialTestResults.h
    Created: 9 Nov 2025 6:10:43pm
    Author:  nicky_hgjk9m6

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

struct SpatialTestResponse {
    float referenceAzimuth;
    float targetAzimuth;
    float snr;
    bool spatialCorrect;
};

struct SpatialTestResults {
    std::vector<SpatialTestResponse> responses;
};

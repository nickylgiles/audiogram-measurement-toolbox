/*
  ==============================================================================

    DualTaskTestResults.h
    Created: 4 Dec 2025 12:31:31pm
    Author:  nicky_hgjk9m6

  ==============================================================================
*/

#pragma once

#include "SpatialTestResults.h"
#include "SpeechInNoiseTestResults.h"

struct DualTaskTestResponse {
    SpeechInNoiseTestResponse wordTestResponse;
    SpatialTestResponse spatialTestResponse;
};

struct DualTaskTestResults {
    std::vector<DualTaskTestResponse> responses;
};
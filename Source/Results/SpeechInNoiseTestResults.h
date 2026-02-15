/*
  ==============================================================================

    SpeechInNoiseTestResults.h
    Created: 4 Dec 2025 12:28:31pm
    Author:  nicky_hgjk9m6

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>


struct SpeechInNoiseTestResponse {
    juce::String targetWord;
    juce::String reportedWord;
    float snr = 0.0f;
    bool wordCorrect;
};

struct SpeechInNoiseTestResults {
    std::vector<SpeechInNoiseTestResponse> responses;
    float srt;
};
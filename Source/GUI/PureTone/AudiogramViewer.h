/*
  ==============================================================================

    AudiogramViewer.h
    Created: 14 Feb 2026 11:30:49am
    Author:  nicky_hgjk9m6

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../../Results/PureToneTestResults.h"

class AudiogramViewer : public juce::Component {
public:
    AudiogramViewer() = default;
    ~AudiogramViewer() noexcept override = default;

    void setResults(const PureToneTestResults& newResults);

    void paint(juce::Graphics& g) override;

    juce::Image createImage(int width, int height);

private:
    void drawAudiogram(juce::Graphics& g, juce::Rectangle<int> bounds);

    void drawDataPoints(juce::Graphics& g, juce::Rectangle<int> bounds, float minF, float maxF);

    void drawGrid(juce::Graphics& g, juce::Rectangle<int> bounds, float minFdB, float maxFdB);

    void drawAxes(juce::Graphics& g, juce::Rectangle<int> bounds, float minFdB, float maxFdB);

    void drawO(juce::Graphics& g, juce::Point<float> p, float diameter);
    void drawX(juce::Graphics& g, juce::Point<float> p, float length);

    void drawLegend(juce::Graphics& g, juce::Rectangle<int> bounds);

    PureToneTestResults results;
};
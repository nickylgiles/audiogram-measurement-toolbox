/*
  ==============================================================================

    PureToneResultsScreen.h
    Created: 4 Nov 2025 7:24:41pm
    Author:  nicky_hgjk9m6

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "../../Results/PureToneTestResults.h" 

class PureToneResultsScreen : public juce::Component {
public:
    PureToneResultsScreen();
    ~PureToneResultsScreen() override = default;

    void setResults(const PureToneTestResults& newResults);

    // Callbacks set in MainComponent
    std::function<void()> onExportClicked;
    std::function<void()> onMenuClicked;

    void resized() override;
    void paint(juce::Graphics& g) override;

private:
    void drawO(juce::Graphics& g, juce::Point<float> p, float diameter);
    void drawX(juce::Graphics& g, juce::Point<float> p, float length);

    void drawAudiogram(juce::Graphics& g, juce::Rectangle<int> bounds);

    juce::TextButton exportButton{ juce::translate("Export Results") };
    juce::TextButton menuButton{ juce::translate("Back to menu") };

    PureToneTestResults results;
    std::vector<float> frequencies;
    std::vector<float> dbLevels;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PureToneResultsScreen)
};
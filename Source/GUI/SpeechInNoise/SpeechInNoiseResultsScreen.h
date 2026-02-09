/*
  ==============================================================================

    SpeechInNoiseResultsScreen.h
    Created: 29 Nov 2025 1:25:21am
    Author:  nicky_hgjk9m6

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "../../Results/SpeechInNoiseTestResults.h"

class SpeechInNoiseResultsScreen : public juce::Component {
public:
    SpeechInNoiseResultsScreen();
    ~SpeechInNoiseResultsScreen() override = default;

    void setResults(const SpeechInNoiseTestResults& newResults);

    // Callbacks set in MainComponent
    std::function<void()> onExportClicked;
    std::function<void()> onMenuClicked;

    void resized() override;
    void paint(juce::Graphics& g) override;

private:
    juce::TextButton exportButton{ juce::translate("Export Results") };
    juce::TextButton menuButton{ juce::translate("Back to menu") };

    SpeechInNoiseTestResults results;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SpeechInNoiseResultsScreen)
};
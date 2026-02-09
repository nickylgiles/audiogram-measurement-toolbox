/*
  ==============================================================================

    DualTaskResultsScreen.h
    Created: 4 Dec 2025 12:29:16pm
    Author:  nicky_hgjk9m6

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "../../Results/DualTaskTestResults.h"

class DualTaskResultsScreen : public juce::Component {
public:
    DualTaskResultsScreen();
    ~DualTaskResultsScreen() override = default;

    void setResults(const DualTaskTestResults& newResults);

    // Callbacks set in MainComponent
    std::function<void()> onExportClicked;
    std::function<void()> onMenuClicked;

    void resized() override;
    void paint(juce::Graphics& g) override;

private:
    juce::TextButton exportButton{ juce::translate("Export Results") };
    juce::TextButton menuButton{ juce::translate("Back to menu") };

    DualTaskTestResults results;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DualTaskResultsScreen)
};
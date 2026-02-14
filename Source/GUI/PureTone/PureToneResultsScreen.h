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
#include "AudiogramViewer.h"

class PureToneResultsScreen : public juce::Component {
public:
    PureToneResultsScreen();
    ~PureToneResultsScreen() override = default;

    void setResults(const PureToneTestResults& newResults);

    // Callbacks set in MainComponent
    std::function<void()> onExportClicked;
    std::function<void()> onMenuClicked;

    void resized() override;

private:
    AudiogramViewer audiogram;

    juce::TextButton exportButton{ juce::translate("Export Results") };
    juce::TextButton menuButton{ juce::translate("Back to menu") };

    PureToneTestResults results;
    std::vector<float> frequencies;
    std::vector<float> dbLevels;

    std::unique_ptr<juce::FileChooser> fileChooser;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PureToneResultsScreen)
};
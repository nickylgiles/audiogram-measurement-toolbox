/*
  ==============================================================================

    DualTaskResultsScreen.cpp
    Created: 4 Dec 2025 12:29:16pm
    Author:  nicky_hgjk9m6

  ==============================================================================
*/

#include "DualTaskResultsScreen.h"

DualTaskResultsScreen::DualTaskResultsScreen() {
    addAndMakeVisible(exportButton);
    exportButton.onClick = [this] {if (onExportClicked) onExportClicked(); };

    addAndMakeVisible(menuButton);
    menuButton.onClick = [this] {if (onMenuClicked) onMenuClicked(); };
}

void DualTaskResultsScreen::setResults(const DualTaskTestResults& newResults) {
    results = newResults;
    resized();
}

void DualTaskResultsScreen::resized() {
    auto area = getLocalBounds().reduced(20);
    auto buttonHeight = area.getHeight() / 4;
    area.removeFromTop(buttonHeight * 2);
    exportButton.setBounds(area.removeFromTop(buttonHeight).reduced(10));
    menuButton.setBounds(area.reduced(10));
}

void DualTaskResultsScreen::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::white);
    g.setColour(juce::Colours::black);

    auto bounds = getLocalBounds().reduced(20);
    bounds.removeFromBottom(bounds.getHeight() / 2 + 30);

    size_t total = results.responses.size();

    int spatialCorrect = 0;
    int wordsCorrect = 0;

    for (auto& r : results.responses) {
        if (r.spatialTestResponse.spatialCorrect)
            ++spatialCorrect;
        if (r.wordTestResponse.wordCorrect)
            ++wordsCorrect;
    }

    float percentSpatialCorrect = (total > 0 ? (100.0f * spatialCorrect / total) : 0.0f);
    float percentWordsCorrect = (total > 0 ? (100.0f * wordsCorrect / total) : 0.0f);

    juce::String text;
    text << juce::translate("Spatial") << ": " << spatialCorrect << " / " << total << " " << juce::translate("correct") << " (" << percentSpatialCorrect << "%)";
    text << "\n" << juce::translate("Speech") << ": " << wordsCorrect << " / " << total << juce::translate("correct") << " (" << percentWordsCorrect << "%)";

    g.drawText(text, bounds, juce::Justification::centred);
}

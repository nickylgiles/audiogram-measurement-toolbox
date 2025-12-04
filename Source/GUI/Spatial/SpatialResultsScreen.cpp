/*
  ==============================================================================

    SpatialResultsScreen.cpp
    Created: 26 Nov 2025 11:40:54pm
    Author:  nicky_hgjk9m6

  ==============================================================================
*/

#include "SpatialResultsScreen.h"

SpatialResultsScreen::SpatialResultsScreen() {
    addAndMakeVisible(exportButton);
    exportButton.onClick = [this] {if (onExportClicked) onExportClicked(); };

    addAndMakeVisible(menuButton);
    menuButton.onClick = [this] {if (onMenuClicked) onMenuClicked(); };
}

void SpatialResultsScreen::setResults(const SpatialTestResults& newResults) {
    results = newResults;
    resized();
}

void SpatialResultsScreen::resized() {
    auto area = getLocalBounds().reduced(20);
    auto buttonHeight = area.getHeight() / 4;
    area.removeFromTop(buttonHeight * 2);
    exportButton.setBounds(area.removeFromTop(buttonHeight).reduced(10));
    menuButton.setBounds(area.reduced(10));
}

void SpatialResultsScreen::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::white);
    g.setColour(juce::Colours::black);
    
    auto bounds = getLocalBounds().reduced(20);
    bounds.removeFromBottom(bounds.getHeight() / 2 + 30);

    int total = results.responses.size();
    int correct = 0;
    for (auto& r : results.responses) {
        if (r.spatialCorrect)
            ++correct;
    }

    float percentCorrect = (total > 0 ? (100.0f * correct / total) : 0.0f);
    juce::String text;
    text << "Correct: " << correct << "/" << total;
    g.drawText(text, bounds, juce::Justification::centred);
}

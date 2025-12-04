/*
  ==============================================================================

    SpeechInNoiseResultsScreen.cpp
    Created: 29 Nov 2025 1:25:21am
    Author:  nicky_hgjk9m6

  ==============================================================================
*/

#include "SpeechInNoiseResultsScreen.h"

SpeechInNoiseResultsScreen::SpeechInNoiseResultsScreen() {
    addAndMakeVisible(exportButton);
    exportButton.onClick = [this] { if (onExportClicked) onExportClicked(); };

    addAndMakeVisible(menuButton);
    menuButton.onClick = [this] { if (onMenuClicked) onMenuClicked(); };
}

void SpeechInNoiseResultsScreen::setResults(const SpeechInNoiseTestResults& newResults) {
    results = newResults;
    resized();
}

void SpeechInNoiseResultsScreen::resized() {
    auto area = getLocalBounds().reduced(20);
    auto buttonHeight = area.getHeight() / 4;
    area.removeFromTop(buttonHeight * 2);
    exportButton.setBounds(area.removeFromTop(buttonHeight).reduced(10));
    menuButton.setBounds(area.reduced(10));
}

void SpeechInNoiseResultsScreen::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::white);
    g.setColour(juce::Colours::black);

    auto bounds = getLocalBounds().reduced(20);
    bounds.removeFromBottom(bounds.getHeight() / 2 + 30);

    int total = results.responses.size();
    int correct = 0;
    for (auto& r : results.responses) {
        if (r.wordCorrect)
            ++correct;
    }

    float percentCorrect = (total > 0 ? (100.0f * correct / total) : 0.0f);
    juce::String scoreText;
    scoreText << "Correct: " << correct << "/" << total << "\n"
        << "SRT: " << juce::String(results.srt, 2) <<" dB";

    g.drawText(scoreText, bounds, juce::Justification::centred);


}

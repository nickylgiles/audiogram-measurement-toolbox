/*
  ==============================================================================

    PureToneResultsScreen.cpp
    Created: 4 Nov 2025 7:24:41pm
    Author:  nicky_hgjk9m6

  ==============================================================================
*/

#include "PureToneResultsScreen.h"

PureToneResultsScreen::PureToneResultsScreen() {
    addAndMakeVisible(exportButton);
    exportButton.onClick = [this] {if (onExportClicked) onExportClicked(); };

    addAndMakeVisible(menuButton);
    menuButton.onClick = [this] {if (onMenuClicked) onMenuClicked(); };

    addAndMakeVisible(audiogram);

    onExportClicked = [this] {
        fileChooser = std::make_unique<juce::FileChooser>("Save Audiogram as PNG",
            juce::File::getSpecialLocation(
                juce::File::userDocumentsDirectory),
            "*.png");

        auto fileChooserFlags = juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::canSelectFiles;

        fileChooser->launchAsync(fileChooserFlags, [this](const juce::FileChooser& fc) {
            auto result = fc.getResult();
            if (result != juce::File{}) {
                
                juce::PNGImageFormat png;
                if (auto stream = result.createOutputStream())
                    png.writeImageToStream(audiogram.createImage(1200, 800), *stream);
            }
        });
    };
}

void PureToneResultsScreen::setResults(const PureToneTestResults& newResults) {
    results = newResults;
    audiogram.setResults(newResults);
}

void PureToneResultsScreen::resized() {
    auto area = getLocalBounds().reduced(20);
    auto buttonHeight = area.getHeight() / 6;

    auto audiogramArea = area.removeFromTop(buttonHeight * 4);
    audiogram.setBounds(audiogramArea);

    exportButton.setBounds(area.removeFromTop(buttonHeight).reduced(10));
    menuButton.setBounds(area.reduced(10));
}

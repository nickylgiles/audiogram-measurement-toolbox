/*
  ==============================================================================

    SettingsScreen.cpp
    Created: 11 Dec 2025 4:43:28am
    Author:  nicky_hgjk9m6

  ==============================================================================
*/

#include "SettingsScreen.h"

SettingsScreen::SettingsScreen() {
    addAndMakeVisible(backButton);
    backButton.onClick = [this] {if (onBackClicked) onBackClicked(); };

    addAndMakeVisible(exportButton);
    exportButton.onClick = [this] {if (onExportClicked) onExportClicked(); };

    addAndMakeVisible(chooseWordGroupsJsonButton);
    chooseWordGroupsJsonButton.onClick = [this] {if (onChooseWordGroupsJsonClicked) onChooseWordGroupsJsonClicked(); };
    
}

void SettingsScreen::resized() {
    auto area = getLocalBounds().reduced(40);
    auto buttonHeight = area.getHeight() / 3;

    exportButton.setBounds(area.removeFromTop(buttonHeight).reduced(10));
    chooseWordGroupsJsonButton.setBounds(area.removeFromTop(buttonHeight).reduced(10));
    backButton.setBounds(area.reduced(10));
}

void SettingsScreen::setWordGroupsJsonPath(juce::String path) {
    chooseWordGroupsJsonButton.setButtonText("Select word groups JSON file\n(" + ( path=="" ? "none" : path ) + ")");
}

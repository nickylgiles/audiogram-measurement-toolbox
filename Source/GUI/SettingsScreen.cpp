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
}

void SettingsScreen::resized() {
    auto area = getLocalBounds().reduced(40);
    auto buttonHeight = area.getHeight() / 2;

    exportButton.setBounds(area.removeFromTop(buttonHeight).reduced(10));
    backButton.setBounds(area.reduced(10));
}

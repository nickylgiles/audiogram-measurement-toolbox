/*
  ==============================================================================

    SettingsScreen.cpp
    Created: 11 Dec 2025 4:43:28am
    Author:  nicky_hgjk9m6

  ==============================================================================
*/

#include "SettingsScreen.h"

SettingsScreen::SettingsScreen() {
    listModel = std::make_unique<SettingsListModel>(this, settingsListBox);
    settingsListBox.setModel(listModel.get());
    settingsListBox.setRowHeight(50);
    settingsListBox.setColour(juce::ListBox::backgroundColourId,
        getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    addAndMakeVisible(settingsListBox);

    addAndMakeVisible(backButton);
    backButton.onClick = [this] {if (onBackClicked) onBackClicked(); };

}

void SettingsScreen::resized() {

    backButton.setBounds(10, 10, 100, 40);

    auto area = getLocalBounds().reduced(40);
    area.removeFromTop(60);

    settingsListBox.setBounds(area);
}

void SettingsScreen::addSetting(const juce::String& settingName, std::function<void()> onSettingClicked) {
    settings.push_back({ settingName, onSettingClicked });
    settingsListBox.updateContent();
}

void SettingsScreen::renameSetting(int index, const juce::String& settingName) {
    if (index < 0 || index >= settings.size())
        return;

    settings[index].first = settingName;
}

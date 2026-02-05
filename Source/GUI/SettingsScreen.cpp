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

    for (auto& s : settings) {
        if (s.type == Setting::Type::TextInput && s.editorComponent != nullptr) {
            s.editorComponent->setBounds(160, 5, settingsListBox.getWidth() - 170, 40);
            s.editorComponent->setFont(18.0f);
            s.editorComponent->setJustification(juce::Justification::centredLeft);
        }
    }
}

void SettingsScreen::paint(juce::Graphics& g) {
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    g.setColour(getLookAndFeel().findColour(juce::Label::textColourId));
    g.setFont(juce::Font(40.0f, juce::Font::bold));
    g.drawText("Settings", getLocalBounds().removeFromTop(120),
        juce::Justification::centred, true);
}

size_t SettingsScreen::addTextSetting(const juce::String& settingName, std::function<void(const juce::String&)> onTextChanged, const juce::String& defaultValue) {
    Setting setting;
    setting.type = Setting::Type::TextInput;
    setting.name = settingName;
    setting.textValue = defaultValue;
    setting.onTextChanged = onTextChanged;

    settings.push_back(setting);
    settingsListBox.updateContent();

    return settings.size() - 1;
}

size_t SettingsScreen::addTitleSetting(const juce::String& title) {
    Setting setting;
    setting.type = Setting::Type::Title;
    setting.name = title;

    settings.push_back(setting);
    settingsListBox.updateContent();

    return settings.size() - 1;
}

size_t SettingsScreen::addButtonSetting(const juce::String& settingName, std::function<void()> onSettingClicked) {
    Setting setting;
    setting.type = Setting::Type::Button;
    setting.name = settingName;
    setting.onClick = onSettingClicked;

    settings.push_back(setting);
    settingsListBox.updateContent();

    return settings.size() - 1;
}


/*
  ==============================================================================

    MenuScreen.cpp
    Created: 4 Nov 2025 7:25:01pm
    Author:  nicky_hgjk9m6

  ==============================================================================
*/

#include "MenuScreen.h"

MenuScreen::MenuScreen() {
    listModel = std::make_unique<MenuListModel>(this, testListBox);
    testListBox.setModel(listModel.get());
    testListBox.setRowHeight(50);
    testListBox.setColour(juce::ListBox::backgroundColourId,
        getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    
    addAndMakeVisible(testListBox);

    addAndMakeVisible(settingsButton);
    settingsButton.onClick = [this] {if (onSettingsClicked) onSettingsClicked(); };

    userIdLabel.setJustificationType(juce::Justification::centredRight);
    addAndMakeVisible(userIdLabel);
}

void MenuScreen::addTest(const juce::String& name, std::function<void()> onTestPressed) {
    tests.push_back({ name, onTestPressed });
    testListBox.updateContent();
}

void MenuScreen::setUserId(const juce::String& userId) {
    if (userId == "") {
        userIdLabel.setText(juce::translate("User ID not set."), juce::dontSendNotification);
    }
    else {
        userIdLabel.setText(juce::translate("User ID: ") + userId, juce::dontSendNotification);
    }
}

void MenuScreen::resized() {
    settingsButton.setBounds(getWidth() - 110, 10, 100, 40);

    auto area = getLocalBounds().reduced(40);
    area.removeFromTop(60);

    testListBox.setBounds(area);

    userIdLabel.setBounds(0, getHeight() - 40, getWidth(), 40);
}

void MenuScreen::paint(juce::Graphics& g) {
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    g.setColour(getLookAndFeel().findColour(juce::Label::textColourId));
    g.setFont(juce::Font(40.0f, juce::Font::bold));
    g.drawText(juce::translate("Select Test") + juce::String(":"), getLocalBounds().removeFromTop(120),
        juce::Justification::centred, true);
}


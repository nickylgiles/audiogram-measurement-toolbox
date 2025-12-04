/*
  ==============================================================================

    SpeechInNoiseTestScreen.cpp
    Created: 29 Nov 2025 1:25:06am
    Author:  nicky_hgjk9m6

  ==============================================================================
*/

#include "SpeechInNoiseTestScreen.h"

SpeechInNoiseTestScreen::SpeechInNoiseTestScreen() {
    addAndMakeVisible(stopButton);
    stopButton.onClick = [this] {if (onStopClicked) onStopClicked(); };

    for (int i = 0; i < 10; ++i) {
        auto button = std::make_unique<juce::TextButton>(juce::String(i));
        addAndMakeVisible(*button);
        button->onClick = [this, i]() { if (onDigitClicked) onDigitClicked(i);};

        digitButtons.push_back(std::move(button));
    }
}

void SpeechInNoiseTestScreen::setDigitsEnabled(bool enable) {
    digitsEnabled = enable;
    for (auto& b : digitButtons)
        b->setVisible(enable);
    resized();
}

void SpeechInNoiseTestScreen::resized() {
    auto area = getLocalBounds().reduced(10);
    auto buttonWidth = area.getWidth() / 3;
    auto buttonHeight = area.getHeight() / 5;

    if (digitsEnabled) {
        int x = 0, y = 0;
        for (int i = 1; i < digitButtons.size(); ++i) {
            digitButtons[i]->setBounds(x * buttonWidth, y * buttonHeight, buttonWidth, buttonHeight);
            ++x;
            if (x > 2) {
                x = 0;
                y++;
            }
        }
        digitButtons[0]->setBounds(buttonWidth, 3 * buttonHeight, buttonWidth, buttonHeight);
    }

    stopButton.setBounds(0, 4 * buttonHeight, area.getWidth(), buttonHeight);
}

void SpeechInNoiseTestScreen::paint(juce::Graphics& g) {
}

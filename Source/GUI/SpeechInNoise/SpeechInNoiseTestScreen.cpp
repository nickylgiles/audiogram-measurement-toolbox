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

    setWantsKeyboardFocus(true);
}

void SpeechInNoiseTestScreen::setDigitsEnabled(bool enable) {
    digitsEnabled = enable;
    for (auto& b : digitButtons)
        b->setVisible(enable);
    resized();
}

void SpeechInNoiseTestScreen::resized() {
    auto area = getLocalBounds().reduced(10);
    area.removeFromTop(40);

    auto buttonWidth = area.getWidth() / 3;
    auto buttonHeight = area.getHeight() / 4;

    if (digitsEnabled) {
        int x = 0, y = 0;
        for (int i = 1; i < digitButtons.size(); ++i) {
            digitButtons[i]->setBounds(juce::Rectangle(area.getX() + x * buttonWidth, area.getY() + y * buttonHeight, buttonWidth, buttonHeight)
                .reduced(5));

            ++x;
            if (x > 2) {
                x = 0;
                y++;
            }
        }
        digitButtons[0]->setBounds(juce::Rectangle(area.getX() + buttonWidth, area.getY() + 3 * buttonHeight, buttonWidth, buttonHeight)
            .reduced(5));
    }

    stopButton.setBounds(10, 10, 100, 40);
}

void SpeechInNoiseTestScreen::paint(juce::Graphics& g) {
    
    if (!hasKeyboardFocus(false)) {
        grabKeyboardFocus();
    }
}

bool SpeechInNoiseTestScreen::keyPressed(const juce::KeyPress& key) {
    if (key == juce::KeyPress::escapeKey) {
        if (onStopClicked)
            onStopClicked();
        return true;
    }
    
    else if (std::isdigit(key.getTextCharacter())) {
        int digit = key.getTextCharacter() - '0';
        if (onDigitClicked)
            onDigitClicked(digit);
        return true;
    }

    return false;
}

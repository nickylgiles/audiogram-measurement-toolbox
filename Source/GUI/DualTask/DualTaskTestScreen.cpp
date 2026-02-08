/*
  ==============================================================================

    DualTaskTestScreen.cpp
    Created: 4 Dec 2025 12:29:06pm
    Author:  nicky_hgjk9m6

  ==============================================================================
*/

#include "DualTaskTestScreen.h"

DualTaskTestScreen::DualTaskTestScreen() {
    addAndMakeVisible(leftButton);
    leftButton.onClick = [this] {if (onLeftClicked) onLeftClicked(); };

    addAndMakeVisible(rightButton);
    rightButton.onClick = [this] {if (onRightClicked) onRightClicked(); };

    addAndMakeVisible(stopButton);
    stopButton.onClick = [this] {if (onStopClicked) onStopClicked(); };
}

void DualTaskTestScreen::setInputEnabled(bool enable) {
    inputEnabled = enable;

    for (int i = 0; i < wordButtons.size(); ++i) {
        wordButtons[i]->setVisible(inputEnabled);
    }

    leftButton.setVisible(inputEnabled);
    rightButton.setVisible(inputEnabled);

    resized();
}

void DualTaskTestScreen::setWords(const std::vector<juce::String>& newWords) {
    words = newWords;
    addWordButtons();
    resized();
}

void DualTaskTestScreen::resized() {
    auto area = getLocalBounds().reduced(20);
    area.removeFromTop(40);

    auto buttonWidth = area.getWidth() / 2;
    auto buttonHeight = area.getHeight() / 4;

    if (inputEnabled) {
        int x = 0, y = 0;
        for (int i = 0; i < wordButtons.size(); ++i) {
            wordButtons[i]->setBounds(
                juce::Rectangle(area.getX() + x * buttonWidth, area.getY() + y * buttonHeight, buttonWidth, buttonHeight)
                .reduced(5)
            );
            ++x;
            if (x > 1) {
                x = 0;
                y++;
            }
        }

        area.removeFromTop(buttonHeight * 3);

        leftButton.setBounds(area.removeFromLeft(area.getWidth() / 2)
            .reduced(5));

        rightButton.setBounds(area
            .reduced(5));
    }

    stopButton.setBounds(10, 10, 100, 40);
}

void DualTaskTestScreen::paint(juce::Graphics& g) {
}

void DualTaskTestScreen::addWordButtons() {
    wordButtons.clear();
    for (int i = 0; i < words.size(); ++i) {
        auto button = std::make_unique<juce::TextButton>(words[i]);
        addAndMakeVisible(*button);
        button->onClick = [this, i]() {
            if (onWordClicked) {
                DBG("Word " << i << " clicked");
                onWordClicked(i); 
            }
            };

        wordButtons.push_back(std::move(button));
    }
}

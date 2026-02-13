/*
  ==============================================================================

    SpatialTestScreen.cpp
    Created: 26 Nov 2025 11:40:40pm
    Author:  nicky_hgjk9m6

  ==============================================================================
*/

#include "SpatialTestScreen.h"

SpatialTestScreen::SpatialTestScreen() {
    addAndMakeVisible(leftButton);
    leftButton.onClick = [this] {if (onLeftClicked) onLeftClicked(); };

    addAndMakeVisible(rightButton);
    rightButton.onClick = [this] {if (onRightClicked) onRightClicked(); };

    addAndMakeVisible(stopButton);
    stopButton.onClick = [this] {if (onStopClicked) onStopClicked(); };

    setWantsKeyboardFocus(true);
}

void SpatialTestScreen::resized() {
    auto area = getLocalBounds().reduced(20);
    auto buttonHeight = area.getHeight() / 3;

    auto bottomArea = area.removeFromBottom(buttonHeight).reduced(10);

    stopButton.setBounds(10, 10, 100, 40);

    leftButton.setBounds(bottomArea.removeFromLeft(bottomArea.getWidth() / 2).reduced(10));
    rightButton.setBounds(bottomArea.reduced(10));
    
}

void SpatialTestScreen::paint(juce::Graphics& ) {

    if (!hasKeyboardFocus(false)) {
        grabKeyboardFocus();
    }
}

bool SpatialTestScreen::keyPressed(const juce::KeyPress& key) {
    if (key == juce::KeyPress::escapeKey) {
        if (onStopClicked)
            onStopClicked();
        return true;
    }

    return false;
}
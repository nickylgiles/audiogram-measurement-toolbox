/*
  ==============================================================================

    PureToneTestScreen.cpp
    Created: 4 Nov 2025 7:24:17pm
    Author:  nicky_hgjk9m6

  ==============================================================================
*/

#include "PureToneTestScreen.h"

PureToneTestScreen::PureToneTestScreen() {
    addAndMakeVisible(hearButton);
    hearButton.onClick = [this] {if (onHearClicked) onHearClicked(); };

    addAndMakeVisible(stopButton);
    stopButton.onClick = [this] {if (onStopClicked) onStopClicked(); };

    instructionLabel.setText(juce::translate(
        "Press the button below when you hear the tone.  If you do not hear "
        "anything, wait and do not press the button."),
        juce::dontSendNotification
    );
    addAndMakeVisible(instructionLabel);

    setWantsKeyboardFocus(true);

}

void PureToneTestScreen::resized() {
    auto area = getLocalBounds().reduced(40);
    auto buttonHeight = area.getHeight() / 3;

    stopButton.setBounds(10, 10, 100, 40);
    
    hearButton.setBounds(area.removeFromBottom(buttonHeight));

    instructionLabel.setBounds(area);
    
}

void PureToneTestScreen::paint(juce::Graphics& g) {
    if (!hasKeyboardFocus(false)) {
        grabKeyboardFocus();
    }
}

bool PureToneTestScreen::keyPressed(const juce::KeyPress& key) {
    if (key == juce::KeyPress::escapeKey) {
        if (onStopClicked)
            onStopClicked();
        return true;
    }

    else if (key == juce::KeyPress::spaceKey) {
        if (onHearClicked)
            onHearClicked();
        return true;
    }


    return false;
}
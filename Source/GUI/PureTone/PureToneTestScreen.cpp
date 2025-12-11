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
}

void PureToneTestScreen::resized() {
    auto area = getLocalBounds().reduced(40);
    auto buttonHeight = area.getHeight() / 2;

    hearButton.setBounds(area.removeFromTop(buttonHeight).reduced(10));
    stopButton.setBounds(area.reduced(10));
}

void PureToneTestScreen::paint(juce::Graphics& g) {
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}


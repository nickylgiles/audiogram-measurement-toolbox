/*
  ==============================================================================

    MenuScreen.cpp
    Created: 4 Nov 2025 7:25:01pm
    Author:  nicky_hgjk9m6

  ==============================================================================
*/

#include "MenuScreen.h"

MenuScreen::MenuScreen() {
    addAndMakeVisible(pureToneButton);
    pureToneButton.onClick = [this] {if (onPureToneClicked) onPureToneClicked(); };

    addAndMakeVisible(spatialButton);
    spatialButton.onClick = [this] {if (onSpatialClicked) onSpatialClicked(); };

    addAndMakeVisible(digitsInNoiseButton);
    digitsInNoiseButton.onClick = [this] {if (onDigitsInNoiseClicked) onDigitsInNoiseClicked(); };

    addAndMakeVisible(dualTaskButton);
    dualTaskButton.onClick = [this] {if (onDualTaskClicked) onDualTaskClicked(); };


    addAndMakeVisible(settingsButton);
    settingsButton.onClick = [this] {if (onSettingsClicked) onSettingsClicked(); };
}

void MenuScreen::resized() {
    auto area = getLocalBounds().reduced(40);
    auto buttonHeight = area.getHeight() / 2;

    auto leftColumn = area.removeFromLeft(area.getWidth() / 2);
    auto rightColumn = area;

    pureToneButton.setBounds(leftColumn.removeFromTop(buttonHeight).reduced(10));
    spatialButton.setBounds(leftColumn.reduced(10));

    digitsInNoiseButton.setBounds(rightColumn.removeFromTop(buttonHeight).reduced(10));
    dualTaskButton.setBounds(rightColumn.reduced(10));

    settingsButton.setBounds(getWidth() - 50, 10, 40, 40);
}

void MenuScreen::paint(juce::Graphics& g) {
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}


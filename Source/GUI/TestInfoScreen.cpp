/*
  ==============================================================================

    TestInfoScreen.cpp
    Created: 3 Feb 2026 3:01:47am
    Author:  nicky_hgjk9m6

  ==============================================================================
*/

#include "TestInfoScreen.h"

TestInfoScreen::TestInfoScreen(const juce::String& title, const juce::String& instructions, std::function<void()> onStartPressed, std::function<void()> onBackPressed) 
    : title(title), onStart(onStartPressed), onBack(onBackPressed) {

    instructionsLabel.setText(instructions, juce::dontSendNotification);
    instructionsLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(instructionsLabel);

    startButton.setButtonText("Start");
    startButton.onClick = [this] { if (onStart) onStart();};
    addAndMakeVisible(startButton);

    addAndMakeVisible(backButton);
    backButton.onClick = [this] {if (onBack) onBack(); };
}

void TestInfoScreen::resized() {
    backButton.setBounds(10, 10, 100, 40);

    auto area = getLocalBounds().reduced(40);
    area.removeFromTop(60);

    auto startButtonArea = area.removeFromBottom(50);
    startButton.setBounds(startButtonArea.withCentre(startButtonArea.getCentre()));
    instructionsLabel.setBounds(area);
}

void TestInfoScreen::paint(juce::Graphics& g) {
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    g.setColour(getLookAndFeel().findColour(juce::Label::textColourId));
    g.setFont(juce::Font(40.0f, juce::Font::bold));
    g.drawText(title, getLocalBounds().removeFromTop(120),
        juce::Justification::centred, true);


}

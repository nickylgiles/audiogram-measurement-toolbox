/*
  ==============================================================================

    TestInfoScreen.h
    Created: 3 Feb 2026 3:01:47am
    Author:  nicky_hgjk9m6

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class TestInfoScreen : public juce::Component {
public:
    TestInfoScreen(const juce::String& title, const juce::String& instructions, std::function<void()> onStart, std::function<void()> onBack);

    void resized() override;
    void paint(juce::Graphics& g) override;
private:
    juce::String title;
    juce::Label instructionsLabel;
    juce::TextButton startButton;
    juce::TextButton backButton{ juce::translate("Back") };

    std::function<void()> onStart;
    std::function<void()> onBack;
};
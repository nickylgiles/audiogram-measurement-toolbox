/*
  ==============================================================================

    SpeechInNoiseTestScreen.h
    Created: 29 Nov 2025 1:25:06am
    Author:  nicky_hgjk9m6

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class SpeechInNoiseTestScreen : public juce::Component {
public:
    SpeechInNoiseTestScreen();
    ~SpeechInNoiseTestScreen() override = default;

    // Callbacks set in MainComponent
    std::function<void()> onStopClicked;
    std::function<void(int)> onDigitClicked;

    void setDigitsEnabled(bool enable);
    void resized() override;
    void paint(juce::Graphics& g) override;

    bool keyPressed(const juce::KeyPress& key) override;

private:
    std::vector<std::unique_ptr<juce::TextButton>> digitButtons;
    juce::TextButton stopButton{ "Stop Test" };

    bool digitsEnabled = false;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SpeechInNoiseTestScreen)
};
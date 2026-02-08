/*
  ==============================================================================

    DualTaskTestScreen.h
    Created: 4 Dec 2025 12:29:06pm
    Author:  nicky_hgjk9m6

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class DualTaskTestScreen : public juce::Component {
public:
    DualTaskTestScreen();
    ~DualTaskTestScreen() override = default;

    // Callbacks set in MainComponent
    std::function<void()> onStopClicked;
    std::function<void(int)> onWordClicked;
    std::function<void()> onLeftClicked;
    std::function<void()> onRightClicked;

    void setInputEnabled(bool enable);
    void setWords(const std::vector<juce::String>& newWords);
    void resized() override;
    void paint(juce::Graphics& g) override;

private:
    void addWordButtons();
    std::vector<std::unique_ptr<juce::TextButton>> wordButtons;
    std::vector<juce::String> words;

    juce::TextButton leftButton{ "Left" };
    juce::TextButton rightButton{ "Right" };

    juce::TextButton stopButton{ "Stop Test" };

    bool inputEnabled = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DualTaskTestScreen)
};
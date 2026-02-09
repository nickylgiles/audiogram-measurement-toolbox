/*
  ==============================================================================

    SpatialTestScreen.h
    Created: 26 Nov 2025 11:40:40pm
    Author:  nicky_hgjk9m6

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class SpatialTestScreen : public juce::Component {
public:
    SpatialTestScreen();
    ~SpatialTestScreen() override = default;

    // Callbacks set in MainComponent
    std::function<void()> onLeftClicked;
    std::function<void()> onRightClicked;
    std::function<void()> onStopClicked;

    void resized() override;
    void paint(juce::Graphics& g) override;
    bool keyPressed(const juce::KeyPress& key) override;

private:
    juce::TextButton leftButton{ juce::translate("Left") };
    juce::TextButton rightButton{ juce::translate("Right") };
    juce::TextButton stopButton{ juce::translate("Stop Test") };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SpatialTestScreen)
};
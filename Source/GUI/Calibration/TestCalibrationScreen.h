/*
  ==============================================================================

    TestCalibrationScreen.h
    Created: 16 Mar 2026 12:18:57am
    Author:  nicky_hgjk9m6

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class TestCalibrationScreen : public juce::Component {
public:
    TestCalibrationScreen();

    std::function<void(float)> onFrequencyChanged;
    std::function<void(float)> onLevelChanged;
    std::function<void(bool)> onToggle;
    std::function<void(float)> onOffsetChanged;
    std::function<void()> onStopClicked;

    void setCurrentOffset(float offset);

private:
    juce::ComboBox frequencyBox;
    juce::Label frequencyLabel;

    juce::Slider outputLevelSlider;
    juce::Label outputLevelLabel;

    juce::TextButton toggleButton;

    juce::Slider offsetSlider;
    juce::Label offsetLabel;

    juce::TextButton stopButton{ juce::translate("Stop") };

    void resized() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TestCalibrationScreen)
};

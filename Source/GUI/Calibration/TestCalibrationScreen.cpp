/*
  ==============================================================================

    TestCalibrationScreen.cpp
    Created: 16 Mar 2026 12:18:57am
    Author:  nicky_hgjk9m6

  ==============================================================================
*/

#include "TestCalibrationScreen.h"

TestCalibrationScreen::TestCalibrationScreen() {
    stopButton.onClick = [this] {
        if (onStopClicked) { onStopClicked(); };
    };
    addAndMakeVisible(stopButton);

    frequencyLabel.setText(juce::translate("Frequency (Hz)"), juce::dontSendNotification);
    addAndMakeVisible(frequencyLabel);

    frequencyBox.addItem("250 Hz", 250);
    frequencyBox.addItem("500 Hz", 500);
    frequencyBox.addItem("1000 Hz", 1000);
    frequencyBox.addItem("2000 Hz", 2000);
    frequencyBox.addItem("4000 Hz", 4000);
    frequencyBox.addItem("8000 Hz", 8000);
    frequencyBox.addItem("16000 Hz", 16000);

    frequencyBox.setSelectedId(1000, juce::dontSendNotification);
    addAndMakeVisible(frequencyBox);

    frequencyBox.onChange = [this] {
        if (onFrequencyChanged) { onFrequencyChanged(static_cast<float>(frequencyBox.getSelectedId())); }
    };

    outputLevelLabel.setText(juce::translate("Output Level (dB SPL)"), juce::dontSendNotification);
    addAndMakeVisible(outputLevelLabel);

    outputLevelSlider.setRange(0.0f, 100.0f, 0.1f);
    outputLevelSlider.setValue(0.0f);
    outputLevelSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    outputLevelSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    addAndMakeVisible(outputLevelSlider);

    outputLevelSlider.onValueChange = [this] {
        if (onLevelChanged) { onLevelChanged(static_cast<float>(outputLevelSlider.getValue())); };
    };

    toggleButton.setButtonText("Start");
    addAndMakeVisible(toggleButton);
    toggleButton.onClick = [this] {
        if (onToggle) { onToggle(toggleButton.getToggleState()); };
        toggleButton.setButtonText(toggleButton.getToggleState() ? "Stop" : "Start");
    };
    toggleButton.setClickingTogglesState(true);

    offsetLabel.setText("Set Calibration Offset (dB)", juce::dontSendNotification);
    addAndMakeVisible(offsetLabel);

    offsetSlider.setRange(-20.0f, 20.0f, 0.1f);
    offsetSlider.setValue(0.0f);
    offsetSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    offsetSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    addAndMakeVisible(offsetSlider);

    offsetSlider.onValueChange = [this] {
        if (onOffsetChanged) { onOffsetChanged(static_cast<float>(offsetSlider.getValue())); }
        };
}

void TestCalibrationScreen::setCurrentOffset(float offset) {
    offsetSlider.setValue(offset);
}

void TestCalibrationScreen::resized() {
    auto area = getLocalBounds().reduced(10);
    
    int rowHeight = area.getHeight() / 5;

    area.removeFromTop(rowHeight);

    auto row = area.removeFromTop(rowHeight);
    frequencyLabel.setBounds(row.removeFromLeft(150));
    frequencyBox.setBounds(row);

    row = area.removeFromTop(rowHeight);
    outputLevelLabel.setBounds(row.removeFromLeft(150));
    outputLevelSlider.setBounds(row);

    row = area.removeFromTop(rowHeight);
    toggleButton.setBounds(row.removeFromLeft(150));

    row = area.removeFromTop(rowHeight);
    offsetLabel.setBounds(row.removeFromLeft(150));
    offsetSlider.setBounds(row);

    stopButton.setBounds(10, 10, 100, 40);
}

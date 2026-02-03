/*
  ==============================================================================

    TestControllerTimer.h
    Created: 2 Feb 2026 11:08:43pm
    Author:  nicky_hgjk9m6

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class TestControllerTimer : public juce::Timer {
public:
    TestControllerTimer(std::function<void()> callbackFunction);
    void timerCallback() override;
    void setCallback(std::function<void()> callback);
private:
    std::function<void()> callback;
};
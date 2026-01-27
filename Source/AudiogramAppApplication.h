/*
  ==============================================================================

    AudiogramAppApplication.h
    Created: 27 Jan 2026 12:19:46am
    Author:  nicky_hgjk9m6

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class AudiogramAppApplication : public juce::JUCEApplication {
public:
    juce::ApplicationProperties applicationProperties;

    const juce::String getApplicationName() override;
    const juce::String getApplicationVersion() override;
    bool moreThanOneInstanceAllowed() override;

    void initialise(const juce::String&) override;
    void shutdown() override;
private:
    class MainWindow;
    std::unique_ptr<MainWindow> mainWindow;
};
#pragma once

#include <JuceHeader.h>

#include "Control/PureToneTestController.h"
#include "Control/SpatialTestController.h"
#include "Control/DigitsInNoiseController.h"
#include "Control/DualTasktestController.h"

#include "GUI/MenuScreen.h"
#include "GUI/SettingsScreen.h"

#include "GUI/PureTone/PureToneTestScreen.h"
#include "GUI/PureTone/PureToneResultsScreen.h"

#include "GUI/Spatial/SpatialTestScreen.h"
#include "GUI/Spatial/SpatialResultsScreen.h"

#include "GUI/SpeechInNoise/SpeechInNoiseTestScreen.h"
#include "GUI/SpeechInNoise/SpeechInNoiseResultsScreen.h"

#include "GUI/DualTask/DualTaskTestScreen.h"
#include "GUI/DualTask/DualTaskResultsScreen.h"

#include "Results/ResultsLogger.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent  : public juce::AudioAppComponent
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    void paint (juce::Graphics& g) override;
    void resized() override;

    void testEnd();

    void showMenuScreen(); 
    void showSettingsScreen();

    void showPureToneTestScreen();
    void showPureToneResultsScreen();

    void showSpatialTestScreen();
    void showSpatialResultsScreen();

    void showSpeechInNoiseTestScreen();
    void showSpeechInNoiseResultsScreen();

    void showDualTaskTestScreen();
    void showDualTaskResultsScreen();

private:
    //==============================================================================
    // Your private member variables go here...

    std::unique_ptr<TestController> testController;
    std::unique_ptr<SoundEngine> soundEngine;

    ResultsLogger resultsLogger;

    bool testStarted = false;

    std::unique_ptr<juce::Component> currentScreen;

    juce::File dbFile;
    std::unique_ptr<juce::FileChooser> fileChooser;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};

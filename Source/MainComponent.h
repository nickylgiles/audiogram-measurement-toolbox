#pragma once

#include <JuceHeader.h>

#include "Control/SpatialTestController.h"
#include "Control/DigitsInNoiseController.h"
#include "Control/DualTasktestController.h"

#include "GUI/MenuScreen.h"
#include "GUI/SettingsScreen.h"

#include "Results/ResultsLogger.h"

#include "GUI/AppLookAndFeel.h"

class Test;

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

    void showMenuScreen(); 
    void showSettingsScreen();

    void showScreen(std::unique_ptr<juce::Component>&& screen);

    template <typename ScreenT, typename ControllerT>
    void showResultsScreen(ControllerT& tc) {

        auto results = tc.getResults();
        auto screen = std::make_unique<ScreenT>();

        screen->setResults(results);
        screen->onExportClicked = [this, results] {
            if (resultsLogger.logResults(results)) {
                DBG("Test results logged successfully.");
            }
            else {
                DBG("Failed to save test results.");
            }
            };
        screen->onMenuClicked = [this] {
            showMenuScreen();
            };

        currentScreen = std::move(screen);
        addAndMakeVisible(currentScreen.get());
        resized();
    }

private:
    //==============================================================================
    // Your private member variables go here...

    template<typename TestType>
    void addTestToMenu(MenuScreen* screen, const juce::String& name) {
        screen->addTest(name,
            [this] {
                currentTest = std::make_unique<TestType>(*this, *soundEngine);
                currentTest->displayInfo();
            });
    }

    std::unique_ptr<Test> currentTest;

    std::unique_ptr<TestController> testController;
    std::unique_ptr<SoundEngine> soundEngine;

    ResultsLogger resultsLogger;

    bool testStarted = false;

    std::unique_ptr<juce::Component> currentScreen;

    juce::File dbFile;
    std::unique_ptr<juce::FileChooser> fileChooser;

    AppLookAndFeel lookAndFeel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};

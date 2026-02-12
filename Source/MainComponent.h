#pragma once

#include <JuceHeader.h>

#include "Audio/SoundEngine.h"

#include "GUI/MenuScreen.h"
#include "GUI/SettingsScreen.h"

#include "Results/ResultsLogger.h"

#include "GUI/AppLookAndFeel.h"

#include "Tests/Test.h"


class AudiogramAppApplication;

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

    void refreshSettingsScreen();

    void showScreen(std::unique_ptr<juce::Component>&& screen);

    const juce::String& getUserId();

    template <typename ScreenT, typename ControllerT>
    void showResultsScreen(ControllerT& tc) {

        auto results = tc.getResults();
        auto screen = std::make_unique<ScreenT>();

        juce::String userId = userSettings->getValue("userId", "");

        screen->setResults(results);
        screen->onExportClicked = [this, results, userId] {
            

            if (resultsLogger.logResults(userId, results)) {
                DBG("Test results logged successfully.");
            }
            else {
                DBG("Failed to save test results.");
            }
            };
        screen->onMenuClicked = [this] {
            showMenuScreen();
            };

        showScreen(std::move(screen));
    }

private:
    //==============================================================================
    // Your private member variables go here...

    void setLanguageFromData(const void* data, int size);

    template<typename TestType>
    void addTestToMenu(MenuScreen* screen) {

        // Load config file from path stored in userSettings (if present)
        juce::File configFile;
        
        if (userSettings) {
            juce::String path = userSettings->getValue("config_" + TestType::getName().replaceCharacters(" ", "_"), "");

            if (path.isNotEmpty()) {
                juce::File f(path);
                if (f.existsAsFile())
                    configFile = f;
            }
        } 

        screen->addTest(TestType::getName(),
            [this, configFile] {
                currentTest = std::make_unique<TestType>(*this, *soundEngine, configFile);
                currentTest->displayInfo();
            });
    }

    template<typename TestType>
    void addTestConfigSetting(SettingsScreen* settings) {
        settings->addButtonSetting(
            TestType::getName() + juce::String(" (") + userSettings->getValue("config_" + TestType::getName().replaceCharacters(" ", "_"), "Default") + juce::String(")"),
            [this, testName = TestType::getName()] {
                fileChooser = std::make_unique<juce::FileChooser>(
                    juce::translate("Select") + juce::String(" ") + juce::translate(testName) + juce::String(" ") + juce::translate("config JSON file"),
                    juce::File::getSpecialLocation(juce::File::userDocumentsDirectory),
                    "*.json");

                auto fileChooserFlags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles;

                fileChooser->launchAsync(fileChooserFlags, [this, testName](const juce::FileChooser& fc) {
                    auto result = fc.getResult();
                    if (result != juce::File{}) {
                        // Store the file path in user settings
                        userSettings->setValue("config_" + TestType::getName().replaceCharacters(" ", "_"), result.getFullPathName());
                        userSettings->saveIfNeeded();
                        DBG("Loaded config file for " << testName);
                    }
                    fileChooser.reset();
                    });

                resized();
            });

    }

    std::unique_ptr<Test> currentTest;

    std::unique_ptr<SoundEngine> soundEngine;

    ResultsLogger resultsLogger;

    std::unique_ptr<juce::Component> currentScreen;

    juce::File dbFile;
    std::unique_ptr<juce::FileChooser> fileChooser;

    AppLookAndFeel lookAndFeel;

    juce::PropertiesFile* userSettings = nullptr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};

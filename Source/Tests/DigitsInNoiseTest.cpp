/*
  ==============================================================================

    DigitsInNoiseTest.cpp
    Created: 3 Feb 2026 9:31:42pm
    Author:  nicky_hgjk9m6

  ==============================================================================
*/

#include "DigitsInNoiseTest.h"

DigitsInNoiseTest::DigitsInNoiseTest(MainComponent& mainComponentRef, SoundEngine& soundEngineRef, const juce::File& testConfigFile)
    : Test(mainComponentRef, soundEngineRef),
      controller(mainComponentRef, soundEngineRef, testConfigFile) 
{
    controller.onTestFinished = [this] {onTestFinish();};
}

void DigitsInNoiseTest::displayInfo() {
    auto infoScreen = std::make_unique<TestInfoScreen>(
        juce::translate("Digits-in-noise Test"),
        juce::translate("You will hear three digits read aloud. ")
        + juce::translate("Afterwards, you must input the digits you hear in the correct order to the keypad. ")
        + juce::translate("If you are unsure, guess.")
        + "\n\n" + juce::translate("Press 'Start' to begin the test."),
        [this] { startTest();},
        [this] { exitTest();}
    );

    mainComponent.showScreen(std::move(infoScreen));
}

void DigitsInNoiseTest::startTest() {
    auto screen = std::make_unique<SpeechInNoiseTestScreen>();
    controller.onTestFinished = [this] {onTestFinish();};

    screen->onStopClicked = [this] {
        controller.buttonClicked("stopButton");
        exitTest();
        };

    screen->onDigitClicked = [this](int digit) {
        controller.buttonClicked(juce::String(digit));
        };

    // Get inputsEnabled() from SpeechInNoiseController
    // SafePointer needed in case the screen is deleted before async call is executed
    juce::Component::SafePointer<SpeechInNoiseTestScreen> scrPtr = screen.get();
    if (scrPtr) {
        controller.setInputsEnabled = [scrPtr](bool enabled) {
            juce::MessageManager::callAsync([scrPtr, enabled] {
                if (scrPtr) scrPtr->setDigitsEnabled(enabled);
                });
            };
    }

    controller.startTest();

    mainComponent.showScreen(std::move(screen));
}

void DigitsInNoiseTest::exitTest() {
    controller.stopTest();
    mainComponent.showMenuScreen();
}

void DigitsInNoiseTest::onTestFinish() {
    mainComponent.logTestResults(controller.getResults());

    mainComponent.showResultsScreen<SpeechInNoiseResultsScreen, DigitsInNoiseController>(controller);
}

juce::String DigitsInNoiseTest::getName() {
    return juce::translate("Digits-in-noise Test");
}


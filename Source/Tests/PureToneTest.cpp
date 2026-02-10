/*
  ==============================================================================

    PureToneTest.cpp
    Created: 3 Feb 2026 9:31:18pm
    Author:  nicky_hgjk9m6

  ==============================================================================
*/

#include "PureToneTest.h"


PureToneTest::PureToneTest(MainComponent& mainComponentRef, SoundEngine& soundEngineRef) 
    : Test(mainComponentRef, soundEngineRef), controller(mainComponentRef, soundEngineRef)
{
    controller.onTestFinished = [this] {onTestFinish();};
}

void PureToneTest::displayInfo() {
    auto infoScreen = std::make_unique<TestInfoScreen>(
        juce::translate("Pure Tone Test"),
        "You will hear a series of tones. Press the button when you hear a tone. "
        "Otherwise, do not press anything. "
        "\n\nPress \"Start\" to begin the test.",
        [this] { startTest();},
        [this] { exitTest();}
    );

    mainComponent.showScreen(std::move(infoScreen));
}

void PureToneTest::startTest() {
    controller.startTest();

    auto screen = std::make_unique<PureToneTestScreen>();
    screen->onHearClicked = [this] {
        controller.buttonClicked("hearButton");
        };
    screen->onStopClicked = [this] {
        controller.buttonClicked("stopButton");
        exitTest();
        };


    mainComponent.showScreen(std::move(screen));
}

void PureToneTest::exitTest() {
    controller.stopTest();
    mainComponent.showMenuScreen();
}

void PureToneTest::onTestFinish() {
    mainComponent.showResultsScreen<PureToneResultsScreen, PureToneTestController>(controller);
}

juce::String PureToneTest::getName() {
    return juce::translate("Pure Tone Test");
}

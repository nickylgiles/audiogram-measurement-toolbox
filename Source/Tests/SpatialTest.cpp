/*
  ==============================================================================

    SpatialTest.cpp
    Created: 3 Feb 2026 9:31:29pm
    Author:  nicky_hgjk9m6

  ==============================================================================
*/

#include "SpatialTest.h"

SpatialTest::SpatialTest(MainComponent& mainComponentRef, SoundEngine& soundEngineRef, const juce::File& testConfigFile)
    : Test(mainComponentRef, soundEngineRef), controller(mainComponentRef, soundEngineRef) 
{
    controller.onTestFinished = [this] {onTestFinish();};
}

void SpatialTest::displayInfo() {
    auto infoScreen = std::make_unique<TestInfoScreen>(
        juce::translate("Spatial Test"),

        juce::translate("Two sounds will play from different directions. ")
        + juce::translate("Press 'Left' if the second sound comes to the left of the first; ") 
        + juce::translate("press 'Right' if the second sound comes to the right of the first. ")
        + juce::translate("If you are unsure, guess.")
        + "\n\n" + juce::translate("Press 'Start' to begin the test."),
        [this] { startTest();},
        [this] { exitTest();}
    );

    mainComponent.showScreen(std::move(infoScreen));
}

void SpatialTest::startTest() {
    controller.startTest();

    controller.onTestFinished = [this] {onTestFinish();};

    auto screen = std::make_unique<SpatialTestScreen>();
    screen->onLeftClicked = [this] {
        controller.buttonClicked("leftButton");
        };
    screen->onRightClicked = [this] {
        controller.buttonClicked("rightButton");
        };
    screen->onStopClicked = [this] {
        controller.buttonClicked("stopButton");
        exitTest();
        };

    mainComponent.showScreen(std::move(screen));
}

void SpatialTest::exitTest() {
    controller.stopTest();
    mainComponent.showMenuScreen();
}

void SpatialTest::onTestFinish() {
    mainComponent.showResultsScreen<SpatialResultsScreen, SpatialTestController>(controller);
}

juce::String SpatialTest::getName() {
    return juce::translate("Spatial Test");
}

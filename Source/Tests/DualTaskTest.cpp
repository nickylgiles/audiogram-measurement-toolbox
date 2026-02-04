/*
  ==============================================================================

    DualTaskTest.cpp
    Created: 3 Feb 2026 9:31:50pm
    Author:  nicky_hgjk9m6

  ==============================================================================
*/

#include "DualTaskTest.h"

DualTaskTest::DualTaskTest(MainComponent& mainComponentRef, SoundEngine& soundEngineRef) 
    : Test(mainComponentRef, soundEngineRef), controller(mainComponentRef, soundEngineRef) 
{
    controller.onTestFinished = [this] {onTestFinish();};
}

void DualTaskTest::displayInfo() {
    auto infoScreen = std::make_unique<TestInfoScreen>(
        "Dual-Task Test",
        "You will hear two words spoken from different directions.  Afterwards, you must choose the second word that was spoken from the options presented. "
        "Press \"Left\" if the second sound comes to the left of the first; "
        "press \"Right\" if the second sound comes to the right of the first. "
        "If you are unsure, guess. "
        "\n\nPress \"Start\" to begin the test.",
        [this] { startTest();},
        [this] { exitTest();}
    );

    mainComponent.showScreen(std::move(infoScreen));
}

void DualTaskTest::startTest() {
    controller.onTestFinished = [this] { onTestFinish();};

    auto screen = std::make_unique<DualTaskTestScreen>();

    screen->onStopClicked = [this] {
        controller.buttonClicked("stopButton");
        exitTest();
        };
    screen->onLeftClicked = [this] {
        controller.buttonClicked("leftButton");
        };
    screen->onRightClicked = [this] {
        controller.buttonClicked("rightButton");
        };

    screen->onWordClicked = [this](int wordIdx) {
        controller.buttonClicked("word" + juce::String(wordIdx));
        };


    // Async callbacks for enabling/ disabling input buttons on the GUI and setting word list to show
    juce::Component::SafePointer<DualTaskTestScreen> scrPtr = screen.get();
    if (scrPtr) {
        controller.setInputsEnabled = [scrPtr](bool enabled) {
            juce::MessageManager::callAsync([scrPtr, enabled] {
                if (scrPtr) scrPtr->setInputEnabled(enabled);
                });
            };
        DBG("Dual task enable Callback set");

        controller.setWords = [scrPtr](std::vector<juce::String> words) {
            juce::MessageManager::callAsync([scrPtr, words] {
                if (scrPtr) scrPtr->setWords(words);
                });
            };
    }
   

    controller.startTest();

    mainComponent.showScreen(std::move(screen));
}

void DualTaskTest::exitTest() {
    controller.stopTest();
    mainComponent.showMenuScreen();
}

void DualTaskTest::onTestFinish() {
    mainComponent.showResultsScreen<DualTaskResultsScreen, DualTaskTestController>(controller);
}
/*
  ==============================================================================

    TestCalibrationTest.cpp
    Created: 25 Mar 2026 2:16:28pm
    Author:  nicky_hgjk9m6

  ==============================================================================
*/

#include "TestCalibrationTest.h"

TestCalibrationTest::TestCalibrationTest(MainComponent& mainComponentRef, SoundEngine& soundEngineRef) 
    : Test(mainComponentRef, soundEngineRef), soundEngine(soundEngineRef)
{
}

void TestCalibrationTest::displayInfo() {
    startTest(); // no need to display info
}

void TestCalibrationTest::startTest() {
    auto screen = std::make_unique<TestCalibrationScreen>();

    screen->onStopClicked = [this] {
        exitTest();
    };

    screen->onFrequencyChanged = [this] (float f) {
        frequency = f;
        updateTone();
    };

    screen->onLevelChanged = [this] (float l) {
        level = l;
        updateTone();
    };

    screen->onHLChanged = [this](bool h) {
        htl = h;
        updateTone();
        };

    screen->onChannelChanged = [this](int c) {

        switch (c) {
        case 0:
            channel = ToneChannel::LEFT;
            break;
        case 1:
            channel = ToneChannel::RIGHT;
            break;
        case 2:
            channel = ToneChannel::BOTH;
            break;
        default:
            break;
        }
        updateTone();
        };

    screen->onOffsetChanged = [this](float newOffset) {
        soundEngine.setCalibrationSPLOffset(newOffset);
        DBG("Global SPL offset: " << newOffset << " dB.");
    };

    screen->onToggle = [this](bool toggle) {
        playing = toggle;
        updateTone();
    };

    screen->setCurrentOffset(soundEngine.getCalibrationSPLOffset());
    mainComponent.showScreen(std::move(screen));
}

void TestCalibrationTest::exitTest() {
    stopTone();
    mainComponent.showSettingsScreen();
}

void TestCalibrationTest::onTestFinish() {
}

juce::String TestCalibrationTest::getName() {
    return juce::String("Check Levels");
}

void TestCalibrationTest::updateTone() {
    soundEngine.stop();
    if (!playing)
        return;

    // Comvert db SPL -> db FS -> amplitude
    float dBFS = level - soundEngine.getCalibrationMetadata().targetSPL;

    if (htl) {
        dBFS = HTL::toSPL(level, soundEngine.getCalibrationMetadata().targetSPL, frequency);
    }

    float amplitude = juce::Decibels::decibelsToGain(dBFS);

    DBG("Update tone");
    switch (channel) {
    case TestCalibrationTest::ToneChannel::LEFT:
        soundEngine.playTone(frequency, amplitude, 600.0f, 0);
        break;
    case TestCalibrationTest::ToneChannel::RIGHT:
        soundEngine.playTone(frequency, amplitude, 600.0f, 1);
        break;
    case TestCalibrationTest::ToneChannel::BOTH:
        soundEngine.playTone(frequency, amplitude, 600.0f, 0);
        soundEngine.playTone(frequency, amplitude, 600.0f, 1);
        break;
    default:
        break;
    }
}

void TestCalibrationTest::stopTone() {
    soundEngine.stop();
}

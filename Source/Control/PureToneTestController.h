/*
  ==============================================================================

    PureToneTestController.h
    Created: 7 Nov 2025 4:33:08pm
    Author:  nicky_hgjk9m6

  ==============================================================================
*/
#include "TestController.h"
#include "../Results/PureToneTestResults.h"
#include "TestControllerTimer.h"
#include "../Audio/Calibration/HTL.h"
#pragma once

class PureToneTestController : public TestController {
public:
    // Configuration parameters
    struct Config {
        juce::String name = "";
        std::vector<float> testTones = { 
            125.0f, 
            250.0f, 
            500.0f, 
            1000.0f, 
            2000.0f, 
            4000.0f, 
            8000.0f, 
            16000.0f 
        };

        // db HL values
        float dbLevelMin = -10.0f;
        float dbLevelMax = 80.0f;
        float dbIncrementAscending = 10.0f;
        float dbIncrementDescending = 5.0f;

        int toneDelayMs = 2000;

        static Config loadFromFile(const juce::File& file);
    };

    PureToneTestController(MainComponent& mainComponentRef, SoundEngine& soundEngineRef, const juce::File& configFile);

    void startTest() override;
    void stopTest() override;

    void buttonClicked(const juce::String& id) override;

    const PureToneTestResults getResults();

private:
    Config config;

    TestControllerTimer timer;
    void timerCallback();
    void toneHeard();
    
    void playCurrentTone();
    void scheduleNextTone(int delayMs);

    static constexpr bool floatsEqual(float a, float b);

    PureToneTestResults toneThresholds;

    int currentTone = 0;
    int currentEar = 0;
    float currentThreshold = -20.0f;

    bool currentToneDetected = false;
    
    enum class TestState {
        START,
        DB_ASCENDING,
        DB_DESCENDING,
        NEXT_TONE,
        END
    };
    
    TestState currentState{ TestState::END };
};

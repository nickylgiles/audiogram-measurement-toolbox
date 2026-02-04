/*
  ==============================================================================

    ResultsLogger.h
    Created: 27 Nov 2025 4:10:24am
    Author:  nicky_hgjk9m6

  ==============================================================================
*/

#pragma once

#include "Database.h"
#include "SpatialTestResults.h"
#include "SpeechInNoiseTestResults.h"
#include "DualTaskTestResults.h"
#include "PureToneTestResults.h"

class ResultsLogger {
public:
    ResultsLogger();
    ~ResultsLogger();

    bool openDatabase(const juce::File& file);

    bool logResults(const juce::String& userId, const SpatialTestResults& results);
    bool logResults(const juce::String& userId, const PureToneTestResults& results);

    bool logResults(const juce::String& userId, const SpeechInNoiseTestResults& results);
    bool logResults(const juce::String& userId, const DualTaskTestResults& results);

private:
    int getNextTestId(const juce::String& tableName);

    Database db;
};
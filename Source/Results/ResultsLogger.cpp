/*
  ==============================================================================

    ResultsLogger.cpp
    Created: 27 Nov 2025 4:10:24am
    Author:  nicky_hgjk9m6

  ==============================================================================
*/

#include "ResultsLogger.h"

ResultsLogger::ResultsLogger() {
}

ResultsLogger::~ResultsLogger() {
}

bool ResultsLogger::openDatabase(const juce::File& file) {
    if (!db.open(file))
        return false;

    return true;
}

bool ResultsLogger::logSpatialResults(const SpatialTestResults& results) {
    return false;
}

bool ResultsLogger::logPureToneResults(const PureToneTestResults& results) {
    return false;
}

bool ResultsLogger::logSpeechInNoiseResults(const SpeechInNoiseTestResults& results) {
    return false;
}

bool ResultsLogger::logDualTaskResults(const DualTaskTestResults& results) {
    return false;
}

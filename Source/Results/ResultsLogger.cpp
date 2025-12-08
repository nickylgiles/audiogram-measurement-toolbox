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
    if (!db.execute(
        "CREATE TABLE IF NOT EXISTS  SpatialResults ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "timestamp TEXT NOT NULL,"
        "referenceAzimuth REAL NOT NULL,"
        "targetAzimuth REAL NOT NULL,"
        "snr REAL NOT NULL,"
        "correct INTEGER NOT NULL);"
    )) {
        return false;
    }

    juce::String timestamp = juce::Time::getCurrentTime().toString(true, true, true, true);

    for (auto& r : results.responses) {
        float refAz = r.referenceAzimuth;
        float targAz = r.targetAzimuth;
        float snr = r.snr;
        int correct = r.spatialCorrect;

        juce::String sql = "INSERT INTO SpatialResults (timestamp, referenceAzimuth, targetAzimuth, snr, correct) VALUES ('"
            + timestamp + "', '"
            + juce::String(refAz) + "', "
            + juce::String(targAz) + ", "
            + juce::String(snr) + ", "
            + juce::String(correct) + ");";

        if (!db.execute(sql))
            return false;
    }

    return true;
}

bool ResultsLogger::logPureToneResults(const PureToneTestResults& results) {
    if (!db.execute(
        "CREATE TABLE IF NOT EXISTS  PureToneResults ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "timestamp TEXT NOT NULL,"
        "ear TEXT NOT NULL,"
        "frequency REAL NOT NULL,"
        "threshold REAL NOT NULL);"
    )) {
        return false;
    }

    juce::String timestamp = juce::Time::getCurrentTime().toString(true, true, true, true);

    for (auto earIdx : { 0, 1 }) {
        juce::String earName = (earIdx == 0) ? "left" : "right";

        for (auto& entry : results[earIdx]) {
            float freq = entry.first;
            float threshold = entry.second;

            juce::String sql = "INSERT INTO PureToneResults (timestamp, ear, frequency, threshold) VALUES ('"
                + timestamp + "', '"
                + earName + "', "
                + juce::String(freq) + ", "
                + juce::String(threshold) + ");";

            if (!db.execute(sql))
                return false;

        }
    }
    return true;
}

bool ResultsLogger::logSpeechInNoiseResults(const SpeechInNoiseTestResults& results) {
    return false;
}

bool ResultsLogger::logDualTaskResults(const DualTaskTestResults& results) {
    return false;
}

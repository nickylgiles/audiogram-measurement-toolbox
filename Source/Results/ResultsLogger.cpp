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

bool ResultsLogger::logResults(const SpatialTestResults& results) {
    if (!db.execute(
        "CREATE TABLE IF NOT EXISTS SpatialResults ("
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

bool ResultsLogger::logResults(const PureToneTestResults& results) {
    if (!db.execute(
        "CREATE TABLE IF NOT EXISTS PureToneResults ("
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

bool ResultsLogger::logResults(const SpeechInNoiseTestResults& results) {
    if (!db.execute(
        "CREATE TABLE IF NOT EXISTS DigitsInNoiseResults ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "timestamp TEXT NOT NULL,"
        "targetWord TEXT NOT NULL, "
        "reportedWord TEXT NOT NULL,"
        "snr REAL NOT NULL,"
        "correct INTEGER NOT NULL);"
    )) {
        return false;
    }

    juce::String timestamp = juce::Time::getCurrentTime().toString(true, true, true, true);

    for (auto& r : results.responses) {
        juce::String targWord = r.targetWord;
        juce::String repWord = r.reportedWord;
        float snr = r.snr;
        int correct = r.wordCorrect;

        juce::String sql = "INSERT INTO DigitsInNoiseResults (timestamp, targetWord, reportedWord, snr, correct) VALUES ('"
            + timestamp + "', '"
            + targWord + "', '"
            + repWord + "', "
            + juce::String(snr) + ", "
            + juce::String(correct) + ");";

        if (!db.execute(sql))
            return false;
    }

    return true;
}

bool ResultsLogger::logResults(const DualTaskTestResults& results) {
    if (!db.execute(
        "CREATE TABLE IF NOT EXISTS DualTaskResults ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "timestamp TEXT NOT NULL,"
        "targetWord TEXT NOT NULL, "
        "reportedWord TEXT NOT NULL,"
        "snr REAL NOT NULL,"
        "wordCorrect INTEGER NOT NULL,"
        "referenceAzimuth REAL NOT NULL,"
        "targetAzimuth REAL NOT NULL,"
        "spatialCorrect INTEGER NOT NULL"
        ");"
    )) {
        return false;
    }

    juce::String timestamp = juce::Time::getCurrentTime().toString(true, true, true, true);

    for (auto& r : results.responses) {
        juce::String targWord = r.wordTestResponse.targetWord;
        juce::String repWord = r.wordTestResponse.reportedWord;
        float snr = r.wordTestResponse.snr;
        int wordCorrect = r.wordTestResponse.wordCorrect;

        float refAz = r.spatialTestResponse.referenceAzimuth;
        float targAz = r.spatialTestResponse.targetAzimuth;
        int spatialCorrect = r.spatialTestResponse.spatialCorrect;

        juce::String sql = "INSERT INTO DualTaskResults (timestamp, targetWord, reportedWord, snr, wordCorrect, referenceAzimuth, targetAzimuth, spatialCorrect) VALUES ('"
            + timestamp + "', '"
            + targWord + "', '"
            + repWord + "', "
            + juce::String(snr) + ", "
            + juce::String(wordCorrect) + ","
            + juce::String(refAz) + ", "
            + juce::String(targAz) + ", "
            + juce::String(spatialCorrect) + ");";

        if (!db.execute(sql))
            return false;
    }

    return true;
}

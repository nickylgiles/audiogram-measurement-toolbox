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

bool ResultsLogger::logResults(const juce::String& userId, const juce::String& calibrationId, const SpatialTestResults& results) {
    if (!db.execute(
        "CREATE TABLE IF NOT EXISTS SpatialResults ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "testId INTEGER NOT NULL,"
        "userId TEXT NOT NULL,"
        "calibrationId TEXT NOT NULL,"
        "timestamp TEXT NOT NULL,"
        "referenceAzimuth REAL NOT NULL,"
        "targetAzimuth REAL NOT NULL,"
        "snr REAL NOT NULL,"
        "correct INTEGER NOT NULL);"
    )) {
        return false;
    }

    juce::String timestamp = juce::Time::getCurrentTime()
        .toString(true, true, true, true);

    int testId = getNextTestId("SpatialResults");

    for (auto& r : results.responses) {
        float refAz = r.referenceAzimuth;
        float targAz = r.targetAzimuth;
        float snr = r.snr;
        int correct = r.spatialCorrect;

        juce::String sql = "INSERT INTO SpatialResults (testId, userId, calibrationId, timestamp, referenceAzimuth, targetAzimuth, snr, correct) VALUES ('"
            + juce::String(testId) + "', '"
            + userId + "', '"
            + calibrationId + "', '"
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

bool ResultsLogger::logResults(const juce::String& userId, const juce::String& calibrationId, const PureToneTestResults& results) {
    if (!db.execute(
        "CREATE TABLE IF NOT EXISTS PureToneResults ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "testId INTEGER NOT NULL,"
        "userId TEXT NOT NULL,"
        "calibrationId TEXT NOT NULL,"
        "timestamp TEXT NOT NULL,"
        "ear TEXT NOT NULL,"
        "frequency REAL NOT NULL,"
        "threshold REAL NOT NULL);"
    )) {
        return false;
    }

    juce::String timestamp = juce::Time::getCurrentTime()
        .toString(true, true, true, true);

    int testId = getNextTestId("PureToneResults");

    for (auto earIdx : { 0, 1 }) {
        juce::String earName = (earIdx == 0) ? "left" : "right";

        for (auto& entry : results[earIdx]) {
            float freq = entry.first;
            float threshold = entry.second;

            juce::String sql = "INSERT INTO PureToneResults (testId, userId, calibrationId, timestamp, ear, frequency, threshold) VALUES ('"
                + juce::String(testId) + "', '"
                + userId + "', '"
                + calibrationId + "', '"
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

bool ResultsLogger::logResults(const juce::String& userId, const juce::String& calibrationId, const SpeechInNoiseTestResults& results) {
    if (!db.execute(
        "CREATE TABLE IF NOT EXISTS DigitsInNoiseResults ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "testId INTEGER NOT NULL,"
        "userId TEXT NOT NULL,"
        "calibrationId TEXT NOT NULL,"
        "timestamp TEXT NOT NULL,"
        "targetWord TEXT NOT NULL, "
        "reportedWord TEXT NOT NULL,"
        "snr REAL NOT NULL,"
        "correct INTEGER NOT NULL);"
    )) {
        return false;
    }

    juce::String timestamp = juce::Time::getCurrentTime()
        .toString(true, true, true, true);

    int testId = getNextTestId("DigitsInNoiseResults");

    for (auto& r : results.responses) {
        juce::String targWord = r.targetWord;
        juce::String repWord = r.reportedWord;
        float snr = r.snr;
        int correct = r.wordCorrect;

        juce::String sql = "INSERT INTO DigitsInNoiseResults (testId, userId, calibrationId, timestamp, targetWord, reportedWord, snr, correct) VALUES ('"
            + juce::String(testId) + "', '"
            + userId + "', '"
            + calibrationId + "', '"
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

bool ResultsLogger::logResults(const juce::String& userId, const juce::String& calibrationId, const DualTaskTestResults& results) {
    if (!db.execute(
        "CREATE TABLE IF NOT EXISTS DualTaskResults ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "testId INTEGER NOT NULL,"
        "userId TEXT NOT NULL,"
        "calibrationId TEXT NOT NULL,"
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

    int testId = getNextTestId("DualTaskResults");

    for (auto& r : results.responses) {
        juce::String targWord = r.wordTestResponse.targetWord;
        juce::String repWord = r.wordTestResponse.reportedWord;
        float snr = r.wordTestResponse.snr;
        int wordCorrect = r.wordTestResponse.wordCorrect;

        float refAz = r.spatialTestResponse.referenceAzimuth;
        float targAz = r.spatialTestResponse.targetAzimuth;
        int spatialCorrect = r.spatialTestResponse.spatialCorrect;

        juce::String sql = "INSERT INTO DualTaskResults (testId, userId, calibrationId, timestamp, targetWord, reportedWord, snr, wordCorrect, referenceAzimuth, targetAzimuth, spatialCorrect) VALUES ('"
            + juce::String(testId) + "', '"
            + userId + "', '"
            + calibrationId + "', '"
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

int ResultsLogger::getNextTestId(const juce::String& tableName) {
    juce::String sql = "SELECT MAX(testId) FROM " + tableName + ";";

    int maxId = db.getIntValue(sql);
    
    return maxId + 1;
}

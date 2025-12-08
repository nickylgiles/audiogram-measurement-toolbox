/*
  ==============================================================================

    Database.h
    Created: 27 Nov 2025 4:17:20am
    Author:  nicky_hgjk9m6

  ==============================================================================
*/

#pragma once

#include "../../ThirdParty/sqlite-amalgamation-3510000/sqlite3.h"
#include <JuceHeader.h>

class Database {
public:
    Database() = default;
    ~Database();

    bool open(const juce::File& file); 
    void close();

    bool execute(const juce::String& sql);
    sqlite3* getHandle();
private:
    sqlite3* db = nullptr;
};
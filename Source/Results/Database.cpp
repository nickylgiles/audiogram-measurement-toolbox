/*
  ==============================================================================

    Database.cpp
    Created: 27 Nov 2025 4:17:20am
    Author:  nicky_hgjk9m6

  ==============================================================================
*/

#include "Database.h"

Database::~Database() {
    close();
}

bool Database::open(const juce::File& file) {
    close();
    int rc = sqlite3_open(file.getFullPathName().toRawUTF8(), &db);
    if (rc != SQLITE_OK) {
        DBG("Failed to open database: " << sqlite3_errmsg(db));
        return false;
    }
    return true;
}

void Database::close() {
    if (db) sqlite3_close(db);
    db = nullptr;
}

bool Database::execute(const juce::String& sql) {
    char* errMsg = nullptr;
    if (sqlite3_exec(db, sql.toRawUTF8(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
        DBG(errMsg);
        if (errMsg) sqlite3_free(errMsg);
        return false;
    }
    return true;
}

sqlite3* Database::getHandle() {
    return db;
}

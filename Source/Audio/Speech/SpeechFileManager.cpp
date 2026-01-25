/*
  ==============================================================================

    SpeechFileManager.cpp
    Created: 29 Nov 2025 1:28:44pm
    Author:  nicky_hgjk9m6

  ==============================================================================
*/

#include "SpeechFileManager.h"

SpeechFileManager::SpeechFileManager() {
    loadBinaryData();
    loadWordGroups("C:\\Users\\nicky_hgjk9m6\\Documents\\______ENG YEAR 5\\Project\\mrt\\wordgroups.json");
}

/*
   Loads audio files stored in the app as binary data (digits-in-noise audio)
*/
void SpeechFileManager::loadBinaryData() {
    for (int i = 0; i < BinaryData::namedResourceListSize; ++i) {
        juce::String name(BinaryData::namedResourceList[i]);

        if (!name.startsWith("speech_")) {
            continue;
        }

        auto tokens = juce::StringArray::fromTokens(name, "_", "");
        if (tokens.size() < 3)
            continue;

        bool isDigit = true;

        for (auto c : tokens[1]) {
            if (!juce::CharacterFunctions::isDigit(c)) {
                isDigit = false;
                break;
            }
        }

        int size = 0;
        SpeechResource res;
        res.data = BinaryData::getNamedResource(name.toRawUTF8(), size);

        if (res.data == nullptr || size <= 0) {
            DBG("Failed to load BinaryData resource: " << name);
            continue;
        }

        res.dataSize = (size_t)size;

        if (isDigit) {
            int digit = tokens[1].getIntValue();
            digitsMap[digit] = res;
        }

    }
}


/*
    Load word groups specified in the given JSON file.
*/
void SpeechFileManager::loadWordGroups(const juce::String& jsonFilePath) {
    juce::File jsonFile(jsonFilePath);

    juce::var json = juce::JSON::parse(jsonFile);

    if (json.isVoid()) {
        DBG("Failed to parse JSON");
        return;
    }

    DBG("JSON found");

    auto* root = json.getDynamicObject();
    if (!root) {
        return;
    }

    auto groupsProp = root->getProperty("groups");
    if (!groupsProp.isArray()) {
        return;
    }
    auto* groups = groupsProp.getArray();

    juce::File baseDir = jsonFile.getParentDirectory();
    
    for (auto& g : *groups) {
        auto* group = g.getDynamicObject();
        if (!group) {
            continue;
        }

        juce::String groupId = group->getProperty("group_id").toString();

        auto itemsProp = group->getProperty("items");
        if (!itemsProp.isArray()) {
            continue;
        }
        auto* items = itemsProp.getArray();

        std::vector<juce::String> wordList;

        for (auto& i : *items) {
            auto* item = i.getDynamicObject();
            if (!item) {
                continue;
            }

            juce::String filePath = item->getProperty("file").toString();
            juce::String word = item->getProperty("word").toString();

            juce::File file = baseDir.getChildFile(filePath);

            if (!file.existsAsFile()) {
                DBG("Audio file not found: " << file.getFullPathName());
                continue;
            }

            loadWordFile(word, file);

            wordList.push_back(word);

        }

        addWordGroup(groupId, wordList);
    }
}

void SpeechFileManager::loadWordFile(const juce::String& word, const juce::File& file) {
    wordsMap[word] = file;
}

void SpeechFileManager::addWordGroup(const juce::String& groupId, const std::vector<juce::String>& words) {
    wordGroups[groupId] = words;

    DBG("Loaded word group '" << groupId << "' {");
    for (auto w : words) {
        DBG("\t" << w);
    }
    DBG("}");
}

const SpeechFileManager::SpeechResource& SpeechFileManager::getDigit(int digit) {
    auto it = digitsMap.find(digit);
    if (it == digitsMap.end()) {
        throw std::out_of_range("Digit not found: " + std::to_string(digit));
    }

    return it->second;
}

const juce::File& SpeechFileManager::getWordFile(const juce::String& word) {
    auto it = wordsMap.find(word);
    if (it == wordsMap.end()) {
        throw std::out_of_range("Word not found: " + word.toStdString());
    }

    return it->second;
}

const std::vector<juce::String> SpeechFileManager::getWordsInGroup(const juce::String& groupId) {
    auto it = wordGroups.find(groupId);
    if (it == wordGroups.end()) {
        return {};
    }

    return it->second;
}

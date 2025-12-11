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
}

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
        else {
            juce::String word = tokens[1];
            wordsMap[word] = res;
        }

    }
}

const SpeechFileManager::SpeechResource& SpeechFileManager::getDigit(int digit) {
    auto it = digitsMap.find(digit);
    if (it == digitsMap.end()) {
        throw std::out_of_range("Digit not found: " + std::to_string(digit));
    }

    return it->second;
}

const SpeechFileManager::SpeechResource& SpeechFileManager::getWord(const juce::String& word) {
    auto it = wordsMap.find(word);
    if (it == wordsMap.end()) {
        throw std::out_of_range("Word not found: " + word.toStdString());
    }

    return it->second;
}

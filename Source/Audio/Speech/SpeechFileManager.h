/*
  ==============================================================================

    SpeechFileManager.h
    Created: 29 Nov 2025 1:28:44pm
    Author:  nicky_hgjk9m6

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <BinaryData.h>

class SpeechFileManager {
public:
    SpeechFileManager();

    void loadBinaryData(); 
    void loadWordGroups(const juce::String& jsonFilePath);

    struct SpeechResource {
        const void* data;
        size_t dataSize;
    };

    const SpeechResource& getDigit(int digit);

    const juce::File& getWordFile(const juce::String& word);

    const std::vector<juce::String> getWordsInGroup(const juce::String& groupId);

private:
    void loadWordFile(const juce::String& word, const juce::File& file);
    void addWordGroup(const juce::String& groupId, const std::vector<juce::String>& words);

    std::map<int, SpeechResource> digitsMap;

    std::map<juce::String, juce::File> wordsMap;

    std::map<juce::String, std::vector<juce::String>> wordGroups;

};
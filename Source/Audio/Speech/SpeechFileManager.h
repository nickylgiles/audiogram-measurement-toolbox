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

    struct SpeechResource {
        const void* data;
        size_t dataSize;
    };

    const SpeechResource& getDigit(int digit);

    const SpeechResource& getWord(const juce::String& word);

private:
    std::map<int, SpeechResource> digitsMap;

    std::map<juce::String, SpeechResource> wordsMap;
};
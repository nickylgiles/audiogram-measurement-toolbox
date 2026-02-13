/*
  ==============================================================================

    SoundFilePlayer.cpp
    Created: 1 Nov 2025 9:05:31am
    Author:  nicky_hgjk9m6

  ==============================================================================
*/

#include "SoundFilePlayer.h"

SoundFilePlayer::SoundFilePlayer() {
    currentSample = 0;
    totalSamples = 0;
    sampleRate = 44100.0;
    fileLoaded = false;
    fileSampleRate = 44100.0;
    playbackIncrement = 1.0;
    soundPlaying = false;
    audioFormatManager.registerBasicFormats();
}

bool SoundFilePlayer::loadFile(const juce::File& file, bool normalise) {
    std::unique_ptr<juce::AudioFormatReader> reader(audioFormatManager.createReaderFor(file));

    if (reader.get() == nullptr) {
        fileLoaded = false;
        DBG("Failed to load file");
        return false;
    }

    totalSamples = static_cast<int>(reader->lengthInSamples);
    buffer.setSize(static_cast<int>(reader->numChannels), totalSamples);
    reader->read(&buffer, 0, totalSamples, 0, true, true);

    currentSample = 0;
    fileLoaded = true;
    soundPlaying = false;
    DBG("File loaded.  Length = " << totalSamples << " samples");

    fileSampleRate = reader->sampleRate;
    playbackIncrement = fileSampleRate / sampleRate;

    if (normalise) {
        normaliseBuffer(buffer);
    }
    return true;
}

bool SoundFilePlayer::loadBinaryData(const void* data, size_t size, bool normalise) {
    auto inputStream = std::make_unique<juce::MemoryInputStream>(data, size, false);

    std::unique_ptr<juce::AudioFormatReader> reader(
        audioFormatManager.createReaderFor(std::move(inputStream)));

    if (reader == nullptr) {
        DBG("Failed to load binary resource");
        return false;
    }
    
    totalSamples = static_cast<int>(reader->lengthInSamples);
    buffer.setSize((int)reader->numChannels, totalSamples);
    reader->read(&buffer, 0, totalSamples, 0, true, true);
    currentSample = 0.0;
    fileLoaded = true;
    soundPlaying = false;
    DBG("Binary resource loaded. Length = " << totalSamples << " samples");


    fileSampleRate = reader->sampleRate;
    playbackIncrement = fileSampleRate / sampleRate;

    if (normalise) {
        normaliseBuffer(buffer);
    }
    return true;
}

bool SoundFilePlayer::loadNoise(int length) {
    juce::Random random = juce::Random();
    buffer.setSize(1, length);
    auto* bufferWritePtr = buffer.getWritePointer(0);

    for (int i = 0; i < length; ++i) {
        bufferWritePtr[i] = random.nextFloat() * 2.0f - 1.0f;
    }

    fileLoaded = true;
    soundPlaying = false;
    currentSample = 0.0;
    totalSamples = length;

    return true;
}

bool SoundFilePlayer::loadNoise(int length, juce::dsp::IIR::Coefficients<float> filterCoeffs) {
    if (!loadNoise(length)) 
       return false;

    juce::dsp::IIR::Filter<float> iirFilter;
    iirFilter.coefficients = filterCoeffs;

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = length;
    spec.numChannels = 1;
    iirFilter.prepare(spec);

    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);
    iirFilter.process(context);

    return true;
}

void SoundFilePlayer::setSampleRate(double newSampleRate) {
    sampleRate = newSampleRate;
    if (fileLoaded)
        playbackIncrement = fileSampleRate / sampleRate;
}

void SoundFilePlayer::reset() {
    currentSample = 0.0;
    soundPlaying = false;
}

float SoundFilePlayer::nextSample() {
    if (!fileLoaded || totalSamples == 0 || !soundPlaying)
        return 0.0f;

    if (currentSample >= totalSamples - 1) {
        reset();
        return 0.0f;
    }

    int index = static_cast<int>(currentSample);
    float frac = static_cast<float>(currentSample - index);

    float s1 = buffer.getSample(0, index);
    float s2 = buffer.getSample(0, index + 1);
    float sample = s1 + frac * (s2 - s1);

    currentSample += playbackIncrement;

    return sample;
}

bool SoundFilePlayer::fileIsLoaded() const {
    return fileLoaded;
}

bool SoundFilePlayer::soundIsPlaying() const {
    return soundPlaying;
}

void SoundFilePlayer::startPlaying() {
    if (!fileLoaded) return;
    soundPlaying = true;
    currentSample = 0;
}

int SoundFilePlayer::getLength()
{
    if (!fileLoaded)
        return 0;
    return totalSamples;
}

bool SoundFilePlayer::isFinished() const {
    return fileLoaded && !soundPlaying;
}

int SoundFilePlayer::samplesRemaining() {
    return totalSamples - static_cast<int>(currentSample);
}

void SoundFilePlayer::normaliseBuffer(juce::AudioBuffer<float>& buf) {
    float maxMag = 0.0f;

    for (int ch = 0; ch < buf.getNumChannels(); ++ch) {
        maxMag = std::max(maxMag, buf.getMagnitude(ch, 0, buf.getNumSamples()));
    }

    if (maxMag > 0.0f) {
        buf.applyGain(1.0f / maxMag);
    }

    DBG("Normalised buffer by " << (1.0f / maxMag));
}

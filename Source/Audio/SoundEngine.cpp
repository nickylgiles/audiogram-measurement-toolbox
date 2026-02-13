/*
  ==============================================================================

    SoundEngine.cpp
    Created: 28 Oct 2025 11:07:04pm
    Author:  nicky_hgjk9m6

  ==============================================================================
*/

#include "SoundEngine.h"

SoundEngine::SoundEngine() {

}

void SoundEngine::playTone(float frequency, float amplitude, float duration, int channel) {
    addSource(std::make_unique<ToneSource>(sampleRate, frequency, amplitude, duration, channel));
}

void SoundEngine::playToneMasked(float frequency, float amplitude, float duration, int channel) {
    addSource(std::make_unique<ToneSource>(sampleRate, frequency, amplitude, duration, channel));
    int noiseChannel = (channel == 0 ? 1 : 0);
    addSource(std::make_unique<NoiseSource>(sampleRate, amplitude, duration, noiseChannel, true, frequency));
}

void SoundEngine::playSample(const void* data, size_t size, float gain, bool normaliseAudio) {
    addSource(std::make_unique<SoundFileSource>(
        sampleRate, data, size, gain, normaliseAudio)
    );
}

void SoundEngine::playSampleSpatial(const void* data, size_t size, float elevation, float azimuth, float gain, bool normaliseAudio) {
    addSource(std::make_unique<SpatialisedSoundFileSource>(
        sampleRate, data, size, hrtfManager, elevation, azimuth, gain, normaliseAudio)
    );
}

void SoundEngine::playSample(const juce::File& file, float gain, bool normaliseAudio) {
    addSource(std::make_unique<SoundFileSource>(
        sampleRate, file, gain, normaliseAudio)
    );
}

void SoundEngine::playSampleSpatial(const juce::File& file, float elevation, float azimuth, float gain, bool normaliseAudio) {
    addSource(std::make_unique<SpatialisedSoundFileSource>(
        sampleRate, file, hrtfManager, elevation, azimuth, gain, normaliseAudio)
    );
}

void SoundEngine::playNoise(float amplitude, float duration, int channel) {
    addSource(std::make_unique<NoiseSource>(sampleRate, amplitude, duration, channel));
}

void SoundEngine::playNoiseSpatial(float amplitude, float duration, float elevation, float azimuth) {
    addSource(std::make_unique<SpatialisedNoiseSource>(
        sampleRate, amplitude, duration, hrtfManager, elevation, azimuth
    ));

}

void SoundEngine::stop() {
    juce::SpinLock::ScopedLockType lock(sourceLock);
    sources.clear();
    DBG("All sounds stopped");
}

bool SoundEngine::isPlaying() const {
    return sources.size() > 0;
}

void SoundEngine::setSampleRate(double newSampleRate) {
    sampleRate = newSampleRate;
    hrtfManager.setSampleRate(newSampleRate);
    hrtfManager.loadBinaryData();
    stop();
}

void SoundEngine::setSamplesPerBlockExpected(int) {
    // Nothing to do currently
}


void SoundEngine::processBlock(float* outputL, float* outputR, int numSamples) {

    juce::AudioBuffer<float> sourcesL(1, numSamples);
    juce::AudioBuffer<float> sourcesR(1, numSamples);

    float* sourcesLPtr = sourcesL.getWritePointer(0);
    float* sourcesRPtr = sourcesR.getWritePointer(0);

    for (int i = 0; i < numSamples; ++i) {
        sourcesLPtr[i] = 0.0f;
        sourcesRPtr[i] = 0.0f;
    }

    
    juce::AudioBuffer<float> leftBuf(1, numSamples);
    juce::AudioBuffer<float> rightBuf(1, numSamples);

    float* tempL = leftBuf.getWritePointer(0);
    float* tempR = rightBuf.getWritePointer(0);

    const juce::SpinLock::ScopedLockType lock(sourceLock);

    for (auto it = sources.begin(); it != sources.end(); ) {
        (*it)->process(tempL, tempR, numSamples);

        for (int i = 0; i < numSamples; ++i) {
            sourcesLPtr[i] += tempL[i];
            sourcesRPtr[i] += tempR[i];
        }

        if ((*it)->isFinished()) {
            it = sources.erase(it);
            continue;
        }

        ++it;
    }

    // Apply headphone calibration
    // juce::dsp::IIR

    for (int i = 0; i < numSamples; ++i) {
        outputL[i] = sourcesLPtr[i];
        outputR[i] = sourcesRPtr[i];
    }
    
    
}

void SoundEngine::addSource(std::unique_ptr<SoundSource> source) {
    juce::SpinLock::ScopedLockType lock(sourceLock);
    sources.push_back(std::move(source));
}


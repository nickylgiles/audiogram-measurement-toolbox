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
    sources.push_back(std::make_unique<ToneSource>(sampleRate, frequency, amplitude, duration, channel));
    int noiseChannel = (channel == 0 ? 1 : 0);
    addSource(std::make_unique<NoiseSource>(sampleRate, amplitude, duration, noiseChannel, true, frequency));
}

void SoundEngine::playSample(const void* data, size_t size, float gain) {
    sources.push_back(std::make_unique<SoundFileSource>(sampleRate, data, size, gain));
}

void SoundEngine::playSampleSpatial(const void* data, size_t size, float elevation, float azimuth, float gain) {
    addSource(std::make_unique<SpatialisedSoundFileSource>(
        sampleRate, data, size, hrtfManager, elevation, azimuth, gain));

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


void SoundEngine::processBlock(float* outputL, float* outputR, int numSamples) {
 
    for (int i = 0; i < numSamples; ++i) {
        outputL[i] = 0.0f;
        outputR[i] = 0.0f;
    }
    
    juce::AudioBuffer<float> leftBuf(1, numSamples);
    juce::AudioBuffer<float> rightBuf(1, numSamples);

    float* tempL = leftBuf.getWritePointer(0);
    float* tempR = rightBuf.getWritePointer(0);

    const juce::SpinLock::ScopedLockType lock(sourceLock);

    for (auto it = sources.begin(); it != sources.end(); ) {
        (*it)->process(tempL, tempR, numSamples);

        for (int i = 0; i < numSamples; ++i) {
            outputL[i] += tempL[i];
            outputR[i] += tempR[i];
        }

        if ((*it)->isFinished()) {
            it = sources.erase(it);
            continue;
        }

        ++it;
    }


}

void SoundEngine::addSource(std::unique_ptr<SoundSource> source) {
    juce::SpinLock::ScopedLockType lock(sourceLock);
    sources.push_back(std::move(source));
}


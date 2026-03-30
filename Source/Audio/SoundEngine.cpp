/*
  ==============================================================================

    SoundEngine.cpp
    Created: 28 Oct 2025 11:07:04pm
    Author:  nicky_hgjk9m6

  ==============================================================================
*/

#include "SoundEngine.h"

SoundEngine::SoundEngine() 
    : calibrationFilter(44100, 1)
{
    sampleRate = 44100.0;
}

void SoundEngine::playSource(std::unique_ptr<SoundSource>&& source) {
    addSource(std::move(source));
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
    calibrationFilter.setSampleRate(newSampleRate);
    hrtfManager.setSampleRate(newSampleRate);
    hrtfManager.loadBinaryData();
    stop();
}

void SoundEngine::setSamplesPerBlockExpected(int samplesPerBlock) {
    tempBuffer.setSize(2, samplesPerBlock);
    sourceBuffer.setSize(2, samplesPerBlock);
}


void SoundEngine::processBlock(float* outputL, float* outputR, int numSamples) {

    sourceBuffer.setSize(2, numSamples, true, false, true);
    tempBuffer.setSize(2, numSamples, true, false, true);

    sourceBuffer.clear();

    float* sourcesLPtr = sourceBuffer.getWritePointer(0);
    float* sourcesRPtr = sourceBuffer.getWritePointer(1);

    float* tempL = tempBuffer.getWritePointer(0);
    float* tempR = tempBuffer.getWritePointer(1);

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
    calibrationFilter.processBlock(sourceBuffer);

    // Copy calibrated block to output, adjusted for calibration SPL offset
    float linearOffset = juce::Decibels::decibelsToGain(calibrationSPLOffset);

    bool clipping = false;
    for (int i = 0; i < numSamples; ++i) {
        outputL[i] = sourcesLPtr[i] * linearOffset;
        outputR[i] = sourcesRPtr[i] * linearOffset;

        clipping = (outputL[i] > 1.0f || outputR[i] > 1.0f || outputL[i] < -1.0f || outputR[i] < -1.0f) 
            ? true : clipping;
    }

    if (clipping) {
        // mute entire block to prevent unsafe levels
        for (int i = 0; i < numSamples; ++i) {
            outputL[i] = 0.0f;
            outputR[i] = 0.0f;
        }
        DBG("Output audio clipped. Muting entire block.");
        if (onClip)
            onClip(true);

        lastBlockClipped = true;
    }
    else if (lastBlockClipped) {
        if (onClip)
            onClip(false);
    }
    
    
}

void SoundEngine::loadCalibration(const juce::File& calibrationFile) {
    if (!calibrationFile.existsAsFile()) {
        DBG("Calibration file not found");
        return;
    }

    calibrationFilter.loadCalibrationProfile(calibrationFile);
    DBG("Calibration loaded");
}

const CalibrationFilter::Metadata& SoundEngine::getCalibrationMetadata() {
    return calibrationFilter.getMetadata();
}

const float SoundEngine::getCalibrationSPLOffset() {
    return calibrationSPLOffset;
}

void SoundEngine::setCalibrationSPLOffset(float offset) {
    calibrationSPLOffset = offset;
}

void SoundEngine::addSource(std::unique_ptr<SoundSource> source) {
    juce::SpinLock::ScopedLockType lock(sourceLock);
    sources.push_back(std::move(source));
}


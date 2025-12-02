/*
  ==============================================================================

    HRTFManager.cpp
    Created: 19 Nov 2025 11:12:50pm
    Author:  nicky_hgjk9m6

  ==============================================================================
*/

#include "HRTFManager.h"

HRTFManager::HRTFManager() {
    audioFormatManager.registerBasicFormats();
}

/*
    Expects HRIRs with name format HRIR_EL_AZ_C.wav
    EL = elevation, AZ = azimuth, C = Channel (L or R)
    Negative elevations: mXX
*/
void HRTFManager::loadBinaryData() {
    irMap.clear();
    for (int i = 0; i < BinaryData::namedResourceListSize; ++i) {
        auto name = juce::String(BinaryData::namedResourceList[i]);
        if (!name.startsWith("HRIR_"))
            continue;
        
        auto tokens = juce::StringArray::fromTokens(name, "_", "");
        if (tokens.size() < 5)
            continue;

        int elevation = tokens[1].startsWith("m") ?
            -tokens[1].substring(1).getIntValue() :
            tokens[1].getIntValue();

        int azimuth = tokens[2].getIntValue();

        
        char channel = tokens[3][0];

        if (channel == 'L') {
            loadIR(name, irMap[elevation][azimuth].left);
        }
        else {
            loadIR(name, irMap[elevation][azimuth].right);
        }

    }

    int nIRs = 0;
    for (auto m : irMap) {
        nIRs += m.second.size();
    }
    DBG(nIRs << " HRIR pairs loaded.");
}

void HRTFManager::setSampleRate(double newSampleRate) {
    sampleRate = newSampleRate;
}

const double HRTFManager::getIRSampleRate() {
    return irSampleRate;
}

juce::AudioBuffer<float>& HRTFManager::getIR(float elevation, float azimuth, int channel) {
    while (azimuth < 0.0f) azimuth += 360.0f;
    while (azimuth >= 360.0f) azimuth -= 360.0f;


    int elInt = static_cast<int>(std::round(elevation));
    int azInt = static_cast<int>(std::round(azimuth));


    // Get closest elevation
    auto it = irMap.lower_bound(elInt);

    if (it == irMap.end())
        it = std::prev(it);
    else if (it != irMap.begin()) {
        int lower = std::prev(it)->first;
        int upper = it->first;
        if (std::abs(elevation - lower) < std::abs(elevation - upper))
            it = std::prev(it);
    }
    DBG("Get IR: el = " << elevation << ". IR el = " << it->first);
    int closestEl = it->first;

    // Get closest azimuth
    auto itAz = irMap[closestEl].lower_bound(azInt);
    if (itAz == irMap[closestEl].end())
        itAz = std::prev(itAz);
    else if (itAz != irMap[closestEl].begin()) {
        int lower = std::prev(itAz)->first;
        int upper = itAz->first;
        if (std::abs(azimuth - lower) < std::abs(azimuth - upper))
            itAz = std::prev(itAz);
    }
    DBG("Get IR: az = " << azimuth << ". IR az = " << itAz->first);
    int closestAz = itAz->first;

    
    // Return correct channel
    if (channel == 0) return itAz->second.left;
    else return itAz->second.right;
}

bool HRTFManager::loadIR(const juce::String& name, juce::AudioBuffer<float>& dest) {

    int size = 0;
    auto* data = BinaryData::getNamedResource(name.toRawUTF8(), size);
    if (data == nullptr || size <= 0) {
        DBG("Could not find HRIR " << name);
        return false;
    }
    auto inputStream = std::make_unique<juce::MemoryInputStream>(data, size, false);

    std::unique_ptr<juce::AudioFormatReader> reader(
        audioFormatManager.createReaderFor(std::move(inputStream)));

    if (reader == nullptr) {
        DBG("Failed to load binary resource " << name);
        return false;
    }

    int length = reader->lengthInSamples;
    dest.setSize(1, length);
    reader->read(&dest, 0, length, 0, true, false);

    DBG("HRIR loaded. Length = " << length << " samples");


    irSampleRate = reader->sampleRate;

    DBG("HRIR sample rate: " << irSampleRate);
    if (sampleRate > 0 && irSampleRate != sampleRate) {
        resampleBuffer(dest, irSampleRate, sampleRate);
        irSampleRate = sampleRate;
        DBG("HRIR resampled to device sample rate: " << irSampleRate);
    }

    return true;
}

void HRTFManager::resampleBuffer(juce::AudioBuffer<float>& buffer, double srcRate, double destRate) {
    if (srcRate == destRate)
        return;

    float ratio = static_cast<float>(srcRate / destRate);
    int newLength = static_cast<int>(std::ceil(buffer.getNumSamples() * destRate / srcRate));

    juce::AudioBuffer<float> resampled(1, newLength);

    const float* in = buffer.getReadPointer(0);
    float* out = resampled.getWritePointer(0);

    interpolator.process(ratio, in, out, newLength);
    interpolator.reset();

    buffer = std::move(resampled);
}

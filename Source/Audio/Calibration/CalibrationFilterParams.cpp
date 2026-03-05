/*
  ==============================================================================

    CalibrationFilterParams.cpp
    Created: 20 Feb 2026 2:38:39pm
    Author:  nicky_hgjk9m6

  ==============================================================================
*/

#include "CalibrationFilterParams.h"

CalibrationFilterParams::BiquadParams::BiquadParams(BiquadParams::Type type, float fc, float gain, float q)
    : type(type), fc(fc), gain(gain), q(q) {
}

std::pair<CalibrationFilterParams, CalibrationFilterParams> 
CalibrationFilterParams::loadFromFile(const juce::File& file) {
    CalibrationFilterParams left;
    CalibrationFilterParams right;

    left.channel = Channel::LEFT;
    right.channel = Channel::RIGHT;

    if (!file.existsAsFile()) {
        DBG("Calibration file not found.  Using defaults.");
        return { left, right };
    }

    juce::var json = juce::JSON::parse(file);

    if (!json.isObject()) {
        DBG("Invalid JSON format in calibration profile. Using defaults.");
        return { left, right };
    }

    auto* root = json.getDynamicObject();
    if (!root) {
        DBG("Invalid JSON format in calibration profile. Using defaults.");
        return { left, right };
    }

    if (root->hasProperty("left_filters")) {
        if (auto* arr = root->getProperty("left_filters").getArray()) {
            for (auto& filter : *arr) {
                left.biquads.push_back(
                    left.parseFilter(filter)
                );
            }
        }
    }

    if (root->hasProperty("right_filters")) {
        if (auto* arr = root->getProperty("right_filters").getArray()) {
            for (auto& filter : *arr) {
                right.biquads.push_back(
                    right.parseFilter(filter)
                );
            }
        }
    }

    return { left, right };
}

const std::vector<CalibrationFilterParams::BiquadParams>&
CalibrationFilterParams::getBiquadParams() {
    return biquads;
}

CalibrationFilterParams::BiquadParams CalibrationFilterParams::parseFilter(const juce::var& filter) {
    BiquadParams params;

    if (auto* obj = filter.getDynamicObject()) {
        params.fc = static_cast<float>(obj->getProperty("fc"));
        params.gain = static_cast<float>(obj->getProperty("gain"));
        params.q = static_cast<float>(obj->getProperty("q"));

        juce::String typeStr = obj->getProperty("type").toString();

        if (typeStr == "PK") 
            params.type = BiquadParams::Type::PK;
        else if (typeStr == "LP")
            params.type = BiquadParams::Type::LP;
        else if (typeStr == "HP")
            params.type = BiquadParams::Type::HP;
        else if (typeStr == "HS")
            params.type = BiquadParams::Type::HS;
        else if (typeStr == "LS")
            params.type = BiquadParams::Type::LS;
    }

    return params;
}




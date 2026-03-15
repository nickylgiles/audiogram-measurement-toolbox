/*
  ==============================================================================

    HTL.h
    Created: 15 Mar 2026 8:18:48pm
    Author:  nicky_hgjk9m6

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

/*
* Converts from an absolute Sound Pressure Level (dBSPL) to a Hearing Threshold Level
* as defined in ISO-226:2003 (Acoustics — Normal equal-loudness-level contours)
* This is only defined up to 12.5kHz.  Beyond this point 
*/
namespace HTL {
    /*
    * Thresholds of hearing from ISO 226:2003 Table 1 (Tf).
    * { frequency (Hz), threshold (dBSPL) }
    */
    constexpr int nThresholds = 29;

    constexpr std::array<std::pair<float, float>, nThresholds> thresholds = { {
        { 20.0f, 78.5f },
        { 25.0f, 68.7f },
        { 31.5f, 59.5f },
        { 40.0f, 51.1f },
        { 50.0f, 44.0f },
        { 63.0f, 37.5f },
        { 80.0f, 31.5f },
        { 100.0f, 26.5f },
        { 125.0f, 22.1f },
        { 160.0f, 17.9f },
        { 200.0f, 14.4f },
        { 250.0f, 11.4f },
        { 315.0f, 8.6f },
        { 400.0f, 6.2f },
        { 630.0f, 3.0f },
        { 800.0f, 2.2f },
        { 1000.0f, 2.4f },
        { 1250.0f, 3.5f },
        { 1600.0f, 1.7f },
        { 2000.0f, -1.3f },
        { 2500.0f, -4.2f },
        { 3150.0f, -6.0f },
        { 4000.0f, -5.4f },
        { 5000.0f, -1.5f },
        { 6300.0f, 6.0f },
        { 8000.0f, 12.6f },
        { 10000.0f, 13.9f },
        { 12500.0f, 12.3f },
    }};

    constexpr std::array<float, nThresholds> logFreqs = { {
        1.3010299956639813f, // log10(20)
        1.3979400086720377f, // log10(25)
        1.4983105537896004f, // log10(31.5)
        1.6020599913279623f, // log10(40)
        1.6989700043360187f, // log10(50)
        1.7993405494535817f, // log10(63)
        1.9030899869919435f, // log10(80)
        2.0f, // log10(100)
        2.0969100130080562f, // log10(125)
        2.2041199826559246f, // log10(160)
        2.3010299956639813f, // log10(200)
        2.3979400086720375f, // log10(250)
        2.4983105537896004f, // log10(315)
        2.6020599913279625f, // log10(400)
        2.6989700043360187f, // log10(500)
        2.7993405494535817f, // log10(630)
        2.9030899869919438f, // log10(800)
        3.0f, // log10(1000)
        3.0969100130080562f, // log10(1250)
        3.2041199826559246f, // log10(1600)
        3.3010299956639813f, // log10(2000)
        3.3979400086720375f, // log10(2500)
        3.4983105537896004f, // log10(3150)
        3.6020599913279625f, // log10(4000)
        3.6989700043360187f, // log10(5000)
        3.7993405494535817f, // log10(6300)
        3.9030899869919438f, // log10(8000)
        4.0f, // log10(10000)
        4.096910013008056f, // log10(12500)
    } };

    struct SplineCoeffs {
        std::array<float, nThresholds> a{};
        std::array<float, nThresholds - 1> b{};
        std::array<float, nThresholds - 1> c{};
        std::array<float, nThresholds - 1> d{};
    };

    // Numerical Analysis, Burden & Faires, Algorithm 3.4
    constexpr SplineCoeffs computeSpline() {
        SplineCoeffs coeffs; // a, b, c, d

        // f(x) = log-frequencies
        std::array<float, nThresholds> x{};

        for (int i = 0; i < nThresholds; ++i) {
            // x[i] = std::log10(thresholds[i].first); 
            // std::log10 cannot be made constexpr in C++17 but will be supported in C++26
            // For now, precomputed log frequencies are in logFreqs.
            x[i] = logFreqs[i];
            coeffs.a[i] = thresholds[i].second;
        }
        
        // Step 1
        std::array<float, nThresholds - 1> h{};

        for (int i = 0; i < nThresholds - 1; ++i) {
            h[i] = x[i + 1] - x[i];
        }

        // Step 2
        std::array<float, nThresholds> alpha{};

        for (int i = 1; i < nThresholds - 1; ++i) {
            alpha[i] = (3.0f / h[i]) * (coeffs.a[i + 1] - coeffs.a[i])
                - (3.0f / h[i - 1]) * (coeffs.a[i] - coeffs.a[i - 1]);
        }

        // Step 3
        std::array<float, nThresholds> l{};
        std::array<float, nThresholds> mu{};
        std::array<float, nThresholds> z{};

        l[0] = 1.0f;
        mu[0] = 0.0f;
        z[0] = 0.0f;

        // Step 4
        for (int i = 1; i < nThresholds - 1; ++i) {
            l[i] = 2.0f * (x[i + 1] - x[i - 1]) 
                - (h[i - 1] * mu[i - 1]);

            mu[i] = h[i] / l[i];

            z[i] = (alpha[i] - h[i - 1] * z[i - 1]) / l[i];
        }

        // Step 5
        l[nThresholds - 1] = 1.0f;
        z[nThresholds - 1] = 0.0f;

        std::array<float, nThresholds> c{};

        c[nThresholds - 1] = 0.0f;

        // Step 6
        for (int j = nThresholds - 2; j >= 0; --j) {
            c[j] = z[j] - mu[j] * c[j + 1];
            coeffs.c[j] = c[j];
            coeffs.b[j] = (coeffs.a[j + 1] - coeffs.a[j]) / h[j]
                - (h[j] * (c[j + 1] + 2.0f * c[j])) / 3.0f;

            coeffs.d[j] = (c[j + 1] - c[j]) / (3.0f * h[j]);
        }

        // Step 7
        return coeffs;
    }

    constexpr SplineCoeffs Spline = computeSpline();

    inline float fromSPL(float spl, float frequency) {
        // Return table value if present.
        const float epsilon = 0.5f;
        for (int i = 0; i < thresholds.size(); ++i) {
            if (std::abs(thresholds[i].first - frequency) < epsilon) {
                return thresholds[i].second;
            }
        }

        // Clamp below range given
        if (frequency < thresholds[0].first) {
            return thresholds[0].second;
        }

        // Interpolate frequencies using natural cubic spline on log of frequency
        float logFreq = std::log10(frequency);

        // Find spline segment containing frequency
        int i = 0;
        while (i < nThresholds - 2 && logFreqs[i + 1] < logFreq)
            ++i;
        
        i = std::min(i, nThresholds - 2);

        // Distance from left knot
        float t = logFreq - logFreqs[i];

        // Compute spline polynomial at t : a + b*t + c*t^2 + d*t^3 
        float tF = Spline.a[i] 
            + Spline.b[i] * t 
            + Spline.c[i] * t * t 
            + Spline.d[i] * t * t * t;

        // dB difference between SPL and threshold
        return spl - tF;
    }

}
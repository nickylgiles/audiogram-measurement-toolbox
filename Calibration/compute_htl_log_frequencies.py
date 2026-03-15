import numpy as np

# Prints the log10 values of the frequencies in ISO 226:2003, Table 1
# So they can be pasted into Source/Audio/Calibration/HTL.h

freqs = [
    20,
    25,
    31.5,
    40,
    50,
    63,
    80,
    100,
    125,
    160,
    200,
    250,
    315,
    400,
    500,
    630,
    800,
    1000,
    1250,
    1600,
    2000,
    2500,
    3150,
    4000,
    5000,
    6300,
    8000,
    10000,
    12500
]

print("constexpr std::array<float, nThresholds> logFreqs = {{")

for f in freqs:
    print(f"    {np.log10(f)}f, // log10({f})")

print("}};")
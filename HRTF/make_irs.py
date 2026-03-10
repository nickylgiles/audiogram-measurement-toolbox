import pyfar
import os
import numpy as np

print(os.getcwd())
# simple script to export the default pyfar HRTFs to wav file
# see https://pyfar.readthedocs.io/en/stable/modules/pyfar.signals.files.html#pyfar.signals.files.head_related_impulse_responses

output_folder = "HRIRs_pyfar"
os.makedirs(output_folder, exist_ok=True)

dfc = True # diffuse field compensation, bets to enable this
sr = 48000 #define your desired sampling rate, e.g., 44100, 48000, 96000

# Load all HRIRs on horizontal plane
h, coords = pyfar.signals.files.head_related_impulse_responses(position="horizontal", diffuse_field_compensation=dfc, sampling_rate=sr)
for i in range(coords.csize):
    az_raw = np.rad2deg(coords.azimuth[i])
    print(f"Index {i}: raw azimuth = {az_raw:.1f}°")

for i in range(coords.csize):

    az = round((np.rad2deg(coords.azimuth[i])- 90) % 360 )# 0 degrees = front, going anticlockwise
    el = round(np.rad2deg(coords.elevation[i]))

    h_l = pyfar.Signal(h.time[i, 0, :], h.sampling_rate)
    h_r = pyfar.Signal(h.time[i, 1, :], h.sampling_rate)

    print(f"HRIR: coordinates: {coords.azimuth[i]} azi, {coords.elevation[i]} elevation, Length: {h.n_samples} samples, Sampling Rate: {h.sampling_rate} Hz")
    filename_l = os.path.join(output_folder, f'HRIR_{el}_{az}_L.wav')
    filename_r = os.path.join(output_folder, f'HRIR_{el}_{az}_R.wav')

    # Write as float to prevent clipping
    pyfar.io.write_audio(h_l,filename_l, subtype="FLOAT")
    pyfar.io.write_audio(h_r,filename_r, subtype="FLOAT")
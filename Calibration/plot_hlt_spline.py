import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns

data = pd.read_csv("HLT_spline.csv")
data = pd.DataFrame({'x' : data["frequency"], 'y' : data["tf"]})

plt.figure(figsize=(8,5))

sns.lineplot(data=data, x='x', y='y')

plt.xlabel("Frequency (Hz)")
plt.ylabel("T$_{f}$ (dB)")
plt.title("Hearing Level Thresholds by Frequency")

plt.grid(True, which="both")

plt.show()
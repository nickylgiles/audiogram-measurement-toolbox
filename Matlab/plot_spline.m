clear; close all;
data = readtable("HLT_spline.csv");

f = data.frequency;
tf = data.tf;

f_iso =  [20, 25, 31.5, 40, 50, 63, 80, 100, 125, 160, 200, 250, 315, 400, 500, 630,800,1000,1250,1600,2000,2500,3150,4000,5000,6300,8000,10000,12500];
tf_iso = [78.5 68.7 59.5 51.1 44.0 37.5 31.5 26.5 22.1 17.9 14.4 11.4 8.6 6.2 4.4 3.0 2.2 2.4 3.5 1.7 -1.3 -4.2 -6.0 -5.4 -1.5 6.0 12.6 13.9 12.3 ];

f_k = [14000 16000];
tf_k = [18.4 40.2];

plot(f,tf, 'Color', [0 0.5 1])
hold on

plot(f_iso,tf_iso, '.', 'MarkerSize',20, 'Color', 'r')

plot(f_k,tf_k, '.', 'MarkerSize',20, 'Color', [0 0.5 0])

xlabel("Frequency (Hz)")
ylabel("T_f (db SPL)", 'Rotation', 0)
set(gca,'XScale','log')
xticks([20, 40, 80, 125, 250, 500, 1e3, 2e3, 4e3, 8e3, 16e3, 20e3]);
xlim([15, 22e3]);

legend('Interpolated', 'ISO 226:2003', 'K&M (2008)')

grid on

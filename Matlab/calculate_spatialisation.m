close all; clc;

sofaFile = 'dtf_nh2.sofa';

s = sofaread(sofaFile);
fs = s.SamplingRate;

% Select 0-elevation indices
sel = find(s.SourcePosition(:,2) == 0);

azimuth = zeros(length(sel),1);
itd = zeros(length(sel),1);
ild = zeros(length(sel),1);

for i = 1:length(sel)
    j = sel(i);
    az = (s.SourcePosition(j, 1));
    el = (s.SourcePosition(j, 2));

    hL = squeeze(s.Numerator(j, 1, :));
    hR = squeeze(s.Numerator(j, 2, :));

    azimuth(i) = az;

    [c, lags] = xcorr(hR, hL);

    [~, idx_max] = max(c);
    lag = lags(idx_max);

    itd(i) = lag/fs * 1e3;
    
    rmsL = sqrt(mean(hL.^2));
    rmsR = sqrt(mean(hR.^2));

    ild_db(i) = 20*log10(rmsL/rmsR);


end

[azimuth, idx] = sort(azimuth);
itd = itd(idx);
ild = ild_db(idx);

% Approximation of ILD - van Opstal (2016) ILD = 0.18 sqrt(f) * sin(theta)
theta = deg2rad(azimuth);
f = 10000;
ild_approx = 0.18 * sqrt(f) * sin(theta);

% Approximation of ITD - Woodworth (1938)
r = 0.0875; % Assuming head radius = 8.75cm (Aaronson & Hartmann, 2014)
c = 343;
itd_approx = zeros(size(theta));
itd_approx(azimuth <= 90) = (r/c) * (theta(azimuth <= 90) + sin(theta(azimuth <= 90))) .* 1e3;

itd_approx(azimuth >= 270) = (r/c) * ((theta(azimuth >= 270)-2*pi) + sin(theta(azimuth >= 270))) .* 1e3;
woodworth_idx = (azimuth >= 270 | azimuth <= 90); % only valid in front plane



figure
plot(azimuth, itd, '.-')
hold on
plot(azimuth(azimuth <= 90), itd_approx(azimuth <= 90), 'Color','r')
plot(azimuth(azimuth >= 270), itd_approx(azimuth >= 270), 'Color','r')
xlabel('Azimuth (deg)')
ylabel('ITD (ms)')
title('ITD vs Azimuth (0° elevation)')
grid on
xlim([0 360])
xticks([0:45:360]);
legend('HRIRs', 'Theoretical')

figure
plot(azimuth, ild, '.-')
hold on;
plot(azimuth, ild_approx, 'Color','r')
xlabel('Azimuth (deg)')
ylabel('ILD (dB)')
title('ILD vs Azimuth (0° elevation)')
grid on
xlim([0 360])
xticks([0:45:360]);
legend('HRIRs', 'Theoretical')



% Compute correlation and MSE for ILD and ITD vs theoretical
r_itd = corr(itd(woodworth_idx), itd_approx(woodworth_idx))
r_ild = corr(ild, ild_approx)

rmse_itd = sqrt(mean((itd(woodworth_idx) - itd_approx(woodworth_idx)).^2))
rmse_ild = sqrt(mean((ild - ild_approx).^2))

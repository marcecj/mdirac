clear;
close all;

fs = 44100;
% sig = rand(1,fs);
sig = sin(2*pi*100*linspace(0,1,fs));
a = TimeStretchDirac(sig,fs,1.15,1);
b = TimeStretchDirac([sig;sig]);

% figure;
% subplot(2,1,1);
% plot(sig);
% set(gca,'XLim',[0 fs*1.15]);
% subplot(2,1,2);
% plot(a);
% set(gca,'XLim',[0 fs*1.15]);

% b = TimeStretchDirac('test.wav');

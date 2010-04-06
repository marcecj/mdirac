clear;
close all;

fs = 44100;
% sig = rand(1,fs);
sig = sin(2*pi*100*linspace(0,1,fs));
sig2 = cos(2*pi*100*linspace(0,1,fs));
% a = TimeStretchDirac(sig,fs,1.15,1);
% b = TimeStretchDirac([sig;sig;sig]);
a = mDirac(sig,fs,1.15,1);
b = mDirac([sig;sig;sig],fs,1.15,1);

figure;
subplot(2,1,1);
plot(sig);
set(gca,'XLim',[0 fs*1.15]);
subplot(2,1,2);
plot(a);
set(gca,'XLim',[0 fs*1.15]);

figure;
subplot(3,1,1);
plot(b(1,:));
subplot(3,1,2);
plot(b(2,:));
subplot(3,1,3);
plot(b(3,:));

figure;
subplot(3,1,1);
plot(b(1,:)-b(2,:));
subplot(3,1,2);
plot(b(1,:)-b(3,:));
subplot(3,1,3);
plot(b(2,:)-b(3,:));

% b = TimeStretchDirac('test.wav');

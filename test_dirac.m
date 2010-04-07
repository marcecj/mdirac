clear;
close all;

% fs = 44100;
% sig = rand(1,fs);
% sig = sin(2*pi*100*linspace(0,1,fs));
% sig2 = cos(2*pi*100*linspace(0,1,fs));
% a = TimeStretchDirac(sig,fs,1.15,1);
% b = TimeStretchDirac([sig;sig;sig]);
% a = mDirac(sig,fs,1.15,1);
% b = mDirac([sig;sig;sig],fs,1.15,1);

[input, fs] = wavread('test.wav');
output = mDirac(input,fs);

figure;
subplot(2,1,1);
plot(output(1,:),'r');
hold on;
plot(input(1,:));
subplot(2,1,2);
plot(output(2,:),'r');
hold on;
plot(input(2,:));

% figure;
% subplot(2,1,1);
% plot(sig);
% set(gca,'XLim',[0 fs*1.15]);
% subplot(2,1,2);
% plot(a);
% set(gca,'XLim',[0 fs*1.15]);

% figure;
% subplot(3,1,1);
% plot(b(1,:));
% subplot(3,1,2);
% plot(b(2,:));
% subplot(3,1,3);
% plot(b(3,:));

% figure;
% subplot(3,1,1);
% plot(c(1,:));
% subplot(3,1,2);
% plot(c(2,:));
% subplot(3,1,3);
% plot(c(3,:));

% figure;
% subplot(3,1,1);
% plot(b(1,:)-b(2,:));
% subplot(3,1,2);
% plot(b(1,:)-b(3,:));
% subplot(3,1,3);
% plot(b(2,:)-b(3,:));

% b = TimeStretchDirac('test.wav');

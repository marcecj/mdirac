% Simple script for testing mDirac/TimeStretchDirac

clear;
close all;

%
% test 1: audio file (2 channels, 44100 Hz)
%

[input, fs] = wavread('test.wav');
output = mDirac(input,fs,1.5,3);

figure;
subplot(2,1,1);
plot(output(:,1),'r');
hold on;
plot(input(:,1));
subplot(2,1,2);
plot(output(:,2),'r');
hold on;
plot(input(:,2));

%
% test 2: sinusoids (to test if multichannel stretching works correctly or if
% the channels get mixed up)
%

fs = 44100;
ts      = 1.5;
sig_len = 1;
t_in = linspace(0,sig_len,fs);
t_out = linspace(0,ceil(sig_len*ts),ceil(fs*ts));
sig = sin(2*pi*100*t_in)';

% a = TimeStretchDirac(sig,fs,1.15,1);
% b = TimeStretchDirac([sig;sig;sig]);
a = mDirac(sig,fs,ts,1);
b = mDirac([sig sig sig],fs,ts,3);

% plot input and output in seperate subplots
figure;
subplot(2,1,1);
plot(t_in,sig);
set(gca,'XLim',[0 t_out(end)]);
subplot(2,1,2);
plot(t_out,a);
set(gca,'XLim',[0 t_out(end)]);

% plot differences between channels (should all be zero)
figure;
subplot(3,1,1);
plot(b(:,1)-b(:,3));
subplot(3,1,2);
plot(b(:,2)-b(:,3));
subplot(3,1,3);
plot(b(:,3)-b(:,2));

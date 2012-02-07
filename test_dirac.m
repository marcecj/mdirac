% Simple script for testing mDirac and TimeStretchDirac

clear;
close all;

addpath('mDirac');

%
% test 1: audio file (2 channels, 44100 Hz)
%

[input, fs] = wavread('test.wav');
output      = mDirac(input,fs,1,1.5,3,3);

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
% test 2: sinusoids (to test if the interleaved array indices get calculated
% correctly for multichannel stretching)
%

fs      = 44100;
ts      = 1.5;
sig_len = 1;
t_in    = linspace(0,sig_len,fs);
t_out   = linspace(0,ceil(sig_len*ts),ceil(fs*ts));
sig     = sin(2*pi*100*t_in)';

a = mDirac(sig,fs,1,ts,1,1);
b = mDirac([sig sig sig],fs,1,ts,3,3);

% plot input and output in seperate subplots
figure;
subplot(2,1,1);
plot(t_in,sig);
set(gca,'XLim',[0 t_out(end)]);
subplot(2,1,2);
plot(t_out,a);
set(gca,'XLim',[0 t_out(end)]);

% plot differences between channels (should all be zero!)
figure;
subplot(3,1,1);
plot(b(:,1)-b(:,2));
subplot(3,1,2);
plot(b(:,2)-b(:,3));
subplot(3,1,3);
plot(b(:,3)-b(:,1));

%
% test 3: test TimeStretchDirac wrapper m-file
%

output2 = TimeStretchDirac(input);
output2 = TimeStretchDirac(input,fs);
output2 = TimeStretchDirac(input,fs,1.5);
output2 = TimeStretchDirac(input,fs,1.5,3);
output2 = TimeStretchDirac(input,fs,1.5,3,3);

figure;
subplot(2,1,1);
plot(output2(:,1),'r');
hold on;
plot(input(:,1));
subplot(2,1,2);
plot(output2(:,2),'r');
hold on;
plot(input(:,2));

output3 = TimeStretchDirac('test.wav');
output3 = TimeStretchDirac('test.wav',fs);
output3 = TimeStretchDirac('test.wav',fs,1.5);
output3 = TimeStretchDirac('test.wav',fs,1.5,3);
output3 = TimeStretchDirac('test.wav',fs,1.5,3,3);

figure;
subplot(2,1,1);
plot(output3(:,1),'r');
hold on;
plot(input(:,1));
subplot(2,1,2);
plot(output3(:,2),'r');
hold on;
plot(input(:,2));

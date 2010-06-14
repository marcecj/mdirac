function OutData = mDirac(InData, fs, TimeStretchFactor, Mode, Quality)
% A function to change duration of audio data without changing the pitch by
% using the dirac library.
%
% Usage: OutData = mDirac(InData,fs,TimeStretchFactor, Mode)
% Input parameters:
%      InData:            a vector containing data (Len x Chns)
%      fs:                the sampling frequency
%      TimeStretchFaktor: the change factor range (0.5 ... 2) (0.5 means 2 times faster)
%      Mode:              the Dirac mode (precision definition between time and
%                         frequency). Valid values are integers from 0
%                         (Preview), 1 (highest time precision) to 5 (highest
%                         frequency precision).
%      Quality:           the Dirac mode (precision definition between time and
%                         frequency). Valid values are between 0 (preview) and 4
%                         (best quality).
% Output parameters:
%      OutData:           the new data vector (Len x Chns)
%
% Thanks to www.dspdimension.com and Stephan M. Bernsee for the publication if
% the DiracLE library (see www.dspdimension.com).

% (c) Marc Joliet, Jade-Hochschule, Institut für Hoertechnik und Audiologie,
% www.hoertechnik-audiologie.de

% 12th April, 2010
% History: 1.0 - finished version
% Licence: see file 'LICENCE'

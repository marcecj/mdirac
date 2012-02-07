function OutData = TimeStretchDirac(InData,fs,TimeStretchFactor, Mode, Quality)
% A function to change duration of audio data without changing the pitch by
% using the dirac library.
%
% Usage: OutData = TimeStretchDirac(InData, fs, TimeStretchFactor, Mode)
% Input parameters:
%      InData:            Can be a string describing a wave-File or a vector
%                         containing Data (Len x Chns)
%      fs:                sampling frequency: ignored if InData is a string
%      TimeStretchFactor: the time stretch factor (0.5 ... 2) (0.5 means 2 times
%                         faster)
%      Mode:              the Dirac mode (precision definition between time and
%                         frequency). Valid values are integers from 0
%                         (Preview), 1 (highest time precision) to 5 (highest
%                         frequency precision).
%      Quality:           the Dirac Quality setting. Valid values are between 0
%                         (preview) and 4 (best quality).
% Output parameters:
%      OutData:           the new data vector (Len x Chns)
%
% Thanks to www.dspdimension.com and Stephan M. Bernsee for the publication if
% the DiracLE library (see www.dspdimension.com)

% (c) Marc Joliet and Jörg Bitzer, Jade-Hochschule, Institut für Hoertechnik und
% Audiologie, www.hoertechnik-audiologie.de
% 29th April, 2010, 1.0 - first properly working version
% Licence:  see file 'LICENCE'

need_to_resample = false;

if nargin < 1
    error('We need at least one input argument!');
end

if nargin < 2 && isfloat(InData)
    disp('Sampling rate not given! Defaulting to 44.1 kHz');
    fs = 44100;
end

if nargin < 3
    disp('Time stretch factor not given, defaulting to 1.0.');
    TimeStretchFactor = 1.0;
end

if nargin < 4
    disp('Dirac Mode not given, defaulting to ''Preview''.');
    Mode = 0;
end

if nargin < 5
    disp('Dirac Quality not given, defaulting to ''Preview''.');
    Quality = 0;
end

if ischar(InData)
    [InData, fs] = wavread(InData);
end

if fs ~= 44100 && fs ~= 48000
    disp(['Sampling rate not compatible with Dirac! You''re data will' ...
    'automatically be resampled to 48 kHz before and back to ' num2str(fs) ...
    ' after stretching.']);
    need_to_resample = true;
end

if need_to_resample
    InData  = resample(InData, 48000, fs);
    OutData = mDirac(InData, 48000, 1, TimeStretchFactor, Mode, Quality);
    OutData = resample(OutData, fs, 48000);
else
    OutData = mDirac(InData, fs, 1, TimeStretchFactor, Mode, Quality);
end

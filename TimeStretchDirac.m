function OutData = TimeStretchDirac(InData,fs,TimeStretchFaktor, Mode)
% function to change duration of Audiodata without changing the pitch
% by using the dirac library
% Usage: OutData = TimeStretchDirac(InData,fs,TimeStretchFaktor, Mode)
% InParamter:   
%      InData:      Could be a string and describe a wave-File
%                   or a vector containing Data (Len x Chns)
%      fs:          Samplingfrequency: Ignored if InData is a string
%      TimeStretchFaktor: The change factor Range (0.25 ... 4) (0.25 means 4 times faster)
%      Mode:        The dirac mode, (Presicion definition between time and frequency)
% OutParam:
%      OutData:     The new data vector (Len x Chns)
% Thanks to www.dspdimension.com and Stephan M. Bernsee for the 
%           publication if the DiracLE library (see www.dspdimension.com)

% (c) Author, Jade-Hochschule, Institut für Hoertechnik und Audiologie,
%             www.hoertechnik-audiologie.de
% Datum, Versions-History
% Licence:  see end of file (MIT licence)


% Um dieses Interface zu supporten muss das Matlab File folgende Dinge tun.

% Die Daten an die Dirac LE Samplingraten 44100 und 48000 anpassen.
% Mehrkanalige Daten in Einzelkanäle aufspalten.
% und natürlich die Dirac Bibliothek ansprechen

need_to_resample = 0;

if nargin < 1
    error('We need at least one input!');
end

if nargin < 2
    disp('Sampling rate not given! Defaulting to 44.1 kHz');
    fs = 44100;
elseif (fs ~= 44100 && fs ~= 48000) && isfloat(InData)
    disp('Sampling rate not compatible with Dirac! You''re data will automatically be resampled before and after stretching.');
    need_to_resample = 1;
end

if nargin < 3
    disp('Time stretch factor not given, defaulting to 1.15.');
    TimeStretchFaktor = 1.15;
end

if nargin < 4
    disp('Dirac Mode not given, defaulting to ''Preview''.');
    Mode = 0;
end

if ischar(InData)
    % [InData, fs] = wavread(InData);
    [InData] = wavread(InData);
    fs = 44100; % hack for testing under Gentoo
end

if need_to_resample
    InData = resample(InData, 48000, fs);
end

OutData = mDirac(InData, fs, TimeStretchFaktor, Mode);

if need_to_resample
    OutData = resample(OutData, fs, 48000);
end

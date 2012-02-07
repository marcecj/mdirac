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


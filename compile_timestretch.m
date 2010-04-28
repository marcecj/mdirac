% Simple script for compiling mDirac. Afterward compilation a test
% script is run.

if strcmp(computer, 'PCWIN') || strcmp(computer, 'PCWIN64')
    mex -LWin -lDiracLE -Iinclude mDirac.c
elseif strcmp(computer, 'GLNX86') || strcmp(computer, 'GLNXA64')
    mex -LLinux -lDirac -Iinclude mDirac.c
elseif strcmp(computer, 'MACI') || strcmp(computer, 'MACI')
    mex -LMac -lDiracLE -Iinclude mDirac.c
end

% run test script
test_dirac

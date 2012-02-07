% Simple script for compiling msndfile.

%
%% compile
%

out_dir = 'build';

if strcmp(computer, 'PCWIN') || strcmp(computer, 'PCWIN64')
    mex -Lsrc/Win -lDiracLE -Isrc/include src/mDirac.c -outdir 'build'
elseif strcmp(computer, 'GLNX86') || strcmp(computer, 'GLNXA64')
    mex -Lsrc/Linux -lDirac -Isrc/include src/mDirac.c -outdir 'build' CFLAGS='\$CFLAGS -std=c99'
elseif strcmp(computer, 'MACI') || strcmp(computer, 'MACI')
    mex -Lsrc/Mac -lDiracLE -Isrc/include src/mDirac.c -outdir 'build' CFLAGS='\$CFLAGS -std=c99'
end

if ~exist(out_dir, 'dir')
    mkdir('.', out_dir);
end

%
%% install
%

inst_dir = 'mDirac/';
if ~exist(inst_dir, 'dir')
    mkdir('.', inst_dir);
end

copyfile(fullfile(out_dir, ['mDirac.' mexext]),   inst_dir)
copyfile(fullfile('src',   'mDirac.m'),           inst_dir)
copyfile(fullfile('src',   'TimeStretchDirac.m'), inst_dir)

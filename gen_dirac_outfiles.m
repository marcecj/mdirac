audio_dir = fullfile('audiofiles', 'Speaker') ;
out_dir = fullfile('audiofiles', 'dirac');
audio_files = dir(audio_dir);
fnames = {'f_t_n.wav', 'm_mj_n.wav', 'm_fh_n.wav', 'm_tb_n.wav', 'f_js_n.wav'};
% fnames = {'f_t_n.wav'};

stretch_facs = {[13.15/15.15, 22.58/15.15], ...
                [7.48/13.15, 28.05/13.15], ...
                [32.9/45.35, 58.9/45.35], ...
                [32.3/49.5, 66.2/49.5], ...
                [1/1.307, 1/0.708]};

for kk=1:length(audio_files)
    idx_mask = strcmp(audio_files(kk).name, fnames);
    
    if(sum(idx_mask) == 1)
        infname = fullfile(audio_dir, fnames{idx_mask});
        [x, fs] = wavread(infname);

        disp(sprintf('Stretching %s...', fnames{idx_mask}))

        for ii=1:2
            y = TimeStretchDirac(x, fs, stretch_facs{idx_mask}(ii), 1, 3);
            outfname = [out_dir filesep fnames{idx_mask}(1:end-4) '_dirac_' num2str(round(stretch_facs{idx_mask}(ii)*100)) '.wav'];
            wavwrite(y, fs, 16, outfname);
        end
        disp('done.')
    end
end

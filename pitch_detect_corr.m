function [ pitch ] = pitch_detect_corr( frame )

fs=8e3;
len=length(frame);
autocorr=xcorr(frame);
fmin_phys=50;
fmax_phys=400;
autocorr=autocorr(len:2*len-1); %% since autocorr is symmetric
relevant_range=autocorr(((fs/fmax_phys)+1):((fs/fmin_phys)-1));
pitch=find(relevant_range==max(relevant_range));

end


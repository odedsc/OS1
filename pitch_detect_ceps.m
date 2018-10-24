function [ pitch ] = pitch_detect_ceps( frame )

fs=8e3;
S=20*log10(abs(fft(frame)));
fmin=50; fmax=400;
C=real(ifft(S));
T=1/fs;
C(1:1/fmax/T)=0;
C(1/fmin/T:length(C))=0;
pitch=1/((find(C==max(C)))*T);

end


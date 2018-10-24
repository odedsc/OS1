clear all;
close all;
clc;

%% Q1A:
f1=1e3; f2=2e3; f3=3e3;
fs=10e3;
t=(1/fs).*(0:255);
x=sin(2*pi*f1*t)+sin(2*pi*f2*t)+sin(2*pi*f3*t);
hamming_x=x'.*hamming(256);
dft_hammingX=fftshift(fft(hamming_x));
DFT_signal=20*log10(abs(dft_hammingX));
A_f_vector=linspace(-fs/2,fs/2,length(DFT_signal));

figure(1);
plot(A_f_vector,DFT_signal);
title('X DFT Amplitude vs Frequency with fs=10KHz');
xlabel('Frequency [Hz]');
ylabel('DFT Amplitude(dB)');
axis([0 fs/2 -30 40]);

%% Q1B:
f1=1e3; f2=2e3; f3=9e3;
fs=10e3;
t=(1/fs).*(0:255);
x=sin(2*pi*f1*t)+sin(2*pi*f2*t)+sin(2*pi*f3*t);
hamming_x=x'.*hamming(256);
dft_hammingX=fftshift(fft(hamming_x));
DFT_signal=20*log10(abs(dft_hammingX));
A_f_vector=linspace(-fs/2,fs/2,length(DFT_signal));

figure(2);
plot(A_f_vector,DFT_signal);
title('X DFT Amplitude vs Frequency with fs=10KHz');
xlabel('Frequency [Hz]');
ylabel('DFT Amplitude(dB)');
axis([0 fs/2 -50 40]);

%% Q7:
f=0.5e3;
fs=10e3;
t=(1/fs).*(1:200);
x=sin(2*pi*f*t);
CL=0.65*max(abs(x));
figure(3);
subplot(2,1,1);
plot(t,x);
title('x as function of time before clipping');
xlabel('time(sec)');
ylabel('amplitude');
subplot(2,1,2);
plot(t,clip(x)); 
title('x as function of time after clipping');
xlabel('time(sec)');
ylabel('amplitude');
axis([0 0.02 -1 1]);

%% Q8:
figure(4);
stem(Coeff_filter);
title('filter coefficients');
xlabel('n');
ylabel('h[n]');
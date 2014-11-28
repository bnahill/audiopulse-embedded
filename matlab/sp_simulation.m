clear all;clc;close


%Define simulation parameters
Fs=48000;  %Original sampling frequency
T=8;       %Recording time in seconds
F1=2000;   %First stimulus
F2=2400;   %Second stimulus
F3=2*F1-F2; %Expected Response
A=65;      %Stimulus amplitude in dBu
L=512;     %Averaging windowLength L <= Fs*T, total windows ~ Fs*T/L
step=L/2;  %Amount of over in samples, (step=L/2 -> 50%, step=L -> 0%)
aproxFreq=false;  %If this is set to true, will approximate F1 and F2 such that FFT leakage due to choice of L is minimized (see below).
noisedB=60; 
%End of simulation parameters %

%Decimation parameters
decFs=16000;      %Final frequency
b=fir1(28, 6/24); % Filter parameters for low pass to be applied before decimation

%Note: In order to avoid leakage due to the number of samples in the FFT,
%the frequency should be a multiple of 2*pi/L (See See Hayes 1999 pg 239
%for details). 
if(aproxFreq) 
     Fstep=Fs/L;
     F1=round(F1/Fstep)*Fstep;
     F2=round(F2/Fstep)*Fstep;
     F3=2*F1-F2;
end


tm=[0:1/Fs:T];
N=length(tm);
tone1=sin(2*pi*F1*tm);
tone2=sin(2*pi*F2*tm);
tone3=sin(2*pi*F3*tm);
noise=randn(1,N);
RMS_TONE=sqrt(2);
RMS_NOISE=sqrt(mean(noise.^2));

%Normalization factor
gain=RMS_TONE*10^(A/20);
ngain=RMS_NOISE.*10^(noisedB/20);

%Normalize the tones and generate the stimulus
tone1=tone1.*gain;
tone2=tone2.*gain;
tone3=tone3.*gain;
noise=noise.*ngain;
x= tone1 + tone2 + tone3 + noise;


%Filter and decimate the signal
if(~isempty(decFs))
    K=Fs/decFs;
    x=filter(b,1,x);
    x=x(1:K:end);
    Fs=decFs;
    tm=[0:1/Fs:T];
    N=length(tm);
end


M=floor(N/step);
if(M*step > L)
    M=M-1;
end


%Define window types  to compare
WIN={'rectwin','blackman','hamming','nuttallwin'};
clr=['brkg'];
NWIN=length(WIN);

for nwin=1:NWIN


%Define windows and get its RMS
eval(['win=' WIN{nwin} '(L);'])
POW_WIN=mean(abs(win).^2);

%Get running average of FFTs (we are estimating via WELCH's method)
X=zeros(1,L);
for m=1:M
    ind1=(m-1)*step+1;
    Xtmp=  ( abs(fft(x(ind1:ind1+L-1).*win')).^2 )./(L*L*POW_WIN/2) ;
    X= ( (m-1).*X + Xtmp ) ./m;
end

%Convert to dB
X=10*log10(X);
%Get Frequency axis
freq = 0:Fs/length(X):Fs;
freq(end)=[];

%Get stats
[maxX,indMax]=max(X);
RMS_X=20*log10(sqrt(mean(x.^2)));

%Plot everything
plot(freq,X,clr(nwin)); hold on
grid on
title(['Fmax= ' num2str(maxX) ' dB, Amp= ' num2str(A) ' dB, x RMS (dB) = ' num2str(RMS_X) ' , Fmax= ' num2str(freq(indMax))])
xlabel('Frequency (Hz)')
ylabel('Power/Frequency (dB/Hz)')
xlim([0 4000])

end
legend(WIN(:))


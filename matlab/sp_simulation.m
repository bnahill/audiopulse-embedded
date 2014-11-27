clear all;clc;close

%Define simulation parameters

Fs=48000;  %Original sampling frequency
T=8;       %Recording time in seconds
F1=1000;   %First stimulus
F2=2000;   %Second stimulus
A=65;      %Stimulus amplitude in dBu
L=24*10;%384;%Fs*T/1000;   %Averaging windowLength L <= Fs*T, total windows ~ Fs*T/L
step=L/2;  %Amount of over in samples, (step=L/2 -> 50%, step=L -> 0%)
magicOffset=0;
%End of simulation parameters %


tm=[0:1/Fs:T];
tone1=sin(2*pi*F1*tm);
tone2=sin(2*pi*F2*tm);
RMS_TONE=sqrt(2);


%Define windows and get its RMS
win=rectwin(L);%nuttallwin(L);
POW_WIN=mean(abs(win).^2);


%Normalize gain by the RMS of TONE and WINDOW
gain=RMS_TONE*10^(A/20);

%Normalize the tones and generate the stimulus
tone1=tone1.*gain;
tone2=tone2.*gain;
x=tone1+tone2;

N=length(tm);
M=floor(N/step);
if(M*step > L)
    M=M-1;
end

%Get running average of FFTs (we are estimating via WELCH's method)
X=zeros(1,L);
for m=1:M
    ind1=(m-1)*step+1;
    Xtmp=  ( abs(fft(x(ind1:ind1+L-1).*win')).^2 )./(L*L*POW_WIN/2) ;
    X= ( (m-1).*X + Xtmp ) ./m;
end

%Convert to dB
X=10*log10(X)-magicOffset;
%Get Frequency axis
freq = 0:Fs/length(X):Fs;
freq(end)=[];

%Get stats
maxX=max(X);
RMS_X=20*log10(sqrt(mean(x.^2)));

%Plot everything
plot(freq,X)
grid on
title(['Fmax= ' num2str(maxX) ' dB, Amp= ' num2str(A) ' dB, x RMS (dB) = ' num2str(RMS_X)])
xlabel('Frequency (Hz)')
ylabel('Power/Frequency (dB/Hz)')
xlim([0 4000])

 

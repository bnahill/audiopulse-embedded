clear all;clc;close


Fs=16000;
F1=1000;
F2=2000;
L=512;
A=10^(65/20); % 10*log10(x)=y -> x=10^(y/10)
tm=[0:1/Fs:8];
tone1=A*sin(2*pi*F1*tm);
tone2=A*sin(2*pi*F2*tm);
N=length(tm);
L=512;
step=L/2;
M=floor(N/step);
if(M*step > L)
   M=M-1; 
end
x=tone1+tone2;
X=zeros(L,M);
win=nuttallwin (L);
for m=1:M
    ind1=(m-1)*step+1;
   X(:,m)=x(ind1:ind1+L-1).*win';
end
%Take FFT and estimate power espetrum
x=mean(X,2);
N = length(x);
xdft = fft(x);
xdft = xdft(1:N/2+1);
psdx = (1/(Fs*N)) * abs(xdft).^2;
psdx(2:end-1) = 2*psdx(2:end-1);
freq = 0:Fs/length(x):Fs/2;

plot(freq,10*log10(psdx))
grid on
title('Periodogram Using FFT')
xlabel('Frequency (Hz)')
ylabel('Power/Frequency (dB/Hz)')
xlim([0 4000])


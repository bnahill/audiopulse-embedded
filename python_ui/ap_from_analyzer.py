# -*- coding: utf-8 -*-
"""
Spyder Editor

This is a temporary script file.
"""

import numpy as np
import scipy.signal
import scipy as sp
import pylab
from multiprocessing import Pool

fname = '/home/ben/Desktop/untitled8_ben_high.txt'
fname = '/home/ben/Desktop/untitled9_coupler_low.txt'
fname = '/home/ben/Desktop/untitled_vin.txt'

part = fname.rpartition('/')
outfile = part[0] + '/processed_' + part[2].rpartition('.')[0] + '.npz'
    
mic_channel = 2
fs=48000.0
window = fs / 0.5

filedata = np.loadtxt(fname, skiprows=1, delimiter=',', unpack=False)
datapcm = np.array([np.int32(x[2]) for x in filedata if int(x[1]) == mic_channel], dtype=np.float32)

f1 = 2000.0
f2= 2440.0

d = [(i+2)*f1 - (i+1)*f2 for i in range(4)]

#%%


    

# (0.278 / 631549): Measured gain from codec ADC (differential) at 1kHz
#   Could be more accurate but should be close
# 46.78: Gain of preamp at 1kHz
datav = datapcm * (0.278 / 631549) / 46.76

pylab.plot(datav[:512])
pylab.show()

(freq,psdv) = scipy.signal.welch(datav, fs=48000, window='nuttall', nperseg=window, noverlap=window/2)
psddbv = 10*np.log10((psdv)/1)
psddbspl = psddbv + (94 + 28)
pylab.plot(freq[:window/8], psddbspl[:window/8])
pylab.show


#%%

def goertzel(x, k):
    N = len(x)
    n = np.arange(N, dtype=x.dtype)
    #print(k)
    return np.sum(x * np.exp(-2j*np.pi*n*k/N))

def goertzel(x, k):
    coeff = 2*np.cos(2*np.pi*k)
    sprev = np.float32(0)
    sprev2 = np.float32(0)
    for sample in x:
        s = sample + coeff * sprev - sprev2
        sprev2 = sprev
        sprev = s
    return sprev2**2 + sprev**2 - coeff * sprev * sprev2
    
        

windowed = datav * scipy.signal.windows.hann(len(datav))
def proc_f(f):
    center_k = int(np.round(f*len(datav)/fs))
    center_k = float(f)/fs
    print("F: {} -> K: {}".format(f, center_k))
    nbins = 32
    ks = np.arange(center_k - nbins, center_k + nbins+1)
    # Generate a sequence of k values spaced by 1/2Hz
    # This may
    spacing = 0.5
    ks = (np.arange(nbins * 2, dtype=np.float32) - nbins) / (fs / spacing) + center_k
    freqs = np.float32(ks*fs)
    return (freqs, np.array([10*np.log10(np.abs(goertzel(windowed, k))) for k in ks], dtype=np.float32))

#a = p.map(proc_f, [f1, f2, d1, d2, d3, d4, d5])
goertzeled_select_db = [proc_f(f) for f in  [f1, f2] + d]



#vals = [(f, np.abs(goertzel(datav, k))**2) for (f, k) in zip(freqs, ks)]
#%%
ks = np.arange(len(datav)/10)
freqs = ks / (float(len(datav))/fs)

windowed = datav * scipy.signal.windows.hann(len(datav))
def proc_k(k):
    return np.abs(goertzel(windowed, k))**2
p = Pool(4)
a = p.map(proc_k, ks)

goertzeled_full_db = 10*np.log10(a)

#%%
np.savez(outfile, {
    'datav' : datav,
    'f1'    : f1,
    'f2'    : f2,
    'goertzeled_select_db' : goertzeled_select_db,
    'goertzeled_full_db' : goertzeled_full_db,
})
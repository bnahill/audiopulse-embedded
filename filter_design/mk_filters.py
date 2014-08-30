#!/usr/bin/env python
"""
This file is part of AudioPulse Embedded

AudioPulse is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

AudioPulse is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with AudioPulse.  If not, see <http://www.gnu.org/licenses/>.
"""

""" mk_filters.py

@brief  A tool to generate various filter coefficients in AudioPulse
@author Ben Nahill<bnahill@gmail.com>
@date   August 30, 2014
"""

from scipy import signal
import numpy as np
import pylab

pylab.close('all')

fs0 = 48000.0
fs1 = 16000.0

frac_bits = 29

# Stopband frequencies
fstop = np.array([100, 8000], dtype=float)
fpass = np.array([1000, 6000], dtype=float)

wstop = (fstop / (fs0 / 2))
wpass = (fpass / (fs0 / 2))

print("Wstop:")
print(wstop)
print("Wpass:")
print(wpass)

(z, p, k) = signal.iirdesign(wp=wpass, ws=wstop, gpass=1.0, gstop=40.0,
                            ftype='ellip', analog=False, output='zpk')

zeros = list()
poles = list()
# Break into distinct pole pairs and zero pairs
for i in range(len(z) / 2):
    zero = z[(i / 2) * 4 + i % 2]
    pole = p[(i / 2) * 4 + i % 2]
    # And move all to upper half-plane
    zeros.append(np.real(zero) + abs(np.imag(zero))*1j)
    poles.append(np.real(pole) + abs(np.imag(pole))*1j)

def complex_cmp(x, y):
    if abs(x) > abs(y):
        return 1
    if abs(x) == abs(y):
        return 0
    return -1

# Sort by magnitude
zeros.sort(cmp=complex_cmp, reverse=True)
poles.sort(cmp=complex_cmp, reverse=True)

colors = "rbgyo"

print("")
print("***********************************************************************")
print("Assigning pole-zero pairs")
print("***********************************************************************")


pairs = list()
qpairs = list()
fqpairs = list()
# Pick off descending poles by magnitude and pair with nearest zero
for (color, p) in zip(colors[:len(poles)], poles):
    def distance_cmp(x, y):
        return complex_cmp(x - p, y - p)

    nearest_zero = sorted(zeros, distance_cmp)[0]
    zeros.remove(nearest_zero)

    pole = np.complex128([p, np.conj(p)])
    zero = np.complex128([nearest_zero, np.conj(nearest_zero)])

    (b, a) = signal.zpk2tf(zero, pole, 1.0 if zeros else k)


    qa = np.int32(a * 2**frac_bits)
    qb = np.int32(b * 2**frac_bits)

    (fqz, fqp, fqk) = signal.tf2zpk(b.astype(np.float_) / 2**frac_bits,
                              a.astype(np.float_) / 2**frac_bits)

    pairs.append((pole, zero))
    qpairs.append((qb, qa))
    fqpairs.append((fqp, fqz))
    print("Pair: ")
    print("\tPoles: " + str(pole))
    print("\tZeros: " + str(zero))

# Draw the pole-zero plot
print("Drawing unquantized pole-zero plot")
def draw_pz(title, pzpairs):
    for (color, (ps, zs)) in zip(colors[:len(pzpairs)], pzpairs):
        for p in ps:
            pylab.scatter([np.real(p)], [np.imag(p)], c=color, marker='x')
        for z in zs:
            pylab.scatter([np.real(z)], [np.imag(z)], c=color, marker='o', edgecolors='none')
    circle= pylab.Circle((0,0), 1.0, fill=False)
    pylab.gcf().gca().add_artist(circle)
    pylab.xlim((-1.1, 1.1))
    pylab.ylim((-1.1, 1.1))
    pylab.xlabel("Real")
    pylab.ylabel("Imag")
    pylab.title(title)

draw_pz("Unquantized Pairs", pairs)
pylab.figure()
draw_pz("Quantized Pairs", fqpairs)
pylab.show()

print("")
print("***********************************************************************")
print("Quantized pole-zero pairs")
print("***********************************************************************")
for (pair, qpair) in zip(pairs, fqpairs):
    print "Poles:"
    print map(np.abs, np.complex256(qpair[0]))
    print "Zeros:"
    print map(np.abs, np.complex256(qpair[1]))
    if max(map(abs, qpair[0])) > 1.0:
        print "Warning! Quantized pole outside of unit circle!"
        print "Unquantized Poles:"
        print map(np.abs, np.float128(pair[0]))
        print "Unquantized Zeros:"
        print map(np.abs, np.float128(pair[1]))

print("")
print("***********************************************************************")
print("Quantized sections (Q{}.{}):".format(31-frac_bits, frac_bits))
print("***********************************************************************")
for (i, (qb, qa)) in zip(range(len(qpairs)), qpairs):
    print("Section {}:".format(i))
    b = map(lambda x: "{:0.18f}".format(np.float128(x)/2**frac_bits), qb)
    a = map(lambda x: "{:0.12f}".format(np.float128(x)/2**frac_bits), qa)
    print("\tB: " + ", ".join(b))
    print("\tA: " + ", ".join(a))

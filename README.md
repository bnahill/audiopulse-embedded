Android HiFi DPOAE Audio Interface
===================

This is part of the [Sana AudioPulse project](http://sana.mit.edu/audiopulse/) and may someday be officially moved over there.

The basic objective is an audio interface for an Android phone to output high-fidelity audio (two channels) to an earpiece and to record back audio from a microphone in the earpiece. As the features-of-interest in the response are below 0dB SPL, this requires very high precision.

Licensing
===================
This is an open-source project. Except where otherwise stated, this work is copyright 2014, Ben Nahill, free to be used, modified, and redistributed in accordance with terms of the GPLv3 license. Note that initially it was provided under a BSD-3 license.

Components
===================

*Updated to refer to rev2*

Control
-------------------
The main controller onboard is a Freescale Kinetis K20 ARM Cortex-M4 microcontroller. It has a 72MHz DSC core (running at 48MHz), a USB OTG interface with full-speed PHY, a full-duplex I2S interface to communicate with the CODEC, I2C to communicate with the headphone amplifier.

The USB interface is capable of up to 12Mb/s, which should support full-duplex 24-bit audio at 48kHz, which should require ~4.4Mb/s plus some inefficiency. That's fine but Android doesn't yet support USB audio input so processing is done onboard. An efficient mixed-precision C++ DSP library is included to perform processing similiar to that described in James Hall's *Handbook of Otoacoustic Emissions*.

CODEC
-------------------
The CODEC (ADC+DAC) must be as precise as possible on a budget. I've chosen the Asahi Kasei AK4621 which has stereo 24-bit differential input and output at 5V and up to 192kHz. Practically, the sampling rate will be 96kHz. S/(N+D) is 102dB for input and 100dB for output, which beats just about everything else I can find. The data interface is roughly I2S and is compatible with the K20 SAI.

Microphone
-------------------
Because of the extremely low required noise floor, the lowest self noise is desired in a microphone. As affordable microphones go, the best seems to be the Primo EM172 electret condenser microphone (ECM) with a noise floor around 14dB. Hopefully this is low enough to measure the necessary features in an averaged spectrum. Unfortunately, I haven't been able to find anything better than this.

Audio In
-------------------
The microphone input is scaled (roughly) to use the full-scale range of the ADC for an ~85dB SPL sine wave. The input conditioning changed substantially from rev1 to eliminate some of the discrete components and reduce power consumption and cost. It consists of a high impedance buffer followed by cascading Sallen-Key Chebyshev low-pass filters and inversion to feed the differential ADC.

A second audio input (since it's a stereo CODEC) is intended for use with the Etymotic ER-10C which does not need the gain stage. The differential input is not fully utilized in this case, as one end is tied to a virtual ground. We can also use this extra input for more advanced tests once DPOAE and some trials are in the bag.

Audio Out
-------------------
The output from the AK4621 is passed to the TI TPA6130A2 headphone amplifier. It has differential input and 98dB S/(N+D) and I2C volume control. It's optimized for a 32R load. This is currently a Knowles SR-series driver. Because the DAC generates lots of high-frequency nonsense that seems to upsed the amplifier, 2nd-order Sallen-Key filters had to be added to the differential signals.

Power
-------------------
Power became a bigger thing after realizing USB power is worse than expected (but that we really want to use it) in rev1. This is now dealt with by using a multi-level supply, boosting the USB input to ~6.5V before using a high-PSRR LDO to get a clean 5V. Digital stuff still runs off the USB supply and we get the ability to completely shut down power-hungry amplifiers and the CODEC when not in use.

Design
===================
The design so far consists of a PCB for the above electronics. The design (as of rev2b) is designed around OSH Park's 6/6 2-layer process, though it is also compatible with many other cheap services.

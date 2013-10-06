Android HiFi DPOAE Audio Interface
===================

This is part of the [Sana AudioPulse project](http://sana.mit.edu/audiopulse/) and may someday be officially moved over there.

The basic objective is an audio interface for an Android phone to output high-fidelity audio (two channels) to an earpiece and to record back audio from a microphone in the earpiece. As the features-of-interest in the response are below 0dB SPL, this requires very high precision.

Components
===================

Control
-------------------
The main controller onboard is a Freescale Kinetis K20 ARM Cortex-M4 microcontroller. It has a 50MHz DSC core, a USB OTG interface with full-speed PHY, a full-duplex I2S interface to communicate with the CODEC, I2C to communicate with the headphone amplifier.

The USB interface is capable of up to 12Mb/s, which should support full-duplex 24-bit audio at 48kHz, which should require ~4.4Mb/s plus some inefficiency. It would become significantly more expensive to move to a high-speed USB interface. An alternate plan is to perform the processing on the microcontroller, which is, in part, why I went with a DSC capable of handling transforms efficiently.

CODEC
-------------------
The CODEC (ADC+DAC) must be as precise as possible on a budget. Power isn't much of a concern. I've chosen the Asahi Kasei AK4621 which has stereo 24-bit differential input and output at 5V and up to 192kHz. Practically, the sampling rate will be 96kHz. S/(N+D) is 102dB for input and 100dB for output, which beats just about everything else I can find. The data interface is roughly I2S and is compatible with the K20 SAI.

Microphone
-------------------
Because of the extremely low required noise floor, the lowest self noise is desired in a microphone. As affordable microphones go, the best seems to be the Primo EM172 electret condenser microphone (ECM) with a noise floor around 14dB. Hopefully this is low enough to measure the necessary features in an averaged spectrum. Unfortunately, I haven't been able to find anything better than this.

Audio In
-------------------
There are two inputs, though obviously only one is used by the microphone.

The microphone input is scaled to use the full-scale range of the ADC for an ~85dB SPL sine wave. This will be adjusted after experimentation, but allows for better use of the dynamic range of the ADC. The preamp has been designed in simulation (included in repository) but has yet to be verified experimentally. It consists of an extremely-low-noise ~unity-gain buffer (using BC550 NPN) follower by a 50V/V gain stage (using LT6201 dual opamp), a 4th-order Sallen-Key Chebyshev filter (using LTC6242 quad opamp), and then a differential ouput to the ADC. An alternate design which performed worse in simulation but intuitively should be similar is also implemented with the spare LT6201.

A second input is provided because the CODEC supports it and there is an extra LTC6242 already on the BOM.

Audio Out
-------------------
The output from the AK4621 is passed to the TI TPA6130A2 headphone amplifier. It has differential input and 98dB S/(N+D) and I2C volume control. It's optimized for a 16R load.


Design
===================
Wait for it...

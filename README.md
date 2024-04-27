# Arduino Due USB Audio

This is a minimum working example of USB audio on an Arduino Due. USB audio can be extremely complex, but this is simply one channel of 48kHz 16-bit PCM audio.

## Portability
The USBAudio.* files should be pretty portable to other Arduino platforms that support PluggableUSB (e.g. Zero), as long as the `USBD_*` functions are swapped to their appropriate counterparts. I opted not to make this a proper library because of the many USB parameters that you may want to change on an application-by-application basis.

## Sample output
The main sketch is specific to the SAM3X/SAM3A chips and their DAC peripheral/DMA. The DAC is clocked by the PWM peripheral at 48kHz. This is independent of the sample clock in the USB host, so the clocks may slowly drift apart. To address this, there is a ring buffer of buffers, and the sample clock is slightly adjusted based on the current fill level. If the sample rates are very different or there is a glitch, some logic drops or inserts buffers of samples. DMA is used to reduce interrupt load and jitter.

There is an [alternate branch](https://github.com/aWZHY0yQH81uOYvH/Arduino-Due-USB-Audio/tree/pwm-output) demonstrating output using PWM.

## Go forth and program
Audio is output on DAC1. You should be able to add additional functionality to the sketch as long as the code in `loop()` is run frequently enough.

You can play around with adding volume control, more channels, or other formats by looking at the USB docs and adjusting the descriptors (namely the `USBAudio_ASType1FormatDescriptor`).

Beware of the fragility of the Due's DAC pins — don't hook a speaker directly to them!

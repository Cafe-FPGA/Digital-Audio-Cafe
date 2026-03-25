# Digital-Audio-Cafe
To facilitate my library of legally acquired lossless audio files, I aim to create a small and portable audio player. I will prototype, assemble, and program everything myself. Through this process I will further my understanding of C++, embedded systems, and system architecting!

## Criteria:

- Must use normal SD Cards
- Dual-brain architecture
- Use tactile controls
- Use as few screw sizes as possible
- Support gapless audio
- Support crossfading


## Constraints

- Budget...
- Only my P1S is accessible for 3d printing
- One PCBA run only

# Progress

## STM32

- [x] Send UART "VOL: __" commands to ESP
- [x] Read ENC11 encoder spins for volume
- [x] Debounce ENC11
- [ ] Fix MUTE state logic

## ESP32-S3

- [x] Parse STM32 UART messages for volume
- [x] Output Sinewave to DAC
- [ ] Volume ramping logic (exponential & S-curve)
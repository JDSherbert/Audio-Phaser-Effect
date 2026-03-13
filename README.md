# Audio DSP: Phaser
<!-- Header Start -->
<img align="right" alt="Stars Badge" src="https://img.shields.io/github/stars/jdsherbert/Audio-Phaser-Effect?label=%E2%AD%90"/>
<img align="right" alt="Forks Badge" src="https://img.shields.io/github/forks/jdsherbert/Audio-Phaser-Effect?label=%F0%9F%8D%B4"/>
<img align="right" alt="Watchers Badge" src="https://img.shields.io/github/watchers/jdsherbert/Audio-Phaser-Effect?label=%F0%9F%91%81%EF%B8%8F"/>
<img align="right" alt="Issues Badge" src="https://img.shields.io/github/issues/jdsherbert/Audio-Phaser-Effect?label=%E2%9A%A0%EF%B8%8F"/>
<img align="right" src="https://hits.seeyoufarm.com/api/count/incr/badge.svg?url=https%3A%2F%2Fgithub.com%2FJDSherbert%2FAudio-Phaser-Effect%2Fhit-counter%2FREADME&count_bg=%2379C83D&title_bg=%23555555&labelColor=0E1128&title=🔍&style=for-the-badge">
<a href="https://learn.microsoft.com/en-us/cpp/cpp-language">
  <img height="40" width="40" src="https://cdn.simpleicons.org/cplusplus">
</a>
<!-- Header End -->
 
-----------------------------------------------------------------------
 
<a href="">
  <img align="left" alt="Audio Processing" src="https://img.shields.io/badge/Audio%20Processing-black?style=for-the-badge&logo=audacity&logoColor=white&color=black&labelColor=black">
</a>
 
<a href="https://choosealicense.com/licenses/mit/">
  <img align="right" alt="License" src="https://img.shields.io/badge/License%20:%20MIT-black?style=for-the-badge&logo=mit&logoColor=white&color=black&labelColor=black">
</a>
 
<br></br>
 
-----------------------------------------------------------------------
 
## Overview
 
A phaser effect implemented in plain C++, intended as a learning resource for understanding how LFO-modulated allpass filter chains work from first principles. No libraries, no black boxes, just the DSP.
 
A phaser works by passing the signal through a chain of allpass filters whose cutoff frequencies are continuously swept by an LFO (Low Frequency Oscillator). Allpass filters shift the phase of frequencies without changing their amplitude. When this phase-shifted signal is mixed back with the dry signal via feedback, certain frequencies cancel (notches) and others reinforce (peaks). Sweeping the filter cutoffs moves these notches through the spectrum, producing the characteristic phaser "whoosh".
 
-----------------------------------------------------------------------
 
## Files
 
| File | Description |
|---|---|
| `AllpassFilter.h / .cpp` | First-order allpass filter; the core DSP building block |
| `Phaser.h / .cpp` | LFO, allpass chain, feedback loop, and parameter management |
| `main.cpp` | Example: generates a 440Hz sine wave and demonstrates the effect |
 
-----------------------------------------------------------------------
 
## How It Works
 
### Step 1 — Allpass Filter
 
An allpass filter passes all frequencies at equal amplitude but shifts their phase by an amount that varies with frequency. It is implemented using a single delay state sample and one coefficient:
 
```
output       = (coefficient * input) + delayedSample
delayedSample = input - (coefficient * output)   [update state for next sample]
```
 
The coefficient `a` is derived from a cutoff frequency `fc` using the bilinear transform:
 
```
a = (tan(π * fc / sampleRate) - 1) / (tan(π * fc / sampleRate) + 1)
```
 
Changing `fc` shifts where in the spectrum the phase rotation is concentrated. This is what the LFO modulates.
 
---
 
### Step 2 — LFO Sweep
 
A sine wave LFO advances its phase by `2π * rate / sampleRate` radians on every sample. Its output is scaled by `depth` and mapped to a cutoff frequency range (100Hz – 4000Hz):
 
```
lfoOutput     = sin(lfoPhase)
lfoNormalized = (lfoOutput * depth + 1.0) * 0.5      // scale to (0, 1)
fc            = lfoMinFreq + lfoNormalized * (lfoMaxFreq - lfoMinFreq)
```
 
Each filter stage receives a slightly offset LFO phase (`stageIndex * π / numStages`), spreading the notches evenly across the spectrum rather than stacking them.
 
---
 
### Step 3 — Allpass Chain + Feedback
 
The signal passes through all stages in series. Before entering the chain, a portion of the previous output is added back to the input:
 
```
chainInput = input + (feedbackSample * feedback)
output     = allpassStage[0] → allpassStage[1] → ... → allpassStage[N]
feedbackSample = output   [stored for next sample]
```
 
More stages = more notches. More feedback = deeper, more resonant notches. The number of notches equals `numStages / 2`.
 
---
 
### Signal Flow
 
```
input ──►──────────────────────────────────────────────────► mix ──► output
          │                                                    ▲
          ▼                                               (feedback)
       [allpass 0] → [allpass 1] → ... → [allpass N] ──────────┘
            ▲              ▲
            │              │
          LFO           LFO + phase offset
       (fc sweep)
```
 
-----------------------------------------------------------------------
 
## Parameters
 
| Parameter | Type | Range | Description |
|---|---|---|---|
| `numStages` | `int` | > 0 | Number of allpass stages. Controls notch count (`numStages / 2`). Typical: 2, 4, 6, 8. |
| `rate` | `float` | > 0.0 Hz | LFO speed. Controls how fast the sweep moves. Typical: 0.1 – 5.0 Hz. |
| `depth` | `float` | 0.0 – 1.0 | LFO modulation depth. Controls how wide the sweep is. |
| `feedback` | `float` | 0.0 – <1.0 | Output fed back into input. Higher values = deeper, more resonant notches. |
| `sampleRate` | `float` | > 0.0 | Audio sample rate in Hz. Required for correct coefficient and LFO calculation. |
 
**Choosing `numStages`:**
 
| Stages | Notches | Character |
|---|---|---|
| 2 | 1 | Subtle, gentle sweep |
| 4 | 2 | Classic phaser sound |
| 6 | 3 | Rich, complex sweep |
| 8 | 4 | Dense, thick modulation |
 
-----------------------------------------------------------------------
 
## Usage
 
### Basic
 
```cpp
// 4-stage phaser, 0.5Hz sweep, moderate depth and feedback, at 44100Hz
Sherbert::Phaser phaser(4, 0.5f, 0.7f, 0.5f, 44100.0f);
 
// Process one sample at a time in your audio loop
float output = phaser.ProcessSample(input);
```
 
### Wet/Dry Mix
 
`ProcessSample` returns the fully wet signal. Blend with the dry input to control the mix:
 
```cpp
const float wetAmount = 0.5f;
 
float wet    = phaser.ProcessSample(input);
float output = (wetAmount * wet) + ((1.0f - wetAmount) * input);
```
 
### Changing Parameters at Runtime
 
All parameters can be updated during playback without calling `reset()`:
 
```cpp
phaser.setRate(2.0f);      // Speed up the sweep
phaser.setDepth(1.0f);     // Maximum sweep width
phaser.setFeedback(0.8f);  // Deep, resonant notches
```
 
`setNumStages()` is the exception - it rebuilds the filter chain and calls `reset()` internally.
 
### Resetting State
 
Call `reset()` when playback stops or the effect is bypassed:
 
```cpp
phaser.reset();
```
 
-----------------------------------------------------------------------
 
## API Reference
 
### `AllpassFilter`
 
| Method | Description |
|---|---|
| `ProcessSample(input)` | Process one sample through the allpass filter. |
| `setCoefficient(a)` | Set the allpass coefficient directly. Range: (-1.0, 1.0). |
| `getCoefficient()` | Returns the current coefficient. |
| `reset()` | Clears internal delay state. |
 
### `Phaser`
 
| Method | Description |
|---|---|
| `Phaser(numStages, rate, depth, feedback, sampleRate)` | Construct with initial parameters. |
| `ProcessSample(input)` | Process one sample. Call once per sample in your audio loop. |
| `reset()` | Clears all filter state, feedback buffer, and LFO phase. |
| `setRate(value)` | Update LFO rate in Hz. |
| `setDepth(value)` | Update LFO depth (0.0 – 1.0). |
| `setFeedback(value)` | Update feedback amount (0.0 – <1.0). |
| `setNumStages(value)` | Update stage count. Calls `reset()` internally. |
 
-----------------------------------------------------------------------
 
## Limitations & Next Steps
 
This is an intentionally minimal implementation for learning purposes. A few things worth knowing about before using it in a real context:
 
**Hardcoded LFO frequency range**: the sweep range of 100Hz -> 4000Hz is baked in as constants. A production implementation would expose `minFrequency` and `maxFrequency` as parameters so the sweep can be tuned to the signal.
 
**Sine LFO only**: real phaser pedals and plugins often offer triangle, square, or user-defined LFO shapes. A triangle LFO produces a more linear sweep; a square LFO produces a stepped jump effect.
 
**No stereo spread**: a common technique is to run two phaser instances with opposite LFO phases (one offset by π radians) on left and right channels, creating a wide stereo image.
 
**No sample rate change handling**: if the sample rate changes after construction, the coefficients will be wrong. A `prepare(newSampleRate)` method would be the correct fix.
 
If you want to explore further, the natural next steps from here are:
- Exposing min/max LFO frequency as parameters
- Adding a triangle or random LFO shape option
- A stereo version with opposite-phase LFO on each channel
- A flanger variant (very short delay line instead of allpass filters)
 
-----------------------------------------------------------------------

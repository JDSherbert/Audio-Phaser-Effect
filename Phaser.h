// Copyright (c) 2024 JDSherbert. All rights reserved.
 
#pragma once
 
#include <vector>
 
#include "AllpassFilter.h"
 
// ======================================================================= //
// Phaser
//
// A phaser effect built from a chain of first-order allpass filters whose
// cutoff frequencies are swept by a sine wave LFO (Low Frequency Oscillator).
//
// How a phaser works:
//   1. The input signal is passed through a chain of allpass filters.
//      These filters do not change amplitude — only phase.
//   2. The filtered signal is mixed back with the dry signal (via feedback).
//      Phase-shifted frequencies cancel the dry signal, producing notches.
//   3. An LFO continuously moves the allpass cutoff frequencies up and down.
//      This sweeps the notches through the spectrum — the phaser "whoosh".
//
// The number of stages controls how many notches appear in the spectrum.
// More stages = more notches = richer, more complex sweep.
// Typical values: 2, 4, 6, or 8 stages (always even for symmetry).
//
// Usage:
//   Sherbert::Phaser phaser(4, 0.5f, 0.7f, 0.5f, 44100);
//   float output = phaser.ProcessSample(input);
//
// ======================================================================= //
 
namespace Sherbert
{
 
    class Phaser
    {

    public:
 
        // ------------------------------------------------------------------
        // Constructor
        //
        // numStages  - Number of allpass filter stages.
        //              Controls how many notches appear in the frequency spectrum.
        //              Must be > 0. Typical values: 2, 4, 6, 8.
        //
        // rate       - LFO frequency in Hz. Controls sweep speed.
        //              Range: > 0.0 (typical: 0.1 – 5.0 Hz)
        //
        // depth      - LFO modulation depth. Controls how wide the sweep is.
        //              Range: [0.0, 1.0]
        //              0.0 = no sweep, 1.0 = maximum sweep range.
        //
        // feedback   - Amount of output fed back into the input.
        //              Range: [0.0, 1.0)
        //              Higher values produce more pronounced, resonant notches.
        //
        // sampleRate - Audio sample rate in Hz (e.g. 44100).
        //              Required to calculate LFO phase increment and filter
        //              coefficients correctly. Must be > 0.
        // ------------------------------------------------------------------
 
        Phaser(int numStages, float rate, float depth, float feedback, float sampleRate);
 
        // ------------------------------------------------------------------
        // ProcessSample
        //
        // Process one sample through the phaser.
        // Call this once per sample in your audio callback loop.
        // Input is expected to be in the range [-1.0, 1.0].
        // ------------------------------------------------------------------
 
        [[nodiscard]] float ProcessSample(float input);
 
        // ------------------------------------------------------------------
        // reset
        //
        // Clears all allpass filter state, the feedback buffer, and resets
        // the LFO phase to zero. Call when playback stops or effect is bypassed.
        // ------------------------------------------------------------------
 
        void reset();
 
        // ------------------------------------------------------------------
        // Getters / Setters
        // ------------------------------------------------------------------
 
        float getRate()     const noexcept { return rate; }
        float getDepth()    const noexcept { return depth; }
        float getFeedback() const noexcept { return feedback; }
        int   getNumStages() const noexcept { return numStages; }
 
        // Note: setNumStages calls reset() internally to rebuild the filter chain.
        void setRate(float newRate);
        void setDepth(float newDepth);
        void setFeedback(float newFeedback);
        void setNumStages(int newNumStages);
 
    private:
 
        // Calculates the allpass coefficient for a given cutoff frequency.
        // fc - cutoff frequency in Hz
        [[nodiscard]] float calculateCoefficient(float fc) const noexcept;
 
        int   numStages;
        float rate;
        float depth;
        float feedback;
        float sampleRate;
 
        float lfoPhase     = 0.0f;  // Current LFO phase in radians
        float feedbackSample = 0.0f; // Single-sample feedback state
 
        std::vector<AllpassFilter> allpassFilters;
 
        // LFO sweep range — the cutoff frequency oscillates between these values.
        // These are chosen to cover a musically useful range of the spectrum.
        static constexpr float lfoMinFrequency = 100.0f;  // Hz
        static constexpr float lfoMaxFrequency = 4000.0f; // Hz
 
    };
 
}
 
// ======================================================================= //

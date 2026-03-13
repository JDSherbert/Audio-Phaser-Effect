// Copyright (c) 2024 JDSherbert. All rights reserved.
 
#pragma once
 
// ======================================================================= //
// AllpassFilter
//
// A first-order allpass filter used as the core building block of the
// Phaser effect. An allpass filter passes all frequencies at equal amplitude
// but shifts their phase depending on frequency. It is this phase shift —
// not any change in volume — that creates the phaser's characteristic sound.
//
// When a phase-shifted copy of the signal is mixed with the dry signal,
// certain frequencies cancel (notches) and others reinforce (peaks).
// Sweeping the filter's cutoff frequency up and down with an LFO moves
// these notches through the frequency spectrum, producing the phaser sweep.
//
// The filter coefficient 'a' controls the cutoff frequency:
//   a = (tan(π * fc / sampleRate) - 1) / (tan(π * fc / sampleRate) + 1)
//
// Where fc is the cutoff frequency in Hz. The Phaser class updates this
// coefficient on every sample to implement LFO modulation.
//
// Transfer function:  H(z) = (a + z^-1) / (1 + a * z^-1)
//
// ======================================================================= //
 
namespace Sherbert
{
 
    class AllpassFilter
    {
    public:
 
        AllpassFilter();
 
        // ------------------------------------------------------------------
        // ProcessSample
        //
        // Process one sample through the allpass filter.
        // The coefficient must be set before calling this — see setCoefficient().
        // ------------------------------------------------------------------
 
        [[nodiscard]] float ProcessSample(float input);
 
        // ------------------------------------------------------------------
        // setCoefficient
        //
        // Sets the allpass coefficient directly.
        // Range: (-1.0, 1.0)
        //
        // In the Phaser, this is calculated from the LFO-modulated cutoff
        // frequency on every sample:
        //   a = (tan(π * fc / sampleRate) - 1) / (tan(π * fc / sampleRate) + 1)
        // ------------------------------------------------------------------
 
        void setCoefficient(float a) noexcept { coefficient = a; }
        float getCoefficient() const noexcept { return coefficient; }
 
        // ------------------------------------------------------------------
        // reset
        //
        // Clears the internal delay state.
        // ------------------------------------------------------------------
 
        void reset() noexcept;
 
    private:
 
        float coefficient  = 0.0f;  // Allpass coefficient; controls phase shift frequency
        float delayedSample = 0.0f; // Single-sample delay state (z^-1)
 
    };
 
}
 
// ======================================================================= //

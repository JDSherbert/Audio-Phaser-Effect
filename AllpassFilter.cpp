// Copyright (c) 2024 JDSherbert. All rights reserved.
 
#include "AllpassFilter.h"
 
// ======================================================================= //
 
Sherbert::AllpassFilter::AllpassFilter()
    : coefficient(0.0f)
    , delayedSample(0.0f)
{
}
 
// ======================================================================= //
 
float Sherbert::AllpassFilter::ProcessSample(float input)
{
    // === ALLPASS FILTER STRUCTURE ======================================
    //
    // The first-order allpass filter is implemented as:
    //
    //   output = (coefficient * input) + delayedSample
    //   delayedSample = input - (coefficient * output)   [update state]
    //
    // This is the standard transposed direct form II structure.
    // It passes all frequencies at the same amplitude but shifts their
    // phase by an amount that varies with frequency — controlled by
    // the coefficient. When this phase-shifted output is later mixed
    // with the original dry signal in the Phaser, frequency-dependent
    // cancellation produces the characteristic notch pattern.
    // ===================================================================
 
    const float output = (coefficient * input) + delayedSample;
    delayedSample = input - (coefficient * output);
    return output;
}
 
// ======================================================================= //
 
void Sherbert::AllpassFilter::reset() noexcept
{
    delayedSample = 0.0f;
}
 
// ======================================================================= //

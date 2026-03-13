// Copyright (c) 2024 JDSherbert. All rights reserved.
 
#include "Phaser.h"
 
#include <cassert>
#include <cmath>
 
// ======================================================================= //
 
Sherbert::Phaser::Phaser(int numStages, float rate, float depth, float feedback, float sampleRate)
    : numStages(numStages)
    , rate(rate)
    , depth(depth)
    , feedback(feedback)
    , sampleRate(sampleRate)
    , lfoPhase(0.0f)
    , feedbackSample(0.0f)
{
    assert(numStages > 0);
    assert(rate > 0.0f);
    assert(depth >= 0.0f && depth <= 1.0f);
    assert(feedback >= 0.0f && feedback < 1.0f);
    assert(sampleRate > 0.0f);
 
    allpassFilters.resize(numStages);
}
 
// ======================================================================= //
 
float Sherbert::Phaser::ProcessSample(float input)
{
    // === HOW THE PHASER WORKS ==========================================
    //
    // On every sample:
    //
    //  1. ADVANCE THE LFO
    //     The LFO is a sine wave oscillating at 'rate' Hz. Each sample
    //     advances its phase by (2π * rate / sampleRate) radians.
    //     The phase wraps at 2π to prevent it growing unbounded.
    //
    //  2. CALCULATE THE CUTOFF FREQUENCY
    //     The LFO output (range -1 to +1) is scaled by 'depth' and mapped
    //     to a frequency range [lfoMinFrequency, lfoMaxFrequency].
    //     This is the frequency at which the allpass filters will shift phase.
    //
    //  3. UPDATE ALLPASS COEFFICIENTS
    //     Each filter stage gets a slightly offset LFO phase so the notches
    //     are spread across the spectrum rather than all stacking together.
    //     The coefficient for each stage is calculated from its cutoff frequency.
    //
    //  4. CHAIN THE SIGNAL THROUGH ALL STAGES
    //     The signal passes through each allpass filter in sequence.
    //     Each stage adds more phase shift, contributing another notch.
    //
    //  5. APPLY FEEDBACK
    //     A portion of the output is fed back into the input before processing.
    //     This deepens and sharpens the notches, making the sweep more resonant.
    // ===================================================================
 
    // Step 1 — Advance LFO phase
    const float lfoPhaseIncrement = 2.0f * static_cast<float>(M_PI) * rate / sampleRate;
    lfoPhase += lfoPhaseIncrement;
    if (lfoPhase >= 2.0f * static_cast<float>(M_PI))
        lfoPhase -= 2.0f * static_cast<float>(M_PI);
 
    // Step 2 — Map LFO to cutoff frequency range
    // lfo output is in (-1, 1); scale to (0, 1) then interpolate frequency range
    const float lfoOutput   = std::sin(lfoPhase);
    const float lfoNormalized = (lfoOutput * depth + 1.0f) * 0.5f;
    const float centerFrequency = lfoMinFrequency
        + lfoNormalized * (lfoMaxFrequency - lfoMinFrequency);
 
    // Step 3 — Update each filter's coefficient with a per-stage phase offset
    // Spreading the phase offsets distributes notches evenly across the spectrum
    const float phaseSpread = static_cast<float>(M_PI) / static_cast<float>(numStages);
    for (int i = 0; i < numStages; ++i)
    {
        const float stagePhase = lfoPhase + (static_cast<float>(i) * phaseSpread);
        const float stageLfo   = std::sin(stagePhase);
        const float stageNorm  = (stageLfo * depth + 1.0f) * 0.5f;
        const float stageFc    = lfoMinFrequency + stageNorm * (lfoMaxFrequency - lfoMinFrequency);
        allpassFilters[i].setCoefficient(calculateCoefficient(stageFc));
    }
 
    // Step 4 — Feed input + feedback through the allpass chain
    float output = input + feedbackSample * feedback;
    for (int i = 0; i < numStages; ++i)
        output = allpassFilters[i].ProcessSample(output);
 
    // Step 5 — Store feedback sample for next call
    feedbackSample = output;
 
    return output;
}
 
// ======================================================================= //
 
float Sherbert::Phaser::calculateCoefficient(float fc) const noexcept
{
    // Calculates the first-order allpass coefficient for a given cutoff
    // frequency using the bilinear transform:
    //   a = (tan(π * fc / sampleRate) - 1) / (tan(π * fc / sampleRate) + 1)
    //
    // When a = 0: no phase shift (filter is transparent)
    // When a → -1: phase shift concentrated at low frequencies
    // When a → +1: phase shift concentrated at high frequencies
 
    const float tanValue = std::tan(static_cast<float>(M_PI) * fc / sampleRate);
    return (tanValue - 1.0f) / (tanValue + 1.0f);
}
 
// ======================================================================= //
 
void Sherbert::Phaser::reset()
{
    lfoPhase      = 0.0f;
    feedbackSample = 0.0f;
 
    for (auto& filter : allpassFilters)
        filter.reset();
}
 
// ======================================================================= //
 
void Sherbert::Phaser::setRate(float newRate)
{
    assert(newRate > 0.0f);
    rate = newRate;
}
 
// ======================================================================= //
 
void Sherbert::Phaser::setDepth(float newDepth)
{
    assert(newDepth >= 0.0f && newDepth <= 1.0f);
    depth = newDepth;
}
 
// ======================================================================= //
 
void Sherbert::Phaser::setFeedback(float newFeedback)
{
    assert(newFeedback >= 0.0f && newFeedback < 1.0f);
    feedback = newFeedback;
}
 
// ======================================================================= //
 
void Sherbert::Phaser::setNumStages(int newNumStages)
{
    assert(newNumStages > 0);
    numStages = newNumStages;
    allpassFilters.resize(numStages);
    reset();
}
 
// ======================================================================= //

// Copyright (c) 2024 JDSherbert. All rights reserved.
 
#include <cmath>
#include <iostream>
#include <vector>
 
#include "Phaser.h"
 
// ======================================================================= //
 
// Generates a single sample of a sine wave at the given frequency and time.
// frequency - Hz (e.g. 440.0 = concert A)
// time      - position in seconds (e.g. sampleIndex / sampleRate)
float GenerateInputSignal(float frequency, float time)
{
    return std::sin(2.0f * static_cast<float>(M_PI) * frequency * time);
}
 
// ======================================================================= //
 
int main()
{
    // == Signal Parameters ==============================================
    const int   sampleRate = 44100;   // 44.1 kHz
    const float frequency  = 440.0f;  // Concert A
    const float duration   = 0.1f;    // 0.1 seconds — enough to see the sweep
 
    const int numSamples = static_cast<int>(sampleRate * duration);
 
    // == Generate Sine Wave =============================================
    // Each sample index is divided by sampleRate to convert it to seconds.
    // This is then passed to the sine function as the time argument:
    //   sample = sin(2π * frequency * time)
    std::vector<float> sineWave(numSamples);
    for (int i = 0; i < numSamples; ++i)
        sineWave[i] = GenerateInputSignal(frequency, i / static_cast<float>(sampleRate));
 
    // == Set Up Phaser ==================================================
    // numStages: 4 — produces 4 notches in the frequency spectrum
    // rate:      0.5f Hz — a slow, gentle sweep
    // depth:     0.7f — moderately wide sweep range
    // feedback:  0.5f — noticeable but not overwhelming resonance
    // sampleRate: 44100
    Sherbert::Phaser phaser(4, 0.5f, 0.7f, 0.5f, static_cast<float>(sampleRate));
 
    // == Process and Print ==============================================
    // Print the first 10 samples before and after the phaser so the
    // effect of the allpass sweep is visible in the output values.
    const int printCount = 10;
 
    std::cout << "--- Dry vs Wet (first " << printCount << " samples) ---\n\n";
 
    for (int i = 0; i < printCount; ++i)
    {
        const float dry = sineWave[i];
        const float wet = phaser.ProcessSample(dry);
 
        std::cout << "Sample " << i << "  |  Dry: " << dry << "  |  Wet: " << wet << "\n";
    }
 
    // == Demonstrate Parameter Changes =================================
    // Reset and re-run with a faster, deeper sweep to show the difference.
    phaser.reset();
    phaser.setRate(2.0f);
    phaser.setDepth(1.0f);
    phaser.setFeedback(0.8f);
 
    std::cout << "\n--- Fast Deep Sweep (rate: 2.0Hz, depth: 1.0, feedback: 0.8) ---\n\n";
 
    for (int i = 0; i < printCount; ++i)
    {
        const float dry = sineWave[i];
        const float wet = phaser.ProcessSample(dry);
 
        std::cout << "Sample " << i << "  |  Dry: " << dry << "  |  Wet: " << wet << "\n";
    }
 
    return 0;
}
 
// ======================================================================= //

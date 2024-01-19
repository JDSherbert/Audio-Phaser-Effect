// ©2024 JDSherbert. All Rights Reserved.

#include "Phaser.h"

Phaser::Phaser(int numStages, float rate, float depth, float feedback)
: numStages(numStages), rate(rate), depth(depth), feedback(feedback) 
{
    for (int i = 0; i < numStages; ++i) 
    {
        allpassFilters.emplace_back();
    }
}

float Phaser::ProcessSample(float input) 
{
    float output = input;

    for (int i = 0; i < numStages; ++i) 
    {
        output = allpassFilters[i].processSample(output);
    }

    // Feedback loop
    feedbackBuffer = output * feedback + input * (1 - feedback);

    return output;
}

void Phaser::SetRate(float newRate) 
{
    rate = newRate;
    // Recalculate filter coefficients based on the new rate
    for (int i = 0; i < numStages; ++i) 
    {
        allpassFilters[i].setRate(rate);
    }
}

Phaser::AllpassFilter::AllpassFilter() 
: delayBuffer(44100, 0)
, writeIndex(0)
, rate(1.0)
, feedback(0.0) 
{
}

float Phaser::AllpassFilter::ProcessSample(float input) 
{
    float delayedSample = delayBuffer[writeIndex];
    float output = -input + delayedSample;
    delayBuffer[writeIndex] = input + output * feedback;

    writeIndex = (writeIndex + 1) % delayBuffer.size();

    return output;
}

void Phaser::AllpassFilter::SetRate(float newRate) 
{
    rate = newRate;
    // Adjust the delay time based on the new rate
    delayBuffer.resize(static_cast<size_t>(44100 / rate), 0);
}

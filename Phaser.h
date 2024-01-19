// ©2024 JDSherbert. All Rights Reserved.

#pragma once

#include <vector>

class Phaser 
{
public:
    Phaser(int numStages, float rate, float depth, float feedback);

    float processSample(float input);
    void setRate(float newRate);

private:
    
    class AllpassFilter 
    {
    public:
        AllpassFilter();

        float processSample(float input);

        void setRate(float newRate);

    private:
        std::vector<float> delayBuffer;
        size_t writeIndex;
        float rate;
        float feedback;
    };

    int numStages;
    float rate;
    float depth;
    float feedback;
    float feedbackBuffer;
    std::vector<AllpassFilter> allpassFilters;
};

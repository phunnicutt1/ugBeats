// EnvelopeGenerator.h
#pragma once

#include <JuceHeader.h>
#include <vector>

// Preset structure for storing envelope settings
struct EnvelopePreset
{
    juce::String name;
    float attack;
    float decay;
    float sustain;
    float release;
    float attackCurve;
    float decayCurve;
    float releaseCurve;
};

class EnvelopeGenerator
{
public:
    enum class EnvelopeStage
    {
        Idle,
        Attack,
        Decay,
        Sustain,
        Release
    };

    EnvelopeGenerator();
    ~EnvelopeGenerator();
    
    // Setup methods
    void prepare(double sampleRate);
    
    // Parameter setters
    void setAttack(float attackTimeMs);
    void setDecay(float decayTimeMs);
    void setSustain(float sustainLevel);
    void setRelease(float releaseTimeMs);
    void setCurves(float attackCurve, float decayCurve, float releaseCurve);
    
    // Trigger methods
    void noteOn();
    void noteOff();
    
    // Processing methods
    float getNextSample();
    void processBlock(float* outputBuffer, int numSamples);
    void processBlock(juce::AudioBuffer<float>& buffer);
    
    // Visualization helper
    void getEnvelopePoints(std::vector<float>& timePoints, std::vector<float>& levelPoints, int numPoints = 100);
    
    // Preset methods
    void loadPreset(const EnvelopePreset& preset);
    EnvelopePreset saveAsPreset(const juce::String& name);
    
    // State queries
    EnvelopeStage getCurrentStage() const;
    float getCurrentLevel() const;
    
    // Get current parameters
    float getAttack() const;
    float getDecay() const;
    float getSustain() const;
    float getRelease() const;

private:
    // Envelope parameters (in seconds and normalized levels)
    float attack = 0.01f;   // 10ms
    float decay = 0.1f;     // 100ms
    float sustain = 0.7f;   // 70%
    float release = 0.2f;   // 200ms
    
    // Current state
    EnvelopeStage currentStage = EnvelopeStage::Idle;
    float currentLevel = 0.0f;
    float stageDelta = 0.0f;
    
    // Curve parameters (1.0 = linear, > 1.0 = logarithmic, < 1.0 = exponential)
    float attackCurve = 1.0f;
    float decayCurve = 1.0f;
    float releaseCurve = 1.0f;
    
    // Timing variables
    double sampleRate = 44100.0;
    
    // Calculate envelope value based on current stage and curve
    float calculateCurvedLevel(float start, float end, float position, float curveAmount);
    
    // Update the delta value based on the current stage
    void updateStageDelta();
};
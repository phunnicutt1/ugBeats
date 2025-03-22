/*
 * Underground Beats
 * EnvelopeProcessor.cpp
 * 
 * Implementation of advanced ADSR envelope processor
 */

#include "EnvelopeProcessor.h"
#include <cmath>

namespace UndergroundBeats {

EnvelopeProcessor::EnvelopeProcessor()
    : attackTime(10.0f)
    , decayTime(100.0f)
    , sustainLevel(0.7f)
    , releaseTime(200.0f)
    , attackCurve(1.0f)
    , decayCurve(1.0f)
    , releaseCurve(1.0f)
    , currentStage(EnvelopeStage::Idle)
    , currentLevel(0.0f)
    , currentSampleRate(44100.0)
    , attackRate(0.0f)
    , decayRate(0.0f)
    , releaseRate(0.0f)
    , levelAtReleaseStart(0.0f)
{
    // Initialize parameters vector with default values
    for (int i = 0; i < MAX_PARAMETERS; ++i) {
        parameters[i].store(0.0f);
    }
    
    // Store ADSR parameters in the parameters array
    parameters[0].store(attackTime);
    parameters[1].store(decayTime);
    parameters[2].store(sustainLevel);
    parameters[3].store(releaseTime);
    
    // Store curve parameters
    parameters[4].store(attackCurve);
    parameters[5].store(decayCurve);
    parameters[6].store(releaseCurve);
    
    updateRates();
}

EnvelopeProcessor::~EnvelopeProcessor()
{
}

void EnvelopeProcessor::setAttackTime(float timeMs)
{
    attackTime = timeMs;
    parameters[0].store(attackTime);
    updateRates();
}

float EnvelopeProcessor::getAttackTime() const
{
    return parameters[0].load();
}

void EnvelopeProcessor::setDecayTime(float timeMs)
{
    decayTime = timeMs;
    parameters[1].store(decayTime);
    updateRates();
}

float EnvelopeProcessor::getDecayTime() const
{
    return parameters[1].load();
}

void EnvelopeProcessor::setSustainLevel(float level)
{
    sustainLevel = juce::jlimit(0.0f, 1.0f, level);
    parameters[2].store(sustainLevel);
    updateRates();
}

float EnvelopeProcessor::getSustainLevel() const
{
    return parameters[2].load();
}

void EnvelopeProcessor::setReleaseTime(float timeMs)
{
    releaseTime = timeMs;
    parameters[3].store(releaseTime);
    updateRates();
}

float EnvelopeProcessor::getReleaseTime() const
{
    return parameters[3].load();
}

void EnvelopeProcessor::setCurves(float attack, float decay, float release)
{
    attackCurve = juce::jlimit(0.1f, 10.0f, attack);
    decayCurve = juce::jlimit(0.1f, 10.0f, decay);
    releaseCurve = juce::jlimit(0.1f, 10.0f, release);
    
    parameters[4].store(attackCurve);
    parameters[5].store(decayCurve);
    parameters[6].store(releaseCurve);
}

float EnvelopeProcessor::getAttackCurve() const
{
    return parameters[4].load();
}

float EnvelopeProcessor::getDecayCurve() const
{
    return parameters[5].load();
}

float EnvelopeProcessor::getReleaseCurve() const
{
    return parameters[6].load();
}

EnvelopeStage EnvelopeProcessor::getCurrentStage() const
{
    return currentStage;
}

float EnvelopeProcessor::getCurrentLevel() const
{
    return currentLevel;
}

bool EnvelopeProcessor::isActive() const
{
    return currentStage != EnvelopeStage::Idle;
}

void EnvelopeProcessor::noteOn()
{
    currentStage = EnvelopeStage::Attack;
    levelAtReleaseStart = 0.0f; // Reset release level
}

void EnvelopeProcessor::noteOff()
{
    if (currentStage != EnvelopeStage::Idle && currentStage != EnvelopeStage::Release)
    {
        levelAtReleaseStart = currentLevel;
        currentStage = EnvelopeStage::Release;
    }
}

void EnvelopeProcessor::reset()
{
    currentStage = EnvelopeStage::Idle;
    currentLevel = 0.0f;
    levelAtReleaseStart = 0.0f;
}

float EnvelopeProcessor::getSample()
{
    // Return 0 if in idle state
    if (currentStage == EnvelopeStage::Idle)
        return 0.0f;
    
    // Process the envelope based on the current stage
    switch (currentStage)
    {
        case EnvelopeStage::Attack:
        {
            // Increment level based on attack rate
            currentLevel += attackRate;
            
            // Apply attack curve
            float attackPosition = currentLevel; // Position from 0 to 1
            float curvedLevel = calculateCurve(0.0f, 1.0f, attackPosition, attackCurve);
            
            // Check for end of attack stage
            if (currentLevel >= 1.0f)
            {
                currentLevel = 1.0f;
                currentStage = EnvelopeStage::Decay;
            }
            return curvedLevel;
        }
            
        case EnvelopeStage::Decay:
        {
            // Decrement level based on decay rate
            currentLevel -= decayRate;
            
            // Apply decay curve
            float decayPosition = (1.0f - currentLevel) / (1.0f - sustainLevel);
            float curvedDecayLevel = calculateCurve(1.0f, sustainLevel, decayPosition, decayCurve);
            
            // Check for end of decay stage
            if (currentLevel <= sustainLevel)
            {
                currentLevel = sustainLevel;
                currentStage = EnvelopeStage::Sustain;
            }
            return curvedDecayLevel;
        }
            
        case EnvelopeStage::Sustain:
            // Hold at sustain level
            currentLevel = sustainLevel;
            return sustainLevel;
            
        case EnvelopeStage::Release:
        {
            // Decrement level based on release rate
            currentLevel -= releaseRate;
            
            // Apply release curve
            float releasePosition = 1.0f - (currentLevel / levelAtReleaseStart);
            float curvedReleaseLevel = calculateCurve(levelAtReleaseStart, 0.0f, releasePosition, releaseCurve);
            
            // Check for end of release stage
            if (currentLevel <= 0.0f)
            {
                currentLevel = 0.0f;
                currentStage = EnvelopeStage::Idle;
            }
            return curvedReleaseLevel;
        }
            
        default:
            return 0.0f;
    }
}

void EnvelopeProcessor::process(float* buffer, int numSamples)
{
    for (int i = 0; i < numSamples; ++i)
    {
        buffer[i] = getSample();
    }
}

void EnvelopeProcessor::process(const float* inputBuffer, float* outputBuffer, int numSamples)
{
    for (int i = 0; i < numSamples; ++i)
    {
        outputBuffer[i] = inputBuffer[i] * getSample();
    }
}

void EnvelopeProcessor::prepare(double sampleRate)
{
    currentSampleRate = sampleRate;
    updateRates();
}

void EnvelopeProcessor::getVisualizationPoints(std::vector<float>& timePoints, std::vector<float>& levelPoints, int numPoints)
{
    // Clear and resize the vectors
    timePoints.resize(numPoints);
    levelPoints.resize(numPoints);
    
    // Calculate total envelope time in seconds for visualization
    // Use attack + decay + a small sustain period + release
    float sustainDisplayTime = 0.2f; // 200ms of sustain for visualization
    float totalTime = (attackTime + decayTime + sustainDisplayTime + releaseTime) / 1000.0f; // Convert to seconds
    
    // Calculate time points for visualization
    for (int i = 0; i < numPoints; ++i)
    {
        float normalizedTime = static_cast<float>(i) / (numPoints - 1);
        timePoints[i] = normalizedTime * totalTime;
        
        // Calculate corresponding level
        float timeInMs = timePoints[i] * 1000.0f;
        float attackTimeMs = attackTime;
        float decayTimeMs = attackTimeMs + decayTime;
        float releaseStartTimeMs = decayTimeMs + sustainDisplayTime;
        
        if (timeInMs <= attackTimeMs)
        {
            // Attack phase
            float position = timeInMs / attackTimeMs;
            levelPoints[i] = calculateCurve(0.0f, 1.0f, position, attackCurve);
        }
        else if (timeInMs <= decayTimeMs)
        {
            // Decay phase
            float position = (timeInMs - attackTimeMs) / decayTime;
            levelPoints[i] = calculateCurve(1.0f, sustainLevel, position, decayCurve);
        }
        else if (timeInMs <= releaseStartTimeMs)
        {
            // Sustain phase
            levelPoints[i] = sustainLevel;
        }
        else
        {
            // Release phase
            float position = (timeInMs - releaseStartTimeMs) / releaseTime;
            position = juce::jlimit(0.0f, 1.0f, position);
            levelPoints[i] = calculateCurve(sustainLevel, 0.0f, position, releaseCurve);
        }
    }
}

void EnvelopeProcessor::loadPreset(const EnvelopePreset& preset)
{
    setAttackTime(preset.attackTime);
    setDecayTime(preset.decayTime);
    setSustainLevel(preset.sustainLevel);
    setReleaseTime(preset.releaseTime);
    setCurves(preset.attackCurve, preset.decayCurve, preset.releaseCurve);
}

EnvelopePreset EnvelopeProcessor::saveAsPreset(const juce::String& name)
{
    EnvelopePreset preset;
    preset.name = name;
    preset.attackTime = attackTime;
    preset.decayTime = decayTime;
    preset.sustainLevel = sustainLevel;
    preset.releaseTime = releaseTime;
    preset.attackCurve = attackCurve;
    preset.decayCurve = decayCurve;
    preset.releaseCurve = releaseCurve;
    return preset;
}

void EnvelopeProcessor::updateRates()
{
    // Calculate sample increments for each stage based on times and sample rate
    attackRate = 1.0f / (attackTime * 0.001f * currentSampleRate);
    decayRate = (1.0f - sustainLevel) / (decayTime * 0.001f * currentSampleRate);
    releaseRate = sustainLevel / (releaseTime * 0.001f * currentSampleRate);
    
    // Ensure minimum rates to prevent hanging notes
    const float minimumRate = 1.0f / (10.0f * currentSampleRate); // 10 second maximum
    attackRate = std::max(attackRate, minimumRate);
    decayRate = std::max(decayRate, minimumRate);
    releaseRate = std::max(releaseRate, minimumRate);
}

float EnvelopeProcessor::calculateCurve(float start, float end, float position, float curveAmount)
{
    // Clamp position to 0-1 range
    position = juce::jlimit(0.0f, 1.0f, position);
    
    // Linear interpolation when curve is close to 1.0
    if (std::abs(curveAmount - 1.0f) < 0.001f)
        return start + (end - start) * position;
    
    // Apply curve based on curve amount
    float curvedPosition;
    if (curveAmount < 1.0f)
    {
        // Exponential curve (convex)
        curvedPosition = 1.0f - std::pow(1.0f - position, 1.0f / curveAmount);
    }
    else
    {
        // Logarithmic curve (concave)
        curvedPosition = std::pow(position, curveAmount);
    }
    
    // Interpolate between start and end values
    return start + (end - start) * curvedPosition;
}

} // namespace UndergroundBeats
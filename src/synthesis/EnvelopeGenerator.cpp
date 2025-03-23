// EnvelopeGenerator.cpp
#include "EnvelopeGenerator.h"
#include <cmath>

EnvelopeGenerator::EnvelopeGenerator()
{
    // Initialize with default values
}

EnvelopeGenerator::~EnvelopeGenerator() = default;

void EnvelopeGenerator::prepare(double newSampleRate)
{
    sampleRate = newSampleRate;
}

void EnvelopeGenerator::setAttack(float attackTimeMs)
{
    attack = attackTimeMs / 1000.0f; // Convert to seconds
    updateStageDelta();
}

void EnvelopeGenerator::setDecay(float decayTimeMs)
{
    decay = decayTimeMs / 1000.0f; // Convert to seconds
    updateStageDelta();
}

void EnvelopeGenerator::setSustain(float sustainLevel)
{
    sustain = juce::jlimit(0.0f, 1.0f, sustainLevel);
    updateStageDelta();
}

void EnvelopeGenerator::setRelease(float releaseTimeMs)
{
    release = releaseTimeMs / 1000.0f; // Convert to seconds
    updateStageDelta();
}

void EnvelopeGenerator::setCurves(float newAttackCurve, float newDecayCurve, float newReleaseCurve)
{
    attackCurve = juce::jlimit(0.1f, 10.0f, newAttackCurve);
    decayCurve = juce::jlimit(0.1f, 10.0f, newDecayCurve);
    releaseCurve = juce::jlimit(0.1f, 10.0f, newReleaseCurve);
}

void EnvelopeGenerator::noteOn()
{
    // Start from current level for smooth transitions
    currentStage = EnvelopeStage::Attack;
    updateStageDelta();
}

void EnvelopeGenerator::noteOff()
{
    // Only switch to release stage if we're not already in idle or release
    if (currentStage != EnvelopeStage::Idle && currentStage != EnvelopeStage::Release)
    {
        currentStage = EnvelopeStage::Release;
        updateStageDelta();
    }
}

float EnvelopeGenerator::getNextSample()
{
    float outputLevel = currentLevel;
    
    switch (currentStage)
    {
        case EnvelopeStage::Idle:
            // Remain at zero
            break;
            
        case EnvelopeStage::Attack:
            // Increase level based on attack curve
            currentLevel += stageDelta;
            // Apply curve
            outputLevel = calculateCurvedLevel(0.0f, 1.0f, currentLevel, attackCurve);
            
            // Check if attack phase is complete
            if (currentLevel >= 1.0f)
            {
                currentLevel = 1.0f;
                currentStage = EnvelopeStage::Decay;
                updateStageDelta();
            }
            break;
            
        case EnvelopeStage::Decay:
            // Decrease level based on decay curve
            currentLevel -= stageDelta;
            // Apply curve
            outputLevel = calculateCurvedLevel(1.0f, sustain, 1.0f - (1.0f - currentLevel) / (1.0f - sustain), decayCurve);
            
            // Check if decay phase is complete
            if (currentLevel <= sustain)
            {
                currentLevel = sustain;
                currentStage = EnvelopeStage::Sustain;
                // No need to update delta for sustain stage
            }
            break;
            
        case EnvelopeStage::Sustain:
            // Maintain sustain level
            currentLevel = sustain;
            outputLevel = sustain;
            break;
            
        case EnvelopeStage::Release:
            // Decrease level based on release curve
            currentLevel -= stageDelta;
            // Apply curve
            outputLevel = calculateCurvedLevel(sustain, 0.0f, currentLevel / sustain, releaseCurve);
            
            // Check if release phase is complete
            if (currentLevel <= 0.0f)
            {
                currentLevel = 0.0f;
                currentStage = EnvelopeStage::Idle;
                // No need to update delta for idle stage
            }
            break;
    }
    
    return outputLevel;
}

void EnvelopeGenerator::processBlock(float* outputBuffer, int numSamples)
{
    for (int i = 0; i < numSamples; ++i)
    {
        outputBuffer[i] = getNextSample();
    }
}

void EnvelopeGenerator::processBlock(juce::AudioBuffer<float>& buffer)
{
    int numSamples = buffer.getNumSamples();
    
    // Apply envelope to all channels
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        float* channelData = buffer.getWritePointer(channel);
        
        for (int i = 0; i < numSamples; ++i)
        {
            channelData[i] *= getNextSample();
        }
    }
}

void EnvelopeGenerator::getEnvelopePoints(std::vector<float>& timePoints, std::vector<float>& levelPoints, int numPoints)
{
    timePoints.resize(numPoints);
    levelPoints.resize(numPoints);
    
    // Calculate the total envelope time in seconds (excluding sustain)
    float totalTime = attack + decay + release;
    
    // Calculate the sustain point index
    int sustainStartIndex = static_cast<int>((attack + decay) / totalTime * numPoints);
    int sustainEndIndex = static_cast<int>((attack + decay + 0.1f) / (totalTime + 0.1f) * numPoints); // Add a small sustain segment
    
    // Fill time and level points
    for (int i = 0; i < numPoints; ++i)
    {
        float normalizedTime = static_cast<float>(i) / (numPoints - 1);
        timePoints[i] = normalizedTime * totalTime;
        
        if (i < static_cast<int>(attack / totalTime * numPoints))
        {
            // Attack phase
            float attackPosition = timePoints[i] / attack;
            levelPoints[i] = calculateCurvedLevel(0.0f, 1.0f, attackPosition, attackCurve);
        }
        else if (i < sustainStartIndex)
        {
            // Decay phase
            float decayPosition = (timePoints[i] - attack) / decay;
            levelPoints[i] = calculateCurvedLevel(1.0f, sustain, decayPosition, decayCurve);
        }
        else if (i < sustainEndIndex)
        {
            // Sustain phase
            levelPoints[i] = sustain;
        }
        else
        {
            // Release phase
            float releasePosition = (timePoints[i] - (attack + decay + 0.1f)) / release;
            releasePosition = juce::jlimit(0.0f, 1.0f, releasePosition);
            levelPoints[i] = calculateCurvedLevel(sustain, 0.0f, releasePosition, releaseCurve);
        }
    }
}

void EnvelopeGenerator::loadPreset(const EnvelopePreset& preset)
{
    setAttack(preset.attack * 1000.0f);  // Convert to ms
    setDecay(preset.decay * 1000.0f);    // Convert to ms
    setSustain(preset.sustain);
    setRelease(preset.release * 1000.0f); // Convert to ms
    setCurves(preset.attackCurve, preset.decayCurve, preset.releaseCurve);
}

EnvelopePreset EnvelopeGenerator::saveAsPreset(const juce::String& name)
{
    EnvelopePreset preset;
    preset.name = name;
    preset.attack = attack;
    preset.decay = decay;
    preset.sustain = sustain;
    preset.release = release;
    preset.attackCurve = attackCurve;
    preset.decayCurve = decayCurve;
    preset.releaseCurve = releaseCurve;
    return preset;
}

EnvelopeGenerator::EnvelopeStage EnvelopeGenerator::getCurrentStage() const
{
    return currentStage;
}

float EnvelopeGenerator::getCurrentLevel() const
{
    return currentLevel;
}

float EnvelopeGenerator::getAttack() const
{
    return attack * 1000.0f; // Convert to ms
}

float EnvelopeGenerator::getDecay() const
{
    return decay * 1000.0f; // Convert to ms
}

float EnvelopeGenerator::getSustain() const
{
    return sustain;
}

float EnvelopeGenerator::getRelease() const
{
    return release * 1000.0f; // Convert to ms
}

float EnvelopeGenerator::calculateCurvedLevel(float start, float end, float position, float curveAmount)
{
    // Limit position to 0-1 range
    position = juce::jlimit(0.0f, 1.0f, position);
    
    // Linear interpolation for curveAmount = 1
    if (std::abs(curveAmount - 1.0f) < 0.001f)
        return start + (end - start) * position;
    
    // Exponential curve for values != 1
    float expPosition;
    if (curveAmount < 1.0f)
    {
        // Convex curve (exponential)
        expPosition = 1.0f - std::pow(1.0f - position, 1.0f / curveAmount);
    }
    else
    {
        // Concave curve (logarithmic)
        expPosition = std::pow(position, curveAmount);
    }
    
    return start + (end - start) * expPosition;
}

void EnvelopeGenerator::updateStageDelta()
{
    switch (currentStage)
    {
        case EnvelopeStage::Idle:
            stageDelta = 0.0f;
            break;
            
        case EnvelopeStage::Attack:
            // Calculate the change in level per sample for attack stage
            stageDelta = 1.0f / (attack * static_cast<float>(sampleRate));
            break;
            
        case EnvelopeStage::Decay:
            // Calculate the change in level per sample for decay stage
            stageDelta = (1.0f - sustain) / (decay * static_cast<float>(sampleRate));
            break;
            
        case EnvelopeStage::Sustain:
            stageDelta = 0.0f;
            break;
            
        case EnvelopeStage::Release:
            // Calculate the change in level per sample for release stage
            stageDelta = sustain / (release * static_cast<float>(sampleRate));
            break;
    }
}
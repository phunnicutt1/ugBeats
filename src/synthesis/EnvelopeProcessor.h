/*
 * Underground Beats
 * EnvelopeProcessor.h
 * 
 * Advanced ADSR envelope processor with curve control and visualization support
 */

#pragma once

#include <JuceHeader.h>
#include <vector>
#include "../audio-engine/ProcessorNode.h"
#include "Envelope.h" // Include Envelope.h to use its EnvelopeStage enum

namespace UndergroundBeats {

/**
 * @brief Structure for storing envelope presets
 */
struct EnvelopePreset {
    juce::String name;
    float attackTime;
    float decayTime;
    float sustainLevel;
    float releaseTime;
    float attackCurve;
    float decayCurve;
    float releaseCurve;
};

// Using EnvelopeStage from Envelope.h instead of redefining it

/**
 * @class EnvelopeProcessor
 * @brief Advanced ADSR envelope generator with curve control
 * 
 * EnvelopeProcessor extends the basic ADSR envelope with customizable curve shapes
 * for each stage, visualization capabilities, and preset management.
 */
class EnvelopeProcessor : public ProcessorNode {
public:
    EnvelopeProcessor();
    ~EnvelopeProcessor() override;
    
    /**
     * @brief Set the attack time
     * 
     * @param timeMs Attack time in milliseconds
     */
    void setAttackTime(float timeMs);
    
    /**
     * @brief Get the current attack time
     * 
     * @return Attack time in milliseconds
     */
    float getAttackTime() const;
    
    /**
     * @brief Set the decay time
     * 
     * @param timeMs Decay time in milliseconds
     */
    void setDecayTime(float timeMs);
    
    /**
     * @brief Get the current decay time
     * 
     * @return Decay time in milliseconds
     */
    float getDecayTime() const;
    
    /**
     * @brief Set the sustain level
     * 
     * @param level Sustain level (0.0 to 1.0)
     */
    void setSustainLevel(float level);
    
    /**
     * @brief Get the current sustain level
     * 
     * @return Sustain level (0.0 to 1.0)
     */
    float getSustainLevel() const;
    
    /**
     * @brief Set the release time
     * 
     * @param timeMs Release time in milliseconds
     */
    void setReleaseTime(float timeMs);
    
    /**
     * @brief Get the current release time
     * 
     * @return Release time in milliseconds
     */
    float getReleaseTime() const;
    
    /**
     * @brief Set the curve shape for each envelope stage
     * 
     * @param attackCurve Attack curve (1.0 = linear, >1.0 = logarithmic, <1.0 = exponential)
     * @param decayCurve Decay curve
     * @param releaseCurve Release curve
     */
    void setCurves(float attackCurve, float decayCurve, float releaseCurve);
    
    /**
     * @brief Get the current attack curve
     * 
     * @return Attack curve value
     */
    float getAttackCurve() const;
    
    /**
     * @brief Get the current decay curve
     * 
     * @return Decay curve value
     */
    float getDecayCurve() const;
    
    /**
     * @brief Get the current release curve
     * 
     * @return Release curve value
     */
    float getReleaseCurve() const;
    
    /**
     * @brief Get the current envelope stage
     * 
     * @return Current envelope stage
     */
    EnvelopeStage getCurrentStage() const;
    
    /**
     * @brief Get the current envelope level
     * 
     * @return Current level (0.0 to 1.0)
     */
    float getCurrentLevel() const;
    
    /**
     * @brief Check if the envelope is active
     * 
     * @return true if the envelope is in any stage other than Idle
     */
    bool isActive() const;
    
    /**
     * @brief Start the envelope (trigger attack stage)
     */
    void noteOn();
    
    /**
     * @brief Release the envelope (start release stage)
     */
    void noteOff();
    
    /**
     * @brief Reset the envelope to idle state
     */
    void reset() override;
    
    /**
     * @brief Get the next envelope sample
     * 
     * @return The next envelope value (0.0 to 1.0)
     */
    float getSample();
    
    /**
     * @brief Process a buffer with the envelope
     * 
     * @param buffer Buffer to fill with envelope values
     * @param numSamples Number of samples to process
     */
    void process(float* buffer, int numSamples);
    
    /**
     * @brief Apply the envelope to an input buffer
     * 
     * @param inputBuffer Input buffer
     * @param outputBuffer Output buffer
     * @param numSamples Number of samples to process
     */
    void process(const float* inputBuffer, float* outputBuffer, int numSamples);
    
    /**
     * @brief Prepare the envelope processor for playback
     * 
     * @param sampleRate Sample rate in Hz
     */
    void prepare(double sampleRate);
    
    /**
     * @brief Get points for visualizing the envelope shape
     * 
     * @param timePoints Vector to store time points
     * @param levelPoints Vector to store level points
     * @param numPoints Number of points to generate
     */
    void getVisualizationPoints(std::vector<float>& timePoints, std::vector<float>& levelPoints, int numPoints = 100);
    
    /**
     * @brief Load an envelope preset
     * 
     * @param preset The preset to load
     */
    void loadPreset(const EnvelopePreset& preset);
    
    /**
     * @brief Save the current settings as a preset
     * 
     * @param name Name for the preset
     * @return The created preset
     */
    EnvelopePreset saveAsPreset(const juce::String& name);
    
private:
    // Envelope parameters
    float attackTime;     // in milliseconds
    float decayTime;      // in milliseconds
    float sustainLevel;   // 0.0 to 1.0
    float releaseTime;    // in milliseconds
    
    // Curve shape parameters
    float attackCurve;    // 1.0 = linear, >1.0 = logarithmic, <1.0 = exponential
    float decayCurve;
    float releaseCurve;
    
    // Current state
    EnvelopeStage currentStage;
    float currentLevel;
    
    // Sample rate and timing
    double currentSampleRate;
    
    // Calculated values for efficient processing
    float attackRate;
    float decayRate;
    float releaseRate;
    float levelAtReleaseStart;
    
    // Update timing values based on sample rate and parameter changes
    void updateRates();
    
    // Calculate curved response for envelope segments
    float calculateCurve(float start, float end, float position, float curveAmount);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EnvelopeProcessor)
};

} // namespace UndergroundBeats
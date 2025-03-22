/*
 * Underground Beats
 * FilterEnvelope.h
 * 
 * Filter with envelope modulation for cutoff and resonance
 */

#pragma once

#include <JuceHeader.h>
#include "Filter.h"
#include "EnvelopeProcessor.h"
#include "../audio-engine/ProcessorNode.h"

namespace UndergroundBeats {

/**
 * @class FilterEnvelope
 * @brief Filter with envelope modulation
 * 
 * The FilterEnvelope class combines a multi-mode filter with an envelope generator
 * to modulate the filter's cutoff frequency and/or resonance over time. This enables
 * dynamic filter sweeps commonly used for synthesizer sounds.
 */
class FilterEnvelope : public ProcessorNode {
public:
    FilterEnvelope();
    ~FilterEnvelope() override;
    
    /**
     * @brief Set the base cutoff frequency
     * 
     * @param frequencyHz Cutoff frequency in Hertz
     */
    void setBaseCutoff(float frequencyHz);
    
    /**
     * @brief Get the base cutoff frequency
     * 
     * @return The base cutoff frequency in Hertz
     */
    float getBaseCutoff() const;
    
    /**
     * @brief Set the base resonance
     * 
     * @param amount Resonance amount (0.0 to 1.0)
     */
    void setBaseResonance(float amount);
    
    /**
     * @brief Get the base resonance
     * 
     * @return The base resonance amount (0.0 to 1.0)
     */
    float getBaseResonance() const;
    
    /**
     * @brief Set the filter type
     * 
     * @param type The filter type to use
     */
    void setFilterType(FilterType type);
    
    /**
     * @brief Get the current filter type
     * 
     * @return The current filter type
     */
    FilterType getFilterType() const;
    
    /**
     * @brief Set the envelope amount for cutoff modulation
     * 
     * @param amount Modulation amount (-1.0 to 1.0, negative for downward sweep)
     */
    void setCutoffEnvelopeAmount(float amount);
    
    /**
     * @brief Get the cutoff envelope modulation amount
     * 
     * @return The cutoff modulation amount
     */
    float getCutoffEnvelopeAmount() const;
    
    /**
     * @brief Set the envelope amount for resonance modulation
     * 
     * @param amount Modulation amount (-1.0 to 1.0, negative for decreasing resonance)
     */
    void setResonanceEnvelopeAmount(float amount);
    
    /**
     * @brief Get the resonance envelope modulation amount
     * 
     * @return The resonance modulation amount
     */
    float getResonanceEnvelopeAmount() const;
    
    /**
     * @brief Set the envelope's attack time
     * 
     * @param timeMs Attack time in milliseconds
     */
    void setAttackTime(float timeMs);
    
    /**
     * @brief Get the envelope's attack time
     * 
     * @return The attack time in milliseconds
     */
    float getAttackTime() const;
    
    /**
     * @brief Set the envelope's decay time
     * 
     * @param timeMs Decay time in milliseconds
     */
    void setDecayTime(float timeMs);
    
    /**
     * @brief Get the envelope's decay time
     * 
     * @return The decay time in milliseconds
     */
    float getDecayTime() const;
    
    /**
     * @brief Set the envelope's sustain level
     * 
     * @param level Sustain level (0.0 to 1.0)
     */
    void setSustainLevel(float level);
    
    /**
     * @brief Get the envelope's sustain level
     * 
     * @return The sustain level
     */
    float getSustainLevel() const;
    
    /**
     * @brief Set the envelope's release time
     * 
     * @param timeMs Release time in milliseconds
     */
    void setReleaseTime(float timeMs);
    
    /**
     * @brief Get the envelope's release time
     * 
     * @return The release time in milliseconds
     */
    float getReleaseTime() const;
    
    /**
     * @brief Set the envelope's curve shapes
     * 
     * @param attackCurve Attack curve (1.0 = linear, >1.0 = logarithmic, <1.0 = exponential)
     * @param decayCurve Decay curve
     * @param releaseCurve Release curve
     */
    void setCurves(float attackCurve, float decayCurve, float releaseCurve);
    
    /**
     * @brief Trigger the envelope (note on)
     */
    void noteOn();
    
    /**
     * @brief Release the envelope (note off)
     */
    void noteOff();
    
    /**
     * @brief Process a buffer of samples through the filter envelope
     * 
     * @param buffer Buffer of samples to process
     * @param numSamples Number of samples to process
     */
    void process(float* buffer, int numSamples);
    
    /**
     * @brief Process a stereo buffer of samples through the filter envelope
     * 
     * @param leftBuffer Left channel buffer
     * @param rightBuffer Right channel buffer
     * @param numSamples Number of samples to process
     */
    void processStereo(float* leftBuffer, float* rightBuffer, int numSamples);
    
    /**
     * @brief Prepare the filter envelope for playback
     * 
     * @param sampleRate The sample rate in Hz
     */
    void prepare(double sampleRate);
    
    /**
     * @brief Reset the filter envelope state
     */
    void reset() override;
    
    /**
     * @brief Get the filter for direct access
     * 
     * @return Pointer to the filter
     */
    Filter* getFilter();
    
    /**
     * @brief Get the envelope for direct access
     * 
     * @return Pointer to the envelope
     */
    EnvelopeProcessor* getEnvelope();
    
private:
    Filter filter;
    EnvelopeProcessor envelope;
    
    // Base parameters
    float baseCutoff;
    float baseResonance;
    
    // Modulation amounts
    float cutoffEnvelopeAmount;
    float resonanceEnvelopeAmount;
    
    // Calculate and apply the modulated filter parameters
    void updateFilterParameters();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FilterEnvelope)
};

} // namespace UndergroundBeats
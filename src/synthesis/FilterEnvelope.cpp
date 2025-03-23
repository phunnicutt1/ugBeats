/*
 * Underground Beats
 * FilterEnvelope.cpp
 * 
 * Implementation of filter with envelope modulation
 */

#include "FilterEnvelope.h"

namespace UndergroundBeats {

FilterEnvelope::FilterEnvelope()
    : baseCutoff(1000.0f)
    , baseResonance(0.3f)
    , cutoffEnvelopeAmount(0.8f)
    , resonanceEnvelopeAmount(0.0f)
{
    // Initialize filter with default settings
    filter.setType(FilterType::LowPass);
    filter.setCutoff(baseCutoff);
    filter.setResonance(baseResonance);
    
    // Initialize envelope with default settings
    envelope.setAttackTime(10.0f);    // 10 ms
    envelope.setDecayTime(500.0f);    // 500 ms
    envelope.setSustainLevel(0.3f);   // 30%
    envelope.setReleaseTime(200.0f);  // 200 ms
    
    // Initialize parameters for the ProcessorNode
    for (int i = 0; i < MAX_PARAMETERS; ++i) {
        parameters[i].store(0.0f);
    }
    
    // Store parameters
    parameters[0].store(baseCutoff);
    parameters[1].store(baseResonance);
    parameters[2].store(cutoffEnvelopeAmount);
    parameters[3].store(resonanceEnvelopeAmount);
    
    // Envelope ADSR parameters
    parameters[4].store(envelope.getAttackTime());
    parameters[5].store(envelope.getDecayTime());
    parameters[6].store(envelope.getSustainLevel());
    parameters[7].store(envelope.getReleaseTime());
    
    // Envelope curve parameters
    parameters[8].store(envelope.getAttackCurve());
    parameters[9].store(envelope.getDecayCurve());
    parameters[10].store(envelope.getReleaseCurve());
}

FilterEnvelope::~FilterEnvelope()
{
}

void FilterEnvelope::setBaseCutoff(float frequencyHz)
{
    baseCutoff = frequencyHz;
    parameters[0].store(baseCutoff);
    updateFilterParameters();
}

float FilterEnvelope::getBaseCutoff() const
{
    return parameters[0].load();
}

void FilterEnvelope::setBaseResonance(float amount)
{
    baseResonance = amount;
    parameters[1].store(baseResonance);
    updateFilterParameters();
}

float FilterEnvelope::getBaseResonance() const
{
    return parameters[1].load();
}

void FilterEnvelope::setFilterType(FilterType type)
{
    filter.setType(type);
}

FilterType FilterEnvelope::getFilterType() const
{
    return filter.getType();
}

void FilterEnvelope::setCutoffEnvelopeAmount(float amount)
{
    cutoffEnvelopeAmount = amount;
    parameters[2].store(cutoffEnvelopeAmount);
    updateFilterParameters();
}

float FilterEnvelope::getCutoffEnvelopeAmount() const
{
    return parameters[2].load();
}

void FilterEnvelope::setResonanceEnvelopeAmount(float amount)
{
    resonanceEnvelopeAmount = amount;
    parameters[3].store(resonanceEnvelopeAmount);
    updateFilterParameters();
}

float FilterEnvelope::getResonanceEnvelopeAmount() const
{
    return parameters[3].load();
}

void FilterEnvelope::setAttackTime(float timeMs)
{
    envelope.setAttackTime(timeMs);
    parameters[4].store(timeMs);
}

float FilterEnvelope::getAttackTime() const
{
    return parameters[4].load();
}

void FilterEnvelope::setDecayTime(float timeMs)
{
    envelope.setDecayTime(timeMs);
    parameters[5].store(timeMs);
}

float FilterEnvelope::getDecayTime() const
{
    return parameters[5].load();
}

void FilterEnvelope::setSustainLevel(float level)
{
    envelope.setSustainLevel(level);
    parameters[6].store(level);
}

float FilterEnvelope::getSustainLevel() const
{
    return parameters[6].load();
}

void FilterEnvelope::setReleaseTime(float timeMs)
{
    envelope.setReleaseTime(timeMs);
    parameters[7].store(timeMs);
}

float FilterEnvelope::getReleaseTime() const
{
    return parameters[7].load();
}

void FilterEnvelope::setCurves(float attackCurve, float decayCurve, float releaseCurve)
{
    envelope.setCurves(attackCurve, decayCurve, releaseCurve);
    parameters[8].store(attackCurve);
    parameters[9].store(decayCurve);
    parameters[10].store(releaseCurve);
}

void FilterEnvelope::noteOn()
{
    envelope.noteOn();
    updateFilterParameters();
}

void FilterEnvelope::noteOff()
{
    envelope.noteOff();
}

void FilterEnvelope::process(float* buffer, int numSamples)
{
    // Temporary buffer for envelope values
    float envelopeBuffer[512]; // Max block size assumed to be 512
    
    // Process samples in smaller chunks if needed
    const int maxChunkSize = 512;
    
    for (int offset = 0; offset < numSamples; offset += maxChunkSize)
    {
        int chunkSize = std::min(maxChunkSize, numSamples - offset);
        
        // Fill envelope buffer with envelope values
        envelope.process(envelopeBuffer, chunkSize);
        
        // Apply envelope values to filter parameters for each sample
        for (int i = 0; i < chunkSize; ++i)
        {
            // Calculate modulated cutoff frequency
            float envelopeValue = envelopeBuffer[i];
            
            // Modify filter cutoff based on envelope
            if (cutoffEnvelopeAmount > 0.0f)
            {
                // Upward sweep
                float modulatedCutoff = baseCutoff * (1.0f + cutoffEnvelopeAmount * envelopeValue * 10.0f);
                filter.setCutoff(modulatedCutoff);
            }
            else if (cutoffEnvelopeAmount < 0.0f)
            {
                // Downward sweep
                float modulatedCutoff = baseCutoff * (1.0f + cutoffEnvelopeAmount * envelopeValue);
                filter.setCutoff(modulatedCutoff);
            }
            
            // Modify filter resonance based on envelope
            if (resonanceEnvelopeAmount != 0.0f)
            {
                float modulatedResonance = baseResonance + resonanceEnvelopeAmount * envelopeValue * 0.9f;
                modulatedResonance = juce::jlimit(0.0f, 0.99f, modulatedResonance);
                filter.setResonance(modulatedResonance);
            }
            
            // Process the sample through the filter
            buffer[offset + i] = filter.processSample(buffer[offset + i]);
        }
    }
}

void FilterEnvelope::processStereo(float* leftBuffer, float* rightBuffer, int numSamples)
{
    // Temporary buffer for envelope values
    float envelopeBuffer[512]; // Max block size assumed to be 512
    
    // Process samples in smaller chunks if needed
    const int maxChunkSize = 512;
    
    for (int offset = 0; offset < numSamples; offset += maxChunkSize)
    {
        int chunkSize = std::min(maxChunkSize, numSamples - offset);
        
        // Fill envelope buffer with envelope values
        envelope.process(envelopeBuffer, chunkSize);
        
        // Process each sample through the filter with modulated parameters
        for (int i = 0; i < chunkSize; ++i)
        {
            // Calculate modulated cutoff frequency
            float envelopeValue = envelopeBuffer[i];
            
            // Modify filter cutoff based on envelope
            if (cutoffEnvelopeAmount > 0.0f)
            {
                // Upward sweep
                float modulatedCutoff = baseCutoff * (1.0f + cutoffEnvelopeAmount * envelopeValue * 10.0f);
                filter.setCutoff(modulatedCutoff);
            }
            else if (cutoffEnvelopeAmount < 0.0f)
            {
                // Downward sweep
                float modulatedCutoff = baseCutoff * (1.0f + cutoffEnvelopeAmount * envelopeValue);
                filter.setCutoff(modulatedCutoff);
            }
            
            // Modify filter resonance based on envelope
            if (resonanceEnvelopeAmount != 0.0f)
            {
                float modulatedResonance = baseResonance + resonanceEnvelopeAmount * envelopeValue * 0.9f;
                modulatedResonance = juce::jlimit(0.0f, 0.99f, modulatedResonance);
                filter.setResonance(modulatedResonance);
            }
        }
        
        // Process the audio through the filter
        filter.processStereo(leftBuffer + offset, rightBuffer + offset, chunkSize);
    }
}

void FilterEnvelope::prepare(double sampleRate)
{
    filter.prepare(sampleRate);
    envelope.prepare(sampleRate);
}

void FilterEnvelope::reset()
{
    filter.reset();
    envelope.reset();
}

Filter* FilterEnvelope::getFilter()
{
    return &filter;
}

EnvelopeProcessor* FilterEnvelope::getEnvelope()
{
    return &envelope;
}

void FilterEnvelope::updateFilterParameters()
{
    // Apply the base parameters
    if (cutoffEnvelopeAmount == 0.0f)
    {
        // No modulation, just set the base cutoff
        filter.setCutoff(baseCutoff);
    }
    
    if (resonanceEnvelopeAmount == 0.0f)
    {
        // No modulation, just set the base resonance
        filter.setResonance(baseResonance);
    }
}

} // namespace UndergroundBeats

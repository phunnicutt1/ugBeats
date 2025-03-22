/*
 * Underground Beats
 * OscillatorBank.cpp
 * 
 * Implementation of the oscillator bank with multiple oscillators
 */
#include "OscillatorBank.h"
#include <cmath>

namespace UndergroundBeats {

OscillatorBank::OscillatorBank(int numOscillators)
{
    // Initialize parameters for oscillator mixing
    for (int i = 0; i < MAX_PARAMETERS; ++i) {
        parameters[i].store(0.0f);
    }
    
    // Store master level as parameter 0
    parameters[0].store(masterLevel);
    
    // Store FM amount as parameter 1
    parameters[1].store(fmAmount);
    
    // Create default oscillators
    for (int i = 0; i < numOscillators; ++i)
    {
        auto osc = std::make_unique<Oscillator>();
        // Configure default settings
        if (i == 0)
        {
            osc->setWaveform(WaveformType::Sine);
        }
        else if (i == 1)
        {
            osc->setWaveform(WaveformType::Sawtooth);
            osc->setGain(0.3f); // Slightly lower level for second oscillator
        }
        oscillators.push_back(std::move(osc));
    }
}

OscillatorBank::~OscillatorBank() = default;

void OscillatorBank::prepare(double sampleRate)
{
    currentSampleRate = sampleRate;
    
    // Prepare all oscillators
    for (auto& osc : oscillators)
    {
        osc->prepare(sampleRate);
    }
}



void OscillatorBank::process(float* buffer, int numSamples)
{
    // Clear the buffer
    std::fill(buffer, buffer + numSamples, 0.0f);
    
    // Get the current master level
    float currentMasterLevel = parameters[0].load();
    
    // Return if no oscillators
    if (oscillators.empty())
        return;
    
    // Process FM synthesis if enabled
    if (fmEnabled && oscillators.size() >= 2)
    {
        // Create temporary buffer for modulator
        std::vector<float> modulatorBuffer(numSamples, 0.0f);
        
        // Get current FM amount
        float currentFMAmount = parameters[1].load();
        
        // Process modulator oscillator (typically the second oscillator modulates the first)
        oscillators[1]->process(modulatorBuffer.data(), numSamples);
        
        // Modulate carrier with the modulator
        for (int i = 0; i < numSamples; ++i)
        {
            // Scale modulator by FM amount
            modulatorBuffer[i] *= currentFMAmount;
        }
        
        // Use oscillator 0 as carrier
        std::vector<float> carrierBuffer(numSamples, 0.0f);
        oscillators[0]->process(carrierBuffer.data(), numSamples, modulatorBuffer.data());
        
        // Mix carrier into output buffer
        for (int i = 0; i < numSamples; ++i)
        {
            buffer[i] += carrierBuffer[i] * oscillators[0]->getGain() * currentMasterLevel;
        }
        
        // Add any remaining oscillators to the output
        for (size_t oscIndex = 2; oscIndex < oscillators.size(); ++oscIndex)
        {
            std::vector<float> oscBuffer(numSamples, 0.0f);
            oscillators[oscIndex]->process(oscBuffer.data(), numSamples);
            
            for (int i = 0; i < numSamples; ++i)
            {
                buffer[i] += oscBuffer[i] * oscillators[oscIndex]->getGain() * currentMasterLevel;
            }
        }
    }
    else
    {
        // Normal processing without FM
        for (auto& osc : oscillators)
        {
            std::vector<float> oscBuffer(numSamples, 0.0f);
            osc->process(oscBuffer.data(), numSamples);
            
            for (int i = 0; i < numSamples; ++i)
            {
                buffer[i] += oscBuffer[i] * osc->getGain() * currentMasterLevel;
            }
        }
    }
    
    // Apply sync if enabled
    if (syncEnabled && oscillators.size() >= 2)
    {
        // A proper implementation would reset slave oscillator phases when master oscillator completes a cycle
        // This would typically require modification to the Oscillator class to expose phase reset events
    }
}

void OscillatorBank::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    buffer.clear();
    
    // Process MIDI events
    for (const auto metadata : midiMessages)
    {
        auto message = metadata.getMessage();
        
        if (message.isNoteOn())
        {
            noteOn(message.getNoteNumber(), message.getVelocity() / 127.0f);
        }
        else if (message.isNoteOff())
        {
            noteOff(message.getNoteNumber());
        }
    }
    
    // Return if no oscillators
    if (oscillators.empty())
        return;
    
    // Process FM synthesis if enabled
    if (fmEnabled && oscillators.size() >= 2)
    {
        // Create temporary buffer for modulator
        juce::AudioBuffer<float> modulatorBuffer(1, buffer.getNumSamples());
        modulatorBuffer.clear();
        
        // Process modulator oscillator (typically the second oscillator modulates the first)
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            float modulatorSample = oscillators[1]->getSample() * fmAmount;
            modulatorBuffer.setSample(0, i, modulatorSample);
        }
        
        // Apply modulation to carrier frequency
        float baseFreq = oscillators[0]->getFrequency();
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            float modulation = modulatorBuffer.getSample(0, i);
            // Exponential FM modulation: carrier frequency * 2^(modulation)
            float modulatedFreq = baseFreq * std::pow(2.0f, modulation);
            oscillators[0]->setFrequency(modulatedFreq);
            
            // Generate the carrier sample with the modulated frequency
            float sample = oscillators[0]->getSample(0.0f) * masterLevel;
            
            // Add to output buffer
            for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
            {
                buffer.addSample(channel, i, sample);
            }
        }
        
        // Reset carrier frequency
        oscillators[0]->setFrequency(baseFreq);
        
        // Add any remaining oscillators to the output
        for (size_t oscIndex = 2; oscIndex < oscillators.size(); ++oscIndex)
        {
            for (int i = 0; i < buffer.getNumSamples(); ++i)
            {
                float sample = oscillators[oscIndex]->getSample(0.0f) * masterLevel;
                
                for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
                {
                    buffer.addSample(channel, i, sample);
                }
            }
        }
    }
    else
    {
        // Normal processing without FM
        for (auto& osc : oscillators)
        {
            for (int i = 0; i < buffer.getNumSamples(); ++i)
            {
                float sample = osc->getSample(0.0f) * masterLevel;
                
                for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
                {
                    buffer.addSample(channel, i, sample);
                }
            }
        }
    }
    
    // Implement hard sync if enabled
    if (syncEnabled && oscillators.size() >= 2)
    {
        // Reset phase of slave oscillators when master crosses zero (positive to negative)
        // This would typically be implemented inside the oscillator processing loop
        // For simplicity, this is just a placeholder comment
    }
}

void OscillatorBank::addOscillator(std::unique_ptr<Oscillator> osc)
{
    if (osc != nullptr)
    {
        osc->prepare(currentSampleRate);
        osc->setFrequency(masterFrequency);
        oscillators.push_back(std::move(osc));
    }
}

void OscillatorBank::removeOscillator(int index)
{
    if (index >= 0 && index < oscillators.size())
    {
        oscillators.erase(oscillators.begin() + index);
    }
}

int OscillatorBank::getNumOscillators() const
{
    return static_cast<int>(oscillators.size());
}

void OscillatorBank::setMasterFrequency(float frequencyHz)
{
    masterFrequency = frequencyHz;
    updateFrequencies();
}

void OscillatorBank::setMasterLevel(float level)
{
    masterLevel = juce::jlimit(0.0f, 1.0f, level);
    parameters[0].store(masterLevel); // Store in parameters array
}

void OscillatorBank::setFrequency(int oscillatorIndex, float frequencyHz)
{
    if (oscillatorIndex >= 0 && oscillatorIndex < oscillators.size())
    {
        oscillators[oscillatorIndex]->setFrequency(frequencyHz);
    }
}

void OscillatorBank::setWaveform(int oscillatorIndex, WaveformType type)
{
    if (oscillatorIndex >= 0 && oscillatorIndex < oscillators.size())
    {
        oscillators[oscillatorIndex]->setWaveform(type);
    }
}

void OscillatorBank::setMixLevel(int oscillatorIndex, float level)
{
    if (oscillatorIndex >= 0 && oscillatorIndex < oscillators.size())
    {
        oscillators[oscillatorIndex]->setGain(level);
    }
}

void OscillatorBank::setFinetuning(int oscillatorIndex, float cents)
{
    if (oscillatorIndex >= 0 && oscillatorIndex < oscillators.size())
    {
        // Convert cents to frequency ratio: 1 cent = 2^(1/1200)
        float ratio = std::pow(2.0f, cents / 1200.0f);
        float baseFreq = masterFrequency;
        oscillators[oscillatorIndex]->setFrequency(baseFreq * ratio);
    }
}

void OscillatorBank::setPhaseOffset(int oscillatorIndex, float phase)
{
    if (oscillatorIndex >= 0 && oscillatorIndex < oscillators.size())
    {
        oscillators[oscillatorIndex]->setPhase(phase);
    }
}

void OscillatorBank::enableSync(bool enable)
{
    syncEnabled = enable;
}

void OscillatorBank::enableFM(bool enable)
{
    fmEnabled = enable;
}

void OscillatorBank::setFMAmount(float amount)
{
    fmAmount = juce::jlimit(0.0f, 10.0f, amount); // Limit to reasonable range
    parameters[1].store(fmAmount); // Store in parameters array
}

void OscillatorBank::noteOn(int midiNoteNumber, float velocity)
{
    // Convert MIDI note to frequency: 440 * 2^((n-69)/12)
    float frequency = 440.0f * std::pow(2.0f, (midiNoteNumber - 69) / 12.0f);
    setMasterFrequency(frequency);
    isNoteActive = true;
}

void OscillatorBank::noteOff(int midiNoteNumber)
{
    // In a more complex implementation, this would check if this is the active note
    isNoteActive = false;
}

void OscillatorBank::updateFrequencies()
{
    for (size_t i = 0; i < oscillators.size(); ++i)
    {
        // Apply any fine-tuning here if needed
        oscillators[i]->setFrequency(masterFrequency);
    }
}

} // namespace UndergroundBeats
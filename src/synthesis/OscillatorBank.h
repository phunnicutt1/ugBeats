/*
 * Underground Beats
 * OscillatorBank.h
 * 
 * Manages multiple oscillators for mixing, FM synthesis and sync
 */
#pragma once

#include <JuceHeader.h>
#include <vector>
#include <memory>
#include "Oscillator.h"
#include "../audio-engine/ProcessorNode.h"

namespace UndergroundBeats {

/**
 * @class OscillatorBank
 * @brief Manages multiple oscillators with mixing, sync, and FM capabilities
 * 
 * The OscillatorBank class provides a way to combine multiple oscillators
 * with controls for mixing balance, sync, FM modulation, and more.
 */
class OscillatorBank : public ProcessorNode
{
public:
    OscillatorBank(int numOscillators = 2);
    ~OscillatorBank() override;

    /**
     * @brief Prepare for playback with given sample rate
     * 
     * @param sampleRate The sample rate in Hz
     */
    void prepare(double sampleRate);
    
    /**
     * @brief Process a buffer of samples
     * 
     * @param buffer The buffer to fill with generated samples
     * @param numSamples The number of samples to generate
     */
    void process(float* buffer, int numSamples);
    
    /**
     * @brief Process a buffer with MIDI events
     * 
     * @param buffer The audio buffer to fill
     * @param midiMessages The MIDI buffer to process
     */
    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override;

    // Oscillator management
    void addOscillator(std::unique_ptr<Oscillator> osc);
    void removeOscillator(int index);
    int getNumOscillators() const;
    
    // Global parameter methods
    void setMasterFrequency(float frequencyHz);
    void setMasterLevel(float level);
    
    // Individual oscillator parameter methods
    void setFrequency(int oscillatorIndex, float frequencyHz);
    void setWaveform(int oscillatorIndex, WaveformType type);
    void setMixLevel(int oscillatorIndex, float level);
    void setFinetuning(int oscillatorIndex, float cents);
    void setPhaseOffset(int oscillatorIndex, float phase);
    
    // Modulation methods
    void enableSync(bool enable);
    void enableFM(bool enable);
    void setFMAmount(float amount);
    
    // MIDI methods
    void noteOn(int midiNoteNumber, float velocity);
    void noteOff(int midiNoteNumber);

private:
    std::vector<std::unique_ptr<Oscillator>> oscillators;
    
    // Parameters
    float masterFrequency = 440.0f;
    float masterLevel = 0.5f;
    
    // Modulation settings
    bool syncEnabled = false;
    bool fmEnabled = false;
    float fmAmount = 0.0f;
    
    // State
    double currentSampleRate = 44100.0;
    bool isNoteActive = false;
    
    // Updates the frequency of all oscillators based on master frequency and individual settings
    void updateFrequencies();
};

} // namespace UndergroundBeats;
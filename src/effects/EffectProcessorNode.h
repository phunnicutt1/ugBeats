/*
 * Underground Beats
 * EffectProcessorNode.h
 * 
 * Adapter class to use Effect objects in the ProcessorGraph
 */

#pragma once

#include <JuceHeader.h>
#include "Effect.h"
#include "../audio-engine/ProcessorNode.h"

namespace UndergroundBeats {

/**
 * @class EffectProcessorNode
 * @brief Adapter class to use Effect objects in the ProcessorGraph
 * 
 * This class adapts the Effect interface to the ProcessorNode interface,
 * allowing Effect objects to be used in the ProcessorGraph.
 */
class EffectProcessorNode : public ProcessorNode {
public:
    /**
     * @brief Construct a new Effect Processor Node
     * 
     * @param effectToUse The Effect object to wrap (takes ownership)
     */
    EffectProcessorNode(std::unique_ptr<Effect> effectToUse);
    
    /**
     * @brief Destructor
     */
    ~EffectProcessorNode();
    
    /**
     * @brief Get the name of the processor
     * 
     * @return The processor name
     */
    const juce::String getName() const override;
    
    /**
     * @brief Prepare for playback
     * 
     * @param sampleRate The sample rate in Hz
     * @param samplesPerBlock The maximum number of samples per block
     */
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    
    /**
     * @brief Process a block of audio
     * 
     * @param buffer The audio buffer to process
     * @param midiMessages The MIDI buffer (unused)
     */
    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override;
    
    /**
     * @brief Release resources
     */
    void releaseResources() override;
    
    /**
     * @brief Get access to the wrapped effect
     * 
     * @return The wrapped effect
     */
    Effect* getEffect() const { return effect.get(); }
    
private:
    std::unique_ptr<Effect> effect;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EffectProcessorNode)
};

} // namespace UndergroundBeats

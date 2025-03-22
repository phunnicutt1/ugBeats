/*
 * Underground Beats
 * EffectProcessorNode.cpp
 * 
 * Implementation of adapter class to use Effect objects in the ProcessorGraph
 */

#include "EffectProcessorNode.h"

namespace UndergroundBeats {

EffectProcessorNode::EffectProcessorNode(std::unique_ptr<Effect> effectToUse)
    : effect(std::move(effectToUse))
{
    // Nothing else to initialize
}

EffectProcessorNode::~EffectProcessorNode()
{
    // Unique_ptr will clean up
}

const juce::String EffectProcessorNode::getName() const
{
    if (effect)
        return juce::String(effect->getName());
        
    return "Effect Processor";
}

void EffectProcessorNode::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    ProcessorNode::prepareToPlay(sampleRate, samplesPerBlock);
    
    if (effect)
        effect->prepare(sampleRate, samplesPerBlock);
}

void EffectProcessorNode::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    
    // Only process if we have an effect
    if (effect && effect->isEnabled())
    {
        if (buffer.getNumChannels() == 1)
        {
            // Mono processing
            effect->process(buffer.getWritePointer(0), buffer.getNumSamples());
        }
        else if (buffer.getNumChannels() >= 2)
        {
            // Stereo processing
            effect->processStereo(buffer.getWritePointer(0), buffer.getWritePointer(1), buffer.getNumSamples());
        }
    }
}

void EffectProcessorNode::releaseResources()
{
    if (effect)
        effect->reset();
        
    ProcessorNode::releaseResources();
}

} // namespace UndergroundBeats

/*
 * Underground Beats
 * RoutingNode.h
 * 
 * Represents a node in the effects routing graph
 */

#pragma once

#include "Effect.h"
#include <vector>
#include <memory>

namespace UndergroundBeats {

/**
 * @class RoutingNode
 * @brief Represents a node in the effects routing graph
 * 
 * Supports both serial and parallel routing of effects.
 */
class RoutingNode {
public:
    enum class Type {
        Serial,     // Process effects in sequence
        Parallel,   // Process effects in parallel and mix outputs
        Effect      // Single effect node
    };

    RoutingNode(Type type = Type::Serial)
        : type(type), effect(nullptr), mixLevel(1.0f) {}
    
    RoutingNode(std::unique_ptr<Effect> effect)
        : type(Type::Effect), effect(std::move(effect)), mixLevel(1.0f) {}
    
    /**
     * @brief Add a child node
     * 
     * @param node Child node to add
     * @return Pointer to the added node
     */
    RoutingNode* addChild(std::unique_ptr<RoutingNode> node) {
        auto* ptr = node.get();
        children.push_back(std::move(node));
        return ptr;
    }
    
    /**
     * @brief Process audio through this node and its children
     * 
     * @param buffer Buffer to process
     * @param tempBuffer Temporary buffer for parallel processing
     * @param numSamples Number of samples to process
     */
    void process(float* buffer, juce::AudioBuffer<float>& tempBuffer, int numSamples);
    
    /**
     * @brief Process stereo audio through this node and its children
     * 
     * @param leftBuffer Left channel buffer
     * @param rightBuffer Right channel buffer
     * @param tempBuffer Temporary buffer for parallel processing
     * @param numSamples Number of samples to process
     */
    void processStereo(float* leftBuffer, float* rightBuffer, 
                      juce::AudioBuffer<float>& tempBuffer, int numSamples);
    
    /**
     * @brief Set the mix level for parallel processing
     * 
     * @param level Mix level (0.0 to 1.0)
     */
    void setMixLevel(float level) { mixLevel = juce::jlimit(0.0f, 1.0f, level); }
    
    /**
     * @brief Get the mix level
     */
    float getMixLevel() const { return mixLevel; }
    
    /**
     * @brief Get the node type
     */
    Type getType() const { return type; }
    
    /**
     * @brief Get the effect (if this is an effect node)
     */
    Effect* getEffect() const { return effect.get(); }
    
    /**
     * @brief Get child nodes
     */
    const std::vector<std::unique_ptr<RoutingNode>>& getChildren() const { return children; }
    
    /**
     * @brief Prepare for audio processing
     */
    void prepare(double sampleRate, int blockSize);

private:
    Type type;
    std::unique_ptr<Effect> effect;
    std::vector<std::unique_ptr<RoutingNode>> children;
    float mixLevel;
};

} // namespace UndergroundBeats

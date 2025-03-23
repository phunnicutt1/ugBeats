/*
 * Underground Beats
 * EffectsChain.h
 * 
 * Manages a chain of audio effects
 */

#pragma once

#include "Effect.h"
#include "RoutingNode.h"
#include <vector>
#include <memory>
#include <string>
#include <map>

namespace UndergroundBeats {

/**
 * @class EffectsChain
 * @brief Manages a chain of audio effects
 * 
 * The EffectsChain class manages a chain of audio effects, handling the
 * routing of audio through each effect in sequence and providing methods
 * to add, remove, and reorder effects.
 */
class EffectsChain {
public:
    EffectsChain();
    ~EffectsChain();

    // Node management
    int createGroup(RoutingNode::Type type, int parentId = 0);
    int addEffect(std::unique_ptr<Effect> effect, int groupId = 0);
    bool removeNode(int nodeId);
    bool moveNode(int nodeId, int newParentId, int position = -1);
    
    // Effect access
    Effect* getEffect(int nodeId);
    Effect* getEffectByName(const std::string& name);
    int getNumEffects() const;
    
    // Audio processing
    void prepare(double sampleRate, int blockSize);
    void process(float* buffer, int numSamples);
    void processBlock(juce::AudioBuffer<float>& buffer);
    void processStereo(float* leftBuffer, float* rightBuffer, int numSamples);
    void reset();

    // State management
    std::unique_ptr<juce::XmlElement> createStateXml() const;
    bool restoreStateFromXml(const juce::XmlElement* xml);

protected:
    // Node lookup and traversal
    RoutingNode* getNode(int nodeId) const;
    void processNode(RoutingNode* node, float* buffer, int numSamples);

private:
    // Node management internals
    std::unique_ptr<RoutingNode> rootNode;
    std::map<int, RoutingNode*> nodeMap;
    int nextNodeId;
    
    // Processing state
    juce::AudioBuffer<float> tempBuffer;
    double currentSampleRate;
    int currentBlockSize;

    // Node helpers
    int registerNode(RoutingNode* node);
    RoutingNode* findParentNode(int nodeId) const;
    
    // State management helpers
    std::unique_ptr<juce::XmlElement> createNodeXml(const RoutingNode* node) const;
    std::unique_ptr<RoutingNode> restoreNodeFromXml(const juce::XmlElement* xml);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EffectsChain)
};

} // namespace UndergroundBeats

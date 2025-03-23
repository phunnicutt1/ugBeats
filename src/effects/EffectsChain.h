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
    void reset();

    // State management
    std::unique_ptr<juce::XmlElement> createStateXml() const;
    bool restoreStateFromXml(const juce::XmlElement* xml);

protected:
    double currentSampleRate;
    int currentBlockSize;
    std::unique_ptr<RoutingNode> rootNode;
    
    RoutingNode* getNode(int nodeId) const;
    std::unique_ptr<juce::XmlElement> createNodeXml(const RoutingNode* node) const;
    std::unique_ptr<RoutingNode> restoreNodeFromXml(const juce::XmlElement* xml);
    
    /**
     * @brief Process a mono buffer of samples through the effect chain
     * 
     * @param buffer Buffer containing samples to process
     * @param numSamples Number of samples to process
     */
    void process(float* buffer, int numSamples);
    
    /**
     * @brief Process a stereo buffer of samples through the effect chain
     * 
     * @param leftBuffer Left channel buffer
     * @param rightBuffer Right channel buffer
     * @param numSamples Number of samples to process
     */
    void processStereo(float* leftBuffer, float* rightBuffer, int numSamples);
    
    /**
     * @brief Prepare the effect chain for processing
     * 
     * @param sampleRate The sample rate in Hz
     * @param blockSize The maximum block size in samples
     */
    void prepare(double sampleRate, int blockSize);
    
    /**
     * @brief Reset all effects in the chain
     */
    void reset();
    
    /**
     * @brief Create an XML element containing the effect chain's state
     * 
     * @return XML element containing effect chain state
     */
    std::unique_ptr<juce::XmlElement> createStateXml() const;
    
    /**
     * @brief Restore effect chain state from an XML element
     * 
     * @param xml XML element containing effect chain state
     * @return true if state was successfully restored
     */
    bool restoreStateFromXml(const juce::XmlElement* xml);
    
    /**
     * @brief Save the chain's state to XML
     * 
     * @return XML element containing the chain state
     */
    std::unique_ptr<juce::XmlElement> createStateXml() const;
    
    /**
     * @brief Restore the chain's state from XML
     * 
     * @param xml XML element containing the chain state
     * @return true if successful
     */
    bool restoreStateFromXml(const juce::XmlElement* xml);

private:
    /**
     * @brief Save a node's state to XML
     */
    std::unique_ptr<juce::XmlElement> createNodeXml(const RoutingNode* node) const;
    
    /**
     * @brief Restore a node from XML
     */
    std::unique_ptr<RoutingNode> restoreNodeFromXml(const juce::XmlElement* xml);
    std::unique_ptr<RoutingNode> rootNode;
    std::map<int, RoutingNode*> nodeMap;
    int nextNodeId;
    juce::AudioBuffer<float> tempBuffer;

    /**
     * @brief Register a node in the node map
     * 
     * @param node Node to register
     * @return ID assigned to the node
     */
    int registerNode(RoutingNode* node);

    /**
     * @brief Find a node's parent
     * 
     * @param nodeId ID of the node to find parent for
     * @return Parent node or nullptr if not found
     */
    RoutingNode* findParentNode(int nodeId) const;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EffectsChain)
};

} // namespace UndergroundBeats

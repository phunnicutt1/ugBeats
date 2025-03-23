/*
 * Underground Beats
 * EffectsChain.cpp
 * 
 * Implementation of effects chain manager
 */

#include "EffectsChain.h"
#include "Delay.h"
#include "Reverb.h"

namespace UndergroundBeats {

EffectsChain::EffectsChain()
    : nextNodeId(1)
{
    // Create root node as serial chain
    rootNode = std::make_unique<RoutingNode>(RoutingNode::Type::Serial);
    registerNode(rootNode.get());
    
    // Initialize temp buffer with reasonable default size
    tempBuffer.setSize(4, 512); // 4 channels for stereo processing + temp buffers
}

EffectsChain::~EffectsChain() = default;

int EffectsChain::createGroup(RoutingNode::Type type, int parentId) {
    auto parent = getNode(parentId);
    if (!parent) return 0;

    auto newNode = std::make_unique<RoutingNode>(type);
    auto* nodePtr = newNode.get();
    parent->addChild(std::move(newNode));
    
    return registerNode(nodePtr);
}

int EffectsChain::addEffect(std::unique_ptr<Effect> effect, int groupId) {
    if (!effect) return 0;
    
    auto parent = getNode(groupId);
    if (!parent) return 0;

    auto newNode = std::make_unique<RoutingNode>(std::move(effect));
    auto* nodePtr = newNode.get();
    parent->addChild(std::move(newNode));
    
    return registerNode(nodePtr);
}

Effect* EffectsChain::getEffect(int nodeId)
{
    auto node = getNode(nodeId);
    return node ? node->getEffect() : nullptr;
}

Effect* EffectsChain::getEffectByName(const std::string& name)
{
    for (const auto& [id, node] : nodeMap) {
        if (node->getType() == RoutingNode::Type::Effect) {
            auto* effect = node->getEffect();
            if (effect && effect->getName() == name) {
                return effect;
            }
        }
    }
    return nullptr;
}

bool EffectsChain::moveNode(int nodeId, int newParentId, int position)
{
    auto node = getNode(nodeId);
    auto newParent = getNode(newParentId);
    if (!node || !newParent || nodeId == 0) { // Can't move root
        return false;
    }

    // Get current parent
    auto currentParent = findParentNode(nodeId);
    if (!currentParent) {
        return false;
    }

    // Find and extract the node from its current parent
    std::unique_ptr<RoutingNode> nodeToMove;
    auto& currentChildren = const_cast<std::vector<std::unique_ptr<RoutingNode>>&>(currentParent->getChildren());
    for (auto it = currentChildren.begin(); it != currentChildren.end(); ++it) {
        if (it->get() == node) {
            nodeToMove = std::move(*it);
            currentChildren.erase(it);
            break;
        }
    }

    if (!nodeToMove) {
        return false;
    }

    // Add to new parent at specified position
    auto& newChildren = const_cast<std::vector<std::unique_ptr<RoutingNode>>&>(newParent->getChildren());
    if (position < 0 || position > static_cast<int>(newChildren.size())) {
        position = static_cast<int>(newChildren.size());
    }
    newChildren.insert(newChildren.begin() + position, std::move(nodeToMove));

    return true;
}

int EffectsChain::getNumEffects() const
{
    int count = 0;
    for (const auto& [id, node] : nodeMap) {
        if (node->getType() == RoutingNode::Type::Effect) {
            count++;
        }
    }
    return count;
}

void EffectsChain::process(float* buffer, int numSamples) {
    rootNode->process(buffer, tempBuffer, numSamples);
}

void EffectsChain::processStereo(float* leftBuffer, float* rightBuffer, int numSamples) {
    rootNode->processStereo(leftBuffer, rightBuffer, tempBuffer, numSamples);
}

void EffectsChain::prepare(double sampleRate, int blockSize) {
    tempBuffer.setSize(4, blockSize); // Space for stereo processing + temp buffers
    rootNode->prepare(sampleRate, blockSize);
}

int EffectsChain::registerNode(RoutingNode* node) {
    int id = nextNodeId++;
    nodeMap[id] = node;
    return id;
}

void EffectsChain::reset() {
    // Reset all nodes in the chain
    std::function<void(RoutingNode*)> resetNode;
    resetNode = [&resetNode](RoutingNode* node) {
        if (node->getType() == RoutingNode::Type::Effect) {
            if (auto* effect = node->getEffect()) {
                effect->reset();
            }
        }
        for (const auto& child : node->getChildren()) {
            resetNode(child.get());
        }
    };
    
    resetNode(rootNode.get());
}

std::unique_ptr<juce::XmlElement> EffectsChain::createNodeXml(const RoutingNode* node) const {
    if (!node) return nullptr;
    
    auto xml = std::make_unique<juce::XmlElement>("RoutingNode");
    xml->setAttribute("type", static_cast<int>(node->getType()));
    
    if (node->getType() == RoutingNode::Type::Effect) {
        if (auto* effect = node->getEffect()) {
            xml->setAttribute("effectType", effect->getName());
            if (auto effectState = effect->createStateXml()) {
                xml->addChildElement(effectState.release());
            }
        }
    } else {
        xml->setAttribute("mixLevel", node->getMixLevel());
        
        // Save child nodes
        for (const auto& child : node->getChildren()) {
            if (auto childXml = createNodeXml(child.get())) {
                xml->addChildElement(childXml.release());
            }
        }
    }
    
    return xml;
}

std::unique_ptr<RoutingNode> EffectsChain::restoreNodeFromXml(const juce::XmlElement* xml) {
    if (!xml || xml->getTagName() != "RoutingNode") {
        return nullptr;
    }
    
    auto type = static_cast<RoutingNode::Type>(xml->getIntAttribute("type"));
    
    if (type == RoutingNode::Type::Effect) {
        // Restore effect node
        std::string effectType = xml->getStringAttribute("effectType").toStdString();
        std::unique_ptr<Effect> effect;
        
        // Create appropriate effect type
        // TODO: Replace with proper effect factory
        if (effectType == "Delay") {
            effect = std::make_unique<Delay>();
        } else if (effectType == "Reverb") {
            effect = std::make_unique<Reverb>();
        }
        
        if (effect) {
            if (auto* effectState = xml->getFirstChildElement()) {
                effect->restoreStateFromXml(effectState);
            }
            return std::make_unique<RoutingNode>(std::move(effect));
        }
    } else {
        // Restore group node
        auto node = std::make_unique<RoutingNode>(type);
        node->setMixLevel(static_cast<float>(xml->getDoubleAttribute("mixLevel", 1.0)));
        
        // Restore children
        for (auto* childXml = xml->getFirstChildElement(); childXml != nullptr; 
             childXml = childXml->getNextElement()) {
            if (auto child = restoreNodeFromXml(childXml)) {
                node->addChild(std::move(child));
            }
        }
        
        return node;
    }
    
    return nullptr;
}

std::unique_ptr<juce::XmlElement> EffectsChain::createStateXml() const {
    auto xml = std::make_unique<juce::XmlElement>("EffectChainState");
    xml->addChildElement(createNodeXml(rootNode.get()).release());
    return xml;
}

bool EffectsChain::restoreStateFromXml(const juce::XmlElement* xml) {
    if (!xml || xml->getTagName() != "EffectChainState") {
        return false;
    }
    
    // Clear current state
    nodeMap.clear();
    nextNodeId = 1;
    
    // Create new root node from first child
    if (auto* rootXml = xml->getFirstChildElement()) {
        rootNode = restoreNodeFromXml(rootXml);
        if (!rootNode) {
            return false;
        }
        registerNode(rootNode.get());
        return true;
    }
    
    return false;
}

} // namespace UndergroundBeats

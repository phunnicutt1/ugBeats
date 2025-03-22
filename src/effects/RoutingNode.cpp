/*
 * Underground Beats
 * RoutingNode.cpp
 */

#include "RoutingNode.h"
#include <algorithm>

namespace UndergroundBeats {

void RoutingNode::process(float* buffer, juce::AudioBuffer<float>& tempBuffer, int numSamples) {
    switch (type) {
        case Type::Effect:
            if (effect) {
                effect->process(buffer, numSamples);
            }
            break;
            
        case Type::Serial:
            // Process each child in sequence
            for (auto& child : children) {
                child->process(buffer, tempBuffer, numSamples);
            }
            break;
            
        case Type::Parallel:
            if (children.empty()) return;
            
            // Ensure temp buffer is large enough
            if (tempBuffer.getNumSamples() < numSamples) {
                tempBuffer.setSize(2, numSamples, false, true, true);
            }
            
            // Clear accumulation buffer
            tempBuffer.clear();
            float* mixBuffer = tempBuffer.getWritePointer(0);
            
            // Process first child directly into the buffer
            if (!children.empty()) {
                // Copy input to mix buffer
                std::copy(buffer, buffer + numSamples, mixBuffer);
                children[0]->process(mixBuffer, tempBuffer, numSamples);
            }
            
            // Process remaining children and mix
            for (size_t i = 1; i < children.size(); ++i) {
                // Copy input to processing buffer
                float* procBuffer = tempBuffer.getWritePointer(1);
                std::copy(buffer, buffer + numSamples, procBuffer);
                
                // Process the child
                children[i]->process(procBuffer, tempBuffer, numSamples);
                
                // Mix into accumulation buffer
                for (int j = 0; j < numSamples; ++j) {
                    mixBuffer[j] += procBuffer[j];
                }
            }
            
            // Apply mix level and copy back to main buffer
            for (int i = 0; i < numSamples; ++i) {
                buffer[i] = buffer[i] * (1.0f - mixLevel) + 
                           (mixBuffer[i] / static_cast<float>(children.size())) * mixLevel;
            }
            break;
    }
}

void RoutingNode::processStereo(float* leftBuffer, float* rightBuffer, 
                              juce::AudioBuffer<float>& tempBuffer, int numSamples) {
    switch (type) {
        case Type::Effect:
            if (effect) {
                effect->processStereo(leftBuffer, rightBuffer, numSamples);
            }
            break;
            
        case Type::Serial:
            // Process each child in sequence
            for (auto& child : children) {
                child->processStereo(leftBuffer, rightBuffer, tempBuffer, numSamples);
            }
            break;
            
        case Type::Parallel:
            if (children.empty()) return;
            
            // Ensure temp buffer is large enough
            if (tempBuffer.getNumSamples() < numSamples) {
                tempBuffer.setSize(4, numSamples, false, true, true);
            }
            
            // Clear accumulation buffers
            tempBuffer.clear();
            float* mixLeftBuffer = tempBuffer.getWritePointer(0);
            float* mixRightBuffer = tempBuffer.getWritePointer(1);
            
            // Process first child directly into mix buffers
            if (!children.empty()) {
                std::copy(leftBuffer, leftBuffer + numSamples, mixLeftBuffer);
                std::copy(rightBuffer, rightBuffer + numSamples, mixRightBuffer);
                children[0]->processStereo(mixLeftBuffer, mixRightBuffer, tempBuffer, numSamples);
            }
            
            // Process remaining children and mix
            for (size_t i = 1; i < children.size(); ++i) {
                float* procLeftBuffer = tempBuffer.getWritePointer(2);
                float* procRightBuffer = tempBuffer.getWritePointer(3);
                
                std::copy(leftBuffer, leftBuffer + numSamples, procLeftBuffer);
                std::copy(rightBuffer, rightBuffer + numSamples, procRightBuffer);
                
                children[i]->processStereo(procLeftBuffer, procRightBuffer, tempBuffer, numSamples);
                
                for (int j = 0; j < numSamples; ++j) {
                    mixLeftBuffer[j] += procLeftBuffer[j];
                    mixRightBuffer[j] += procRightBuffer[j];
                }
            }
            
            // Apply mix level and copy back to main buffers
            float scale = 1.0f / static_cast<float>(children.size());
            for (int i = 0; i < numSamples; ++i) {
                leftBuffer[i] = leftBuffer[i] * (1.0f - mixLevel) + 
                               mixLeftBuffer[i] * scale * mixLevel;
                rightBuffer[i] = rightBuffer[i] * (1.0f - mixLevel) + 
                                mixRightBuffer[i] * scale * mixLevel;
            }
            break;
    }
}

void RoutingNode::prepare(double sampleRate, int blockSize) {
    if (effect) {
        effect->prepare(sampleRate, blockSize);
    }
    for (auto& child : children) {
        child->prepare(sampleRate, blockSize);
    }
}

} // namespace UndergroundBeats

#include "Engine.h"

namespace UndergroundBeats {

Engine::Engine()
{
    // Create the processor graph
    processorGraph = std::make_unique<ProcessorGraph>();
    
    // Default initialization for test oscillator
    testOscillator.initialise([](float x) { return std::sin(x); }, 128);
    outputGain.setGainLinear(0.5f);
}

Engine::~Engine()
{
    shutdown();
}

bool Engine::initialize(const AudioDeviceSettings& settings)
{
    deviceSettings = settings;
    
    // Set up DSP processing specs
    processSpec.sampleRate = deviceSettings.sampleRate;
    processSpec.maximumBlockSize = deviceSettings.bufferSize;
    processSpec.numChannels = deviceSettings.outputChannels;
    
    // Initialize the processor graph
    processorGraph->setPlayConfigDetails(deviceSettings.inputChannels, deviceSettings.outputChannels, deviceSettings.sampleRate, deviceSettings.bufferSize);
    processorGraph->prepareToPlay(deviceSettings.sampleRate, deviceSettings.bufferSize);
    
    // Initialize the basic processing chain (for test oscillator)
    processingChain.get<0>().initialise([](float x) { return std::sin(x); }, 128);
    processingChain.get<1>().setGainLinear(0.5f);
    processingChain.prepare(processSpec);
    
    // Set default oscillator frequency
    frequencySmoothed.reset(processSpec.sampleRate, 0.01);
    frequencySmoothed.setCurrentAndTargetValue(440.0f);
    
    // Mark as initialized
    initialized = true;
    return true;
}

bool Engine::shutdown()
{
    // Reset the processor graph
    if (processorGraph)
        processorGraph->reset();
    
    initialized = false;
    return true;
}

void Engine::processAudio(const juce::AudioSourceChannelInfo& bufferToFill)
{
    if (!initialized || transportState != TransportState::Playing)
    {
        // Clear the buffer if not playing
        bufferToFill.clearActiveBufferRegion();
        return;
    }

    // Create an audio block from the output buffer
    juce::dsp::AudioBlock<float> block(bufferToFill.buffer->getArrayOfWritePointers(),
                                       bufferToFill.buffer->getNumChannels(),
                                       bufferToFill.startSample,
                                       bufferToFill.numSamples);
    
    // Create a processing context
    juce::dsp::ProcessContextReplacing<float> context(block);
    
    // Update oscillator frequency
    processingChain.get<0>().setFrequency(frequencySmoothed.getNextValue());
    
    // Process the audio block
    processingChain.process(context);
}

bool Engine::start()
{
    if (transportState == TransportState::Stopped || 
        transportState == TransportState::Stopping)
    {
        setTransportState(TransportState::Starting);
        return true;
    }
    return false;
}

bool Engine::stop()
{
    if (transportState == TransportState::Playing || 
        transportState == TransportState::Starting)
    {
        setTransportState(TransportState::Stopping);
        return true;
    }
    return false;
}

bool Engine::isRunning() const
{
    return transportState == TransportState::Playing || 
           transportState == TransportState::Starting;
}

NodeID Engine::addProcessor(std::unique_ptr<ProcessorNode> processor)
{
    if (!processorGraph || !processor)
        return NodeID(0);
    
    // Generate a unique string ID for the processor
    static int processorCounter = 0;
    std::string nodeID = "processor_" + std::to_string(++processorCounter);
    
    // Add to the graph and get the JUCE NodeID
    auto juceNodeID = processorGraph->addProcessor(std::move(processor), nodeID);
    
    // If addition failed (empty ID), return a zero NodeID
    if (juceNodeID != juce::AudioProcessorGraph::NodeID())
        return NodeID(juceNodeID.uid);
    else
        return NodeID(0);
}

bool Engine::connectNodes(NodeID source, int sourceChannel, NodeID destination, int destChannel)
{
    if (!processorGraph)
        return false;
    
    // Convert our NodeIDs to string IDs expected by ProcessorGraph
    std::string sourceID = "processor_" + std::to_string(source.get());
    std::string destID = "processor_" + std::to_string(destination.get());
    
    return processorGraph->connectNodes(sourceID, sourceChannel, destID, destChannel);
}

void Engine::setParameter(NodeID node, int paramIndex, float value)
{
    // Special handling for test oscillator
    if (node.get() == 0 && paramIndex == 0) // Test oscillator frequency
    {
        frequencySmoothed.setTargetValue(value);
        return;
    }
    
    // Forward to processor graph
    if (processorGraph) {
        // Find the node and set parameter directly
        std::string nodeID = "processor_" + std::to_string(node.get());
        auto processorNode = processorGraph->getNodeForID(nodeID);
        if (processorNode != nullptr) {
            processorNode->getProcessor()->setParameter(paramIndex, value);
        }
    }
}

void Engine::setTransportState(TransportState newState)
{
    // Handle state transitions
    if (newState == TransportState::Starting && 
        transportState != TransportState::Playing)
    {
        transportState = TransportState::Playing;
    }
    else if (newState == TransportState::Stopping && 
             transportState != TransportState::Stopped)
    {
        transportState = TransportState::Stopped;
    }
    else
    {
        transportState = newState;
    }
}

TransportState Engine::getTransportState() const
{
    return transportState;
}

} // namespace UndergroundBeats

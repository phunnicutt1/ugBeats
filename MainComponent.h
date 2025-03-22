#pragma once

#include <JuceHeader.h>

class MainTabbedComponent;

class MainComponent : public juce::Component,
                     public juce::AudioIODeviceCallback
{
public:
    MainComponent()
    {
        // Create and add the tabbed component
        tabbedComponent = std::make_unique<MainTabbedComponent>();
        addAndMakeVisible(tabbedComponent.get());
        
        // Initialize audio device manager
        audioDeviceManager.initialiseWithDefaultDevices(2, 2);
        audioDeviceManager.addAudioCallback(this);
        
        // Set up size
        setSize(800, 600);
    }

    ~MainComponent() override
    {
        audioDeviceManager.removeAudioCallback(this);
    }

    void paint(juce::Graphics& g) override
    {
        g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    }

    void resized() override
    {
        tabbedComponent->setBounds(getLocalBounds());
    }

    void audioDeviceIOCallback(const float** inputChannelData,
                             int numInputChannels,
                             float** outputChannelData,
                             int numOutputChannels,
                             int numSamples) override
    {
        // Simple audio processing - implement your audio engine processing here
        
        // Clear output buffers
        for (int channel = 0; channel < numOutputChannels; ++channel)
        {
            if (outputChannelData[channel] != nullptr)
            {
                std::memset(outputChannelData[channel], 0, numSamples * sizeof(float));
            }
        }
        
        // Process audio through your engine
        // engineProcessor.processBlock(outputChannelData, numSamples);
    }

    void audioDeviceAboutToStart(juce::AudioIODevice* device) override
    {
        // Initialize your audio engine with the sample rate and buffer size
        double sampleRate = device->getCurrentSampleRate();
        int bufferSize = device->getCurrentBufferSizeSamples();
        
        // engineProcessor.prepare(sampleRate, bufferSize);
    }

    void audioDeviceStopped() override
    {
        // Clean up your audio engine resources
        // engineProcessor.releaseResources();
    }

private:
    std::unique_ptr<MainTabbedComponent> tabbedComponent;
    juce::AudioDeviceManager audioDeviceManager;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
#pragma once

#include <JuceHeader.h>

class SettingsView : public juce::Component,
                    private juce::Timer
{
public:
    SettingsView(std::shared_ptr<UndergroundBeats::AudioDeviceManager> deviceManager);
    ~SettingsView() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    void timerCallback() override;
    void updateAudioDeviceList();
    void updateSampleRateList();
    void updateBufferSizeList();
    void updateMIDIDeviceLists();
    
    int findSampleRateId(double sampleRate) const;
    int findBufferSizeId(int bufferSize) const;
    double getSampleRateForId(int id) const;
    int getBufferSizeForId(int id) const;
    
    std::shared_ptr<UndergroundBeats::AudioDeviceManager> audioDeviceManager;
    
    // Cache available options
    std::vector<double> availableSampleRates;
    std::vector<int> availableBufferSizes;
    
    struct SettingsGroup : public juce::GroupComponent
    {
        SettingsGroup(const juce::String& name) : juce::GroupComponent(name, name) {}
    };

    // Audio settings
    SettingsGroup audioGroup { "Audio Settings" };
    juce::ComboBox audioDeviceSelector;
    juce::ComboBox sampleRateSelector;
    juce::ComboBox bufferSizeSelector;

    // MIDI settings
    SettingsGroup midiGroup { "MIDI Settings" };
    juce::ComboBox midiInputSelector;
    juce::ComboBox midiOutputSelector;

    // Performance settings
    SettingsGroup performanceGroup { "Performance Settings" };
    juce::ToggleButton multiThreadingEnabled { "Enable Multi-threading" };
    juce::Slider maxVoicesSlider;
    juce::Label maxVoicesLabel { "Max Voices" };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SettingsView)
};

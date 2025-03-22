#include "SettingsView.h"

SettingsView::SettingsView(std::shared_ptr<UndergroundBeats::AudioDeviceManager> deviceManager)
    : audioDeviceManager(deviceManager)
{
    // Initialize Audio Settings
    addAndMakeVisible(audioGroup);
    addAndMakeVisible(audioDeviceSelector);
    audioDeviceSelector.onChange = [this]() {
        if (audioDeviceManager) {
            juce::AudioDeviceManager::AudioDeviceSetup setup;
            audioDeviceManager->getAudioDeviceSetup(setup);
            setup.outputDeviceName = audioDeviceSelector.getText();
            
            // Store current settings before changing device
            double currentSampleRate = setup.sampleRate;
            int currentBufferSize = setup.bufferSize;
            
            // Change device
            auto error = audioDeviceManager->setAudioDeviceSetup(setup, true);
            if (error.isNotEmpty()) {
                // Handle error, possibly show dialog
                juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon,
                    "Audio Device Error",
                    "Failed to set audio device:\n" + error);
            } else {
                // Update available sample rates and buffer sizes for new device
                updateSampleRateList();
                updateBufferSizeList();
                
                // Try to restore previous settings if supported
                if (auto* device = audioDeviceManager->getCurrentAudioDevice()) {
                    auto rates = device->getAvailableSampleRates();
                    auto sizes = device->getAvailableBufferSizes();
                    
                    if (std::find(rates.begin(), rates.end(), currentSampleRate) != rates.end()) {
                        sampleRateSelector.setSelectedId(
                            findSampleRateId(currentSampleRate), juce::dontSendNotification);
                    }
                    
                    if (std::find(sizes.begin(), sizes.end(), currentBufferSize) != sizes.end()) {
                        bufferSizeSelector.setSelectedId(
                            findBufferSizeId(currentBufferSize), juce::dontSendNotification);
                    }
                }
            }
        }
    };
    
    addAndMakeVisible(sampleRateSelector);
    sampleRateSelector.onChange = [this]() {
        if (auto* device = audioDeviceManager->getCurrentAudioDevice()) {
            auto setup = device->getAudioDeviceSetup();
            setup.sampleRate = std::stod(sampleRateSelector.getText().toStdString());
            audioDeviceManager->setAudioDeviceSetup(setup, true);
        }
    };
    
    addAndMakeVisible(bufferSizeSelector);
    bufferSizeSelector.onChange = [this]() {
        if (auto* device = audioDeviceManager->getCurrentAudioDevice()) {
            auto setup = device->getAudioDeviceSetup();
            setup.bufferSize = std::stoi(bufferSizeSelector.getText().toStdString());
            audioDeviceManager->setAudioDeviceSetup(setup, true);
        }
    };

    // Initialize device lists
    updateAudioDeviceList();
    updateSampleRateList();
    updateBufferSizeList();
    
    // Start periodic updates
    startTimerHz(2); // Update every 500ms

    // Initialize MIDI Settings
    addAndMakeVisible(midiGroup);
    addAndMakeVisible(midiInputSelector);
    midiInputSelector.addItem("No MIDI Input", 1);
    
    addAndMakeVisible(midiOutputSelector);
    midiOutputSelector.addItem("No MIDI Output", 1);

    // Initialize Performance Settings
    addAndMakeVisible(performanceGroup);
    addAndMakeVisible(multiThreadingEnabled);
    multiThreadingEnabled.setToggleState(true, juce::dontSendNotification);
    
    addAndMakeVisible(maxVoicesSlider);
    maxVoicesSlider.setRange(8, 128, 1);
    maxVoicesSlider.setValue(32);
    maxVoicesSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    
    addAndMakeVisible(maxVoicesLabel);
    maxVoicesLabel.attachToComponent(&maxVoicesSlider, true);
}

SettingsView::~SettingsView()
{
}

void SettingsView::timerCallback()
{
    updateAudioDeviceList();
    updateSampleRateList();
    updateBufferSizeList();
    updateMIDIDeviceLists();
}

void SettingsView::updateAudioDeviceList()
{
    if (!audioDeviceManager)
        return;

    auto* currentDevice = audioDeviceManager->getCurrentAudioDevice();
    juce::String currentDeviceName = currentDevice ? currentDevice->getName() : "";

    audioDeviceSelector.clear();
    int itemId = 1;
    
    for (auto& deviceName : audioDeviceManager->getAvailableDeviceNames())
    {
        audioDeviceSelector.addItem(deviceName, itemId++);
        if (deviceName == currentDeviceName)
            audioDeviceSelector.setSelectedId(itemId - 1, juce::dontSendNotification);
    }
}

int SettingsView::findSampleRateId(double sampleRate) const
{
    auto it = std::find(availableSampleRates.begin(), availableSampleRates.end(), sampleRate);
    if (it != availableSampleRates.end())
        return static_cast<int>(std::distance(availableSampleRates.begin(), it)) + 1;
    return 1;
}

int SettingsView::findBufferSizeId(int bufferSize) const
{
    auto it = std::find(availableBufferSizes.begin(), availableBufferSizes.end(), bufferSize);
    if (it != availableBufferSizes.end())
        return static_cast<int>(std::distance(availableBufferSizes.begin(), it)) + 1;
    return 1;
}

double SettingsView::getSampleRateForId(int id) const
{
    if (id > 0 && id <= static_cast<int>(availableSampleRates.size()))
        return availableSampleRates[id - 1];
    return 44100.0; // Default
}

int SettingsView::getBufferSizeForId(int id) const
{
    if (id > 0 && id <= static_cast<int>(availableBufferSizes.size()))
        return availableBufferSizes[id - 1];
    return 512; // Default
}

void SettingsView::updateSampleRateList()
{
    if (!audioDeviceManager)
        return;

    auto* currentDevice = audioDeviceManager->getCurrentAudioDevice();
    if (!currentDevice)
        return;

    double currentRate = currentDevice->getCurrentSampleRate();
    sampleRateSelector.clear();
    availableSampleRates.clear();
    
    // Get and sort available rates
    availableSampleRates = currentDevice->getAvailableSampleRates();
    std::sort(availableSampleRates.begin(), availableSampleRates.end());
    
    // Add items to selector
    int itemId = 1;
    for (double rate : availableSampleRates)
    {
        juce::String rateText = juce::String(static_cast<int>(rate)) + " Hz";
        sampleRateSelector.addItem(rateText, itemId++);
    }
    
    // Select current rate
    sampleRateSelector.setSelectedId(findSampleRateId(currentRate), juce::dontSendNotification);
}

void SettingsView::updateBufferSizeList()
{
    if (!audioDeviceManager)
        return;

    auto* currentDevice = audioDeviceManager->getCurrentAudioDevice();
    if (!currentDevice)
        return;

    int currentSize = currentDevice->getCurrentBufferSizeSamples();
    bufferSizeSelector.clear();
    availableBufferSizes.clear();
    
    // Get and sort available sizes
    availableBufferSizes = currentDevice->getAvailableBufferSizes();
    std::sort(availableBufferSizes.begin(), availableBufferSizes.end());
    
    // Add items to selector
    int itemId = 1;
    for (int size : availableBufferSizes)
    {
        juce::String sizeText = juce::String(size) + " samples";
        bufferSizeSelector.addItem(sizeText, itemId++);
    }
    
    // Select current size
    bufferSizeSelector.setSelectedId(findBufferSizeId(currentSize), juce::dontSendNotification);
}

void SettingsView::updateMIDIDeviceLists()
{
    if (!audioDeviceManager)
        return;

    // Clear existing items
    midiInputSelector.clear();
    midiOutputSelector.clear();

    // Add available MIDI input devices
    auto midiInputs = juce::MidiInput::getAvailableDevices();
    int itemId = 1;
    midiInputSelector.addItem("No MIDI Input", 1);
    
    for (const auto& device : midiInputs)
    {
        itemId++;
        midiInputSelector.addItem(device.name, itemId);
        
        // Check if this device is currently enabled
        if (audioDeviceManager->isMidiInputDeviceEnabled(device.identifier))
        {
            midiInputSelector.setSelectedId(itemId, juce::dontSendNotification);
        }
    }

    // Add available MIDI output devices
    auto midiOutputs = juce::MidiOutput::getAvailableDevices();
    itemId = 1;
    midiOutputSelector.addItem("No MIDI Output", 1);
    
    for (const auto& device : midiOutputs)
    {
        itemId++;
        midiOutputSelector.addItem(device.name, itemId);
        
        // Check if this device is currently enabled
        if (audioDeviceManager->getDefaultMidiOutput() &&
            audioDeviceManager->getDefaultMidiOutput()->getIdentifier() == device.identifier)
        {
            midiOutputSelector.setSelectedId(itemId, juce::dontSendNotification);
        }
    }
    
    // Set up MIDI device change handlers if not already done
    static bool handlersInitialized = false;
    if (!handlersInitialized)
    {
        midiInputSelector.onChange = [this]()
        {
            if (midiInputSelector.getSelectedId() > 1)
            {
                auto devices = juce::MidiInput::getAvailableDevices();
                auto selectedIndex = midiInputSelector.getSelectedId() - 2;
                if (selectedIndex >= 0 && selectedIndex < devices.size())
                {
                    auto device = devices[selectedIndex];
                    audioDeviceManager->setMidiInputDeviceEnabled(device.identifier, true);
                }
            }
            else
            {
                // Disable all MIDI inputs
                auto devices = juce::MidiInput::getAvailableDevices();
                for (const auto& device : devices)
                {
                    audioDeviceManager->setMidiInputDeviceEnabled(device.identifier, false);
                }
            }
        };
        
        midiOutputSelector.onChange = [this]()
        {
            if (midiOutputSelector.getSelectedId() > 1)
            {
                auto devices = juce::MidiOutput::getAvailableDevices();
                auto selectedIndex = midiOutputSelector.getSelectedId() - 2;
                if (selectedIndex >= 0 && selectedIndex < devices.size())
                {
                    auto device = devices[selectedIndex];
                    audioDeviceManager->setDefaultMidiOutput(device.identifier);
                }
            }
            else
            {
                audioDeviceManager->setDefaultMidiOutput({});
            }
        };
        
        handlersInitialized = true;
    }
}

void SettingsView::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void SettingsView::resized()
{
    auto area = getLocalBounds().reduced(20);
    const int groupHeight = 150;
    const int controlHeight = 30;
    const int spacing = 10;

    // Layout Audio Settings
    audioGroup.setBounds(area.removeFromTop(groupHeight));
    auto audioArea = audioGroup.getBounds().reduced(10);
    audioDeviceSelector.setBounds(audioArea.removeFromTop(controlHeight));
    audioArea.removeFromTop(spacing);
    sampleRateSelector.setBounds(audioArea.removeFromTop(controlHeight));
    audioArea.removeFromTop(spacing);
    bufferSizeSelector.setBounds(audioArea.removeFromTop(controlHeight));

    area.removeFromTop(spacing);

    // Layout MIDI Settings
    midiGroup.setBounds(area.removeFromTop(groupHeight));
    auto midiArea = midiGroup.getBounds().reduced(10);
    midiInputSelector.setBounds(midiArea.removeFromTop(controlHeight));
    midiArea.removeFromTop(spacing);
    midiOutputSelector.setBounds(midiArea.removeFromTop(controlHeight));

    area.removeFromTop(spacing);

    // Layout Performance Settings
    performanceGroup.setBounds(area.removeFromTop(groupHeight));
    auto perfArea = performanceGroup.getBounds().reduced(10);
    multiThreadingEnabled.setBounds(perfArea.removeFromTop(controlHeight));
    perfArea.removeFromTop(spacing);
    maxVoicesSlider.setBounds(perfArea.removeFromTop(controlHeight).withTrimmedLeft(80));
}

#include "SettingsView.h"

SettingsView::SettingsView(std::shared_ptr<UndergroundBeats::AudioDeviceManager> deviceManager)
    : audioDeviceManager(deviceManager)
{
    // Initialize Audio Settings
    addAndMakeVisible(audioGroup);
    addAndMakeVisible(audioDeviceSelector);
    
    audioDeviceSelector.onChange = [this]() {
        if (auto* device = audioDeviceManager->getCurrentAudioDevice()) {
            auto setup = device->getAudioDeviceSetup();
            setup.outputDeviceName = audioDeviceSelector.getText();
            audioDeviceManager->setAudioDeviceSetup(setup, true);
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

void SettingsView::updateSampleRateList()
{
    if (!audioDeviceManager)
        return;

    auto* currentDevice = audioDeviceManager->getCurrentAudioDevice();
    if (!currentDevice)
        return;

    double currentRate = currentDevice->getCurrentSampleRate();
    sampleRateSelector.clear();
    int itemId = 1;

    for (auto rate : currentDevice->getAvailableSampleRates())
    {
        juce::String rateText = juce::String(static_cast<int>(rate)) + " Hz";
        sampleRateSelector.addItem(rateText, itemId++);
        if (std::abs(rate - currentRate) < 1.0)
            sampleRateSelector.setSelectedId(itemId - 1, juce::dontSendNotification);
    }
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
    int itemId = 1;

    for (auto size : currentDevice->getAvailableBufferSizes())
    {
        juce::String sizeText = juce::String(size) + " samples";
        bufferSizeSelector.addItem(sizeText, itemId++);
        if (size == currentSize)
            bufferSizeSelector.setSelectedId(itemId - 1, juce::dontSendNotification);
    }
}

void SettingsView::updateMIDIDeviceLists()
{
    // Clear existing items
    midiInputSelector.clear();
    midiOutputSelector.clear();

    // Add available MIDI input devices
    auto midiInputs = juce::MidiInput::getAvailableDevices();
    int itemId = 1;
    for (auto& device : midiInputs)
    {
        midiInputSelector.addItem(device.name, itemId++);
    }

    // Add available MIDI output devices
    auto midiOutputs = juce::MidiOutput::getAvailableDevices();
    itemId = 1;
    for (auto& device : midiOutputs)
    {
        midiOutputSelector.addItem(device.name, itemId++);
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

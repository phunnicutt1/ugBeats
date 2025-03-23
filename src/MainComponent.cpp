#include "MainComponent.h"
#include "utils/AudioMath.h"
#include "synthesis/OscillatorBank.h"
#include "synthesis/EnvelopeProcessor.h"
#include "synthesis/FilterEnvelope.h"
#include "ui/components/synth/OscillatorPanel.h"
#include "ui/components/synth/EnvelopePanel.h"
#include "ui/components/synth/FilterPanel.h"
#include "ui/components/synth/FilterEnvelopePanel.h"

using namespace UndergroundBeats;

//==============================================================================
MainComponent::MainComponent() : 
    menuBar(this),
    tabs(juce::TabbedButtonBar::TabsAtTop),
    oscillatorNodeId(0),
    envelopeNodeId(0),
    filterNodeId(0),
    delayNodeId(0),
    reverbNodeId(0)
{
    try
    {
        juce::Logger::writeToLog("MainComponent: Constructor starting...");
        
        // Set up the menu bar
        juce::Logger::writeToLog("MainComponent: Setting up menu bar...");
        menuBar.setModel(this);
        addAndMakeVisible(menuBar);
        juce::Logger::writeToLog("MainComponent: Menu bar set up.");

        // Create tab components for different sections
        juce::Logger::writeToLog("MainComponent: Creating tab components...");
        createTabComponents();
        juce::Logger::writeToLog("MainComponent: Tab components created.");

        // Set up tab component
        juce::Logger::writeToLog("MainComponent: Setting up tab component...");
        addAndMakeVisible(tabs);
        juce::Logger::writeToLog("MainComponent: Tab component added.");
        
        // Add sequencer transport controls
        juce::Logger::writeToLog("MainComponent: Adding transport controls...");
        addAndMakeVisible(playButton);
        addAndMakeVisible(stopButton);
        addAndMakeVisible(recordButton);
        
        tempoSlider.setRange(40.0, 300.0, 1.0);
        tempoSlider.setValue(120.0);
        tempoSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);
        addAndMakeVisible(tempoSlider);
        
        tempoLabel.setText("Tempo (BPM)", juce::dontSendNotification);
        tempoLabel.setJustificationType(juce::Justification::centredLeft);
        addAndMakeVisible(tempoLabel);
        
        juce::Logger::writeToLog("MainComponent: Transport controls added.");
        
        // Connect transport controls to sequencer
        playButton.onClick = [this] { 
            sequencer->play();
            playButton.setEnabled(false);
            stopButton.setEnabled(true);
        };
        
        stopButton.onClick = [this] {
            sequencer->stop();
            playButton.setEnabled(true);
            stopButton.setEnabled(false);
        };
        
        // Play button starts playback
        playButton.onClick = [this] { 
            sequencer->play();
            playButton.setEnabled(false);
            stopButton.setEnabled(true);
        };
        
        // Stop button stops playback
        stopButton.onClick = [this] {
            sequencer->stop();
            playButton.setEnabled(true);
            stopButton.setEnabled(false);
        };
        
        // Record button toggles between play/stop for now (recording to be implemented)
        recordButton.onClick = [this] {
            sequencer->togglePlayStop();
            bool isPlaying = sequencer->isPlaying();
            recordButton.setToggleState(isPlaying, juce::dontSendNotification);
        };
        
        // Initialize button states
        stopButton.setEnabled(false);
        recordButton.setToggleState(false, juce::dontSendNotification);
        
        // Connect tempo slider
        tempoSlider.onValueChange = [this] {
            sequencer->setTempo(tempoSlider.getValue());
        };
        
        // Set up audio format manager and initialize audio processors
        juce::Logger::writeToLog("MainComponent: Setting up audio...");
        formatManager.registerBasicFormats();
        
        // Initialize processors
        createAudioProcessors();
        
        // Initialize audio
        setAudioChannels(2, 2);
        
        juce::Logger::writeToLog("MainComponent: Audio setup complete.");
        
        // Set component size
        juce::Logger::writeToLog("MainComponent: Setting component size...");
        setSize(1200, 800);
        juce::Logger::writeToLog("MainComponent: Component size set.");
        
        juce::Logger::writeToLog("MainComponent: Constructor completed successfully.");
    }
    catch (const std::exception& e)
    {
        juce::Logger::writeToLog("Exception in MainComponent constructor: " + juce::String(e.what()));
        throw;
    }
    catch (...)
    {
        juce::Logger::writeToLog("Unknown exception in MainComponent constructor");
        throw;
    }
}

MainComponent::~MainComponent()
{
    // Clean up resources
    shutdownAudio();
    
    // Clean up JUCE UI listeners
    menuBar.setModel(nullptr);
}

void MainComponent::createTabComponents()
{
    // Create tab components
    oscillatorTab = std::make_unique<juce::Component>();
    envelopeTab = std::make_unique<juce::Component>();
    filterTab = std::make_unique<juce::Component>();
    effectsTab = std::make_unique<juce::Component>();
    sequencerTab = std::make_unique<juce::Component>();
    mixerTab = std::make_unique<juce::Component>();
    
    // Add tabs
    tabs.addTab("Oscillators", juce::Colours::darkgrey, oscillatorTab.get(), false);
    tabs.addTab("Envelopes", juce::Colours::darkgrey, envelopeTab.get(), false);
    tabs.addTab("Filters", juce::Colours::darkgrey, filterTab.get(), false);
    tabs.addTab("Effects", juce::Colours::darkgrey, effectsTab.get(), false);
    tabs.addTab("Sequencer", juce::Colours::darkgrey, sequencerTab.get(), false);
    tabs.addTab("Mixer", juce::Colours::darkgrey, mixerTab.get(), false);
    
    // Create and add oscillator panel to oscillator tab
    juce::Logger::writeToLog("MainComponent: Creating oscillator panel...");
    oscillatorPanel = std::make_unique<OscillatorPanel>("Oscillator Bank");
    oscillatorTab->addAndMakeVisible(oscillatorPanel.get());
    
    // Create and add envelope panel to envelope tab
    juce::Logger::writeToLog("MainComponent: Creating envelope panel...");
    envelopePanel = std::make_unique<EnvelopePanel>("Envelope");
    envelopeTab->addAndMakeVisible(envelopePanel.get());
    
    // Create and add filter panel to filter tab
    juce::Logger::writeToLog("MainComponent: Creating filter panel...");
    filterPanel = std::make_unique<FilterPanel>("Filter");
    filterEnvelopePanel = std::make_unique<FilterEnvelopePanel>("Filter Envelope");
    
    auto* filterTabComponent = filterTab.get();
    filterTabComponent->addAndMakeVisible(filterPanel.get());
    filterTabComponent->addAndMakeVisible(filterEnvelopePanel.get());
}

void MainComponent::createAudioProcessors()
{
    // Create the oscillator bank
    oscillatorBank = std::make_unique<OscillatorBank>(2);
    
    // Create the envelope
    envelopeProcessor = std::make_unique<EnvelopeProcessor>();
    
    // Create the filter
    filter = std::make_unique<Filter>();
    
    // Create the filter envelope
    filterEnvelope = std::make_unique<FilterEnvelope>();
    
    // Create sequencer components
    sequencer = std::make_unique<Sequencer>();
    midiEngine = std::make_unique<MidiEngine>();
    timeline = std::make_shared<Timeline>();
    
    // Create effects
    delay = std::make_unique<Delay>();
    reverb = std::make_unique<UndergroundBeats::Reverb>();
    
    // Configure sequencer
    sequencer->setTempo(120.0);
    sequencer->setTimeSignature(4, 4);
    
    // Connect sequencer to MIDI engine
    sequencer->setNoteEventCallback([this](const NoteEvent& event) {
        if (event.velocity > 0) {  // Note On
            oscillatorBank->setMasterFrequency(AudioMath::midiNoteToFrequency(event.note));
            envelopeProcessor->noteOn();
            filterEnvelope->noteOn();
        } else {  // Note Off
            envelopeProcessor->noteOff();
            filterEnvelope->noteOff();
        }
    });
    
    // Set up the oscillator bank
    oscillatorBank->setMasterFrequency(440.0f);
    oscillatorBank->setMasterLevel(0.5f);
    
    // Set up the envelope
    envelopeProcessor->setAttackTime(10.0f);
    envelopeProcessor->setDecayTime(100.0f);
    envelopeProcessor->setSustainLevel(0.7f);
    envelopeProcessor->setReleaseTime(200.0f);
    
    // Set up the filter
    filter->setType(FilterType::LowPass);
    filter->setCutoff(1000.0f);
    filter->setResonance(0.5f);
    
    // Set up the filter envelope
    filterEnvelope->setBaseCutoff(1000.0f);
    filterEnvelope->setBaseResonance(0.5f);
    filterEnvelope->setCutoffEnvelopeAmount(0.8f);
    
    // Set up effects
    delay->setDelayTime(0, 300.0f); // Fix: Add channel parameter (0 = left)
    delay->setFeedback(0, 0.4f); // Fix: Add channel parameter (0 = left)
    delay->setMix(0.3f);
    
    reverb->setRoomSize(0.7f);
    reverb->setDamping(0.5f);
    reverb->setWidth(0.8f);
    reverb->setMix(0.3f);
    
    // Connect UI components to processors
    connectUIToProcessors();
}

void MainComponent::connectUIToProcessors()
{
    // Connect oscillator panel to oscillator bank
    oscillatorPanel->setFrequencyChangeCallback([this](float freq) {
        oscillatorBank->setMasterFrequency(freq);
    });
    
    oscillatorPanel->setWaveformChangeCallback([this](int index, WaveformType type) {
        oscillatorBank->setWaveform(index, type);
    });
    
    oscillatorPanel->setMixLevelChangeCallback([this](int index, float level) {
        oscillatorBank->setMixLevel(index, level);
    });
    
    oscillatorPanel->setFineTuningChangeCallback([this](int index, float cents) {
        oscillatorBank->setFinetuning(index, cents);
    });
    
    // Connect envelope panel to envelope processor
    envelopePanel->setAttackTimeChangeCallback([this](float time) {
        envelopeProcessor->setAttackTime(time);
    });
    
    envelopePanel->setDecayTimeChangeCallback([this](float time) {
        envelopeProcessor->setDecayTime(time);
    });
    
    envelopePanel->setSustainLevelChangeCallback([this](float level) {
        envelopeProcessor->setSustainLevel(level);
    });
    
    envelopePanel->setReleaseTimeChangeCallback([this](float time) {
        envelopeProcessor->setReleaseTime(time);
    });
    
    envelopePanel->setCurvesChangeCallback([this](float attack, float decay, float release) {
        envelopeProcessor->setCurves(attack, decay, release);
    });
    
    // Connect filter panel to filter
    filterPanel->setFilterTypeChangeCallback([this](FilterType type) {
        filter->setType(type);
    });
    
    filterPanel->setCutoffChangeCallback([this](float freq) {
        filter->setCutoff(freq);
    });
    
    filterPanel->setResonanceChangeCallback([this](float res) {
        filter->setResonance(res);
    });
    
    // Connect filter envelope panel to filter envelope
    filterEnvelopePanel->setCutoffModulationChangeCallback([this](float amount) {
        filterEnvelope->setCutoffEnvelopeAmount(amount);
    });
    
    filterEnvelopePanel->setResonanceModulationChangeCallback([this](float amount) {
        filterEnvelope->setResonanceEnvelopeAmount(amount);
    });
    
    // Set envelope values directly for now until the envelope panel is implemented
    // We'll skip trying to access filterEnvelopePanel->getEnvelopePanel() which throws exceptions
    filterEnvelope->setAttackTime(10.0f);
    filterEnvelope->setDecayTime(100.0f);
    filterEnvelope->setSustainLevel(0.7f);
    filterEnvelope->setReleaseTime(200.0f);
}

//==============================================================================
void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    // Prepare all audio processors
    juce::Logger::writeToLog("MainComponent: Preparing audio processors...");
    
    try {
        oscillatorBank->prepare(sampleRate);
        envelopeProcessor->prepare(sampleRate);
        filter->prepare(sampleRate);
        filterEnvelope->prepare(sampleRate);
        delay->prepare(sampleRate, samplesPerBlockExpected); // Fix: Add blockSize parameter
        reverb->prepare(sampleRate, samplesPerBlockExpected); // Fix: Add blockSize parameter
        
        // Prepare audio buffer
        audioBuffer.setSize(2, samplesPerBlockExpected);
        
        // Prepare sequencer components
        sequencer->prepare(sampleRate, samplesPerBlockExpected);
        
        // MidiEngine doesn't need prepare - it's handled by the sequencer
        
        juce::Logger::writeToLog("MainComponent: Audio processors and sequencer prepared.");
    }
    catch (const std::exception& e) {
        juce::Logger::writeToLog("Exception in prepareToPlay: " + juce::String(e.what()));
        throw;
    }
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    // Clear the buffer first
    bufferToFill.clearActiveBufferRegion();
    
    // Create temporary buffer for mono processing
    if (audioBuffer.getNumSamples() < bufferToFill.numSamples) {
        audioBuffer.setSize(2, bufferToFill.numSamples, false, true, true);
    }
    
    // Fill buffer with oscillator samples
    float* monoBuffer = audioBuffer.getWritePointer(0);
    std::fill(monoBuffer, monoBuffer + bufferToFill.numSamples, 0.0f);
    
    // Process through oscillator bank
    oscillatorBank->process(monoBuffer, bufferToFill.numSamples);
    
    // Process through envelope
    envelopeProcessor->process(monoBuffer, bufferToFill.numSamples);
    
    // Update filter envelope and calculate modulation
    float filterEnvValue = filterEnvelope->getEnvelope()->getCurrentLevel();  // Get current envelope value
    float cutoffMod = filterEnvelope->getBaseCutoff() * (1.0f + filterEnvValue * filterEnvelope->getCutoffEnvelopeAmount());
    float resonanceMod = filterEnvelope->getBaseResonance() * (1.0f + filterEnvValue * filterEnvelope->getResonanceEnvelopeAmount());
    
    // Apply modulated values to filter
    filter->setCutoff(cutoffMod);
    filter->setResonance(resonanceMod);
    
    // Process through filter
    filter->process(monoBuffer, bufferToFill.numSamples);
    
    // Process through effects
    delay->process(monoBuffer, bufferToFill.numSamples);
    reverb->process(monoBuffer, bufferToFill.numSamples);
    
    // Copy processed audio to both channels
    float* leftChannel = bufferToFill.buffer->getWritePointer(0, bufferToFill.startSample);
    float* rightChannel = bufferToFill.buffer->getWritePointer(1, bufferToFill.startSample);
    
    for (int i = 0; i < bufferToFill.numSamples; ++i) {
        leftChannel[i] = monoBuffer[i] * 0.5f;  // Reduce volume to prevent clipping
        rightChannel[i] = monoBuffer[i] * 0.5f;
    }
}

void MainComponent::releaseResources()
{
    // Clean up any resources
    juce::Logger::writeToLog("MainComponent: Releasing resources...");
    
    // Reset audio buffer
    audioBuffer.setSize(0, 0);
    
    juce::Logger::writeToLog("MainComponent: Resources released.");
}

//==============================================================================
void MainComponent::paint(juce::Graphics& g)
{
    // Fill the background
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void MainComponent::resized()
{
    // Simple layout with just the menu bar and tabs
    auto area = getLocalBounds();
    
    // Menu bar at the top
    menuBar.setBounds(area.removeFromTop(20));
    
    // Transport controls at the bottom
    auto transportArea = area.removeFromBottom(40);
    auto buttonWidth = 80;
    playButton.setBounds(transportArea.removeFromLeft(buttonWidth).reduced(5));
    stopButton.setBounds(transportArea.removeFromLeft(buttonWidth).reduced(5));
    recordButton.setBounds(transportArea.removeFromLeft(buttonWidth).reduced(5));
    
    // Tempo slider
    auto sliderWidth = 200;
    auto sliderArea = transportArea.removeFromLeft(sliderWidth).reduced(5);
    tempoSlider.setBounds(sliderArea);
    tempoLabel.setBounds(sliderArea.removeFromTop(20));
    
    // Tab component takes up the rest of the space
    tabs.setBounds(area);
    
    // Resize components in tabs
    resizeTabComponents();
}

void MainComponent::resizeTabComponents()
{
    // Resize oscillator panel
    if (oscillatorPanel && oscillatorTab)
    {
        oscillatorPanel->setBounds(oscillatorTab->getBounds().reduced(10));
    }
    
    // Resize envelope panel
    if (envelopePanel && envelopeTab)
    {
        envelopePanel->setBounds(envelopeTab->getBounds().reduced(10));
    }
    
    // Resize filter panels
    if (filterPanel && filterEnvelopePanel && filterTab)
    {
        juce::Rectangle<int> bounds = filterTab->getBounds().reduced(10);
        
        // Split vertically
        int halfHeight = bounds.getHeight() / 2;
        
        filterPanel->setBounds(bounds.removeFromTop(halfHeight).reduced(0, 5));
        filterEnvelopePanel->setBounds(bounds.reduced(0, 5));
    }
}

// Timer callback
void MainComponent::timerCallback()
{
    // Empty implementation
}

// ChangeListener callback
void MainComponent::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    // Empty implementation
}

// Menu creation and handling
juce::StringArray MainComponent::getMenuBarNames()
{
    return { "File", "Edit", "View", "Project", "Help" };
}

juce::PopupMenu MainComponent::getMenuForIndex(int menuIndex, const juce::String&)
{
    juce::PopupMenu menu;
    
    if (menuIndex == 0) // File menu
    {
        menu.addItem(1, "New Project");
        menu.addItem(2, "Open Project...");
        menu.addSeparator();
        menu.addItem(3, "Save Project");
        menu.addItem(4, "Save Project As...");
        menu.addSeparator();
        menu.addItem(5, "Exit");
    }
    
    return menu;
}

void MainComponent::menuItemSelected(int menuItemID, int)
{
    // Minimal implementation
    if (menuItemID == 5) // Exit
    {
        juce::JUCEApplication::getInstance()->systemRequestedQuit();
    }
}

// Projects
void MainComponent::newProject()
{
    // Empty implementation
}

void MainComponent::openProject()
{
    // Empty implementation
}

void MainComponent::saveProject(bool)
{
    // Empty implementation
}

void MainComponent::exportAudio()
{
    // Empty implementation
}

// Event handlers
void MainComponent::handleNoteEvent(const NoteEvent&)
{
    // Empty implementation
}

void MainComponent::handleParameterChange(const std::string&, float)
{
    // Empty implementation
}

// Processor creation
void MainComponent::createOscillator()
{
    // Empty implementation
}

void MainComponent::createEnvelope()
{
    // Empty implementation
}

void MainComponent::createFilter()
{
    // Empty implementation
}

void MainComponent::createDelay()
{
    // Empty implementation
}

void MainComponent::createReverb()
{
    // Empty implementation
}

// Connection
void MainComponent::connectProcessors()
{
    // Empty implementation
}

// UI update
void MainComponent::updateEffectsUI()
{
    // Empty implementation
}

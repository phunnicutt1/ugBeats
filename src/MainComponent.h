#pragma once

#include <JuceHeader.h>
#include "audio-engine/Engine.h"
#include "synthesis/Oscillator.h"
#include "synthesis/OscillatorBank.h"
#include "synthesis/Envelope.h"
#include "synthesis/EnvelopeProcessor.h"
#include "synthesis/Filter.h"
#include "synthesis/FilterEnvelope.h"
#include "effects/Delay.h"
#include "effects/Reverb.h"
#include "effects/EffectProcessorNode.h"
#include "sequencer/Sequencer.h"
#include "sequencer/MidiEngine.h"
#include "project/ProjectManager.h"
#include "ui/views/MixerView.h"
#include "ui/views/PatternEditorView.h"
#include "ui/components/synth/OscillatorPanel.h"
#include "ui/components/synth/EnvelopePanel.h"
#include "ui/components/synth/FilterPanel.h"
#include "ui/components/synth/FilterEnvelopePanel.h"

using namespace UndergroundBeats;

//==============================================================================
class MainComponent  : public juce::AudioAppComponent,
                       private juce::Timer,
                       private juce::ChangeListener,
                       public juce::MenuBarModel
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    void paint (juce::Graphics& g) override;
    void resized() override;
    
    // Timer callback for handling envelope trigger
    void timerCallback() override;
    
    // ChangeListener callback for project changes
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;

private:
    //==============================================================================
    // Audio engine components
    Engine audioEngine;
    std::unique_ptr<OscillatorBank> oscillatorBank;
    std::unique_ptr<EnvelopeProcessor> envelopeProcessor;
    std::unique_ptr<Filter> filter;
    std::unique_ptr<FilterEnvelope> filterEnvelope;
    std::unique_ptr<Delay> delay;
    std::unique_ptr<UndergroundBeats::Reverb> reverb;
    
    // Sequencer components
    std::unique_ptr<Sequencer> sequencer;
    std::unique_ptr<MidiEngine> midiEngine;
    std::shared_ptr<Timeline> timeline;
    std::unique_ptr<ProjectManager> projectManager;
    
    // Audio buffer for processing
    juce::AudioBuffer<float> audioBuffer;
    
    // Audio format manager
    juce::AudioFormatManager formatManager;
    
    // UI components
    juce::TextButton startButton { "Start Engine" };
    
    // Main menu bar
    juce::MenuBarComponent menuBar;
    
    // UI panel components
    std::unique_ptr<OscillatorPanel> oscillatorPanel;
    std::unique_ptr<EnvelopePanel> envelopePanel;
    std::unique_ptr<FilterPanel> filterPanel;
    std::unique_ptr<FilterEnvelopePanel> filterEnvelopePanel;
    
    // Sequencer controls
    juce::TextButton playButton { "Play" };
    juce::TextButton stopButton { "Stop" };
    juce::TextButton recordButton { "Record" };
    juce::Slider tempoSlider;
    juce::Label tempoLabel { {}, "Tempo (BPM)" };
    
    // Tab component for different sections
    juce::TabbedComponent tabs;
    
    // Container components for each tab
    std::unique_ptr<juce::Component> oscillatorTab;
    std::unique_ptr<juce::Component> envelopeTab;
    std::unique_ptr<juce::Component> filterTab;
    std::unique_ptr<juce::Component> effectsTab;
    std::unique_ptr<juce::Component> sequencerTab;
    std::unique_ptr<juce::Component> mixerTab;
    
    // View components
    std::unique_ptr<MixerView> mixerView;
    std::unique_ptr<PatternEditorView> patternEditorView;
    
    // Node IDs for the processor graph
    UndergroundBeats::NodeID oscillatorNodeId;
    UndergroundBeats::NodeID envelopeNodeId;
    UndergroundBeats::NodeID filterNodeId;
    UndergroundBeats::NodeID delayNodeId;
    UndergroundBeats::NodeID reverbNodeId;
    
    // Menu creation and handling - implement MenuBarModel
    juce::ApplicationCommandManager commandManager;
    juce::StringArray getMenuBarNames() override;
    juce::PopupMenu getMenuForIndex(int menuIndex, const juce::String& menuName) override;
    void menuItemSelected(int menuItemID, int topLevelMenuIndex) override;
    
    // Command IDs for menu items
    enum CommandIDs
    {
        NewProject = 1,
        OpenProject,
        SaveProject,
        SaveProjectAs,
        ExportAudio,
        Preferences,
        
        Undo,
        Redo,
        Cut,
        Copy,
        Paste,
        Delete,
        SelectAll,
        
        AddPattern,
        AddTrack,
        DeletePattern,
        DeleteTrack
    };
    
    // Component creation and setup
    void createTabComponents();
    void createAudioProcessors();
    void connectUIToProcessors();
    void resizeTabComponents();
    
    // Create processors
    void createOscillator();
    void createEnvelope();
    void createFilter();
    void createDelay();
    void createReverb();
    
    // Connect processors in the graph
    void connectProcessors();
    
    // Update UI based on current effect
    void updateEffectsUI();
    
    // Project management
    void newProject();
    void openProject();
    void saveProject(bool saveAs = false);
    void exportAudio();
    
    // Handle sequencer events
    void handleNoteEvent(const NoteEvent& event);
    void handleParameterChange(const std::string& paramId, float value);
    
    // Active effect type
    enum EffectType
    {
        NoEffect,
        DelayEffect,
        ReverbEffect
    };
    
    EffectType currentEffect = NoEffect;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};

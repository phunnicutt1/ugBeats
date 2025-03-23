#pragma once

#include <JuceHeader.h>
#include "../../sequencer/Pattern.h"
#include "../../sequencer/Timeline.h"

class PatternGridComponent : public juce::Component,
                           public juce::Timer
{
public:
    PatternGridComponent();
    ~PatternGridComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent& e) override;
    void mouseDrag(const juce::MouseEvent& e) override;
    void mouseUp(const juce::MouseEvent& e) override;

    void setPattern(std::shared_ptr<UndergroundBeats::Pattern> pattern);
    void setGridSize(int numRows, int numColumns);
    void setSnapToGrid(bool shouldSnap);

private:
    void timerCallback() override;
    void drawGrid(juce::Graphics& g);
    void drawNotes(juce::Graphics& g);
    juce::Point<int> pointToGridPosition(juce::Point<int> point) const;
    
    std::shared_ptr<UndergroundBeats::Pattern> pattern;
    
    int rows { 16 };    // Number of pitch rows
    int columns { 16 }; // Number of time steps
    bool snapToGrid { true };
    
    // Grid state
    struct GridCell {
        bool isActive { false };
        int velocity { 100 };
    };
    std::vector<std::vector<GridCell>> grid;
    
    // Editing state
    bool isEditing { false };
    bool drawMode { true }; // true = draw, false = erase
    juce::Point<int> lastGridPosition { -1, -1 };
    
    // Pattern state tracking
    int lastNoteCount = 0;
    double lastPlaybackPosition = 0.0;
    
    // Constants
    static constexpr int UPDATE_INTERVAL_MS = 50;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PatternGridComponent)
};

#include "PatternGridComponent.h"

PatternGridComponent::PatternGridComponent()
{
    setOpaque(true);
    startTimerHz(1000 / UPDATE_INTERVAL_MS);
    
    // Initialize grid
    grid.resize(rows, std::vector<GridCell>(columns));
}

PatternGridComponent::~PatternGridComponent()
{
    stopTimer();
}

void PatternGridComponent::paint(juce::Graphics& g)
{
    // Fill background
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    
    drawGrid(g);
    drawNotes(g);
}

void PatternGridComponent::drawGrid(juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    float cellWidth = bounds.getWidth() / static_cast<float>(columns);
    float cellHeight = bounds.getHeight() / static_cast<float>(rows);
    
    // Draw vertical lines
    g.setColour(juce::Colours::grey.withAlpha(0.3f));
    for (int x = 0; x <= columns; ++x)
    {
        float xPos = x * cellWidth;
        g.drawLine(xPos, 0.0f, xPos, static_cast<float>(bounds.getHeight()));
    }
    
    // Draw horizontal lines
    for (int y = 0; y <= rows; ++y)
    {
        float yPos = y * cellHeight;
        g.drawLine(0.0f, yPos, static_cast<float>(bounds.getWidth()), yPos);
    }
    
    // Highlight beat divisions
    g.setColour(juce::Colours::grey.withAlpha(0.5f));
    for (int x = 0; x < columns; x += 4)
    {
        float xPos = x * cellWidth;
        g.drawLine(xPos, 0.0f, xPos, static_cast<float>(bounds.getHeight()), 2.0f);
    }
}

void PatternGridComponent::drawNotes(juce::Graphics& g)
{
    if (!pattern)
        return;

    auto bounds = getLocalBounds();
    float cellWidth = bounds.getWidth() / static_cast<float>(columns);
    float cellHeight = bounds.getHeight() / static_cast<float>(rows);
    
    // Draw all notes
    for (int y = 0; y < rows; ++y)
    {
        for (int x = 0; x < columns; ++x)
        {
            if (grid[y][x].isActive)
            {
                float alpha = grid[y][x].velocity / 127.0f;
                g.setColour(juce::Colours::orange.withAlpha(alpha));
                
                float xPos = x * cellWidth;
                float yPos = y * cellHeight;
                g.fillRect(xPos + 1, yPos + 1, cellWidth - 2, cellHeight - 2);
            }
        }
    }
    
    // Highlight active notes
    auto activeNotes = pattern->getActiveNotes();
    for (const auto& note : activeNotes)
    {
        int x = static_cast<int>(note.startTime * 4.0); // Assuming 16th note grid
        int y = rows - 1 - (note.note % rows); // Map MIDI notes to grid rows
        
        if (x >= 0 && x < columns && y >= 0 && y < rows)
        {
            float xPos = x * cellWidth;
            float yPos = y * cellHeight;
            
            // Draw highlight
            g.setColour(juce::Colours::yellow.withAlpha(0.5f));
            g.fillRect(xPos + 1, yPos + 1, cellWidth - 2, cellHeight - 2);
            
            // Draw border
            g.setColour(juce::Colours::yellow);
            g.drawRect(xPos + 1, yPos + 1, cellWidth - 2, cellHeight - 2, 2.0f);
        }
    }
    
    // Draw playback position line
    if (pattern)
    {
        double position = pattern->getCurrentPosition();
        int positionColumn = static_cast<int>(position * 4.0); // Convert beats to grid columns
        
        if (positionColumn >= 0 && positionColumn < columns)
        {
            float xPos = positionColumn * cellWidth;
            
            // Draw vertical playback line
            g.setColour(juce::Colours::white);
            g.drawLine(xPos, 0, xPos, static_cast<float>(bounds.getHeight()), 2.0f);
        }
    }
}

void PatternGridComponent::mouseDown(const juce::MouseEvent& e)
{
    if (!pattern)
        return;

    isEditing = true;
    drawMode = !e.mods.isRightButtonDown(); // Left click = draw, Right click = erase
    
    auto gridPos = pointToGridPosition(e.position.toInt());
    if (gridPos.x >= 0 && gridPos.y >= 0)
    {
        lastGridPosition = gridPos;
        
        if (drawMode)
        {
            // Calculate velocity based on vertical position within cell
            auto bounds = getLocalBounds();
            float cellHeight = bounds.getHeight() / static_cast<float>(rows);
            float cellY = gridPos.y * cellHeight;
            float relativeY = e.position.y - cellY;
            int velocity = juce::jlimit(1, 127, static_cast<int>((1.0f - (relativeY / cellHeight)) * 127.0f));
            
            grid[gridPos.y][gridPos.x].isActive = true;
            grid[gridPos.y][gridPos.x].velocity = velocity;
            
            // Add note to pattern
            int noteNumber = rows - 1 - gridPos.y;
            double startTime = gridPos.x / 4.0; // Convert grid position to beats
            pattern->addNote(noteNumber, velocity, startTime, 0.25); // Quarter beat duration
        }
        else
        {
            // Convert grid position to musical values
            int noteNumber = rows - 1 - gridPos.y;
            double startTime = gridPos.x / 4.0;
            
            // Find and remove note at this position
            int noteIndex = pattern->findNoteAt(startTime, noteNumber);
            if (noteIndex >= 0)
            {
                pattern->removeNote(noteIndex);
                grid[gridPos.y][gridPos.x].isActive = false;
            }
        }
        
        repaint();
    }
}

void PatternGridComponent::mouseDrag(const juce::MouseEvent& e)
{
    if (!isEditing || !pattern)
        return;
        
    auto gridPos = pointToGridPosition(e.position.toInt());
    if (gridPos.x >= 0 && gridPos.y >= 0 && gridPos != lastGridPosition)
    {
        lastGridPosition = gridPos;
        
        if (drawMode)
        {
            // Calculate velocity as in mouseDown
            auto bounds = getLocalBounds();
            float cellHeight = bounds.getHeight() / static_cast<float>(rows);
            float cellY = gridPos.y * cellHeight;
            float relativeY = e.position.y - cellY;
            int velocity = juce::jlimit(1, 127, static_cast<int>((1.0f - (relativeY / cellHeight)) * 127.0f));
            
            grid[gridPos.y][gridPos.x].isActive = true;
            grid[gridPos.y][gridPos.x].velocity = velocity;
            
            // Add note to pattern
            int noteNumber = rows - 1 - gridPos.y;
            double startTime = gridPos.x / 4.0;
            pattern->addNote(noteNumber, velocity, startTime, 0.25);
        }
        else
        {
            // Convert grid position to musical values
            int noteNumber = rows - 1 - gridPos.y;
            double startTime = gridPos.x / 4.0;
            
            // Find and remove note at this position
            int noteIndex = pattern->findNoteAt(startTime, noteNumber);
            if (noteIndex >= 0)
            {
                pattern->removeNote(noteIndex);
                grid[gridPos.y][gridPos.x].isActive = false;
            }
        }
        
        repaint();
    }
}

void PatternGridComponent::mouseUp(const juce::MouseEvent&)
{
    isEditing = false;
    lastGridPosition = juce::Point<int>(-1, -1);
}

void PatternGridComponent::setPattern(std::shared_ptr<UndergroundBeats::Pattern> newPattern)
{
    pattern = newPattern;
    
    if (pattern)
    {
        // Update grid dimensions if needed based on pattern length
        int newColumns = static_cast<int>(pattern->getLength() * 4.0); // 4 grid columns per beat
        if (newColumns != columns)
        {
            columns = newColumns;
            grid.resize(rows, std::vector<GridCell>(columns));
        }
        
        // Initialize tracking variables
        lastNoteCount = pattern->getNoteCount();
        
        // Update grid contents
        updateGridFromPattern();
    }
    else
    {
        // Clear grid if no pattern
        for (auto& row : grid)
            std::fill(row.begin(), row.end(), GridCell());
    }
    
    repaint();
}

void PatternGridComponent::timerCallback()
{
    if (pattern)
    {
        // Always repaint if we have a pattern, to update playback position line
        // and active note highlighting
        repaint();
        
        // Check if notes have changed
        if (pattern->getNoteCount() != lastNoteCount)
        {
            updateGridFromPattern();
            lastNoteCount = pattern->getNoteCount();
        }
    }
}

void PatternGridComponent::updateGridFromPattern()
{
    if (!pattern)
        return;
        
    // Clear grid
    for (auto& row : grid)
        std::fill(row.begin(), row.end(), GridCell());
    
    // Update from pattern
    for (int i = 0; i < pattern->getNoteCount(); ++i)
    {
        auto note = pattern->getNote(i);
        int x = static_cast<int>(note->startTime * 4.0); // Assuming 16th note grid
        int y = rows - 1 - (note->note % rows); // Map MIDI notes to grid rows
        
        if (x >= 0 && x < columns && y >= 0 && y < rows)
        {
            grid[y][x].isActive = true;
            grid[y][x].velocity = note->velocity;
        }
    }
}

void PatternGridComponent::setGridSize(int numRows, int numColumns)
{
    // Validate inputs
    rows = std::max(1, numRows);
    columns = std::max(1, numColumns);
    
    // Resize grid
    grid.resize(rows);
    for (auto& row : grid)
    {
        row.resize(columns);
    }
    
    // If we have a pattern, update its length to match grid
    if (pattern)
    {
        double newLength = columns / 4.0; // 4 grid columns per beat
        pattern->setLength(newLength);
        updateGridFromPattern();
    }
    
    repaint();
}

void PatternGridComponent::setSnapToGrid(bool shouldSnap)
{
    snapToGrid = shouldSnap;
}

juce::Point<int> PatternGridComponent::pointToGridPosition(juce::Point<int> point) const
{
    auto bounds = getLocalBounds();
    float cellWidth = bounds.getWidth() / static_cast<float>(columns);
    float cellHeight = bounds.getHeight() / static_cast<float>(rows);
    
    int gridX = static_cast<int>(point.x / cellWidth);
    int gridY = static_cast<int>(point.y / cellHeight);
    
    if (gridX >= 0 && gridX < columns && gridY >= 0 && gridY < rows)
        return juce::Point<int>(gridX, gridY);
        
    return juce::Point<int>(-1, -1);
}

void PatternGridComponent::resized()
{
    // Grid sizing is handled in paint()
}

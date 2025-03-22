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
    auto bounds = getLocalBounds();
    float cellWidth = bounds.getWidth() / static_cast<float>(columns);
    float cellHeight = bounds.getHeight() / static_cast<float>(rows);
    
    g.setColour(juce::Colours::orange);
    
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
}

void PatternGridComponent::mouseDown(const juce::MouseEvent& e)
{
    isEditing = true;
    drawMode = !e.mods.isRightButtonDown(); // Left click = draw, Right click = erase
    
    auto gridPos = pointToGridPosition(e.position.toInt());
    if (gridPos.x >= 0 && gridPos.y >= 0)
    {
        lastGridPosition = gridPos;
        grid[gridPos.y][gridPos.x].isActive = drawMode;
        repaint();
    }
}

void PatternGridComponent::mouseDrag(const juce::MouseEvent& e)
{
    if (!isEditing)
        return;
        
    auto gridPos = pointToGridPosition(e.position.toInt());
    if (gridPos.x >= 0 && gridPos.y >= 0 && gridPos != lastGridPosition)
    {
        lastGridPosition = gridPos;
        grid[gridPos.y][gridPos.x].isActive = drawMode;
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
    
    // Clear existing grid
    for (auto& row : grid)
        std::fill(row.begin(), row.end(), GridCell());
        
    if (pattern)
    {
        // Convert pattern notes to grid cells
        for (int i = 0; i < pattern->getNoteCount(); ++i)
        {
            auto note = pattern->getNote(i);
            int x = static_cast<int>(note.startTime * 4.0); // Assuming 16th note grid
            int y = rows - 1 - (note.note % rows); // Map MIDI notes to grid rows
            
            if (x >= 0 && x < columns && y >= 0 && y < rows)
            {
                grid[y][x].isActive = true;
                grid[y][x].velocity = note.velocity;
            }
        }
    }
    
    repaint();
}

void PatternGridComponent::timerCallback()
{
    if (pattern)
    {
        // Update grid from pattern if needed
        bool gridChanged = false;
        
        for (int y = 0; y < rows; ++y)
        {
            for (int x = 0; x < columns; ++x)
            {
                if (grid[y][x].isActive)
                {
                    // Convert grid position to note parameters
                    int noteNumber = rows - 1 - y; // Convert row to MIDI note
                    double startTime = x / 4.0; // Convert column to beats
                    
                    // Add or update note in pattern
                    pattern->addNote(noteNumber, grid[y][x].velocity, startTime, 0.25);
                    gridChanged = true;
                }
            }
        }
        
        if (gridChanged)
            pattern->notifyPatternChanged();
    }
    
    repaint();
}

void PatternGridComponent::setGridSize(int numRows, int numColumns)
{
    rows = numRows;
    columns = numColumns;
    grid.resize(rows, std::vector<GridCell>(columns));
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

void PatternGridComponent::timerCallback()
{
    // TODO: Update from pattern playback position
    repaint();
}

void PatternGridComponent::resized()
{
    // Grid sizing is handled in paint()
}

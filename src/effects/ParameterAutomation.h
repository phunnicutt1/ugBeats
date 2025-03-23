/*
 * Underground Beats
 * ParameterAutomation.h
 * 
 * Handles automated parameter changes for effects
 */

#pragma once

#include <JuceHeader.h>
#include <vector>
#include <memory>
#include <string>
#include "../common/AutomationTypes.h"

namespace UndergroundBeats {

/**
 * @class ParameterAutomation
 * @brief Manages automation for effect parameters
 * 
 * Provides functionality for:
 * - Recording and playing back parameter changes
 * - Different interpolation curves between points
 * - Time-synchronized parameter updates
 */
class ParameterAutomation {
public:
    ParameterAutomation(const std::string& parameterName);
    ~ParameterAutomation();
    
    /**
     * @brief Add an automation point
     * 
     * @param time Time in seconds
     * @param value Parameter value
     * @param curveType Type of curve to next point
     */
    void addPoint(double time, float value, CurveType curveType = CurveType::Linear);
    
    /**
     * @brief Remove an automation point
     * 
     * @param index Index of point to remove
     */
    void removePoint(size_t index);
    
    /**
     * @brief Get the value at a specific time
     * 
     * @param time Time in seconds
     * @return Interpolated parameter value
     */
    float getValueAtTime(double time) const;
    
    /**
     * @brief Clear all automation points
     */
    void clear();
    
    /**
     * @brief Get the parameter name
     */
    const std::string& getParameterName() const { return parameterName; }
    
    /**
     * @brief Get all automation points
     */
    const std::vector<AutomationPoint>& getPoints() const { return points; }
    
    /**
     * @brief Save automation data to XML
     */
    std::unique_ptr<juce::XmlElement> createXml() const;
    
    /**
     * @brief Load automation data from XML
     */
    bool loadFromXml(const juce::XmlElement* xml);

private:
    std::string parameterName;
    std::vector<AutomationPoint> points;
    
    /**
     * @brief Interpolate between two points
     */
    float interpolate(const AutomationPoint& p1, const AutomationPoint& p2, double time) const;
};

} // namespace UndergroundBeats

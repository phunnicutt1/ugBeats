/**
 * Underground Beats
 * AutomationTypes.h
 * 
 * Common automation types used across the application
 */

#pragma once

namespace UndergroundBeats {

/**
 * @brief Common curve types for automation
 */
enum class CurveType {
    Linear,
    Exponential,
    Logarithmic,
    SCurve,
    Step
};

/**
 * @brief Data structure to represent a parameter automation point
 */
struct AutomationPoint {
    double time;       // Time in beats/seconds
    float value;       // Parameter value (0-1)
    CurveType curveType;  // Type of curve to next point
    
    AutomationPoint(double t = 0.0, float v = 0.0, CurveType c = CurveType::Linear)
        : time(t), value(v), curveType(c)
    {
    }
};

} // namespace UndergroundBeats

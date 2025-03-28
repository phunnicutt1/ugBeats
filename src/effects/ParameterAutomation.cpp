/*
 * Underground Beats
 * ParameterAutomation.cpp
 */

#include "ParameterAutomation.h"
#include <algorithm>

namespace UndergroundBeats {

ParameterAutomation::ParameterAutomation(const std::string& name)
    : parameterName(name) {}

ParameterAutomation::~ParameterAutomation() = default;

void ParameterAutomation::addPoint(double time, float value, CurveType curveType) {
    AutomationPoint newPoint(time, value, curveType);
    
    // Find insertion point to maintain time-sorted order
    auto it = std::lower_bound(points.begin(), points.end(), newPoint,
        [](const AutomationPoint& a, const AutomationPoint& b) {
            return a.time < b.time;
        });
    
    points.insert(it, newPoint);
}

void ParameterAutomation::removePoint(size_t index) {
    if (index < points.size()) {
        points.erase(points.begin() + index);
    }
}

float ParameterAutomation::getValueAtTime(double time) const {
    if (points.empty()) {
        return 0.0f;
    }
    
    // If time is before first point, return first point's value
    if (time <= points.front().time) {
        return points.front().value;
    }
    
    // If time is after last point, return last point's value
    if (time >= points.back().time) {
        return points.back().value;
    }
    
    // Find the points that surround the requested time
    auto it = std::lower_bound(points.begin(), points.end(), time,
        [](const AutomationPoint& point, double t) {
            return point.time < t;
        });
    
    if (it != points.begin()) {
        auto prevPoint = std::prev(it);
        return interpolate(*prevPoint, *it, time);
    }
    
    return it->value;
}

void ParameterAutomation::clear() {
    points.clear();
}

float ParameterAutomation::interpolate(const AutomationPoint& p1, const AutomationPoint& p2, double time) const {
    if (p1.time == p2.time) {
        return p2.value;
    }
    
    // Calculate normalized position between points (0 to 1)
    double t = (time - p1.time) / (p2.time - p1.time);
    
    // Apply curve type
    switch (p1.curveType) {
        case CurveType::Linear:
            // Linear interpolation
            return p1.value + (p2.value - p1.value) * static_cast<float>(t);
            
        case CurveType::Exponential:
            // Exponential curve
            t = t * t;
            return p1.value + (p2.value - p1.value) * static_cast<float>(t);
            
        case CurveType::Logarithmic:
            // Logarithmic curve
            t = std::sqrt(t);
            return p1.value + (p2.value - p1.value) * static_cast<float>(t);
            
        case CurveType::SCurve:
            // S-curve using sigmoid function
            {
                double x = (t - 0.5) * 2.0; // Scale to -1 to 1
                double s = (1.0 / (1.0 + std::exp(-6.0 * x)) - 0.5) * 2.0;
                return p1.value + (p2.value - p1.value) * static_cast<float>(s);
            }
            
        case CurveType::Step:
            // Step function - jump at midpoint
            return (t < 0.5) ? p1.value : p2.value;
            
        default:
            // Default to linear
            return p1.value + (p2.value - p1.value) * static_cast<float>(t);
    }
}

std::unique_ptr<juce::XmlElement> ParameterAutomation::createXml() const {
    auto xml = std::make_unique<juce::XmlElement>("ParameterAutomation");
    xml->setAttribute("parameter", parameterName);
    
    for (const auto& point : points) {
        auto pointXml = xml->createNewChildElement("Point");
        pointXml->setAttribute("time", point.time);
        pointXml->setAttribute("value", point.value);
        // Store curve type as int for backward compatibility
        pointXml->setAttribute("curveType", static_cast<int>(point.curveType));
    }
    
    return xml;
}

bool ParameterAutomation::loadFromXml(const juce::XmlElement* xml) {
    if (xml == nullptr || xml->getTagName() != "ParameterAutomation") {
        return false;
    }
    
    clear();
    
    for (auto* pointXml : xml->getChildWithTagNameIterator("Point")) {
        double time = pointXml->getDoubleAttribute("time");
        float value = pointXml->getFloatAttribute("value");
        auto curveType = static_cast<juce::CurveType>(pointXml->getIntAttribute("curve"));
        addPoint(time, value, curveType);
    }
    
    return true;
}

} // namespace UndergroundBeats

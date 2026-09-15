#ifndef TELEMETRY_HPP
#define TELEMETRY_HPP

#include <string>
#include <sstream>
#include <iomanip>
#include "celestial_body.hpp"

class TelemetryLogger {
public:
    // Serialize single craft state into compact JSON
    static std::string serialize_craft(double timestamp, const CelestialBody& craft, double fuel_kg) {
        std::ostringstream json;
        json << std::fixed << std::setprecision(3);

        json << "{"
             << "\"t\":" << timestamp << ","
             << "\"name\":\"" << craft.name << "\","
             << "\"pos\":{\"x\":" << craft.position.x << ",\"y\":" << craft.position.y << ",\"z\":" << craft.position.z << "}," 
             << "\"vel\":{\"x\":" << craft.velocity.x << ",\"y\":" << craft.velocity.y << ",\"z\":" << craft.velocity.z << "},"
             << "\"speed\":" << craft.velocity.magnitude() <<","
             << "\"fuel\":" << fuel_kg
             << "}";
        return json.str();
    }
};

#endif


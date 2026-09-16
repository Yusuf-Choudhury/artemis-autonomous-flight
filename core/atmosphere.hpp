#pragma once
#include "physics_engine.hpp"
#include <cmath>

class Atmosphere {
public:
    // Calculates atmospheric density based on altitude (Exponential atmosphere model)
    static double get_density(double altitude_meters) {
        constexpr double RHO_0 = 1.225; // Base density at sea level (kg/m^3)
        constexpr double H = 8500.0;    // Scale height for Earth (meters)

        if (altitude_meters > 150000.0) {
            return 0.0; // Negligible drag above 150km (Karman line boundary)
        }
        return RHO_0 * std::exp(-altitude_meters / H);
    }

    // Calculates aerodynamic drag force vector
    static Vector3 calculate_drag(const CraftState& state, double drag_coefficient, double cross_sectional_area) {
        double altitude = state.position.magnitude() - EARTH_RADIUS;
        double density = get_density(altitude);

        if (density == 0.0) return Vector3(0.0, 0.0, 0.0);

        double velocity_mag = state.velocity.magnitude();
        if (velocity_mag == 0.0) return Vector3(0.0, 0.0, 0.0);

        // Standard Drag Equation: Fd = 0.5 * rho * v^2 * Cd * A
        double drag_magnitude = 0.5 * density * (velocity_mag * velocity_mag) * drag_coefficient * cross_sectional_area;

        // Drag acts in the exact opposite direction of the velocity vector
        Vector3 drag_direction = state.velocity.normalize() * -1.0;

        // Return acceleration due to drag (F = ma -> a = F/m)
        return (drag_direction * drag_magnitude) / state.mass;
    }
};
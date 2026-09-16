#pragma once
#include "vector3.hpp"

// Universal Aerospace Constants
constexpr double EARTH_MASS = 5.9722e24;             // kg
constexpr double G = 6.67430e-11;                    // Gravitational constant N*(m/kg)^2
constexpr double MU = G * EARTH_MASS;                // Standard gravitational parameter (GM)
constexpr double EARTH_RADIUS = 6371000.0;           // meters

// Spacecraft State Vector (6-Degrees of Freedom)
struct CraftState {
    double time;           // Mission Elapsed Time (seconds)
    Vector3 position;      // Position vector relative to Earth center (m)
    Vector3 velocity;      // Velocity vector (m/s)
    double mass;           // Total vehicle mass (kg)
    double fuel;           // Remaining propellant mass (kg)
};

// Derivative container for numerical integration (RK4)
struct Derivative {
    Vector3 dx;            // Velocity
    Vector3 dv;            // Acceleration
};
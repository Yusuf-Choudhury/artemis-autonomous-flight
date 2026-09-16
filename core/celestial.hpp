#pragma once
#include "vector3.hpp"
#include "physics_engine.hpp"
#include <cmath>

class CelestialMechanics {
public:
    // Calculates the J2 perturbation (Earth's equatorial bulge gravitational effect)
    static Vector3 calculate_j2_effect(const Vector3& pos) {
        // J2 Constant for Earth (dimensionless)
        constexpr double J2 = 1.08262668e-3;

        double r = pos.magnitude();
        if (r == 0.0) return Vector3(0.0, 0.0, 0.0);

        double r2 = r * r;
        double r5 = r2 * r2 * r;
        double z2 = pos.z * pos.z;

        // Common mathematical factor for the J2 acceleration vector
        double factor = (1.5 * J2 * MU * (EARTH_RADIUS * EARTH_RADIUS)) / r5;

        // Calculate perturbation along the X, Y, and Z axes
        double ax = factor * pos.x * ((5.0 * z2 / r2) - 1.0);
        double ay = factor * pos.y * ((5.0 * z2 / r2) - 1.0);
        double az = factor * pos.z * ((5.0 * z2 / r2) - 3.0);

        return Vector3(ax, ay, az);
    }
};
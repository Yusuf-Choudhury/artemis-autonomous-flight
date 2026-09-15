#ifndef CELESTIAL_BODY_HPP
#define CELESTIAL_BODY_HPP

#include <string>
#include "vector3.hpp"

struct CelestialBody {
    std::string name;
    double mass;         //kilograms (Kg)
    double radius;       // Surface radius in meters (m)
    Vector3 position;    // 3D Cartesian coordinates (m)
    Vector3 velocity;    // Velocity vector (m/s)
    Vector3 force;       // Accumulated force vector (N)

    CelestialBody(std::string id, double m, double r, Vector3 pos, Vector3 vel)
        : name(id), mass(m), radius(r), position(pos), velocity(vel), force(0.0, 0.0, 0.0) {}

    // Clear accumulated forces at start of each physisc tick
    void clear_force() {
        force = Vector3(0.0, 0.0, 0.0);
    }

    //Sum forces (gravity pulls from multiple celestial bodies + thrusters)
    void apply_force(const Vector3& f) {
        force = force + f;
    }
};

#endif


#ifndef PHYSICS_ENGINE_HPP
#define PHYSICS_ENGINE_HPP

#include <vector>
#include <cmath>
#include "celestial_body.hpp"

class PhysicsEngine {
public:
    const double G = 6.67430e-11; // Universak Gravitational Constant (m^3 kg^1 s^1)
    std::vector<CelestialBody> bodies;

    void add_body(const CelestialBody& body) {
        bodies.push_back(body);
    }

    // Calculate mutual gravitational forces between all celestial bodies
    void compute_gravity() {
        // Reset forces from previous tick
        for (auto& body : bodies) {
            body.clear_force();
        }

        // Compute pairwise N-body gravitational interaction
        for (size_t i = 0; i < bodies.size(); i++) {
            for (size_t j = i + 1; j < bodies.size(); j++) {
                Vector3 displacement = bodies[j].position - bodies[i].position;
                double distance = displacement.magnitude();

                //Prevent division by zero if objects occupy the exact same point
                if (distance < 1.0) continue;

                //F = G * (m1 * m2) / r^2
                double force_mag = (G * bodies[j].mass) /  (distance * distance);
                Vector3 force_dir = displacement.normalize();

                Vector3 force_on_i = force_dir * force_mag;
                Vector3 force_on_j = force_dir * (-force_mag);

                bodies[i].apply_force(force_on_i);
                bodies[j].apply_force(force_on_j);
            }
        }
    }

    // Integrate acceleration to update velocity and position 
    void step(double dt) {
        compute_gravity();

        for (auto& body : bodies) {
            // Newton's second Law: a = F / m
            Vector3 acceleration = body.force * (1.0 / body.mass);

            // Semi-implicit Euler integration
            body.velocity = body.velocity + (acceleration * dt);
            body.position = body.position + (body.velocity * dt);
        }
    }
};

#endif
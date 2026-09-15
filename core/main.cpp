#include <iostream>
#include <iomanip>
#include "physics_engine.hpp"

int main() {
    PhysicsEngine sim;

    // 1. Earth at origin (0, 0, 0)
    CelestialBody earth(
        "Earth",
        5.972e24,                // Mass (Kg)
        6371000.0,               // Radius (m)
        Vector3(0, 0, 0),        // Position (m)
        Vector3(0, 0, 0)         // Velocity (m/s)
    );

    // 2. Moon at lunar distance (~384,400 km)
    CelestialBody moon(
        "Moon",
        7.34e33,                // Mass (Kg)
        1737400.0,              // Radius (m)
        Vector3(384400000.0, 0, 0),
        Vector3(0, 1022.0, 0)   // Orbital speed (~1.022 km/s)
    );

    // 3. Artemis Orion in Low Earth Orbit (~400 km altitude)
    CelestialBody orion(
        "Artemis-Orion",        
        10400.0,                // Mass (Kg)
        5.0,                    // Radius (m)
        Vector3(6771000.0, 0, 0),
        Vector3(0, 7670.0, 0)   // Circular orbital velocity (~7.67 km/s)
    );

    sim.add_body(earth);
    sim.add_body(moon);
    sim.add_body(orion);

    std::cout << "--- ARTEMIS ORBITAL SIMULATION INITIATED ---\n";
    std::cout << std::fixed << std::setprecision(2);

    double dt = 1.0;            // 1-second physics time step
    int total_steps = 100;      // run 100 simulation seconds

    for (int step = 0; step < total_steps; ++step) {
        sim.step(dt);

        // Telemetry readout of the Orion craft every 20 seconds
        if (step % 20 == 0) {
            CelestialBody& craft = sim.bodies[2];
            double dist_from_earth = craft.position.magnitude() / 1000.0;
            double speed = craft.velocity.magnitude();

            std::cout << "T+" << step << "s | Alt: "
                      << (dist_from_earth - 6371.0) << " km | Speed: "
                      << speed << " m\s/n";
        }
    }

    return 0;
}
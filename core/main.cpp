#include <iostream>
#include <vector>
#include "physics_engine.hpp"
#include "rk4.hpp"
#include "telemetry.hpp"

// Instantaneous Earth gravity accleration calculation for RK4
Vector3 compute_earth_acceleration(const Vector3& pos) {
    const double G = 6.67430e-11;
    const double earth_mass = 5.972e24;
    double r = pos.magnitude();
    if (r < 1.0) return Vector3(0, 0, 0);

    double a = (G * earth_mass) / (r * r);
    return pos.normalize() * (-a);
}

int main() {
    // Initial Orion capsule state at 400 km Low Earth Orbit
    State orion_state(
        Vector3(6771000.0, 0.0, 0.0), // 6,371 km radius + 400 km altitude 
        Vector3(0.0, 7670.0, 0.0)       // ~7.67 km/s circular orbit speed
    );

    CelestialBody orion(
        "Artemis-Orion",
        10400.0,
        5.0,
        orion_state.x,
        orion_state.v
    );

    double dt = 0.1;             // High-precision 100ms time step
    double sim_time = 0.0;
    double fuel_kg = 2000.0;     // Orion reserve RCS propellant

    // Run 600 iterations ( 1 minute of simulated mission time)
    for (int step = 0; step <= 600; ++step) {
        orion_state = RK4Integrator::integrate(orion_state, dt, compute_earth_acceleration);

        orion.position = orion_state.x;
        orion.velocity = orion_state.v;

        // Emit telemetry packet at 10 Hz (every step)
        std::string packet = TelemetryLogger::serialize_craft(sim_time, orion, fuel_kg);
        std::cout << packet << std::endl;

        sim_time += dt;
    }

    return 0;
}
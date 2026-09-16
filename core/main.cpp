#include <iostream>
#include <thread>
#include <chrono>
#include "rk4.hpp"
#include "guidance.hpp"

int main() {
    // Initialize spacecraft state in Low Earth Orbit (LEO)
    CraftState state;
    state.time = 0.0;
    state.position = Vector3(6755000.0, 0.0, 0.0); 
    state.velocity = Vector3(0.0, 7670.0, 0.0);     
    state.mass = 2500.0;
    state.fuel = 2000.0;

    double dt = 1.0; // 1-second simulation step

    while (true) {
        // Run standard RK4 gravitational physics
        state = RK4Integrator::step(state, dt);

        // Autonomous test maneuver: Execute a minor orbital correction burn at t = 500s
        if (static_cast<int>(state.time) == 500) {
            GuidanceSystem::apply_burn(state, 15.0, 50.0); // 15 m/s burn, 50kg fuel
        }

        // Output production-grade JSON telemetry stream
        std::cout << "{\"t\":" << state.time 
                  << ", \"name\":\"Artemis-Orion\""
                  << ", \"pos\":{\"x\":" << state.position.x << ",\"y\":" << state.position.y << ",\"z\":" << state.position.z << "}"
                  << ", \"vel\":{\"x\":" << state.velocity.x << ",\"y\":" << state.velocity.y << ",\"z\":" << state.velocity.z << "}"
                  << ", \"speed\":" << state.velocity.magnitude() 
                  << ", \"fuel\":" << state.fuel << "}" << std::endl;

        std::cout << std::flush;

        // Throttle to 60 FPS real-time
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    return 0;
}
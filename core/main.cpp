#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>
#include "rk4.hpp"
#include "guidance.hpp"

// Check for incoming autonomous commands from Python AI via Go relay
void process_uplink_commands(CraftState& state) {
    std::ifstream uplink_file("uplink.json");
    if (uplink_file.is_open()) {
        std::string content((std::istreambuf_iterator<char>(uplink_file)),
                             std::istreambuf_iterator<char>());
        uplink_file.close();
        
        // Remove file immediately to prevent duplicate executions
        std::remove("uplink.json");

        if (content.find("BURN") != std::string::npos) {
            // Autonomous prograde maneuver commanded by AI
            GuidanceSystem::apply_burn(state, 25.0, 50.0); // +25 m/s, 50 kg fuel consumed
            std::cerr << ">> [PHYSICS ENGINE] AI Maneuver Confirmed: Prograde Burn Applied (+25 m/s)\n";
        }
    }
}

int main() {
    // Start craft in Low Earth Orbit with decaying trajectory parameters
    CraftState state;
    state.time = 0.0;
    state.position = Vector3(6700000.0, 0.0, 0.0); // ~329 km altitude (inside burn window)
    state.velocity = Vector3(0.0, 7650.0, 0.0);     
    state.mass = 2500.0;
    state.fuel = 2000.0;

    double dt = 1.0; // 1-second physics step

    while (true) {
        // 1. Process real-time autonomous commands from uplink
        process_uplink_commands(state);

        // 2. Compute 6-DoF numerical calculus (Gravity + J2 + Drag)
        state = RK4Integrator::step(state, dt);

        // 3. Serialize and stream telemetry JSON to stdout
        std::cout << "{\"t\":" << state.time 
                  << ", \"name\":\"Artemis-Orion\""
                  << ", \"pos\":{\"x\":" << state.position.x << ",\"y\":" << state.position.y << ",\"z\":" << state.position.z << "}"
                  << ", \"vel\":{\"x\":" << state.velocity.x << ",\"y\":" << state.velocity.y << ",\"z\":" << state.velocity.z << "}"
                  << ", \"speed\":" << state.velocity.magnitude() 
                  << ", \"fuel\":" << state.fuel << "}" << std::endl;

        std::cout << std::flush;

        // 60 FPS pacing
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    return 0;
}
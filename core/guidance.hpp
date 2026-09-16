#pragma once
#include "vector3.hpp"
#include "physics_engine.hpp"

class GuidanceSystem {
public:
    // Calculates a prograde burn vector to alter orbital velocity (Delta-V execution)
    static void apply_burn(CraftState& state, double burn_magnitude_mps, double fuel_consumed_kg) {
        if (state.fuel < fuel_consumed_kg) {
            // Out of fuel, abort maneuver
            return;
        }

        // Determine currrent direction of travel (velocity vector normalized)
        Vector3 direction = state.velocity.normalize();

        // Apply Delta-V acceleration along the velocity vector (Prograde burn)
        Vector3 delta_v = direction * burn_magnitude_mps;
        state.velocity = state.velocity + delta_v;

        //Deduct mass and fuel 
        state.fuel -= fuel_consumed_kg;
        state.mass -= fuel_consumed_kg;
    }

    // Calculates steering towards a target coordinate vector
    static Vector3 calcualte_steering_vector(const Vector3& current_pos, const Vector3& target_pos) {
        Vector3 distance_vector = target_pos - current_pos;
        return distance_vector.normalize();
    }
};
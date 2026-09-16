#pragma once
#include "physics_engine.hpp"
#include "atmosphere.hpp"
#include "celestial.hpp"

class RK4Integrator {
private:
    static Derivative evaluate(const CraftState& state, double dt, const Derivative& d) {
        CraftState next_state;
        next_state.position = state.position + d.dx * dt;
        next_state.velocity = state.velocity + d.dv * dt;
        next_state.mass = state.mass; // Mass stays constant during passive flight

        // 1. Standard Point-Mass Gravity
        Vector3 gravity_accel = calculate_gravity(next_state.position);

        // 2. Advanced: Earth's Equatorial Bulge (J2 Perturbation)
        Vector3 j2_accel = CelestialMechanics::calculate_j2_effect(next_state.position);

        // 3. Environmental: Atmospheric Drag
        Vector3 drag_accel = Atmosphere::calculate_drag(next_state, 2.2, 15.0);

        // Total Acceleration vector applied to the spacecraft
        Vector3 total_accel = gravity_accel + j2_accel + drag_accel;

        return Derivative{
            next_state.velocity,
            total_accel
        };
    }

    static Vector3 calculate_gravity(const Vector3& pos) {
        double r_mag = pos.magnitude();
        if (r_mag == 0.0) return Vector3(0.0, 0.0, 0.0);

        double factor = -MU / (r_mag * r_mag * r_mag);
        return pos * factor;
    }

public:
    static CraftState step(const CraftState& state, double dt) {
        Derivative a = evaluate(state, 0.0, Derivative());
        Derivative b = evaluate(state, dt * 0.5, a);
        Derivative c = evaluate(state, dt * 0.5, b);
        Derivative d = evaluate(state, dt, c);

        Vector3 d_pos_weighted = (a.dx + (b.dx + c.dx) * 2.0 + d.dx) * (dt / 6.0);
        Vector3 d_vel_weighted = (a.dv + (b.dv + c.dv) * 2.0 + d.dv) * (dt / 6.0);

        CraftState new_state;
        new_state.time = state.time + dt;
        new_state.position = state.position + d_pos_weighted;
        new_state.velocity = state.velocity + d_vel_weighted;
        new_state.mass = state.mass;
        new_state.fuel = state.fuel;

        return new_state;
    }
};
#ifndef RK4_HPP
#define RK4_HPP

#include "vector3.hpp"

struct Derivative {
    Vector3 dx; // Velocity (rate of chnage of position)
    Vector3 dv; // Acceleration (rate of change of velocity)

    Derivative() : dx(0, 0, 0), dv(0, 0, 0) {}
    Derivative(Vector3 velocity, Vector3 acceleration) : dx(velocity), dv(acceleration) {}
};

struct State {
    Vector3 x;  // Position
    Vector3 v;  // Velocity

    State() : x(0, 0, 0), v(0, 0, 0) {}
    State(Vector3 pos, Vector3 vel) : x(pos), v(vel) {}
};

class RK4Integrator {
public:
    // Evaluate state derivatives given an initial state, offset, and instantaneous acceleration
    static Derivative evaluate(const State& initial, double dt, const Derivative& d, Vector3 (*accel_func)(const Vector3&)) {
        State state;
        state.x = initial.x + (d.dx * dt);
        state.v = initial.v + (d.dv * dt);

        Derivative output;
        output.dx = state.v;
        output.dv = accel_func(state.x);
        return output;
    }

    // Integrate state across time step dt using 4 weight slopes
    static State integrate(const State& state, double dt, Vector3 (*accel_func)(const Vector3&)) {
        Derivative a = evaluate(state, 0.0, Derivative(), accel_func);
        Derivative b = evaluate(state, dt * 0.5, a, accel_func);
        Derivative c = evaluate(state, dt * 0.5, b, accel_func);
        Derivative d = evaluate(state, dt, c, accel_func);

        // Weighted slope: dxdt = (a.dx + 2*(b.dx + c.dx) + d.dx) / 6
        Vector3 dxdt = (a.dx + (b.dx + c.dx) * 2.0 + d.dx) * (1.0 / 6.0);
        Vector3 dvdt = (a.dv + (b.dv + c.dv) * 2.0 + d.dv) * (1.0 / 6.0);

        State result;
        result.x = state.x + (dxdt * dt);
        result.v = state.v + (dvdt * dt);
        return result;
    }
};

#endif
#pragma once
#include <cmath>

struct Vector3 {
    double x, y, z;

    // Constructors
    Vector3() : x(0.0), y(0.0), z(0.0) {}
    Vector3(double _x, double _y, double _z) : x(_x), y(_y), z(_z) {}

    // Operator Overloads for Physics Calculations
    Vector3 operator+(const Vector3& v) const {
        return Vector3(x + v.x, y + v.y, z + v.z);
    }

    Vector3 operator-(const Vector3& v) const {
        return Vector3(x - v.x, y - v.y, z - v.z);
    }

    Vector3 operator*(double scalar) const {
        return Vector3(x * scalar, y * scalar, z * scalar);
    }

    Vector3 operator/(double scalar) const {
        return Vector3(x / scalar, y / scalar, z / scalar);
    }

    // Magnitude (Length of vector)
    double magnitude() const {
        return std::sqrt(x * x + y * y + z * z);
    }

    // Normalized Direction Vector
    Vector3 normalize() const {
        double mag = magnitude();
        if (mag == 0.0) return Vector3(0.0, 0.0, 0.0);
        return *this / mag;
    }
};
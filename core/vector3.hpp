#ifndef VECTOR3_HPP
#define VECTOR3_HPP

#include <cmath>

struct Vector3 {
    double x;
    double y;
    double z;

    Vector3() : x(0.0), y(0.0), z(0.0) {}
    Vector3(double x_val, double y_val, double z_val) : x(x_val), y(y_val), z(z_val) {}

    // Vector Addition: v1 +v2
    Vector3 operator+(const Vector3& other) const{
        return Vector3(x + other.x, y + other.y, z + other.z);
    }

    //Vector Subtraction: v1 - v2
    Vector3 operator-(const Vector3& other) const {
        return Vector3(x - other.x, y - other.y, z - other.z);
    }

    //Scalar Multiplication: v * scalar
    Vector3 operator*(double scalar) const {
        return Vector3(x * scalar, y * scalar, z * scalar);
    }

    //Euclidean Magnitude: length of the vector
    double magnitude() const {
        return std::sqrt(x * x + y * y + z * z);
    }

    //Normalized Vector (Unit vector pointing in same direction)
    Vector3 normalize() const {
        double mag = magnitude();
        if (mag == 0.0) return Vector3();
        return Vector3(x / mag, y / mag, z / mag);
    }

};

#endif 
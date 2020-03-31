#pragma once

typedef struct
{
    double X;
    double Y;
    double Z;
} Vector3;

Vector3 vector3_norm(Vector3 vector);

Vector3 vector3_sub(Vector3 left, Vector3 right);

Vector3 vector3_cross(Vector3 left, Vector3 right);

double vector3_dot(Vector3 left, Vector3 right);


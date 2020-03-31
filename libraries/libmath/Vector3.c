#include <libmath/Vector3.h>
#include <libmath/math.h>

Vector3 vector3_norm(Vector3 vector)
{
    double lenght = sqrt(vector.X * vector.X +
                         vector.Y * vector.Y +
                         vector.Z * vector.Z);

    vector.X /= lenght;
    vector.Y /= lenght;
    vector.Z /= lenght;

    return vector;
}

Vector3 vector3_sub(Vector3 left, Vector3 right)
{
    return (Vector3){left.X - right.X, left.Y - right.Y, left.Z - right.Z};
}

Vector3 vector3_cross(Vector3 left, Vector3 right)
{
    Vector3 result = {};

    result.X = left.Y * right.Z - left.Z * right.Y;
    result.Y = left.Z * right.X - left.X * right.Z;
    result.Z = left.X * right.Y - left.Y * right.X;

    result = vector3_norm(result);

    return result;
}

double vector3_dot(Vector3 left, Vector3 right)
{
    return left.X * right.X +
           left.Y * right.Y +
           left.Z * right.Z;
}
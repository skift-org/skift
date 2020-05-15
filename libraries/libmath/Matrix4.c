#include <libmath/Matrix4.h>
#include <libmath/math.h>

Matrix4 matrix_create_projection(double near, double far, double fov, double aspect_ratio)
{
    double fov_rad = 1.0 / tan(fov * 0.5 / 180.0 * PI);
    Matrix4 projection = {};

    projection.m[0][0] = aspect_ratio * fov_rad;
    projection.m[1][1] = fov_rad;
    projection.m[2][2] = far / (far - near);
    projection.m[3][2] = (-far * near) / (far - near);
    projection.m[2][3] = 1.0;
    projection.m[3][3] = 0.0;

    return projection;
}

Matrix4 matrix_create_rotationX(double theta)
{
    Matrix4 mat = {};

    mat.m[0][0] = 1;
    mat.m[1][1] = cos(theta * 0.5);
    mat.m[1][2] = sin(theta * 0.5);
    mat.m[2][1] = -sin(theta * 0.5);
    mat.m[2][2] = cos(theta * 0.5);
    mat.m[3][3] = 1;

    return mat;
}

// Matrix4 matrix_create_rotationY(double theta)
// {
//
// }

Matrix4 matrix_create_rotationZ(double theta)
{
    Matrix4 mat = {};

    mat.m[0][0] = cos(theta);
    mat.m[0][1] = sin(theta);
    mat.m[1][0] = -sin(theta);
    mat.m[1][1] = cos(theta);
    mat.m[2][2] = 1;
    mat.m[3][3] = 1;

    return mat;
}

Vec3f matrix_apply_tranform(Matrix4 matrix, Vec3f vector)
{
    Vec3f result = {};

    result.x = vector.x * matrix.m[0][0] +
               vector.y * matrix.m[1][0] +
               vector.z * matrix.m[2][0] +
               matrix.m[3][0];

    result.y = vector.x * matrix.m[0][1] +
               vector.y * matrix.m[1][1] +
               vector.z * matrix.m[2][1] +
               matrix.m[3][1];

    result.z = vector.x * matrix.m[0][2] +
               vector.y * matrix.m[1][2] +
               vector.z * matrix.m[2][2] +
               matrix.m[3][2];

    double w = vector.x * matrix.m[0][3] +
               vector.y * matrix.m[1][3] +
               vector.z * matrix.m[2][3] +
               matrix.m[3][3];

    if (w != 0.0f)
    {
        result.x /= w;
        result.y /= w;
        result.z /= w;
    }

    return result;
}

#include "std_c.h"
#include "utils.h"

void printMatrix(const char* name, float matrix[4][4]) {
    printf("%s:\n", name);
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            printf("%f ", matrix[i][j]);
        }
        printf("\n");
    }
}

void mat4_multiply(float result[4][4], const float a[4][4], const float b[4][4]) {
    float cA[4][4];
    float cB[4][4];
    memcpy(cA, a, 16 * sizeof(float));
    memcpy(cB, b, 16 * sizeof(float));
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            result[i][j] = 0.0f;
            for (int k = 0; k < 4; ++k) {
                result[i][j] += cA[i][k] * cB[k][j];
            }
        }
    }
}

float lerp(const float a, const float b, const float t, const float dt, const float mindiff) {
    return fabsf(a-b) > mindiff ? (a-b)*powf(t,dt)+b : b;
}

float lerpDegrees(const float a, const float b, const float t, const float dt, const float mindiff) {
    float diff = fmodf(b - a, 2*PI);
    if(diff > PI) diff -= 2*PI;
    if(diff < -PI) diff += 2*PI;
    return fabs(diff) > mindiff ? (-diff)*powf(t,dt)+a+diff : a+diff;
}

void crossProduct(const float a[3], const float b[3], float result[3]) {
    result[0] = a[1] * b[2] - a[2] * b[1];
    result[1] = a[2] * b[0] - a[0] * b[2];
    result[2] = a[0] * b[1] - a[1] * b[0];
}

float radians(float degrees) {
    return degrees * (PI / 180.0f);
}

void normalize(float v[3]) {
    float length = sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
    if (length != 0.0f) {
        v[0] /= length;
        v[1] /= length;
        v[2] /= length;
    }
}

void mat4_ortho(float out[4][4], const float left, const float right, const float bottom, const float top, const float near, const float far) {
    mat4_identity(out);
    out[0][0] = 2.0f / (right - left);
    out[1][1] = 2.0f / (top - bottom);
    out[2][2] = -2.0f / (far - near);
    out[3][0] = -(right + left) / (right - left);
    out[3][1] = -(top + bottom) / (top - bottom);
    out[3][2] = -(far + near) / (far - near);
}

void mat4_identity(float mat[4][4]) {
    memset(mat, 0, 16 * sizeof(float));
    mat[0][0] = mat[1][1] = mat[2][2] = mat[3][3] = 1.0f;
}

void mat4_rotate(float result[4][4], const float mat[4][4], const float angle, const float cords[3]) {
    float cCords[3];
    float cMat[4][4];
    memcpy(cCords, cords, 3 * sizeof(float));
    memcpy(cMat, mat, 16 * sizeof(float));
    normalize(cCords);
    float x = cCords[0], y = cCords[1], z = cCords[2];
    float c = cosf(angle);
    float s = sinf(angle);
    float t = 1.0f - c;

    float rotation[4][4] = {
        {t * x * x + c,     t * x * y - s * z, t * x * z + s * y, 0.0f},
        {t * x * y + s * z, t * y * y + c,     t * y * z - s * x, 0.0f},
        {t * x * z - s * y, t * y * z + s * x, t * z * z + c,     0.0f},
        {0.0f,              0.0f,              0.0f,              1.0f}
    };

    mat4_multiply(result, cMat, rotation);
}

void mat4_lookat(float mat[4][4], const float eye[3], const float center[3], const float up[3]) {
    float f[3], s[3], u[3];
    f[0] = center[0] - eye[0];
    f[1] = center[1] - eye[1];
    f[2] = center[2] - eye[2];
    normalize(f);

    s[0] = f[1] * up[2] - f[2] * up[1];
    s[1] = f[2] * up[0] - f[0] * up[2];
    s[2] = f[0] * up[1] - f[1] * up[0];
    normalize(s);

    u[0] = s[1] * f[2] - s[2] * f[1];
    u[1] = s[2] * f[0] - s[0] * f[2];
    u[2] = s[0] * f[1] - s[1] * f[0];

    mat[0][0] = s[0];
    mat[0][1] = u[0];
    mat[0][2] = -f[0];
    mat[0][3] = 0.0f;

    mat[1][0] = s[1];
    mat[1][1] = u[1];
    mat[1][2] = -f[1];
    mat[1][3] = 0.0f;

    mat[2][0] = s[2];
    mat[2][1] = u[2];
    mat[2][2] = -f[2];
    mat[2][3] = 0.0f;

    mat[3][0] = -(s[0] * eye[0] + s[1] * eye[1] + s[2] * eye[2]);
    mat[3][1] = -(u[0] * eye[0] + u[1] * eye[1] + u[2] * eye[2]);
    mat[3][2] = f[0] * eye[0] + f[1] * eye[1] + f[2] * eye[2];
    mat[3][3] = 1.0f;
}

void mat4_perspective(float mat[4][4], float fov, float aspect, float near, float far) {
    float tanHalfFov = tanf(fov / 2.0f);

    mat[0][0] = 1.0f / (aspect * tanHalfFov);
    mat[0][1] = 0.0f;
    mat[0][2] = 0.0f;
    mat[0][3] = 0.0f;

    mat[1][0] = 0.0f;
    mat[1][1] = 1.0f / tanHalfFov;
    mat[1][2] = 0.0f;
    mat[1][3] = 0.0f;

    mat[2][0] = 0.0f;
    mat[2][1] = 0.0f;
    mat[2][2] = far / (near - far);
    mat[2][3] = -1.0f;

    mat[3][0] = 0.0f;
    mat[3][1] = 0.0f;
    mat[3][2] = (far * near) / (near - far);
    mat[3][3] = 0.0f;
}

// Function to translate a matrix
void mat4_translate(float result[4][4], const float m[4][4], const float pos[3]) {
    float translation[4][4];
    mat4_identity(translation);
    translation[3][0] = pos[0];
    translation[3][1] = pos[1];
    translation[3][2] = pos[2];

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            result[i][j] = m[i][0] * translation[0][j] +
                           m[i][1] * translation[1][j] +
                           m[i][2] * translation[2][j] +
                           m[i][3] * translation[3][j];
        }
    }
}
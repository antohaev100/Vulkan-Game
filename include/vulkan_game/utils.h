#ifndef UTILS_H
#define UTILS_H

#include "shared_buffer.h"
// Utility functions and definitions
#define PI 3.14159265358979323846
void printMatrix(const char* name, float matrix[4][4]);
float lerp(const float a, const float b, const float t, const float dt, const float mindiff);
float lerpDegrees(const float a, const float b, const float t, const float dt, const float mindiff);
void crossProduct(const float a[3], const float b[3], float result[3]);
float radians(float degrees);
void normalize(float v[3]);
void mat4_multiply(float result[4][4], const float a[4][4], const float b[4][4]);
void mat4_identity(float mat[4][4]);
void mat4_rotate(float result[4][4], const float mat[4][4], const float angle, const float cords[3]);
void mat4_lookat(float mat[4][4], const float eye[3], const float center[3], const float up[3]);
void mat4_perspective(float mat[4][4], float fov, float aspect, float near, float far);
void mat4_ortho(float out[4][4], const float left, const float right, const float bottom, const float top, const float near, const float far);
void mat4_translate(float result[4][4], const float m[4][4], const float pos[3]);

void vectorAdd(vec *m, void* newElem);
void vectorRem(vec* m, int index);
void vectorCheckCapacity(vec *m);
void initVector(vec *m, int elemSize, int capacity, int minCapacity);
void deleteVector(vec *m);
#endif // UTILS_H
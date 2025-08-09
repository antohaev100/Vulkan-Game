#include "vk_fun.h"

// Helper function to apply rotation
static void applyRotation(float *vertex, float *normal, float *rotation, float *center) {
    if(fmod(rotation[0], 2 * PI) != 0){
        // Translate vertex to origin
        float y = vertex[1] - center[1];
        float z = vertex[2] - center[2];
        // Rotation around x-axis
        float cosX = cosf(rotation[0]);
        float sinX = sinf(rotation[0]);
        vertex[1] = y * cosX - z * sinX + center[1];
        vertex[2] = y * sinX + z * cosX + center[2];
        // Rotate normal around x-axis
        float ny = normal[1] * cosX - normal[2] * sinX;
        float nz = normal[1] * sinX + normal[2] * cosX;
        normal[1] = ny;
        normal[2] = nz;
    }
    if(fmod(rotation[1], 2 * PI) != 0){
        // Translate vertex to origin
        float x = vertex[0] - center[0];
        float z1 = vertex[2] - center[2];
        // Rotation around y-axis
        float cosY = cosf(rotation[1]);
        float sinY = sinf(rotation[1]);
        vertex[0] = x * cosY + z1 * sinY + center[0];
        vertex[2] = -x * sinY + z1 * cosY + center[2];
        // Rotate normal around y-axis
        float nx = normal[0] * cosY + normal[2] * sinY;
        float nz = -normal[0] * sinY + normal[2] * cosY;
        normal[0] = nx;
        normal[2] = nz;
    }
    if(fmod(rotation[2], 2 * PI) != 0){
        // Translate vertex to origin
        float x2 = vertex[0] - center[0];
        float y1 = vertex[1] - center[1];
        // Rotation around z-axis
        float cosZ = cosf(rotation[2]);
        float sinZ = sinf(rotation[2]);
        vertex[0] = x2 * cosZ - y1 * sinZ + center[0];
        vertex[1] = x2 * sinZ + y1 * cosZ + center[1];
    
        // Rotate normal around z-axis
        float nx = normal[0] * cosZ - normal[1] * sinZ;
        float ny = normal[0] * sinZ + normal[1] * cosZ;
        normal[0] = nx;
        normal[1] = ny;
    }
}

mapSize initMap(vec *pVertices, vec *pIndices){
    vertex_t vertices[] = {
        //ground
        {{-10.0f, -10.0f,  0.0f}, {0.8f, 0.8f, 0.8f}, {0.0f, 0.0f, 1.0f}},
        {{ 10.0f, -10.0f,  0.0f}, {0.8f, 0.8f, 0.8f}, {0.0f, 0.0f, 1.0f}},
        {{-10.0f,  10.0f,  0.0f}, {0.8f, 0.8f, 0.8f}, {0.0f, 0.0f, 1.0f}}, 
        {{ 10.0f,  10.0f,  0.0f}, {0.8f, 0.8f, 0.8f}, {0.0f, 0.0f, 1.0f}}, 
        //roof
        {{-10.0f, -10.0f, 10.0f}, {0.8f, 0.8f, 0.8f}, {1.0f, 1.0f, -1.5f}},
        {{ 10.0f, -10.0f, 10.0f}, {0.8f, 0.8f, 0.8f}, {-1.0f, 1.0f, -1.5f}},
        {{-10.0f,  10.0f, 10.0f}, {0.8f, 0.8f, 0.8f}, {1.0f, -1.0f, -1.5f}}, 
        {{ 10.0f,  10.0f, 10.0f}, {0.8f, 0.8f, 0.8f}, {-1.0f, -1.0f, -1.5f}}, 
        {{  0.0f,   0.0f, 10.0f}, {0.8f, 0.8f, 0.8f}, {0.0f, 0.0f, -1.0f}}, 
        {{-10.0f,   0.0f, 10.0f}, {0.8f, 0.8f, 0.8f}, {1.0f, 0.0f,  0.0f}},
        {{  0.0f, -10.0f, 10.0f}, {0.8f, 0.8f, 0.8f}, {0.0f, 1.0f,  0.0f}},
        {{ 10.0f,   0.0f, 10.0f}, {0.8f, 0.8f, 0.8f}, {-1.0f, 0.0f, 0.0f}}, 
        {{  0.0f,  10.0f, 10.0f}, {0.8f, 0.8f, 0.8f}, {0.0f, -1.0f, 0.0f}}, 
        //front
        {{-10.0f, -10.0f,  0.0f}, {0.8f, 0.8f, 0.8f}, {0.0f, 1.0f, 0.0f}},
        {{ 10.0f, -10.0f,  0.0f}, {0.8f, 0.8f, 0.8f}, {0.0f, 1.0f, 0.0f}},
        {{-10.0f, -10.0f, 10.0f}, {0.8f, 0.8f, 0.8f}, {0.0f, 1.0f, 0.0f}}, 
        {{ 10.0f, -10.0f, 10.0f}, {0.8f, 0.8f, 0.8f}, {0.0f, 1.0f, 0.0f}}, 
        //back
        {{-10.0f,  10.0f,  0.0f}, {0.8f, 0.8f, 0.8f}, {0.0f, -1.0f, 0.0f}},
        {{ 10.0f,  10.0f,  0.0f}, {0.8f, 0.8f, 0.8f}, {0.0f, -1.0f, 0.0f}},
        {{-10.0f,  10.0f, 10.0f}, {0.8f, 0.8f, 0.8f}, {0.0f, -1.0f, 0.0f}}, 
        {{ 10.0f,  10.0f, 10.0f}, {0.8f, 0.8f, 0.8f}, {0.0f, -1.0f, 0.0f}}, 
        //left
        {{-10.0f, -10.0f,  0.0f}, {0.8f, 0.8f, 0.8f}, {1.0f, 0.0f, 0.0f}},
        {{-10.0f,  10.0f,  0.0f}, {0.8f, 0.8f, 0.8f}, {1.0f, 0.0f, 0.0f}},
        {{-10.0f, -10.0f, 10.0f}, {0.8f, 0.8f, 0.8f}, {1.0f, 0.0f, 0.0f}}, 
        {{-10.0f,  10.0f, 10.0f}, {0.8f, 0.8f, 0.8f}, {1.0f, 0.0f, 0.0f}}, 
        //right
        {{ 10.0f, -10.0f,  0.0f}, {0.8f, 0.8f, 0.8f}, {-1.0f, 0.0f, 0.0f}},
        {{ 10.0f,  10.0f,  0.0f}, {0.8f, 0.8f, 0.8f}, {-1.0f, 0.0f, 0.0f}},
        {{ 10.0f, -10.0f, 10.0f}, {0.8f, 0.8f, 0.8f}, {-1.0f, 0.0f, 0.0f}}, 
        {{ 10.0f,  10.0f, 10.0f}, {0.8f, 0.8f, 0.8f}, {-1.0f, 0.0f, 0.0f}}, 
    };
    uint16_t indices[] = {
        // Ground
        0, 1, 2, 2, 1, 3,
        // Roof
        8,4,9,8,10,4,8,5,10,8,11,5,8,7,11,8,12,7,8,6,12,8,9,6,
        // Front
        13, 15, 14, 15, 16, 14,
        // Back
        17, 18, 19, 19, 18, 20,
        // Left
        21, 22, 23, 23, 22, 24,
        // Right
        25, 27, 26, 27, 28, 26
    };
    mapSize map;
    map.indexNum = sizeof(indices) / sizeof(indices[0]);
    map.vertexNum = sizeof(vertices) / sizeof(vertices[0]);
    initVector(pVertices, sizeof(vertex_t), 1024, 1024);
    initVector(pIndices, sizeof(uint16_t), 4096, 4096);
    for(int i = 0; i < map.vertexNum; i++){
        vectorAdd(pVertices, &vertices[i]);
    }
    for(int i = 0; i < map.indexNum; i++){
        vectorAdd(pIndices, &indices[i]);
    }
    return map;
}


void createEllipsoid(obj3d obj, vec *pVertices, vec *pIndices){
    int latSegments = ELLIPSOIDDETAIL;
    int longSegments = ELLIPSOIDDETAIL*2;
    float radiusX = obj.dimension[0] / 2.0f;
    float radiusY = obj.dimension[1] / 2.0f;
    float radiusZ = obj.dimension[2] / 2.0f;
    int n = pVertices->n;
    //middle depth, middle height, right vertex
    vertex_t firstVertex = {
        {obj.pos[0], obj.pos[1] + radiusY, obj.pos[2]}, 
        {obj.color[0], obj.color[1], obj.color[2]},
        {0.0f, 1.0f, 0.0f}
    };
    applyRotation(firstVertex.pos, firstVertex.normal, obj.rotation, obj.pos);
    vectorAdd(pVertices, &firstVertex);

    for (int i = 1; i <= latSegments-1; ++i) {
        float theta = i * PI / latSegments;
        float sinTheta = sinf(theta);
        float cosTheta = cosf(theta);

        for (float j = 0; j <= longSegments; ++j) {
            float phi = j * 2 * PI / longSegments;
            float sinPhi = sinf(phi);
            float cosPhi = cosf(phi);

            float x = radiusX * cosPhi * sinTheta;
            float y = radiusY * cosTheta;
            float z = radiusZ * sinPhi * sinTheta;
            float nx = x / (radiusX * radiusX);
            float ny = y / (radiusY * radiusY);
            float nz = z / (radiusZ * radiusZ);
            float normal[3] = {nx, ny, nz};
            normalize(normal);

            vertex_t vertex = {{x + obj.pos[0], y + obj.pos[1], z + obj.pos[2]}, 
                               {obj.color[0], obj.color[1], obj.color[2]},
                               {normal[0], normal[1], normal[2]}};
            applyRotation(vertex.pos, vertex.normal, obj.rotation, obj.pos);
            vectorAdd(pVertices, &vertex);
        }
    }

    vertex_t lastVertex = {
        {obj.pos[0], obj.pos[1] - radiusY, obj.pos[2]}, 
        {obj.color[0], obj.color[1], obj.color[2]},
        {0.0f, -1.0f, 0.0f}
    };
    applyRotation(lastVertex.pos, lastVertex.normal, obj.rotation, obj.pos);
    vectorAdd(pVertices, &lastVertex);

    for(int i = 1; i <= longSegments; i++){
        uint16_t firstIndices[] = {
            n, n+i, n+i%longSegments +1
        };
        for(int j = sizeof(firstIndices) / sizeof(firstIndices[0]) -1; j >= 0; j--){
            vectorAdd(pIndices, &firstIndices[j]);
        }
    }

    for (int i = 0; i < latSegments-2; ++i) {
        for (int j = 0; j < longSegments + 1; ++j) {
            int first = (i * (longSegments + 1)) + j + n;
            int second = first + longSegments + 1;

            uint16_t indi[] = {
                first, second, first + 1,
                second, second + 1, first + 1
            };
            for(int k = sizeof(indi) / sizeof(indi[0])-1; k >=0; k--){
                vectorAdd(pIndices, &indi[k]);
            }
        }
    }
    n = pVertices->n-1;

    
    for(int i = 1; i <= longSegments; i++){
        uint16_t lastIndices[] = {
            n, n-i, n-i%longSegments -1
        };
        for(int j = sizeof(lastIndices) / sizeof(lastIndices[0]) -1; j >= 0; j--){
            vectorAdd(pIndices, &lastIndices[j]);
        }
    }
}

void createCuboid(obj3d obj, vec *pVertices, vec *pIndices){
    float x1 = obj.pos[0] - obj.dimension[0] / 2.0f;
    float x2 = obj.pos[0] + obj.dimension[0] / 2.0f;
    float y1 = obj.pos[1] - obj.dimension[1] / 2.0f;
    float y2 = obj.pos[1] + obj.dimension[1] / 2.0f;
    float z1 = obj.pos[2] - obj.dimension[2] / 2.0f;
    float z2 = obj.pos[2] + obj.dimension[2] / 2.0f;
    float c[] = {obj.color[0], obj.color[1], obj.color[2]};
    int n = pVertices->n;

    vertex_t vert[] = {
        {{ x1, y1, z1}, {c[0], c[1], c[2]}, {0.0f, 0.0f, -1.0f}},//ground
        {{ x2, y1, z1}, {c[0], c[1], c[2]}, {0.0f, 0.0f, -1.0f}},
        {{ x1, y2, z1}, {c[0], c[1], c[2]}, {0.0f, 0.0f, -1.0f}}, 
        {{ x2, y2, z1}, {c[0], c[1], c[2]}, {0.0f, 0.0f, -1.0f}}, 
        {{ x1, y1, z2}, {c[0], c[1], c[2]}, {0.0f, 0.0f, 1.0f}},//roof
        {{ x2, y1, z2}, {c[0], c[1], c[2]}, {0.0f, 0.0f, 1.0f}},
        {{ x1, y2, z2}, {c[0], c[1], c[2]}, {0.0f, 0.0f, 1.0f}}, 
        {{ x2, y2, z2}, {c[0], c[1], c[2]}, {0.0f, 0.0f, 1.0f}}, 
        {{ x1, y1, z1}, {c[0], c[1], c[2]}, {0.0f, -1.0f, 0.0f}},//front
        {{ x2, y1, z1}, {c[0], c[1], c[2]}, {0.0f, -1.0f, 0.0f}},
        {{ x1, y1, z2}, {c[0], c[1], c[2]}, {0.0f, -1.0f, 0.0f}}, 
        {{ x2, y1, z2}, {c[0], c[1], c[2]}, {0.0f, -1.0f, 0.0f}}, 
        {{ x1, y2, z1}, {c[0], c[1], c[2]}, {0.0f, 1.0f, 0.0f}},//back
        {{ x2, y2, z1}, {c[0], c[1], c[2]}, {0.0f, 1.0f, 0.0f}},
        {{ x1, y2, z2}, {c[0], c[1], c[2]}, {0.0f, 1.0f, 0.0f}}, 
        {{ x2, y2, z2}, {c[0], c[1], c[2]}, {0.0f, 1.0f, 0.0f}}, 
        {{ x1, y1, z1}, {c[0], c[1], c[2]}, {-1.0f, 0.0f, 0.0f}},//left
        {{ x1, y2, z1}, {c[0], c[1], c[2]}, {-1.0f, 0.0f, 0.0f}},
        {{ x1, y1, z2}, {c[0], c[1], c[2]}, {-1.0f, 0.0f, 0.0f}}, 
        {{ x1, y2, z2}, {c[0], c[1], c[2]}, {-1.0f, 0.0f, 0.0f}}, 
        {{ x2, y1, z1}, {c[0], c[1], c[2]}, {1.0f, 0.0f, 0.0f}},//right
        {{ x2, y2, z1}, {c[0], c[1], c[2]}, {1.0f, 0.0f, 0.0f}},
        {{ x2, y1, z2}, {c[0], c[1], c[2]}, {1.0f, 0.0f, 0.0f}}, 
        {{ x2, y2, z2}, {c[0], c[1], c[2]}, {1.0f, 0.0f, 0.0f}}, 
    };
    uint16_t indi[] = {
    n+0,  n+2,  n+1,  n+2,  n+3,  n+1,    // Ground
    n+4,  n+5,  n+6,  n+6,  n+5,  n+7,    // Roof
    n+8,  n+9,  n+10, n+10, n+9,  n+11,    // Front
    n+12, n+14, n+13, n+14, n+15, n+13,    // Back
    n+16, n+18, n+17, n+18, n+19, n+17,    // Left
    n+20, n+21, n+22, n+22, n+21, n+23    // Right
    };

    for(int i = 0; i < sizeof(vert) / sizeof(vert[0]); i++){
        applyRotation(vert[i].pos, vert[i].normal, obj.rotation, obj.pos);
        vectorAdd(pVertices, &vert[i]);
    }
    for(int i = 0; i < sizeof(indi) / sizeof(indi[0]); i++){
        vectorAdd(pIndices, &indi[i]);
    }
}

// Function to create an elliptic cylinder
void createEllipticCylinder(obj3d obj, vec *pVertices, vec *pIndices) {
    int segments = ELLIPSOIDDETAIL * 2; // Number of segments for the cylinder
    float radiusX = obj.dimension[0] / 2.0f;
    float radiusY = obj.dimension[1] / 2.0f;
    float height = obj.dimension[2];
    float halfHeight = height / 2.0f;
    float c[] = {obj.color[0], obj.color[1], obj.color[2]};
    int n = pVertices->n;

    vertex_t fistBottom = {{obj.pos[0], obj.pos[1], obj.pos[2] - halfHeight}, {c[0], c[1], c[2]}, {0.0f, 0.0f, -1.0f}};
    applyRotation(fistBottom.pos, fistBottom.normal, obj.rotation, obj.pos);
    vectorAdd(pVertices, &fistBottom);

    vertex_t fistTop = {{obj.pos[0], obj.pos[1], obj.pos[2] + halfHeight}, {c[0], c[1], c[2]}, {0.0f, 0.0f, 1.0f}};
    applyRotation(fistTop.pos, fistTop.normal, obj.rotation, obj.pos);
    vectorAdd(pVertices, &fistTop);
    // Create vertices
    for (int i = 0; i < segments; ++i) {
        float theta = i * 2.0f * PI / segments;
        float x = radiusX * cosf(theta);
        float y = radiusY * sinf(theta);

        // Bottom circle
        vertex_t bottomVertex = {{x + obj.pos[0], y + obj.pos[1], obj.pos[2] - halfHeight}, {c[0], c[1], c[2]}, {0.0f, 0.0f, -1.0f}};
        applyRotation(bottomVertex.pos, bottomVertex.normal, obj.rotation, obj.pos);
        vectorAdd(pVertices, &bottomVertex);

        // Top circle
        vertex_t topVertex = {{x + obj.pos[0], y + obj.pos[1], obj.pos[2] + halfHeight}, {c[0], c[1], c[2]}, {0.0f, 0.0f, 1.0f}};
        applyRotation(topVertex.pos, topVertex.normal, obj.rotation, obj.pos);
        vectorAdd(pVertices, &topVertex);
    }

    // Create indices for the bottom
    for (int i = 0; i < segments; ++i) {
        uint16_t bottomIndices[] = {n    , n + (2 + 2*i)%(segments*2) + 2, n + 2 + 2*i};
        //printf("%d %d %d\n", bottomIndices[0] - n, bottomIndices[1] - n, bottomIndices[2] - n);
        for (int j = 0; j < 3; ++j) {
            vectorAdd(pIndices, &bottomIndices[j]);
        }
    }
    // Create indices for the top
    for (int i = 0; i < segments; ++i) {
        uint16_t topIndices[] =    {n + 1, n + 3 + 2*i, n +(3 + 2*i)%(segments*2) + 2};
        //printf("%d %d %d\n", topIndices[0] - n, topIndices[1] - n, topIndices[2] - n);
        for (int j = 0; j < 3; ++j) {
            vectorAdd(pIndices, &topIndices[j]);
        }
    }
    n = pVertices->n;
    for (int i = 0; i < segments; ++i) {
        float theta = i * 2.0f * PI / segments;
        float x = radiusX * cosf(theta);
        float y = radiusY * sinf(theta);
        // Bottom circle
        vertex_t bottomVertex = {{x + obj.pos[0], y + obj.pos[1], obj.pos[2] - halfHeight}, {c[0], c[1], c[2]}, {x, y, 0.0f}};
        applyRotation(bottomVertex.pos, bottomVertex.normal,obj.rotation, obj.pos);
        vectorAdd(pVertices, &bottomVertex);

        // Top circle
        vertex_t topVertex = {{x + obj.pos[0], y + obj.pos[1], obj.pos[2] + halfHeight}, {c[0], c[1], c[2]}, {x, y, 0.0f}};
        applyRotation(topVertex.pos, topVertex.normal, obj.rotation, obj.pos);
        vectorAdd(pVertices, &topVertex);
    }

    // Create indices for the side faces
    for (int i = 0; i < segments; ++i) {
        uint16_t sideIndices[] = {
            n +2*i, n + (2 +2*i)%(segments*2), n + (3 +2*i)%(segments*2),
            n +2*i, n + (3 +2*i)%(segments*2), n + 1 +2*i
        };
        for (int j = 0; j < 6; ++j) {
            vectorAdd(pIndices, &sideIndices[j]);
        }
    }
}

void createPlayerSphere(obj3d obj, vec *pVertices, vec *pIndices){
    int latSegments = ELLIPSOIDDETAIL;
    int longSegments = ELLIPSOIDDETAIL*2;
    float radius = obj.dimension[0] / 2.0f;
    int n = pVertices->n;
    //middle depth, middle height, right vertex
    vertex_t firstVertex = {
        {obj.pos[0], obj.pos[1] + radius, obj.pos[2]}, 
        {obj.color[0]-0.1f, obj.color[1]-0.1f, obj.color[2]-0.1f},
        {0.0f, 1.0f, 0.0f}
    };
    applyRotation(firstVertex.pos, firstVertex.normal, obj.rotation, obj.pos);
    vectorAdd(pVertices, &firstVertex);

    for (int i = 1; i <= latSegments-1; ++i) {
        float theta = i * PI / latSegments;
        float sinTheta = sinf(theta);
        float cosTheta = cosf(theta);
        if(i % 2 == 0){
            obj.color[0]-=0.1f;
            obj.color[1]-=0.1f;
            obj.color[2]-=0.1f;
        }

        for (float j = 0; j <= longSegments; ++j) {
            float phi = j * 2 * PI / longSegments;
            float sinPhi = sinf(phi);
            float cosPhi = cosf(phi);

            float x = radius * cosPhi * sinTheta;
            float y = radius * cosTheta;
            float z = radius * sinPhi * sinTheta;
            float nx = x / (radius * radius);
            float ny = y / (radius * radius);
            float nz = z / (radius * radius);
            float normal[3] = {nx, ny, nz};
            normalize(normal);

            vertex_t vertex = {{x + obj.pos[0], y + obj.pos[1], z + obj.pos[2]}, 
                               {obj.color[0], obj.color[1], obj.color[2]},
                               {normal[0], normal[1], normal[2]}};
            applyRotation(vertex.pos, vertex.normal, obj.rotation, obj.pos);
            vectorAdd(pVertices, &vertex);
        }
    }

    vertex_t lastVertex = {
        {obj.pos[0], obj.pos[1] - radius, obj.pos[2]}, 
        {obj.color[0]-0.1f, obj.color[1]-0.1f, obj.color[2]-0.1f},
        {0.0f, -1.0f, 0.0f}
    };
    applyRotation(lastVertex.pos, lastVertex.normal, obj.rotation, obj.pos);
    vectorAdd(pVertices, &lastVertex);

    for(int i = 1; i <= longSegments; i++){
        uint16_t firstIndices[] = {
            n, n+i, n+i%longSegments +1
        };
        for(int j = sizeof(firstIndices) / sizeof(firstIndices[0]) -1; j >= 0; j--){
            vectorAdd(pIndices, &firstIndices[j]);
        }
    }

    for (int i = 0; i < latSegments-2; ++i) {
        for (int j = 0; j < longSegments + 1; ++j) {
            int first = (i * (longSegments + 1)) + j + n;
            int second = first + longSegments + 1;

            uint16_t indi[] = {
                first, second, first + 1,
                second, second + 1, first + 1
            };
            for(int k = sizeof(indi) / sizeof(indi[0])-1; k >=0; k--){
                vectorAdd(pIndices, &indi[k]);
            }
        }
    }
    n = pVertices->n-1;

    
    for(int i = 1; i <= longSegments; i++){
        uint16_t lastIndices[] = {
            n, n-i, n-i%longSegments -1
        };
        for(int j = sizeof(lastIndices) / sizeof(lastIndices[0]) -1; j >= 0; j--){
            vectorAdd(pIndices, &lastIndices[j]);
        }
    }
}

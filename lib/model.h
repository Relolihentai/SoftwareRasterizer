#ifndef _MODEL_H_
#define _MODEL_H_

#include <vector>
#include "vec.h"

struct faceIndex
{
    int vert[3];
    int normal[3];
    int uv[3];
};
class Model
{
    private:
        std::vector<Vec3f> verts;
        std::vector<faceIndex> faces;
        std::vector<Vec2f> uvs;
        std::vector<Vec3f> normals;
    public:
        Model(const char* fileName);
        ~Model();
        int nverts();
        int nfaces();
        Vec3f vert(int i);
        Vec3f normal(int i);
        Vec2f uv(int i);
        faceIndex face(int index);
};

#endif
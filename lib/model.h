#ifndef _MODEL_H_
#define _MODEL_H_

#include <vector>
#include "vec.h"

class Model
{
    private:
        std::vector<Vec3f> verts;
        std::vector<std::vector<int> > faces;
    public:
        Model(const char* fileName);
        ~Model();
        int nverts();
        int nfaces();
        Vec3f vert(int i);
        std::vector<int> face(int index);
};

#endif
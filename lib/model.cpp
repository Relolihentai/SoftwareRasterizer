#include "model.h"
#include <iostream>
#include <fstream>
#include <sstream>

Model::Model(const char* fileName) : verts(), faces()
{
    std::ifstream in;
    in.open(fileName, std::ifstream::in);
    if (in.fail()) return;
    std::string line;
    while(!in.eof())
    {
        std::getline(in, line);
        std::istringstream iss(line.c_str());
        char trash;
        if (!line.compare(0, 2, "v "))
        {
            iss >> trash;
            Vec3f v;
            for(int i = 0; i < 3; i++) iss >> v.raw[i];
            verts.push_back(v);
        }
        else if (!line.compare(0, 3, "vt "))
        {
            iss >> trash;
            iss >> trash;
            Vec2f uv;
            for (int i = 0; i < 2; i++) iss >> uv.raw[i];
            uvs.push_back(uv);
        }
        else if (!line.compare(0, 3, "vn "))
        {
            iss >> trash;
            iss >> trash;
            Vec3f normal;
            for (int i = 0; i < 3; i++) iss >> normal.raw[i];
            normals.push_back(normal);
        }
        else if (!line.compare(0, 2, "f "))
        {
            faceIndex f;
            iss >> trash;
            for (int i = 0; i < 3; i++)
            {
                iss >> f.vert[i];
                f.vert[i]--;
                iss >> trash;
                iss >> f.uv[i];
                f.uv[i]--;
                iss >> trash;
                iss >> f.normal[i];
                f.normal[i]--;
            }
            faces.push_back(f);
        }
    }
    std::cerr << "# v# " << verts.size() << " f# " << faces.size() << std::endl;
}

Model::~Model(){}

int Model::nverts()
{
    return (int)verts.size();
}
int Model::nfaces()
{
    return (int)faces.size();
}
faceIndex Model::face(int index)
{
    return faces[index];
}
Vec3f Model::vert(int i )
{
    return verts[i];
}
Vec3f Model::normal(int i)
{
    return normals[i];
}
Vec2f Model::uv(int i)
{
    return uvs[i];
}
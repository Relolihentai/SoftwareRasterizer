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
            for(int i = 0; i < 3; i++)
            {
                iss >> v.raw[i];
            }
            verts.push_back(v);
        }
        else if (!line.compare(0, 2, "f "))
        {
            std::vector<int> f;
            int itrash, index;
            iss >> trash;
            while (iss >> index >> trash >> itrash >> trash >> itrash)
            {
                index--;
                f.push_back(index);
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
std::vector<int> Model::face(int index)
{
    return faces[index];
}
Vec3f Model::vert(int i )
{
    return verts[i];
}
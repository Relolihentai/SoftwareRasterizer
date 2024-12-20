#include "lib/tgaimage.h"
#include "lib/model.h"

#include <iostream>
const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);
const TGAColor blue = TGAColor(0, 0, 255, 255);
const int width = 1000;
const int height = 1000;

const Vec3f light_dir(0, 0, -1);
Model *model = NULL;

void DrawLine(Vec2i p0, Vec2i p1, TGAImage& image, const TGAColor& color)
{
    bool steep = false;
    if (abs(p1.x - p0.x) < abs(p1.y - p0.y))
    {
        std::swap(p0.x, p0.y);
        std::swap(p1.x, p1.y);
        steep = true;
    }
    if (p0.x > p1.x)
    {
        std::swap(p0, p1);
    }
    int dy = p1.y - p0.y;
    int dx = p1.x - p0.x;
    int derror = std::abs(dy) * 2;
    int error = 0;
    int y = p0.y;
    const int offsetY = p1.y > p0.y ? 1 : -1;
    
    for (int x = p0.x; x <= p1.x; x++)
    {
        if (steep) image.set(y, x, color);
        else image.set(x, y, color);
        error += derror;
        if (error > dx)
        {
            y += offsetY;
            error -= 2 * dx;
        }
    }
}

Vec3f barycentric(Vec3f *pts, Vec3f p)
{
    Vec3f u = Vec3f(pts[1][0] - pts[0][0], pts[2][0] - pts[0][0], pts[0][0] - p[0]) 
            ^ Vec3f(pts[1][1] - pts[0][1], pts[2][1] - pts[0][1], pts[0][1] - p[1]);
    if (std::abs(u.z) < 1e-6) return Vec3f(-1, -1, -1);
    return Vec3f(1.0 - (u.x + u.y) / u.z, u.x / u.z, u.y / u.z);
}

Vec3f barycentric(Vec2i *pts, Vec2i P) {
    Vec3f u = Vec3f(pts[2][0]-pts[0][0], pts[1][0]-pts[0][0], pts[0][0]-P[0])
            ^ Vec3f(pts[2][1]-pts[0][1], pts[1][1]-pts[0][1], pts[0][1]-P[1]);
    /* `pts` and `P` has integer value as coordinates
       so `abs(u[2])` < 1 means `u[2]` is 0, that means
       triangle is degenerate, in this case return something with negative coordinates */
    if (std::abs(u.z) < 1) return Vec3f(-1, -1, -1);
    return Vec3f(1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);

    // //面积法
    // float total = abs((pts[1][0] - pts[0][0]) * (pts[2][1] - pts[0][1]) 
    //                 - (pts[1][1] - pts[0][1]) * (pts[2][0] - pts[0][0])) / 2.0;
    // if (total <= 0.00001) return Vec3f(-1, -1, -1);
    // float a = abs((pts[1][0] - P[0]) * (pts[2][1] - P[1]) 
    //                 - (pts[1][1] - P[1]) * (pts[2][0] - P[0])) / 2.0;
    // float b = abs((P[0] - pts[0][0]) * (pts[2][1] - pts[0][1]) 
    //                 - (P[1] - pts[0][1]) * (pts[2][0] - pts[0][0])) / 2.0;
    // float c = abs((pts[1][0] - pts[0][0]) * (P[1] - pts[0][1]) 
    //                 - (pts[1][1] - pts[0][1]) * (P[0] - pts[0][0])) / 2.0;
    // //std::cout << "a " << a / total << "b " << b / total << "c " << c / total << std::endl;
    // return Vec3f(a / total, b / total, c / total);
    

    // //公式法
    // float b = ((P[1] - pts[0][1]) * (pts[2][0] - pts[0][0]) - (P[0] - pts[0][0]) * (pts[2][1] - pts[0][1])) 
    //         / float((pts[1][1] - pts[0][1]) * (pts[2][0] - pts[0][0]) - (pts[1][0] - pts[0][0]) * (pts[2][1] - pts[0][1]));
    // float c = ((P[1] - pts[0][1]) * (pts[1][0] - pts[0][0]) - (P[0] - pts[0][0]) * (pts[1][1] - pts[0][1]))
    //         / float((pts[2][1] - pts[0][1]) * (pts[1][0] - pts[0][0]) - (pts[2][0] - pts[0][0]) * (pts[1][1] - pts[0][1]));
    // float a = 1 - b - c;
    // //std::cout << "a " << a << " b " << b << " c " << c << std::endl;
    // return Vec3f(a, b, c);
}

void DrawTriangle(Vec3f* pts, Vec3f* pts_normal, Vec2f* pts_uv, float* zBuffer, TGAImage& texture, TGAImage& image, const TGAColor& color)
{
    Vec2f bboxmin(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
    Vec2f bboxmax(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
    Vec2f clamp(image.get_width() - 1, image.get_height() - 1);
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            bboxmin[j] = std::max(0.0f, std::min(bboxmin[j], pts[i][j]));
            bboxmax[j] = std::min(clamp[j], std::max(bboxmax[j], pts[i][j]));
        }
    }
    Vec3f p;
    for (p.x = bboxmin.x; p.x <= bboxmax.x; p.x++)
    {
        for (p.y = bboxmin.y; p.y <= bboxmax.y; p.y++)
        {
            Vec3f bc_screen = barycentric(pts, p);
            if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0) continue;
            p.z = 0;
            for (int i = 0; i < 3; i++) p.z += pts[i][2] * bc_screen[i];
            if (zBuffer[int(p.x + p.y * width)] < p.z)
            {
                zBuffer[int(p.x + p.y * width)] = p.z;

                Vec3f pixelNormal;
                Vec2f pixelUV;
                for (int i = 0; i < 3; i++) 
                {
                    pixelNormal += pts_normal[i] * bc_screen[i];
                    pixelUV += pts_uv[i] * bc_screen[i];
                }
                pixelNormal.normalize();
                pixelNormal += 1.0;
                pixelNormal /= 2.0;
                TGAColor normalColor = TGAColor(pixelNormal.x * 255, pixelNormal.y * 255, pixelNormal.z * 255, 255);
                image.set(p.x, p.y, texture.get(pixelUV.x * texture.get_width(), pixelUV.y * texture.get_height()));
            }
        }
    }
}

int main(int argc, char** argv)
{
    // model = new Model("model/ring.obj");
    // TGAImage texture;
    // texture.read_tga_file("model/soyo.tga");
    // TGAImage image(width, height, TGAImage::RGB);
    // float* zBuffer = new float[width * height];
    // for (int i = width * height; i--; zBuffer[i] = -std::numeric_limits<float>::max());

    // for (int i = 0; i < model->nfaces(); i++)
    // {
    //     faceIndex face = model->face(i);
    //     Vec3f pts[3];
    //     Vec3f pts_normal[3];
    //     Vec2f pts_uv[3];
    //     for (int j = 0; j < 3; j++)
    //     {
    //         Vec3f v = model->vert(face.vert[j]);
    //         pts_normal[j] = model->normal(face.normal[j]);
    //         pts_uv[j] = model->uv(face.uv[j]);
            
    //         //[-1, 1] -> [0, width]
    //         pts[j] = Vec3f(int((v.x + 1) * width / 2 + 0.5), int((v.y + 1) * height / 2 + 0.5), v.z);
    //     }
    //     DrawTriangle(pts, pts_normal, pts_uv, zBuffer, texture, image, TGAColor(rand()%255, rand()%255, rand()%255, 255));
    // }

    TGAImage image(width, height, TGAImage::RGB);


    image.flip_vertically();
    image.write_tga_file("output/output.tga");
    delete model;
    return 0;
}
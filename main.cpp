#include "lib/tgaimage.h"
#include "lib/model.h"

#include <iostream>
const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);
const TGAColor blue = TGAColor(0, 0, 255, 255);
const int width = 1000;
const int height = 1000;
Model *model = NULL;

void DrawLine(int x0, int y0, int x1, int y1, TGAImage& image, const TGAColor& color)
{
    //处理斜率大于1的情况
    bool steep = false;
    if (abs(x1 - x0) < abs(y1 - y0))
    {
        std::swap(x0, y0);
        std::swap(x1, y1);
        steep = true;
    }

    //处理反向绘制的情况
    if (x0 > x1)
    {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    //法一理解：x每走一个像素，就根据x+1和斜率计算出当前的y值，然后取整
    //法二理解：x每走一个像素，高度增加k，设开始e为0，e每次增加k，代表当前y的相对高度
    //如果e大于0.5，说明超过当前半个像素，y应该是当前y++
    //此时要把e--，是因为如果e > 0.5，则一定过两个像素中点，下一个像素的e一定大于1，此时需要与1.5比较
    //所以直接-1，就又变成和0.5比较了
    //如果e小于0.5，说明未超过半个像素，y应该保持当前像素

    //优化浮点数，令e2 = 2 * e * dx，首先消灭除法，两边都乘dx，然后消除0.5，两边都乘2
    //dy加abs是因为我们限制在第一象限，不允许斜率为负，其情况根据y1与y0特殊处理是+1还是-1
    int dy = y1 - y0;
    int dx = x1 - x0;
    int derror = abs(dy) * 2;
    int error = 0;
    int y = y0;
    const int offsetY = y1 > y0 ? 1 : -1;
    
    for (int x = x0; x <= x1; x++)
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
    int derror = abs(dy) * 2;
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

void DrawTriangle(Vec2i* pts, TGAImage& image, const TGAColor& color)
{
    // if (t0.y > t1.y) std::swap(t0, t1);
    // if (t0.y > t2.y) std::swap(t0, t2);
    // if (t1.y > t2.y) std::swap(t1, t2);

    // int th = t2.y - t0.y;
    // int sh = t1.y - t0.y + 1;
    // int sh2 = t2.y - t1.y + 1;
    // for (int y = t0.y; y <= t2.y; y++)
    // {
    //     float alpha = (float)(y - t0.y) / th;
    //     Vec2i A = t0 + (t2 - t0) * alpha;
    //     Vec2i B;
    //     if (y <= t1.y)
    //     {
    //         float beta = (float)(y - t0.y) / sh;
    //         B = t0 + (t1 - t0) * beta;
    //     }
    //     else 
    //     {
    //         float beta = (float)(y - t1.y) / sh2;
    //         B = t1 + (t2 - t1) * beta;
    //     }
    //     if (A.x > B.x) std::swap(A, B);
    //     for (int j = A.x; j <= B.x; j++)
    //     {
    //         image.set(j, y, color);
    //     }
    // }

    Vec2i bboxmin(image.get_width() - 1, image.get_height() - 1);
    Vec2i bboxmax(0, 0);
    Vec2i clamp(image.get_width() - 1, image.get_height() - 1);
    for (int i = 0; i < 3; i++)
    {
        bboxmin.x = std::max(0, std::min(bboxmin.x, pts[i].x));
        bboxmin.y = std::max(0, std::min(bboxmin.y, pts[i].y));
        bboxmax.x = std::min(clamp.x, std::max(bboxmax.x, pts[i].x));
        bboxmax.y = std::min(clamp.y, std::max(bboxmax.y, pts[i].y));
    }

    Vec2i p;
    for (p.x = bboxmin.x; p.x < bboxmax.x; p.x++)
    {
        for (p.y = bboxmin.y; p.y < bboxmax.y; p.y++)
        {
            Vec3f bc_screen = barycentric(pts, p);
            if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0) continue;
            image.set(p.x, p.y, color);
        }
    }
}

int main(int argc, char** argv)
{
    model = new Model("model/ring.obj");
    TGAImage image(width, height, TGAImage::RGB);
    
    for (int i = 0; i < model->nfaces(); i++)
    {
        std::vector<int> face = model->face(i);
        Vec2i screen_coords[3];
        for (int j = 0; j < 3; j++)
        {
            Vec3f world_coords = model->vert(face[j]);
            screen_coords[j] = Vec2i((world_coords.x + 1) * width / 2, (world_coords.y + 1) * height / 2);
        }
        DrawTriangle(screen_coords, image, TGAColor(rand() % 255, rand() % 255, rand() % 255, 255));
    }

    image.flip_vertically();
    image.write_tga_file("output/output.tga");
    delete model;
    return 0;
}
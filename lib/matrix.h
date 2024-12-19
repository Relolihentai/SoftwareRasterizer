#ifndef _MATRIX_H_
#define _MATRIX_H_

#include <vector>
#include <vec.h>

class Matrix
{
    std::vector<std::vector<float> > m;
    int rows, cols;
public:
    Matrix(int r = 4, int c = 4);
    Matrix(Vec3f v);
    int nrows();
    int ncols();
    static Matrix identity(int dimensions);
    std::vector<float>& operator[](const int index);
    Matrix operator*(const Matrix& a);
    Matrix transpose();
    Matrix inverse();
    friend std::ostream& operator<<(std::ostream&s, Matrix& m);
};

#endif
// Matrix.h : header file
// Declares basic matrix functions
// Learned through Jeff Lander's tutorial and 
// demonstration of inverse kinematics on darwin3d.com

#ifndef MATRIX_H
#define MATRIX_H

#include "MathFuncs.h"

// MultVectorByMatrix()
// Multiplies a vector by a 4x4 Matrix in OpenGL format
void MultVectorByMatrix(tMatrix* mat, tVector* v, tVector* result);

#endif
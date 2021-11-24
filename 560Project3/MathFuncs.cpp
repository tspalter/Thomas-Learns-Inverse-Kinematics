// MathFuncs.h : source file
// implements basic math functions
// Learned through Jeff Lander's tutorial and 
// demonstration of inverse kinematics on darwin3d.com

#include "pch.h"
#include "stdafx.h"
#include <math.h>
#include "MathFuncs.h"

// MultVectorByMatrix()
// Multiplies a vector by a 4x4 Matrix in OpenGL format
void MultVectorByMatrix(tMatrix* mat, tVector* v, tVector* result) {
	result->x = (mat->m[0] * v->x) + (mat->m[4] * v->y) + (mat->m[8] * v->z) + mat->m[12];
	result->y = (mat->m[1] * v->x) + (mat->m[5] * v->y) + (mat->m[9] * v->z) + mat->m[13];
	result->z = (mat->m[2] * v->x) + (mat->m[6] * v->y) + (mat->m[10] * v->z) + mat->m[14];
}

// returns the squared length of input vector
double VectorSquaredLength(tVector* v) {
	return((v->x * v->x) + (v->y * v->y) + (v->z * v->z));
}

// returns the length of input vector
double VectorLength(tVector* v) {
	return(sqrt(VectorSquaredLength(v)));
}

// destructively normalizes the input vector
void NormalizeVector(tVector* v) {
	float len = (float)VectorLength(v);
	if (len != 0.0f) {
		v->x /= len;
		v->y /= len;
		v->z /= len;
	}
}

// returns the dot product of two input vectors
double DotProduct(tVector* v1, tVector* v2) {
	return ((v1->x * v2->x) + (v1->y * v2->y) + (v1->z + v2->z));
}

// returns the cross product result v1 x v2
void CrossProduct(tVector* v1, tVector* v2, tVector* result) {
	result->x = (v1->y * v2->z) - (v1->z * v2->y);
	result->y = (v1->z * v2->x) - (v1->x * v2->z);
	result->z = (v1->x * v2->y) - (v1->y * v2->x);
}

// returns the squared distance between two input vectors
double VectorSquaredDistance(tVector* v1, tVector* v2) {
	return( ((v1->x - v2->x) * (v1->x - v2->x)) +
			((v1->y - v2->y) * (v1->y - v2->y)) +
			((v1->z - v2->z) * (v1->z - v2->z)) );
}
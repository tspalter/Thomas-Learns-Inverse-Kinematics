// MathFuncs.h : header file
// Declares basic math structures
// Learned through Jeff Lander's tutorial and 
// demonstration of inverse kinematics on darwin3d.com

#ifndef MATHFUNCS_H
#define MATHFUNCS_H

#define M_PI	3.14159265358979323846f
#define HALF_PI	1.57079632679489661923f

// TRIG MACROS
#define DEGTORAD(A) ((A * M_PI) / 180.0f)
#define RADTODEG(A) ((A * 180.0f) / M_PI)

typedef struct {
	float x, y, z;
} tVector;

// OpenGL accesses 2D arrays strangely, as to why this isn't declared with a float[4][4]
typedef struct {
	float m[16];
} tMatrix;

// These structs help to access vertex data in an array
typedef struct {
	float r, g, b;
	float x, y, z;
} tColoredVertex;

typedef struct {
	float u, v;
	float x, y, z;
} tTexturedVertex;

typedef struct {
	float u, v;
	float r, g, b;
	float x, y, z;
} tTexturedColoredVertex;

// Quaternion Definition
typedef struct {
	float x, y, z, w;
} tQuaternion;

// MultVectorByMatrix()
// Multiplies a vector by a 4x4 Matrix in OpenGL format
void MultVectorByMatrix(tMatrix* mat, tVector* v, tVector* result);

double VectorSquaredLength(tVector* v);
double VectorLength(tVector* v);
void NormalizeVector(tVector* v);
double DotProduct(tVector* v1, tVector* v2);
void CrossProduct(tVector* v1, tVector* v2, tVector* result);
double VectorSquaredDistance(tVector* v1, tVector* v2);

#endif
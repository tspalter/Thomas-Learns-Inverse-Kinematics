// Bitmap.h : header file
// Implementation of Windows BMP Loader
// Learned through Jeff Lander's tutorial and 
// demonstration of inverse kinematics on darwin3d.com
// Originally from the OpenGL Super Bible by Richard Wright Jr. and Michael Sweet

#include <Windows.h>

#include <gl/GL.h>
#include <gl/GLU.h>

extern void* LoadDIBitmap(char* filename, BITMAPINFO** info);
GLubyte* ConvertBitsToGL(BITMAPINFO* info, void* bits);

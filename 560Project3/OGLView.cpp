// OGLView.cpp : source file
// Implements the OpenGL Window and Inverse Kinematic System
// Learned through Jeff Lander's tutorial and 
// demonstration of inverse kinematics on darwin3d.com

#include "pch.h"
#include <math.h>
#include "560Project3.h"
#include "MathFuncs.h"
#include "OGLView.h"
#include "Quaternion.h"
#include "Model.h"  // SoftImage Model Data
#include "Restrict.h" // DOF Restriction Dialog
#include "Bitmap.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Application Definitions
#define OGL_AXIS_DLIST	99		// OpenGL display list ID
#define ROTATE_SPEED	1.0		// Speed of rotation
#define EFFECTOR_POS	5		// This chain has 5 links
#define MAX_IK_TRIES	100		// Times through the CCD loop (Tries = # / Links)
#define IK_POS_THRESH	1.0f	// Threshold for success

COGLView::COGLView() {
	// Initialize the mode keys
	mDrawGeometry = TRUE;

	// Initialize some of the skeleton vars
	ResetBone(&mLink[0], NULL);
	mLink[0].id = 1;
	mLink[0].trans.x = 4.8f;
	mLink[0].trans.y= 6.0f;
	strcpy(mLink[0].name, "Base");
	mLink[0].childCount = 1;
	mLink[0].children = &mLink[1];

	ResetBone(&mLink[1], NULL);
	mLink[1].id = 2;
	mLink[1].trans.y = -1.0f;
	strcpy(mLink[1].name, "Link1");
	mLink[1].childCount = 1;
	mLink[1].children = &mLink[2];

	ResetBone(&mLink[2], NULL);
	mLink[2].id = 3;
	mLink[2].trans.y = -1.0f;
	strcpy(mLink[2].name, "Link2");
	mLink[2].childCount = 1;
	mLink[2].children = &mLink[3];

	ResetBone(&mLink[3], NULL);
	mLink[3].id = 4;
	mLink[3].trans.y = -1.0f;
	strcpy(mLink[3].name, "Link3");
	mLink[3].childCount = 1;
	mLink[3].children = &mLink[4];

	ResetBone(&mLink[4], NULL);
	mLink[4].id = 5;
	mLink[4].trans.y = -1.0f;
	strcpy(mLink[4].name, "Link4");
	mLink[4].childCount = 1;
	mLink[4].children = &mLink[5];

	// Set up end effector
	ResetBone(&mLink[5], NULL);
	mLink[5].id = 6;
	mLink[5].trans.y = -1.0f;
	strcpy(mLink[5].name, "Effector");

	// Set up default settings for the damping for six joints
	mLink[0].dampWidth = 10.0f;
	mLink[1].dampWidth = 10.0f;
	mLink[2].dampWidth = 10.0f;
	mLink[3].dampWidth = 10.0f;
	mLink[4].dampWidth = 10.0f;
	mLink[5].dampWidth = 10.0f;  // End effector, so it isn't really used

	// Set up default setting for the depth of field restrictions
	mLink[0].minRz = -30;
	mLink[1].minRz = -30;
	mLink[2].minRz = -30;
	mLink[3].minRz = -30;
	mLink[4].minRz = -30;
	mLink[5].minRz = -30;  // End effector, so it isn't really used

	mLink[0].maxRz = 30;
	mLink[1].maxRz = 30;
	mLink[2].maxRz = 30;
	mLink[3].maxRz = 30;
	mLink[4].maxRz = 30;
	mLink[5].maxRz = 30;  // End effector, so it isn't really used

	// by default, we don't want any damping or depth of field restriction
	mDamping = FALSE;
	mDOFRestrict = FALSE;
}

COGLView::~COGLView() {

}

// UpdateStatus()
// Update the status bar with orientation info
void COGLView::UpdateStatus() {
	// local vars
	char message[80];

	// write the orientations of the bones in the window status bar
	sprintf(message, "Joint Rot Values (%.2f,%.2f,%.2f,%.2f,%.2f)",
		mLink[0].rot.z, mLink[1].rot.z, mLink[2].rot.z, mLink[3].rot.z,
		mLink[4].rot.z);

	mPtrStatusBar->SetPaneText(1, (LPCTSTR)message);

}

// Converts data for the COGLView class to be read by CWnd
BOOL COGLView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) {
	UpdateStatus(); // Draw initial status bar
	return CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}

// Create the message map between our created class and its base class
BEGIN_MESSAGE_MAP(COGLView, CWnd)
	//{{AFX_MSG_MAP(COGLView)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// COGL message handlers

BOOL COGLView::SetupPixelFormat(HDC hdc) {
	// local vars
	PIXELFORMATDESCRIPTOR pfd, * ppfd;
	int pixelFormat;

	ppfd = &pfd;

	ppfd->nSize = sizeof(PIXELFORMATDESCRIPTOR);
	ppfd->nVersion = 1;
	ppfd->dwFlags = PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	ppfd->dwLayerMask = PFD_MAIN_PLANE;
	ppfd->iPixelType = PFD_TYPE_RGBA;
	ppfd->cColorBits = 16;
	ppfd->cDepthBits = 16;
	ppfd->cAccumBits = 0;
	ppfd->cStencilBits = 0;

	pixelFormat = ChoosePixelFormat(hdc, ppfd);

	// check for choose pixel format failure
	if ((pixelFormat = ChoosePixelFormat(hdc, ppfd)) == 0) {
		MessageBox((LPCTSTR)"ChoosePixelFormat failed", (LPCTSTR)"Error", MB_OK);
		return FALSE;
	}

	// check for palette
	if (pfd.dwFlags & PFD_NEED_PALETTE) {
		MessageBox((LPCTSTR)"Needs palette", (LPCTSTR)"Error", MB_OK);
		return FALSE;
	}
	
	// check if set pixel format failure
	if (SetPixelFormat(hdc, pixelFormat, ppfd) == FALSE) {
		MessageBox((LPCTSTR)"SetPixelFormat failed", (LPCTSTR)"Error", MB_OK);
		return FALSE;
	}

	return TRUE;
}

// function that acts on creation of window
int COGLView::OnCreate(LPCREATESTRUCT lpCreateStruct) {
	// local vars
	RECT rect;

	// window creation failure
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// get HDC from CWnd
	mHDC = ::GetDC(m_hWnd);
	if (!SetupPixelFormat(mHDC))
		PostQuitMessage(0);

	// initialize HGLRC
	mHRC = wglCreateContext(mHDC);
	wglMakeCurrent(mHDC, mHRC);
	GetClientRect(&rect);
	InitializeGL(rect.right, rect.bottom);

	// Create the display list for an axis with arrows pointing in
	// the positive direction (Red = X, Green = Y, Blue = Z)
	glNewList(OGL_AXIS_DLIST, GL_COMPILE);
		glPushMatrix();
		glScalef(2.0f, 2.0f, 2.0f);
		glBegin(GL_LINES);
			glColor3f(1.0f, 0.0f, 0.0f);	// X axis start (Red)
			glVertex3f(-0.2f, 0.0f, 0.0f);
			glVertex3f(0.2f, 0.0f, 0.0f);
			glVertex3f(0.2f, 0.0f, 0.0f);	// Top of arrowhead
			glVertex3f(0.15f, 0.04f, 0.0f);
			glVertex3f(0.2f, 0.0f, 0.0f);	// Bottom of arrowhead
			glVertex3f(0.15f, -0.04f, 0.0f);
			glColor3f(0.0f, 1.0f, 0.0f);	// Y axis start (Green)
			glVertex3f(0.0f, 0.2f, 0.0f);
			glVertex3f(0.0f, -0.2f, 0.0f);
			glVertex3f(0.0f, 0.2f, 0.0f);	// Top of arrowhead
			glVertex3f(0.04f, 0.15f, 0.0f);
			glVertex3f(0.0f, 0.2f, 0.0f);	// Bottom of arrowhead
			glVertex3f(-0.04f, 0.15f, 0.0f);
			glColor3f(0.0f, 0.0f, 1.0f);	// Z axis start (Blue)
			glVertex3f(0.0f, 0.0f, 0.2f);
			glVertex3f(0.0f, 0.0f, -0.2f);
			glVertex3f(0.0f, 0.0f, 0.2f);	// Top of arrowhead
			glVertex3f(0.0f, 0.04f, 0.15f);
			glVertex3f(0.0f, 0.0f, 0.2f);	// Bottom of arrowhead
			glVertex3f(0.0f, -0.04f, 0.15f);
		glEnd();
		glPopMatrix();
	glEndList();

	CreateBoneDLists(&mLink[0]);

	// Load the texture maps for the object
	LoadBoneTexture(&mLink[0], "Snake.bmp");
	mLink[1].visuals = mLink[0].visuals; // Bones 1 - 3 use instanced textures
	mLink[2].visuals = mLink[0].visuals;
	mLink[3].visuals = mLink[0].visuals;
	LoadBoneTexture(&mLink[4], "Head.bmp");
	// no texture for the end effector as there's no visual for it

	// finally, draw the scene
	DrawScene(TRUE);
	return 0;

}

// OpenGL code
GLvoid COGLView::Resize(GLsizei width, GLsizei height) {
	// local vars
	GLfloat aspect;

	// set class values to new width/height and set the viewport
	mWidth = width;
	mHeight = height;

	glViewport(0, 0, width, height);

	// set the aspect ratio
	aspect = (GLfloat)width / (GLfloat)height;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// For this application, we want to use a 2D orthographic view
	gluOrtho2D(0.0f, (GLfloat)width, 0.0f, (GLfloat)height);
	// gluPerspective(10.0, aspect, 1.0, 2000.0;
	glMatrixMode(GL_MODELVIEW);
	mModelScale = (float)height / 6.0f;
	glScalef(mModelScale, mModelScale, 0.0f);

	mLink[0].trans.x = ((float)width / 2.0f) / mModelScale;
}

// LoadBoneTexture()
// Loads texture images for the bone
GLvoid COGLView::LoadBoneTexture(tBone* curBone, char* name) {
	if (mDrawGeometry) {
		BITMAPINFO* info; // bitmap info
		void* bits; // bitmap pixel bits
		GLubyte* glbits; // Bitmap RGB pixels

		// Generate the OpenGL texture ID
		glGenTextures(1, (unsigned int*)&curBone->visuals);
		curBone->visualCount++;

		// load the bitmap
		bits = LoadDIBitmap(name, &info);
		// file couldn't be found
		if (bits == NULL) {
			::MessageBox(NULL, (LPCWSTR)"Unable to Open File...", (LPCWSTR)name, MB_OK);
			curBone->visuals = 0;
			return;
		}

		// convert it to an RGB texture
		glbits = ConvertBitsToGL(info, bits);
		// couldn't convert correctly, free memory before returning
		if (glbits == NULL) {
			free(info);
			free(bits);

			return;
		}

		// bind texture and set parameters
		glBindTexture(GL_TEXTURE_2D, (unsigned int)curBone->visuals);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		// define the 2D texture image
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4); // Force 4-byte alignment
		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
		glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
		glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);

		glTexImage2D(GL_TEXTURE_2D, 0, 3, info->bmiHeader.biWidth, info->bmiHeader.biHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, glbits);

		// free the bitmap and RGB images, then return 0
		free(glbits);
		free(info);
		free(bits);
	}
}

// init function for OpenGL
GLvoid COGLView::InitializeGL(GLsizei width, GLsizei height) {
	// local vars
	GLfloat aspect;

	// basic OpenGL init functions
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glDepthFunc(GL_LEQUAL);
	glShadeModel(GL_SMOOTH);

	glEnable(GL_DEPTH_TEST);

	glMatrixMode(GL_PROJECTION);
	aspect = (GLfloat)width / (GLfloat)height;

	// Establish viewing volume
	// For this app, we want to use a 2D orthographic view
	gluOrtho2D(0.0f, (GLfloat)width, 0.0f, (GLfloat)height);
	// gluPerspective(10.0, aspect, 1, 2000);
	glMatrixMode(GL_MODELVIEW);

	// Set some OpenGL initial states so they are not done in the draw loop
	glPolygonMode(GL_FRONT, GL_FILL);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_CULL_FACE);

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glEnable(GL_TEXTURE_2D);

	glShadeModel(GL_SMOOTH);
	glDisable(GL_LIGHTING);
}

// Get the info on the OpenGL version that's running
void COGLView::GetGLInfo() {
	// local vars
	char* who, *which, *ver, *ext, *message;
	int len;

	who = (char*)::glGetString(GL_VENDOR);
	which = (char*)::glGetString(GL_RENDERER);
	ver = (char*)::glGetString(GL_VERSION);
	ext = (char*)::glGetString(GL_EXTENSIONS);

	len = 200 + strlen(who) + strlen(which) + strlen(ver) + strlen(ext);

	message = (char*)malloc(len);
	sprintf(message, "Who:\t%s\nWhich:\t%s\nVersion:\t%s\nExtensions:\t%s",
		who, which, ver, ext);

	::MessageBox(NULL, (LPCWSTR)message, (LPCWSTR)"GL Info", MB_OK);

	free(message);
}

// CreateBoneDLists()
// Creates the Drawlists for the bones in a skeleton
void COGLView::CreateBoneDLists(tBone* bone) {
	// Only make a bone if a child exists
	if (bone->childCount > 0) {
		// create the display list for a bone
		glNewList(bone->id, GL_COMPILE);
			glBegin(GL_LINE_STRIP);
				glColor3f(1.0f, 1.0f, 0.0f);	// Yellow
				glVertex3f(0.0f, 0.4f, 0.0f);		// 0
				glVertex3f(-0.4f, 0.0f, -0.4f);		// 1
				glVertex3f(0.4f, 0.0f, -0.4f);		// 2
				glVertex3f(0.0f, bone->children->trans.y, 0.0f);		// Base
				glVertex3f(-0.4f, 0.0f, -0.4f);		// 1
				glVertex3f(-0.4f, 0.0f, 0.4f);		// 4
				glVertex3f(0.0f, 0.4f, 0.0f);		// 0
				glVertex3f(0.4f, 0.0f, -0.4f);		// 2
				glVertex3f(0.4f, 0.0f, 0.4f);		// 3
				glVertex3f(0.0f, 0.4f, 0.0f);		// 0
				glVertex3f(-0.4f, 0.0f, 0.4f);		// 4
				glVertex3f(0.0f, bone->children->trans.y, 0.0f);		// Base
				glVertex3f(0.4f, 0.0f, 0.4f);		// 3
				glVertex3f(-0.4f, 0.0f, 0.4f);		// 4
			glEnd();
		glEndList();

		// recursively call function if the bone has children
		if (bone->childCount > 0)
			CreateBoneDLists(bone->children);
	}
}

// draws the model with textures (if available)
GLvoid COGLView::DrawModel(tBone* curBone) {
	// draw the texture if the bone has one
	if (curBone->visuals > 0) {
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, (unsigned int)curBone->visuals);
	}
	// interleave arrays using the SoftImage data from Model.h
	glInterleavedArrays(GL_T2F_C3F_V3F, 0, (GLvoid*)SNAKE);

	glDrawArrays(GL_TRIANGLES, 0, SNAKEPOLYCNT * 3);
	glDisable(GL_TEXTURE_2D);
}

// DrawSkeleton()
// Draws the skeleton recursively
GLvoid COGLView::DrawSkeleton(tBone* rootBone, BOOL actuallyDraw) {
	// local vars
	int loop;
	tBone* curBone;

	curBone = rootBone->children;
	for (loop = 0; loop < rootBone->childCount; loop++) {
		glPushMatrix();

		// Set base orientation and position
		glTranslatef(curBone->trans.x, curBone->trans.y, curBone->trans.z);

		glRotatef(curBone->rot.z, 0.0f, 0.0f, 1.0f);
		glRotatef(curBone->rot.y, 0.0f, 1.0f, 0.0f);
		glRotatef(curBone->rot.x, 1.0f, 0.0f, 0.0f);

		// since scale is local, we must push and pop
		glPushMatrix();
		glScalef(curBone->scale.x, curBone->scale.y, curBone->scale.z);

		if (actuallyDraw) {
			if (mDrawGeometry) {
				if (curBone->childCount > 0) {
					DrawModel(curBone);
				}
			}
			else {
				// Draw the axis OpenGL object
				glCallList(OGL_AXIS_DLIST);
				
				// Draw the actual bone structure
				// only make a bone if there is a child
				if (curBone->childCount > 0) {
					glColor3f(1.0f, 1.0f, 0.0f);  // Selected bone color is yellow
					// draw the bone structure
					glCallList(curBone->id);
				}
			}
		}

		// Grab the matrix at this point, so it can be used for the deformation
		glGetFloatv(GL_MODELVIEW_MATRIX, curBone->matrix.m);

		glPopMatrix(); // For the scale

		// Check if this bone has children, and recursively call if so
		if (curBone->childCount > 0) {
			DrawSkeleton(curBone, actuallyDraw);
		}

		glPopMatrix(); // For the whole matrix
		curBone++;
	}
}

// DrawScene()
// Draws the current OpenGL scene
GLvoid COGLView::DrawScene(BOOL actuallyDraw) {
	if (actuallyDraw) {
		glDisable(GL_DEPTH_TEST); // turn off depth test
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST); // re-enable depth test
	}
	glPushMatrix();

	// Set root skeleton's orientation and position
	glTranslatef(mLink[0].trans.x, mLink[0].trans.y, mLink[0].trans.z);

	// Rotate the root
	glRotatef(mLink[0].rot.z, 0.0f, 0.0f, 1.0f);
	glRotatef(mLink[0].rot.y, 0.0f, 1.0f, 1.0f);
	glRotatef(mLink[0].rot.x, 1.0f, 0.0f, 1.0f);

	// Grab the matrix at this point to be used for deformation
	glGetFloatv(GL_MODELVIEW_MATRIX, mLink[0].matrix.m);

	// draw the model
	if (actuallyDraw) {
		if (mDrawGeometry) {
			DrawModel(mLink);
		}
		else {
			glCallList(mLink[0].id);
			glCallList(OGL_AXIS_DLIST);
		}
	}

	DrawSkeleton(&mLink[0], actuallyDraw);

	glPopMatrix();
	glFinish();

	if (actuallyDraw) {
		SwapBuffers(mHDC);

		// Draw the stats at the bottom of the screen
		UpdateStatus();
	}
}

// activates on destroying the window
void COGLView::OnDestroy() {
	CWnd::OnDestroy();
	if (mHRC)
		wglDeleteContext(mHRC);
	if (mHDC)
		::ReleaseDC(m_hWnd, mHDC);
	mHRC = 0;
	mHDC = 0;
}

// on paint function
void COGLView::OnPaint() {
	CPaintDC dc(this); // device context for painting
	DrawScene(TRUE);

	// Do not call CWnd::OnPaint() for painting messages
}

// function to call on resize
void COGLView::OnSize(UINT nType, int cx, int cy) {
	// Resize the original window
	Resize(cx, cy);
}

// OnLButtonDown()
// Left mouse button grabs and continually updates the IK function while held down
void COGLView::OnLButtonDown(UINT nFlags, CPoint point) {
	// local vars
	CPoint joint1, joint2, effector;

	mMousePos = point;
	point.y = mHeight - point.y - 1;

	// If you click anywhere, solve the IK system
	if ((nFlags & MK_CONTROL) == 0) {
		ComputeCCDLink(point);
		DrawScene(TRUE);
	}
	else {
		ComputeOneCCDLink(point, 2);
		DrawScene(TRUE);
	}
	mGrabRotX = mLink[2].rot.x;
	mGrabRotY = mLink[2].rot.y;
	mGrabRotZ = mLink[2].rot.z;

	mGrabTransX = mLink[2].trans.x;
	mGrabTransY = mLink[2].trans.y;
	mGrabTransZ = mLink[2].trans.z;
	CWnd::OnLButtonDown(nFlags, point);
}

// OnRButtonDown
// Right mouse button grabs the current point to be used for the CCD function
void COGLView::OnRButtonDown(UINT nFlags, CPoint point) {
	mMousePos = point;
	point.y = mHeight - point.y - 1;

	// If you click anywhere, solve the IK system
	if ((nFlags & MK_CONTROL) == 0) {
		ComputeCCDLink(point);
		DrawScene(TRUE);
	}
	else {
		ComputeOneCCDLink(point, 4);
		DrawScene(TRUE);
	}
	mGrabRotX = mLink[4].rot.x;
	mGrabRotY = mLink[4].rot.y;
	mGrabRotZ = mLink[4].rot.z;

	mGrabTransX = mLink[4].trans.x;
	mGrabTransY = mLink[4].trans.y;
	mGrabTransZ = mLink[4].trans.z;
	CWnd::OnRButtonDown(nFlags, point);
}

// OnMButtonDown()
// Has relatively similar functionality to OnRButtonDown, but starts on the bone before it
void COGLView::OnMButtonDown(UINT nFlags, CPoint point) {
	mMousePos = point;
	point.y = mHeight - point.y - 1;

	// If you click anywhere, solve the IK system
	if ((nFlags & MK_CONTROL) == 0) {
		ComputeCCDLink(point);
		DrawScene(TRUE);
	}
	else {
		ComputeOneCCDLink(point, 3);
		DrawScene(TRUE);
	}
	mGrabRotX = mLink[3].rot.x;
	mGrabRotY = mLink[3].rot.y;
	mGrabRotZ = mLink[3].rot.z;

	mGrabTransX = mLink[3].trans.x;
	mGrabTransY = mLink[3].trans.y;
	mGrabTransZ = mLink[3].trans.z;
	CWnd::OnMButtonDown(nFlags, point);
}

void COGLView::HandleKeyDown(UINT nChar) {

}

// function that handles KeyUp cases
void COGLView::HandleKeyUp(UINT nChar) {
	switch (nChar) {
	case 'G':
		mDrawGeometry = !mDrawGeometry;
		break;
	}
}

// OnMouseMove()
// Handles mouse moving while pressed
void COGLView::OnMouseMove(UINT nFlags, CPoint point) {
	if ((nFlags & MK_LBUTTON) > 0) {
		if ((nFlags & MK_CONTROL) > 0) {
			if ((point.x - mMousePos.x) != 0) {
				// mLink[3].rot.z = mGrabRotZ + ((float)ROTATE_SPEED * (point.x - mMousePos.x));
				DrawScene(TRUE);
			}
		}
		else {
			point.y = mHeight - point.y - 1;
			ComputeCCDLink(point);
			DrawScene(TRUE);
		}
	}
	else if ((nFlags & MK_RBUTTON) == MK_RBUTTON) {
		if ((nFlags & MK_CONTROL) > 0) {
			if ((point.x - mMousePos.x) != 0) {
				// mLink[4].rot.z = mGrabRotZ + ((float)ROTATE_SPEED * (point.x - mMousePos.x));
				DrawScene(TRUE);
			}
		}
	}
	else if ((nFlags & MK_MBUTTON) == MK_MBUTTON) {
		if ((nFlags & MK_CONTROL) > 0) {
			if ((point.x - mMousePos.x) != 0) {
				// mLink[2].rot.z = mGrabRotZ + ((float)ROTATE_SPEED * (point.x - mMousePos.x));
				DrawScene(TRUE);
			}
		}
	}
	CWnd::OnMouseMove(nFlags, point);
}

// OnLButtonDblClk()
// Left Double Click to get dialog for Orientation
void COGLView::OnLButtonDblClk(UINT nFlags, CPoint point) {

}

// SetRestrictions()
// Opens Dialog to set up restrictions
void COGLView::SetRestrictions() {
	CRestrict dialog;

	// set damp widths
	dialog.m_Damp0 = mLink[0].dampWidth;
	dialog.m_Damp1 = mLink[1].dampWidth;
	dialog.m_Damp2 = mLink[2].dampWidth;
	dialog.m_Damp3 = mLink[3].dampWidth;
	dialog.m_Damp4 = mLink[4].dampWidth;

	// set minimum rotations
	dialog.m_MinRot0 = mLink[0].minRz;
	dialog.m_MinRot1 = mLink[1].minRz;
	dialog.m_MinRot2 = mLink[2].minRz;
	dialog.m_MinRot3 = mLink[3].minRz;
	dialog.m_MinRot4 = mLink[4].minRz;

	// set maximum rotations
	dialog.m_MaxRot0 = mLink[0].maxRz;
	dialog.m_MaxRot1 = mLink[1].maxRz;
	dialog.m_MaxRot2 = mLink[2].maxRz;
	dialog.m_MaxRot3 = mLink[3].maxRz;
	dialog.m_MaxRot4 = mLink[4].maxRz;

	if (dialog.DoModal()) {
		// set damp widths
		mLink[0].dampWidth = dialog.m_Damp0;
		mLink[1].dampWidth = dialog.m_Damp1;
		mLink[2].dampWidth = dialog.m_Damp2;
		mLink[3].dampWidth = dialog.m_Damp3;
		mLink[4].dampWidth = dialog.m_Damp4;

		// set minimum rotations
		mLink[0].minRz = dialog.m_MinRot0;
		mLink[1].minRz = dialog.m_MinRot1;
		mLink[2].minRz = dialog.m_MinRot2;
		mLink[3].minRz = dialog.m_MinRot3;
		mLink[4].minRz = dialog.m_MinRot4;

		// set maximum rotations
		mLink[0].maxRz = dialog.m_MaxRot0;
		mLink[1].maxRz = dialog.m_MaxRot1;
		mLink[2].maxRz = dialog.m_MaxRot2;
		mLink[3].maxRz = dialog.m_MaxRot3;
		mLink[4].maxRz = dialog.m_MaxRot4;
	}
}

// ComputeOneCCDLink
// Computes the IK Solution to an end effector
BOOL COGLView::ComputeOneCCDLink(CPoint endPos, int link) {
	// local vars
	tVector rootPos, curEnd, desiredEnd, targetVector, curVector, crossResult;
	double cosAngle, turnAngle, turnDeg;

	// set root position
	rootPos.x = mLink[link].matrix.m[12];
	rootPos.y = mLink[link].matrix.m[13];
	rootPos.z = mLink[link].matrix.m[14];

	// set current end effector position
	curEnd.x = mLink[EFFECTOR_POS].matrix.m[12];
	curEnd.y = mLink[EFFECTOR_POS].matrix.m[13];
	curEnd.z = mLink[EFFECTOR_POS].matrix.m[14];

	// set the desired end point
	desiredEnd.x = (float)endPos.x;
	desiredEnd.y = (float)endPos.y;
	desiredEnd.z = 0.0f; // displaying in 2D for now

	// Check to see if we're close enough
	if (VectorSquaredDistance(&curEnd, &desiredEnd) > 1.0f) {
		curVector.x = curEnd.x - rootPos.x;
		curVector.y = curEnd.y - rootPos.y;
		curVector.z = curEnd.z - rootPos.z;

		targetVector.x = endPos.x - rootPos.x;
		targetVector.y = endPos.y - rootPos.y;
		targetVector.z = 0.0f; // displaying in 2D for now

		// normalize both vectors
		NormalizeVector(&curVector);
		NormalizeVector(&targetVector);

		// set the angle based on the dot product of our two vector
		cosAngle = DotProduct(&targetVector, &curVector);

		// check if our angle causes a rotation
		if (cosAngle < 0.99999) {
			// take the cross product of our vectors
			CrossProduct(&targetVector, &curVector, &crossResult);

			// rotate clockwise
			if (crossResult.z > 0.0f) {
				turnAngle = acos((float)cosAngle);
				turnDeg = RADTODEG(turnAngle);
				mLink[link].rot.z -= (float)turnDeg;
			}
			// rotate counter-clockwise
			else if (crossResult.z < 0.0f) {
				turnAngle = acos((float)cosAngle);
				turnDeg = RADTODEG(turnAngle);
				mLink[link].rot.z += (float)turnDeg;
			}
			DrawScene(FALSE); // Change to true to view the iteration
		}
	}

	return TRUE;
}

// ComputeCCDLink()
// Compute an IK Solution to an end effector, handles all links in the chain
BOOL COGLView::ComputeCCDLink(CPoint endPos) {
	// local vars
	tVector		rootPos, curEnd, desiredEnd, targetVector, curVector, crossResult;
	double		cosAngle, turnAngle, turnDeg;
	int			link, tries;

	// Start at the last link in the chain
	link = EFFECTOR_POS - 1;
	tries = 0; // loop counter so the program knows when to quit
	do {
		// The coords of the x, y, z position of the root of this bone is in the matrix
		// translation part which is in the 12, 13, 14 position of the matrix
		rootPos.x = mLink[link].matrix.m[12];
		rootPos.y = mLink[link].matrix.m[13];
		rootPos.z = mLink[link].matrix.m[14];

		// Position of the end effector
		curEnd.x = mLink[EFFECTOR_POS].matrix.m[12];
		curEnd.y = mLink[EFFECTOR_POS].matrix.m[13];
		curEnd.z = mLink[EFFECTOR_POS].matrix.m[14];

		// Desired end effector position
		desiredEnd.x = (float)endPos.x;
		desiredEnd.y = (float)endPos.y;
		desiredEnd.z = 0.0f; // displaying in 2D for now

		// Check to see if we're close enough
		if (VectorSquaredDistance(&curEnd, &desiredEnd) > 1.0f) {
			// create the vector to the current effector position
			curVector.x = curEnd.x - rootPos.x;
			curVector.y = curEnd.y - rootPos.y;
			curVector.z = curEnd.z - rootPos.z;

			// create the desired effector position vector
			targetVector.x = endPos.x - rootPos.x;
			targetVector.y = endPos.y - rootPos.y;
			targetVector.z = 0.0f; // displaying in 2D for now

			// normalize both vectors
			NormalizeVector(&curVector);
			NormalizeVector(&targetVector);

			// set the cosine angle based on the dot product of our two vectors
			cosAngle = DotProduct(&targetVector, &curVector);

			// check if our angle causes a rotation (1 means no rotation)
			if (cosAngle < 0.99999) {
				// take the cross product of our vectors to determine rotation direction
				CrossProduct(&targetVector, &curVector, &crossResult);

				// rotate clockwise
				if (crossResult.z > 0.0f) {
					turnAngle = acos((float)cosAngle);
					turnDeg = RADTODEG(turnAngle);
					// damping
					if (mDamping && turnDeg > mLink[link].dampWidth)
						turnDeg = mLink[link].dampWidth;
					mLink[link].rot.z -= (float)turnDeg;  // turn the link
					// depth of field restrictions
					if (mDOFRestrict && mLink[link].rot.z < (float)mLink[link].minRz)
						mLink[link].rot.z = (float)mLink[link].minRz;
				}
				// rotate counter-clockwise
				else if (crossResult.z < 0.0f) {
					turnAngle = acos((float)cosAngle);
					turnDeg = RADTODEG(turnAngle);
					// damping
					if (mDamping && turnDeg < mLink[link].dampWidth)
						turnDeg = mLink[link].dampWidth;
					mLink[link].rot.z += (float)turnDeg;
					// depth of field restrictions
					if (mDOFRestrict && mLink[link].rot.z > (float)mLink[link].minRz)
						mLink[link].rot.z = (float)mLink[link].minRz;
				}
				// Make false to recalculate all matrices without drawing
				DrawScene(FALSE); // Change to true to view the iteration
			}
			if (--link < 0) link = EFFECTOR_POS - 1; // start of the chain, so restart
		}
	} while (tries++ < MAX_IK_TRIES && VectorSquaredDistance(&curEnd, &desiredEnd) > IK_POS_THRESH);
	// Quit once we're close enough or the loop has run long enough

	return TRUE;
}
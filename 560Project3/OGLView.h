// OGLView.h : header file
// Implements the OpenGL Window and Inverse Kinematic System
// Learned through Jeff Lander's tutorial and 
// demonstration of inverse kinematics on darwin3d.com

#ifndef OGLVIEW_H
#define OGLVIEW_H

#if _MSC_VER >= 1000
#pragma once
#endif

#include <gl/GL.h>
#include <gl/GLU.h>

#include "stdafx.h"
#include "Skeleton.h"

// COGLView windo
class COGLView : public CWnd {
	// ctor
public:
	COGLView();

	// attributes
public:
	CMFCStatusBar* mPtrStatusBar;
	HDC mHDC;
	HGLRC mHRC;
	CPoint mMousePos;
	float mGrabRotX, mGrabRotY, mGrabRotZ;
	float mGrabTransX, mGrabTransY, mGrabTransZ;
	BOOL mDrawGeometry;
	BOOL mUseQuat;
	BOOL mDamping;
	BOOL mDOFRestrict;

	// operations
public:
	BOOL SetupPixelFormat(HDC hdc);
	GLvoid DrawScene(BOOL actuallyDraw);
	VOID CreateBoneDLists(tBone* bone);
	GLvoid COGLView::LoadBoneTexture(tBone* curBone, char* name);
	GLvoid COGLView::DrawModel(tBone* curBone);
	GLvoid DrawSkeleton(tBone* rootBone, BOOL actuallyDraw);
	GLvoid InitializeGL(GLsizei width, GLsizei height);
	GLvoid Resize(GLsizei width, GLsizei height);
	void SetRestrictions();
	BOOL ComputeOneCCDLink(CPoint endPos, int link);
	BOOL ComputeCCDLink(CPoint endPos);
	void GetGLInfo();
	void UpdateStatus();
	void HandleKeyUp(UINT nChar);
	void HandleKeyDown(UINT nChar);

	// overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COGLView)
public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	//}}AFX_VIRTUAL

	// implementation
public:
	virtual ~COGLView();

	// Generated message map functions
protected:
	tBone mLink[6];
	float mModelScale;
	int mWidth, mHeight;
	//{{AFX_MSG(COGLView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif

// 560Project3View.h : interface of the CMy560Project3View class
//

#pragma once


class CMy560Project3View : public CView
{
protected: // create from serialization only
	CMy560Project3View() noexcept;
	DECLARE_DYNCREATE(CMy560Project3View)

// Attributes
public:
	CMy560Project3Doc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CMy560Project3View();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in 560Project3View.cpp
inline CMy560Project3Doc* CMy560Project3View::GetDocument() const
   { return reinterpret_cast<CMy560Project3Doc*>(m_pDocument); }
#endif



// 560Project3View.cpp : implementation of the CMy560Project3View class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "560Project3.h"
#endif

#include "560Project3Doc.h"
#include "560Project3View.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMy560Project3View

IMPLEMENT_DYNCREATE(CMy560Project3View, CView)

BEGIN_MESSAGE_MAP(CMy560Project3View, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CMy560Project3View::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

// CMy560Project3View construction/destruction

CMy560Project3View::CMy560Project3View() noexcept
{
	// TODO: add construction code here

}

CMy560Project3View::~CMy560Project3View()
{
}

BOOL CMy560Project3View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CMy560Project3View drawing

void CMy560Project3View::OnDraw(CDC* /*pDC*/)
{
	CMy560Project3Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
}


// CMy560Project3View printing


void CMy560Project3View::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CMy560Project3View::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CMy560Project3View::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CMy560Project3View::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void CMy560Project3View::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CMy560Project3View::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CMy560Project3View diagnostics

#ifdef _DEBUG
void CMy560Project3View::AssertValid() const
{
	CView::AssertValid();
}

void CMy560Project3View::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMy560Project3Doc* CMy560Project3View::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMy560Project3Doc)));
	return (CMy560Project3Doc*)m_pDocument;
}
#endif //_DEBUG


// CMy560Project3View message handlers

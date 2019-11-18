
// lab1View.cpp : implementation of the Clab1View class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "lab1.h"
#include <math.h>
#endif

#include "lab1Doc.h"
#include "lab1View.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#define RIGHT 3
#define LEFT 2

// Clab1View

IMPLEMENT_DYNCREATE(Clab1View, CView)

BEGIN_MESSAGE_MAP(Clab1View, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

// Clab1View construction/destruction

Clab1View::Clab1View() noexcept
{
	// TODO: add construction code here
	this->numberOfaxlePoint = 5;
	this->axlePoint = new SCPoint[numberOfaxlePoint];
	int distances[3] = { 250,190,30 };
	for (int i = 0; i < numberOfaxlePoint-2; i++) {
		axlePoint[i] = (i == 0)? SCPoint(300, 400, NULL, 0) : SCPoint(&axlePoint[i-1],0,distances[i-1]);
	}
	axlePoint[3] = SCPoint(&axlePoint[numberOfaxlePoint - 3], 0, distances[numberOfaxlePoint - 3]);
	axlePoint[4] = SCPoint(&axlePoint[numberOfaxlePoint - 3], 0, distances[numberOfaxlePoint - 3]);

}

Clab1View::~Clab1View()
{
}

BOOL Clab1View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

void Clab1View::OnDraw(CDC* pDC)
{
	
	for (int i = 0; i < numberOfaxlePoint; i++) {
		axlePoint[i].ReCalculate();
	}

	int prevMode = SetGraphicsMode(pDC->m_hDC, GM_ADVANCED);
	DWORD dw = GetLastError();
	XFORM oldXForm;
	GetWorldTransform(pDC->m_hDC, &oldXForm);

	DrawBase(pDC);
	DrawFirstArm(pDC);
	DrawSecondArm(pDC);


	SetWorldTransform(pDC->m_hDC, &oldXForm);
	SetGraphicsMode(pDC->m_hDC, prevMode);
}

void Clab1View::Rotate(CDC* pDC, double ang, int direction)
{
	XFORM lpXform;
	lpXform.eDx = 0;
	lpXform.eDy = 0;
	lpXform.eM11 = cos(ang * toRad);
	lpXform.eM12 = sin(ang * toRad);
	lpXform.eM21 = -sin(ang * toRad);
	lpXform.eM22 = cos(ang * toRad);
	pDC->ModifyWorldTransform(&lpXform, direction);
}

void Clab1View::Translate(CDC* pDC, double x, double y, int direction)
{
	XFORM lpXform;
	lpXform.eDx = x;
	lpXform.eDy = y;
	lpXform.eM11 = 1;
	lpXform.eM12 = 0;
	lpXform.eM21 = 0;
	lpXform.eM22 = 1;
	pDC->ModifyWorldTransform(&lpXform, direction);
}

void Clab1View::ResetXForm(CDC* pDC)
{
	pDC->ModifyWorldTransform(NULL, 1);
}

void Clab1View::DrawBase(CDC* pDC)
{
	CPoint point = axlePoint[0].toCpoint();
	Translate(pDC,point.x, point.y+36, LEFT);
	DrawBitmap(pDC, 310);
	ResetXForm(pDC);
}

void Clab1View::DrawFirstArm(CDC* pDC)
{
	CPoint lowerAxle = axlePoint[0].toCpoint();
	CPoint UpperAxle = axlePoint[1].toCpoint();
	int x = lowerAxle.x + (UpperAxle.x - lowerAxle.x) / 2;
	int y = UpperAxle.y + (lowerAxle.y - UpperAxle.y) / 2;
	Translate(pDC, x, y, LEFT);
	Rotate(pDC, 90 - axlePoint[1].angle, LEFT);
	DrawBitmap(pDC, 312);
	ResetXForm(pDC);
}

void Clab1View::DrawSecondArm(CDC* pDC)
{
	CPoint lowerAxle = axlePoint[1].toCpoint();
	CPoint UpperAxle = axlePoint[2].toCpoint();
	int x = lowerAxle.x + (UpperAxle.x - lowerAxle.x) / 2;
	int y = UpperAxle.y + (lowerAxle.y - UpperAxle.y) / 2;
	Translate(pDC, x, y, LEFT);
	Rotate(pDC,90-axlePoint[2].angle, LEFT);
	DrawBitmap(pDC, 313);
	ResetXForm(pDC);
}

void Clab1View::DrawArm(CDC* pDC, CPoint bottomPoint, CPoint topPoint, PolygonConfig firstPolygonConfig, TrapezConfig trapezConfig, PolygonConfig secundPolygonConfig)
{
	int orientation = acosl((topPoint.x - topPoint.x) / Distance(topPoint, bottomPoint)) / toRad;
	this->DrawTrapez(pDC, bottomPoint, topPoint, trapezConfig.bottomWidth, trapezConfig.topWidth,trapezConfig.pen,trapezConfig.brush);
	this->DrawPoligonWithInnerPoligon(pDC, bottomPoint, firstPolygonConfig.numberOfPoints, firstPolygonConfig.size, orientation - 360/firstPolygonConfig.numberOfPoints,firstPolygonConfig.pen, firstPolygonConfig.brush);
	this->DrawPoligonWithInnerPoligon(pDC, topPoint, secundPolygonConfig.numberOfPoints, secundPolygonConfig.size, orientation - 360/secundPolygonConfig.numberOfPoints, secundPolygonConfig.pen, secundPolygonConfig.brush);
}

void Clab1View::DrawCircle(CDC * pDC, CPoint center, int radius)
{
	/*COLORREF circleColor = RGB(255, 0, 0);
	CPen CrclePen(PS_SOLID, 7, circleColor);
	CPen *oldPen = pDC->SelectObject(&CrclePen);

	COLORREF circleFillColor = RGB(0, 255, 0);
	CBrush circleBrush(BS_3STATE , circleFillColor);
	CBrush *oldBrush = pDC->SelectObject(&circleBrush);*/

	int x1 = center.x - radius;
	int y1 = center.y - radius;

	int x2 = center.x + radius;
	int y2 = center.y + radius;

	pDC->Ellipse(x1, y1, x2, y2);

	/*pDC->SelectObject(oldBrush);
	pDC->SelectObject(oldPen);*/
}

void Clab1View::DrawHalfCircle(CDC* pDC, CPoint center, int radius, int angle)
{
	int x1 = center.x - radius;
	int y1 = center.y - radius;

	int x2 = center.x + radius;
	int y2 = center.y + radius;

	CRect rect(x1,y1,x2,y2);

	int leftX = center.x + sin(angle * toRad) * radius;
	int leftY = center.y - cos(angle * toRad) * radius;

	int rightX = center.x - sin(angle * toRad) * radius;
	int rightY = center.y + cos(angle * toRad) * radius;

	pDC->ArcTo(rect, CPoint(leftX, leftY), CPoint(rightX, rightY));
}

void Clab1View::DrawLine(CDC * pDC, CPoint p1, CPoint p2)
{
	COLORREF penColor = RGB(0, 0, 255);
	CPen newPen(PS_SOLID, 7, penColor);
	CPen *oldPen = pDC->SelectObject(&newPen);


	pDC->MoveTo(p1);
	pDC->LineTo(p2);

	pDC->SelectObject(&oldPen);
}

void Clab1View::DrawPoligon(CDC * pDC, CPoint center, int numOfPoints,int size, int orientation, CPen* pen, CBrush* brush)
{
	//double toRad = 3.14159265358979323846 / 180;
	CPoint *points = new CPoint[numOfPoints];
	int i = 0;
	for (int fi = 0; fi <= 360; fi += 360 / numOfPoints) {
		double cosin = cos((fi + orientation) * toRad);
		double sinus = sin((fi + orientation) * toRad);
		points[i].x = center.x + cosin * size;
		points[i].y = center.y + sinus *size;
		i++;
	}

	//CPen newPen(PS_SOLID,3,penColor);
	CPen *oldPen = pDC->SelectObject(pen);

	//CBrush polyBrush(brushStyle,backColor);
	CBrush *oldBrush = pDC->SelectObject(brush);
	pDC->Polygon(points, numOfPoints);

	pDC->SelectObject(oldPen);
	pDC->SelectObject(oldBrush);

}

void Clab1View::DrawPoligonWithInnerPoligon(CDC * pDC, CPoint center, int numOfPoints, int size, int orientation, CPen * pen,CBrush *brush)
{
	this->DrawPoligon(pDC, center, numOfPoints, size, orientation,pen,brush);
	this->DrawPoligon(pDC, center, numOfPoints, size / 3, orientation, pen, brush);
}

void Clab1View::DrawClaw(CDC *pDC,CPoint basePoint, CPoint topClawCenterPoint, CPoint bottomClawCenterPoint, ClawConfig config)
{
	int topClawRadius = this->Distance(basePoint,topClawCenterPoint);
	double sinValue = ((double)(basePoint.y - topClawCenterPoint.y)) / topClawRadius;
	int topClawAngle = asin(sinValue) / toRad;
	this->DrawHalfClaw(pDC, config.pen, config.brush, config.numberOfPoints, topClawCenterPoint.x, topClawCenterPoint.y,topClawRadius, config.width, topClawAngle);

	int bottomClawRadius = this->Distance(basePoint,bottomClawCenterPoint);
	sinValue = ((double)(basePoint.y - bottomClawCenterPoint.y)) / bottomClawRadius;
	int bottomClawAngle = asinl(sinValue) / toRad - 180;
	this->DrawHalfClaw(pDC, config.pen, config.brush, config.numberOfPoints, bottomClawCenterPoint.x, bottomClawCenterPoint.y,bottomClawRadius, config.width, bottomClawAngle);
}

//void Clab1View::DrawTrapez(CDC * pDC,CPoint center ,int height, int baseWidth, int topWidth , COLORREF backColor,COLORREF penColor, bool isVertical)
//{
//	CPoint *points = new CPoint[4];
//	if (isVertical) {
//		points[0].y = points[3].y = center.y - height / 2;
//		points[1].y = points[2].y = center.y + height / 2;
//
//		points[0].x = center.x - topWidth / 2;
//		points[3].x = center.x + topWidth / 2;
//
//		points[1].x = center.x - baseWidth / 2;
//		points[2].x = center.x + baseWidth / 2;
//	}
//	else
//	{
//		points[0].x = points[1].x = center.x - height / 2;
//		points[2].x = points[3].x = center.x + height / 2;
//
//		points[0].y = center.y - baseWidth / 2;
//		points[1].y = center.y + baseWidth / 2;
//
//		points[3].y = center.y - topWidth / 2;
//		points[2].y = center.y + topWidth / 2;
//	}
//	
//	CPen newPen(PS_SOLID, 6, penColor);
//	CPen *oldPen = pDC->SelectObject(&newPen);
//
//	CBrush polyBrush(backColor);
//	CBrush *oldBrush = pDC->SelectObject(&polyBrush);
//	pDC->Polygon(points, 4);
//
//	pDC->SelectObject(oldBrush);
//	pDC->SelectObject(oldPen);
//}

void Clab1View::DrawTrapez(CDC* pDC, CPoint bottomPoint, CPoint topPoint, int bottmWith, int topWith, CPen *pen, CBrush *brush)
{
	double distance = Distance(topPoint, bottomPoint);
	double angle = acosl((topPoint.x - bottomPoint.x) / distance);

	CPoint* points = new CPoint[4];

	double temp = sin(angle * toRad);

	points[0].x = bottomPoint.x + sin(angle) * bottmWith / 2;
	points[0].y = bottomPoint.y - cos(angle) * bottmWith / 2;

	points[1].x = bottomPoint.x - sin(angle) * bottmWith / 2;
	points[1].y = bottomPoint.y + cos(angle) * bottmWith / 2;

	points[2].x = topPoint.x - sin(angle) * topWith / 2;
	points[2].y = topPoint.y + cos(angle) * topWith / 2;

	points[3].x = topPoint.x + sin(angle) * topWith / 2;
	points[3].y = topPoint.y - cos(angle) * topWith / 2;

	//CPen newPen(PS_SOLID, 6, RGB(255,0,0));
	CPen *oldPen = pDC->SelectObject(pen);
	
	//CBrush polyBrush(RGB(255, 0, 0));
	CBrush *oldBrush = pDC->SelectObject(brush);
	pDC->Polygon(points, 4);
	
	pDC->SelectObject(oldBrush);
	pDC->SelectObject(oldPen);
}

void Clab1View::DrawHalfClaw(CDC* pDC, CPen *pen, CBrush *brush, int numberOfPoints, int p, int q, int r, int debljina, int ugao) {

	//CPen pen(PS_SOLID, 1, boja1);
	CPen* oldpen = pDC->SelectObject(pen);

	//CBrush brush(boja2);
	CBrush* oldbrush = pDC->SelectObject(brush);

	POINT* mnogougao = new POINT[numberOfPoints];

	for (int i = 0; i < numberOfPoints / 2; i++) {
		double angle = (i * 180 / ((numberOfPoints / 2) - 1)) + ugao;
		double cosin = cos(angle * toRad);
		double sinus = sin(angle * toRad);

		mnogougao[i].x = p + cosin * r;
		mnogougao[i].y = q - sinus * r;
		mnogougao[numberOfPoints - i - 1].x = p + cosin * (r - debljina);
		mnogougao[numberOfPoints - i - 1].y = q - sinus * (r - debljina);
	}
	pDC->Polygon(mnogougao, numberOfPoints);

	//	CrtajKrug((mnogougao[n/2].x+ mnogougao[n/2-1].x)/2, (mnogougao[n/2].y + mnogougao[n/2-1].y) / 2,debljina,boja1,boja2,pDC);
	pDC->SelectObject(oldbrush);
	pDC->SelectObject(oldpen);
}

void Clab1View::DrawBottemRightShape(CDC * pDC, CPoint center, int height, int baseWidth, COLORREF backColor, COLORREF penColor)
{

	

	CRgn topShape, midShape, botShape, comRgn, comRgn2;

	CPoint *topPoly = new CPoint[3];
	topPoly[0].x = center.x;
	topPoly[0].y = center.y - height;
	topPoly[1].x = center.x - baseWidth / 4;
	topPoly[1].y = center.y - 2 * height / 3;
	topPoly[2].x = center.x + baseWidth / 4;
	topPoly[2].y = center.y - 2 * height / 3;
	topShape.CreatePolygonRgn(topPoly, 3, ALTERNATE);

	midShape.CreateRectRgn(topPoly[1].x, topPoly[1].y, topPoly[2].x, topPoly[2].y + height / 3);
	comRgn.CreateRectRgn(0, 0, 0, 0);
	comRgn.CombineRgn(&topShape, &midShape, RGN_OR);


	CPoint *botPoly = new CPoint[4];
	botPoly[0].x = center.x - baseWidth / 4;
	botPoly[0].y = center.y - height /3;

	botPoly[1].x = center.x - baseWidth / 2;
	botPoly[1].y = center.y;

	botPoly[2].x = center.x + baseWidth / 2;
	botPoly[2].y = center.y;

	botPoly[3].x = topPoly[2].x;
	botPoly[3].y = topPoly[2].y + height / 3;

	botShape.CreatePolygonRgn(botPoly, 4, ALTERNATE);

	comRgn2.CreateRectRgn(0, 0, 0, 0);
	comRgn2.CombineRgn(&comRgn, &botShape, RGN_OR);

	CPen newPen(PS_SOLID, 6, penColor);
	CPen *oldPen = pDC->SelectObject(&newPen);

	CBrush polyBrush(backColor);
	CBrush *oldBrush = pDC->SelectObject(&polyBrush);
	
	CBrush borderBrush(BS_AUTO3STATE, backColor);
	CBrush fillBrush(BS_AUTO3STATE,penColor);

	//pDC->SelectClipRgn(&comRgn2);

	pDC->FillRgn(&comRgn2, &fillBrush);
	pDC->FrameRgn(&comRgn2, &borderBrush, 3, 3);

	CBrush newB(RGB(232, 35, 202));
	pDC->SelectObject(newB);
	pDC->Rectangle(center.x - baseWidth / 2 - 10, center.y, center.x + baseWidth / 2 + 10, center.y + 50);

	pDC->SelectObject(oldBrush);
	pDC->SelectObject(oldPen);
}

void Clab1View::DrawFromMetaFile(CDC* pDC, CString filePath)
{
	HENHMETAFILE file = GetEnhMetaFile(filePath.GetBuffer(filePath.GetLength()));
	pDC->MoveTo(500, 500);
	pDC->PlayMetaFile(file,CRect(0,0,1000,1000));
	DeleteEnhMetaFile(file);
}

void Clab1View::DrawFlower(CDC* pDC, CPoint center, int size)
{
	COLORREF circleColor = RGB(255, 0, 0);
	CPen CrclePen(PS_SOLID, 7, circleColor);
	CPen *oldPen = pDC->SelectObject(&CrclePen);

	COLORREF circleFillColor = RGB(0, 255, 0);
	CBrush circleBrush(BS_3STATE , circleFillColor);
	CBrush *oldBrush = pDC->SelectObject(&circleBrush);

	int halfSize = size / 2;
	int thiredSize = size / 3;
	pDC->BeginPath();
	int i = 0;
	for (int i = 0; i < 4; i++) {
		int newX = center.x + sin(90 * i * toRad) * size;
		int newY = center.y - cos(90 * i * toRad) * size;
		this->DrawHalfCircle(pDC, CPoint(newX, newY), size, i*90 +90);
	}
	pDC->EndPath();
	pDC->StrokePath();
	pDC->FillPath();
	this->DrawCircle(pDC, center, size);

	pDC->SelectObject(oldPen);
	pDC->SelectObject(oldBrush);
}

double Clab1View::Distance(CPoint Point1, CPoint point2)
{
	return sqrt(pow((point2.x-Point1.x),2) + pow((point2.y - Point1.y), 2));
}

void Clab1View::DrawBitmap(CDC *pDC ,int resurceId)
{
	//CBitmap bitmap;
	//bitmap.LoadBitmapW(resurceId);
	//BITMAP properties;
	//bitmap.GetBitmap(&properties);
	//CDC *newDC = new CDC();
	//newDC->CreateCompatibleDC(pDC);
	//newDC->SelectObject(bitmap);

	//pDC->BitBlt(-(properties.bmWidth / 2), -(properties.bmHeight / 2), properties.bmWidth, properties.bmHeight, newDC, 0, 0, SRCCOPY);

	//newDC->DeleteDC();
	//delete newDC;
	//bitmap.DeleteObject();

	CBitmap imageMap;
	imageMap.LoadBitmapW(resurceId);
	BITMAP imageProp;
	imageMap.GetBitmap(&imageProp);

	CBitmap mask;
	mask.CreateBitmap(imageProp.bmWidth, imageProp.bmHeight, 1, 1, NULL);

	CDC* srcDC = new CDC();
	srcDC->CreateCompatibleDC(NULL);
	CDC* dstDC = new CDC();
	dstDC->CreateCompatibleDC(NULL);

	CBitmap *oldSrcBtm = srcDC->SelectObject(&imageMap);
	CBitmap *oldDstBtm = dstDC->SelectObject(&mask);

	srcDC->SetBkColor(srcDC->GetPixel(0, 0));

	dstDC->BitBlt(0, 0, imageProp.bmWidth, imageProp.bmHeight, srcDC, 0, 0, SRCCOPY);
	srcDC->SetTextColor(RGB(255, 255, 255));
	srcDC->SetBkColor(RGB(0, 0, 0));
	srcDC->BitBlt(0, 0, imageProp.bmWidth, imageProp.bmHeight, dstDC, 0, 0, SRCAND);


	srcDC->SelectObject(oldSrcBtm);
	dstDC->SelectObject(oldDstBtm);
	srcDC->DeleteDC();
	delete srcDC;
	dstDC->DeleteDC();
	delete dstDC;

	CDC* MemDC = new CDC();
	MemDC->CreateCompatibleDC(NULL);
	MemDC->SelectObject(&mask);

	pDC->BitBlt(-(imageProp.bmWidth / 2), -(imageProp.bmHeight / 2), imageProp.bmWidth, imageProp.bmHeight, MemDC, 0, 0, SRCAND);

	MemDC->SelectObject(&imageMap);
	pDC->BitBlt(-(imageProp.bmWidth / 2), -(imageProp.bmHeight / 2), imageProp.bmWidth, imageProp.bmHeight, MemDC, 0, 0, SRCPAINT);
	MemDC->DeleteDC();
	delete MemDC;
}


// Clab1View printing

BOOL Clab1View::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void Clab1View::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void Clab1View::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// Clab1View diagnostics

#ifdef _DEBUG
void Clab1View::AssertValid() const
{
	CView::AssertValid();
}

void Clab1View::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

Clab1Doc* Clab1View::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(Clab1Doc)));
	return (Clab1Doc*)m_pDocument;
}
#endif //_DEBUG


// Clab1View message handlers


void Clab1View::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default

	switch (nChar)
	{
	case 68: // code to be executed if n = 1;
		this->axlePoint[1].angle -= 10;
		Invalidate();
		RedrawWindow();
		break;
	case 65: 
		this->axlePoint[1].angle += 10;
		Invalidate();
		RedrawWindow();
		break;
	case 87:
		this->axlePoint[2].angle += 10;
		this->axlePoint[3].angle += 10;
		this->axlePoint[4].angle += 10;
		Invalidate();
		RedrawWindow();
		break;
	case 83:
		this->axlePoint[2].angle -= 10;
		this->axlePoint[3].angle -= 10;
		this->axlePoint[4].angle -= 10;
		Invalidate();
		RedrawWindow();
		break;
	case 74:
		this->axlePoint[3].angle += 10;
		this->axlePoint[4].angle -= 10;
		Invalidate();
		RedrawWindow();
		break;
	case 75:
		this->axlePoint[3].angle -= 10;
		this->axlePoint[4].angle += 10;
		Invalidate();
		RedrawWindow();
		break;
	default: 
		;
	}

	/*if (nChar == 68) {
		this->axlePoint[1].angle += 10;
		Invalidate();
		RedrawWindow();
		
		
	}*/

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}


void Clab1View::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	/*CDC *pDC = GetDC();
	this->DrawCircle(pDC, point, CPoint(point.x + 300, point.y + 300));
	this->DrawLine(pDC, point, CPoint(point.x + 300, point.y + 300));*/
	CView::OnLButtonDown(nFlags, point);

}


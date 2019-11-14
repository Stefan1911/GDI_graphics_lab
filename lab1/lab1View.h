
// lab1View.h : interface of the Clab1View class
//

#pragma once



struct TrapezConfig {
	CPen *pen;
	CBrush *brush;
	int bottomWidth, topWidth;

	TrapezConfig(CPen *pen, CBrush *brush,int bottomWidth,int topWidth) {
		this->pen = pen;
		this->brush = brush;
		this->bottomWidth = bottomWidth;
		this->topWidth = topWidth;
	}
};

struct ClawConfig
{
	CPen* pen;
	CBrush* brush;
	int width;
	int numberOfPoints;

	ClawConfig(CPen* pen, CBrush* brush, int width,int numberOfPoints) {
		this->pen = pen;
		this->brush = brush;
		this->width= width;
		this->numberOfPoints = numberOfPoints;
	}
};
struct PolygonConfig {
	CPen* pen;
	CBrush* brush;
	int numberOfPoints;
	int size;
	int orientation;
	PolygonConfig(CPen* pen, CBrush* brush, int numberOfPoints,int size,int orientation) {
		this->pen = pen;
		this->brush = brush;
		this->numberOfPoints = numberOfPoints;
		this->size = size;
		this->orientation = orientation;
	}
};

class SCPoint : CPoint
{
public:

	double toRad = 3.14159265358979323846 / 180;

	int angle;
	int distance;
	SCPoint* referencePoint;
		
	SCPoint() {};

	SCPoint(int x, int y, SCPoint *referencePoint, int angle = 0) : CPoint(x, y)
	{
		this->angle = angle;
		this->referencePoint = referencePoint;
		this->distance = (referencePoint)?this->DistanceTo(*referencePoint):0;
	};

	SCPoint(SCPoint* referencePoint, int angle, int distance) : CPoint(referencePoint->x+cos(angle*toRad)*distance, referencePoint->y - sin(angle * toRad) * distance) {
		this->angle = angle;
		this->distance = distance;
		this->referencePoint = referencePoint;
	}

	void ReCalculate()
	{
		if (this->referencePoint != NULL) {
			double temp2 = cos(angle * toRad);
			this->x = referencePoint->x + cos(angle * toRad) * this->distance;
			this->y = referencePoint->y - sin(angle * toRad) * this->distance;
		}
	}

	double DistanceTo(CPoint toPoint)
	{
		return sqrt(pow((toPoint.x - this->x), 2) + pow((toPoint.y - this->y), 2));
	}

	CPoint toCpoint() {
		return CPoint(this->x, this->y);
	}

private:

};

class Clab1View : public CView
{
protected: // create from serialization only
	Clab1View() noexcept;
	DECLARE_DYNCREATE(Clab1View)
// Attributes
public:
	Clab1Doc* GetDocument() const;

// Operations
public:
	double toRad = 3.14159265358979323846 / 180;

	int numberOfaxlePoint;
	SCPoint* axlePoint;
	int globalAngle;
	int globalXOffset;
	int globalYOffset;

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
	virtual ~Clab1View();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);


public:
	void DrawArm(CDC* pDC, CPoint bottomPoin, CPoint topPoint, PolygonConfig firstPolygonConfig, TrapezConfig trapezConfig, PolygonConfig secundPolygonConfig);
	void DrawCircle(CDC * pDC, CPoint center,int radius);
	void DrawHalfCircle(CDC* pDC, CPoint center, int radius, int angle);
	void DrawLine(CDC * pDC, CPoint p1, CPoint p2);
	void DrawPoligon(CDC *pDC, CPoint center, int numOfPoints,int size, int orientation, CPen* pen, CBrush* brush);
	void DrawPoligonWithInnerPoligon(CDC *pDC, CPoint center, int numOfPoints, int size, int orientation, CPen * pen,CBrush *brush);
	void DrawClaw(CDC *pDC,CPoint basePoint,CPoint topClawCenterPoint, CPoint bottomClawCenterPoint,ClawConfig config);
	void DrawHalfClaw(CDC* pDC, CPen* pen, CBrush* brush, int numberOfPoints, int p, int q, int r, int debljina, int ugao);
	void DrawTrapez(CDC* pDC, CPoint bottomPoint, CPoint topPoint, int bottmWith, int topWith, CPen *pen, CBrush *brush);
	void DrawBottemRightShape(CDC *pDC, CPoint center, int height, int baseWidth, COLORREF backColor, COLORREF penColor);
	void DrawFromMetaFile(CDC* pDC, CString filePath);
	void DrawFlower(CDC* pDC, CPoint center, int size);
	double Distance(CPoint Point1, CPoint point2);
};

#ifndef _DEBUG  // debug version in lab1View.cpp
inline Clab1Doc* Clab1View::GetDocument() const
   { return reinterpret_cast<Clab1Doc*>(m_pDocument); }
#endif


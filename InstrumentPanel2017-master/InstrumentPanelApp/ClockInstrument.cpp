#include "stdafx.h"
#include "ClockInstrument.h"

/*
ID2D1PathGeometry* m_pClockCard;
ID2D1PathGeometry* m_pClockGraticule;

ID2D1PathGeometry* m_pClockHoursPointer;
ID2D1PathGeometry* m_pClockMinutesPointer;
ID2D1PathGeometry* m_pClockSecondsPointer;


private:
	HRESULT CreateClock(CInstrumentPanelContext* pContext);
	HRESULT CreateClockGraticule(CInstrumentPanelContext* pContext);
	HRESULT CreateClockCard(CInstrumentPanelContext* pContext);
	HRESULT CreateClockPointers(CInstrumentPanelContext* pContext);

	float m_hourspointerangle;
	float m_minutespointerangle;
	float m_secondspointerangle;
*/


CClockInstrument::CClockInstrument()
	:CInstrument(0.6f,CLOCKINSTRUMENT_ID)
{
	m_pInstrumentBrush = NULL;
	m_pClockCard = NULL;
	m_pClockGraticule = NULL;
	m_pClockHoursPointer = NULL;
	m_pClockMinutesPointer = NULL;
	m_pClockSecondsPointer = NULL;

	m_hourspointerangle = 0.0f;
	m_minutespointerangle = 0.0f;
	m_secondspointerangle = 0.0f;

	m_xCaseOffset = (XCASE_OFFSET * m_instrumentScale);
	m_yCaseOffset = (YCASE_OFFSET * m_instrumentScale);
}

CClockInstrument::CClockInstrument(float scale)
	: CInstrument(scale, CLOCKINSTRUMENT_ID)
{
	m_pInstrumentBrush = NULL;
	m_pClockCard = NULL;
	m_pClockGraticule = NULL;
	m_pClockHoursPointer = NULL;
	m_pClockMinutesPointer = NULL;
	m_pClockSecondsPointer = NULL;

	m_hourspointerangle = 0.0f;
	m_minutespointerangle = 0.0f;
	m_secondspointerangle = 0.0f;

	m_xCaseOffset = (XCASE_OFFSET * m_instrumentScale);
	m_yCaseOffset = (YCASE_OFFSET * m_instrumentScale);
}



CClockInstrument::~CClockInstrument()
{
}

void CClockInstrument::GetBounds(RECT* rect)
{
	if (rect != NULL)
	{
		float amplitude = GRATICULE_LENGTH * m_instrumentScale;
		rect->left = (LONG)(m_xPosition - amplitude);
		rect->right = (LONG)(m_xPosition + amplitude);
		rect->top = (LONG)(m_yPosition - amplitude);
		rect->bottom = (LONG)(m_yPosition + amplitude);
	}
}

bool CClockInstrument::PtInBounds(POINT pt)
{
	RECT r;
	GetBounds(&r);
	bool x = (pt.x >= r.left) && (pt.x < r.right);
	bool y = (pt.y >= r.top) && (pt.y < r.bottom);
	return (x == true && y == true) ? true : false;
}



HRESULT CClockInstrument::Initialise(CInstrumentPanelContext* pContext)
{
	HRESULT hr = CInstrument::Initialise(pContext);
	if (SUCCEEDED(hr))
			hr = pContext->m_pRenderTarget->CreateSolidColorBrush(GetInstrumentColor(), &m_pInstrumentBrush);

	if (SUCCEEDED(hr))
		hr = pContext->m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF(0xFF0000)), &m_pRedBrush);

	if (SUCCEEDED(hr))
		hr = pContext->m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF(0x00FF00)), &m_pGreenBrush);

	return CreateClock(pContext);
}

HRESULT CClockInstrument::CreateClock(CInstrumentPanelContext* pContext)
{
	HRESULT hr = CreateClockGraticule(pContext);
	if (SUCCEEDED(hr))
	{
		hr = CreateClockCard(pContext);

		if (SUCCEEDED(hr))
			hr = CreateClockPointers(pContext);
	}

	return hr;
}

HRESULT CClockInstrument::CreateClockGraticule(CInstrumentPanelContext* pContext)
{
	ID2D1GeometrySink *pSink = NULL;
	ID2D1PathGeometry* tempPath;
	HRESULT hr = pContext->m_pDirect2dFactory->CreatePathGeometry(&m_pClockGraticule);
	if (SUCCEEDED(hr))
	{
		hr = pContext->m_pDirect2dFactory->CreatePathGeometry(&tempPath);
	}
	if (SUCCEEDED(hr))
	{
		hr = tempPath->Open(&pSink);
	}
	if (SUCCEEDED(hr))
	{
		/* 2 degree divisions
		for (double angle = 2.0; angle <= 360.0; angle += 2.0)
		{
		if (angle < 120.f || angle > 240.f)
		{
		if (fmod(angle, 10.0) != 0.0)
		{

		float x1 = 0, x2 = 0, y1 = 0, y2 = -150; //0 degrees at top
		RotateLine(x1, y1, x2, y2, angle);
		pSink->BeginFigure(D2D1::Point2F(x1, y1), D2D1_FIGURE_BEGIN_HOLLOW);
		pSink->AddLine(D2D1::Point2F(x2, y2));
		pSink->EndFigure(D2D1_FIGURE_END_OPEN);
		}
		}
		}

		for (double angle = 4.0; angle <= 360.0; angle += 4.0)
		{
		//if (angle < 120.f || angle > 240.f)
		//{
		if (fmod(angle, 40.0) != 0.0)
		{

		float x1 = 0, x2 = 0, y1 = 0, y2 = -150; //0 degrees at top
		RotateLine(x1, y1, x2, y2, angle);
		pSink->BeginFigure(D2D1::Point2F(x1, y1), D2D1_FIGURE_BEGIN_HOLLOW);
		pSink->AddLine(D2D1::Point2F(x2, y2));
		pSink->EndFigure(D2D1_FIGURE_END_OPEN);
		}
		//}
		}*/

		/* 3.6 degree */
		for (double angle = 6.0; angle <= 360.0; angle += 6.0)
		{
			//if (angle < 120.f || angle > 240.f)
			//{
			//if ((angle > (324.f + 1.0f) && angle < (360.f)) || (angle > 0 && angle < (36.f)))

			//	//if ((angle > (288.f + 1.0f) && angle < (360.f)) || (angle > 0 && angle < (36.f)))
			//	continue;

			float x1 = 0, x2 = 0, y1 = 0, y2 = (GRATICULE_LENGTH * m_instrumentScale);

			//float x1 = 0, x2 = 0, y1 = 0, y2 = -150 * m_instrumentScale; //0 degrees at top
			RotateLine(x1, y1, x2, y2, angle);
			pSink->BeginFigure(D2D1::Point2F(x1, y1), D2D1_FIGURE_BEGIN_HOLLOW);
			pSink->AddLine(D2D1::Point2F(x2, y2));
			pSink->EndFigure(D2D1_FIGURE_END_OPEN);

			//}
		}

		hr = pSink->Close();
		SafeRelease(&pSink);
	}
	ID2D1GeometrySink* pSinkClip;
	if (SUCCEEDED(hr))
	{
		hr = m_pClockGraticule->Open(&pSinkClip);
	}

	D2D1_ELLIPSE clip;
	ID2D1EllipseGeometry* clipEllipse;
	if (SUCCEEDED(hr))
	{
		//Create a clip ellipse.
		clip = D2D1::Ellipse(
			D2D1::Point2F(0.0f, 0.0f),
			(GRATICULE_OUTER_CLIP)*m_instrumentScale,
			(GRATICULE_OUTER_CLIP)*m_instrumentScale
		);
		hr = pContext->m_pDirect2dFactory->CreateEllipseGeometry(&clip, &clipEllipse);
	}
	if (SUCCEEDED(hr))
	{
		//There's no direct support for clipping path in Direct2D. So we can intersect a path with its clip instead.
		hr = tempPath->CombineWithGeometry(clipEllipse, D2D1_COMBINE_MODE_EXCLUDE /*D2D1_COMBINE_MODE_INTERSECT*/, NULL, 0, pSinkClip);
	}
	if (SUCCEEDED(hr))
	{
		hr = pSinkClip->Close();
		SafeRelease(&pSinkClip);
		SafeRelease(&tempPath);
		SafeRelease(&clipEllipse);
	}
	return hr;
}

HRESULT CClockInstrument::CreateClockCard(CInstrumentPanelContext* pContext)
{
	ID2D1GeometrySink *pSink = NULL;
	ID2D1PathGeometry* tempPath;
	HRESULT hr = pContext->m_pDirect2dFactory->CreatePathGeometry(&m_pClockCard);
	if (SUCCEEDED(hr))
	{
		hr = pContext->m_pDirect2dFactory->CreatePathGeometry(&tempPath);
	}
	if (SUCCEEDED(hr))
	{
		hr = tempPath->Open(&pSink);
	}
	if (SUCCEEDED(hr))
	{
		/* 2 degree divsions

		for (double angle = 10.0; angle <= 360.0; angle += 10.0)
		{
		if (angle < (120.f+1.0f) || angle > (240.f-1.0f))
		{
		float x1 = 0, x2 = 0, y1 = 0, y2 = -150; //0 degs at top
		RotateLine(x1, y1, x2, y2, angle);
		pSink->BeginFigure(D2D1::Point2F(x1, y1), D2D1_FIGURE_BEGIN_HOLLOW);
		pSink->AddLine(D2D1::Point2F(x2, y2));
		pSink->EndFigure(D2D1_FIGURE_END_OPEN);
		}
		}


		for (double angle = 40.0; angle <= 360.0; angle += 40.0)
		{
		//if (angle < (120.f + 1.0f) || angle >(240.f - 1.0f))
		//{
		float x1 = 0, x2 = 0, y1 = 0, y2 = -150; //0 degs at top
		RotateLine(x1, y1, x2, y2, angle);
		pSink->BeginFigure(D2D1::Point2F(x1, y1), D2D1_FIGURE_BEGIN_HOLLOW);
		pSink->AddLine(D2D1::Point2F(x2, y2));
		pSink->EndFigure(D2D1_FIGURE_END_OPEN);
		//}
		}*/

		for (double angle = 0.0; angle <= 360.0; angle += 30.0)
		{
			//if ((angle > (324.f + 1.0f) && angle <= (360.f)) || (angle > 0 && angle < (36.0f)))

				//			if ((angle > (288.f + 1.0f) && angle <= (360.f)) || (angle > 0 && angle < (36.0f)))
				//continue;
			float x1 = 0, x2 = 0, y1 = 0, y2 = (GRATICULE_LENGTH * m_instrumentScale);
			RotateLine(x1, y1, x2, y2, angle);
			pSink->BeginFigure(D2D1::Point2F(x1, y1), D2D1_FIGURE_BEGIN_HOLLOW);
			pSink->AddLine(D2D1::Point2F(x2, y2));
			pSink->EndFigure(D2D1_FIGURE_END_OPEN);

		}
		hr = pSink->Close();
		SafeRelease(&pSink);
	}
	ID2D1GeometrySink* pSinkClip;
	if (SUCCEEDED(hr))
	{
		hr = m_pClockCard->Open(&pSinkClip);
	}

	D2D1_ELLIPSE clip;
	ID2D1EllipseGeometry* clipEllipse;
	if (SUCCEEDED(hr))
	{
		//Create a clip ellipse.
		clip = D2D1::Ellipse(
			D2D1::Point2F(0.0f, 0.0f),
			GRATICULE_INNER_CLIP*m_instrumentScale,
			GRATICULE_INNER_CLIP*m_instrumentScale
		);
		hr = pContext->m_pDirect2dFactory->CreateEllipseGeometry(&clip, &clipEllipse);
	}
	if (SUCCEEDED(hr))
	{
		//There's no direct support for clipping path in Direct2D. So we can intersect a path with its clip instead.
		hr = tempPath->CombineWithGeometry(clipEllipse, D2D1_COMBINE_MODE_EXCLUDE /*D2D1_COMBINE_MODE_INTERSECT*/, NULL, 0, pSinkClip);
	}
	if (SUCCEEDED(hr))
	{
		hr = pSinkClip->Close();
		SafeRelease(&pSinkClip);
		SafeRelease(&tempPath);
		SafeRelease(&clipEllipse);
	}
	return hr;
}

HRESULT CClockInstrument::CreateClockPointers(CInstrumentPanelContext* pContext)
{
	HRESULT hr = S_OK;
	ID2D1GeometrySink *pSink = NULL;
	hr = pContext->m_pDirect2dFactory->CreatePathGeometry(&this->m_pClockSecondsPointer);
	if (SUCCEEDED(hr))
	{

		hr = m_pClockSecondsPointer->Open(&pSink);
	}
	if (SUCCEEDED(hr))
	{
		pSink->BeginFigure(
			D2D1::Point2F((POINTER_EIGHTH_WIDTH*-1.0f) * m_instrumentScale, ((POINTER_OVERRUN*-1.0f) * m_instrumentScale)),
			D2D1_FIGURE_BEGIN_FILLED
		);
		pSink->AddLine(D2D1::Point2F(((POINTER_EIGHTH_WIDTH*-1.0f) * m_instrumentScale), -(POINTER_LENGTH_ANGLE * m_instrumentScale)));
		pSink->AddLine(D2D1::Point2F(0, -(POINTER_LENGTH * m_instrumentScale)) );
		pSink->AddLine(D2D1::Point2F(POINTER_EIGHTH_WIDTH * m_instrumentScale, -(POINTER_LENGTH_ANGLE * m_instrumentScale)));
		pSink->AddLine(D2D1::Point2F(POINTER_EIGHTH_WIDTH * m_instrumentScale, ((POINTER_OVERRUN*-1.0f) * m_instrumentScale)));
		pSink->EndFigure(D2D1_FIGURE_END_CLOSED);
		hr = pSink->Close();
	}
	SafeRelease(&pSink);


	hr = pContext->m_pDirect2dFactory->CreatePathGeometry(&this->m_pClockMinutesPointer);
	if (SUCCEEDED(hr))
	{

		hr = m_pClockMinutesPointer->Open(&pSink);
	}
	if (SUCCEEDED(hr))
	{
		pSink->BeginFigure(
			D2D1::Point2F((POINTER_HALF_WIDTH*-1.0f) * m_instrumentScale, ((POINTER_OVERRUN*-1.0f) * m_instrumentScale)),
			D2D1_FIGURE_BEGIN_FILLED
		);
		pSink->AddLine(D2D1::Point2F(((POINTER_HALF_WIDTH*-1.0f) * m_instrumentScale), -(POINTER_LENGTH_ANGLE * m_instrumentScale)));
		pSink->AddLine(D2D1::Point2F(0, -(POINTER_LENGTH * m_instrumentScale)));
		pSink->AddLine(D2D1::Point2F(POINTER_HALF_WIDTH * m_instrumentScale, -(POINTER_LENGTH_ANGLE * m_instrumentScale)));
		pSink->AddLine(D2D1::Point2F(POINTER_HALF_WIDTH * m_instrumentScale, ((POINTER_OVERRUN*-1.0f) * m_instrumentScale)));
		pSink->EndFigure(D2D1_FIGURE_END_CLOSED);
		hr = pSink->Close();
	}
	SafeRelease(&pSink);

	//Hours
	hr = pContext->m_pDirect2dFactory->CreatePathGeometry(&this->m_pClockHoursPointer);
	if (SUCCEEDED(hr))
	{

		hr = m_pClockHoursPointer->Open(&pSink);
	}

	if (SUCCEEDED(hr))
	{
		pSink->BeginFigure(
			D2D1::Point2F((POINTER_HALF_WIDTH*-1.0f) * m_instrumentScale, ((POINTER_OVERRUN*-1.0f) * m_instrumentScale)),
			D2D1_FIGURE_BEGIN_FILLED
		);
		pSink->AddLine(D2D1::Point2F(((POINTER_HALF_WIDTH*-1.0f) * m_instrumentScale), -((POINTER_LENGTH_ANGLE+(POINTER_OVERRUN*3)) * m_instrumentScale)));
		pSink->AddLine(D2D1::Point2F(0, -((POINTER_LENGTH + (POINTER_OVERRUN*3)) * m_instrumentScale)));
		pSink->AddLine(D2D1::Point2F(POINTER_HALF_WIDTH * m_instrumentScale, -((POINTER_LENGTH_ANGLE + (POINTER_OVERRUN*3)) * m_instrumentScale)));
		pSink->AddLine(D2D1::Point2F(POINTER_HALF_WIDTH * m_instrumentScale, ((POINTER_OVERRUN*-1.0f) * m_instrumentScale)));
		pSink->EndFigure(D2D1_FIGURE_END_CLOSED);
		hr = pSink->Close();
	}
	SafeRelease(&pSink);

	return hr;
}

HRESULT CClockInstrument::Update(FSX_SIM_DATA* fsxData)
{
	m_simstatus = fsxData->simstatus;


	time_t t = time(0);   // get time now
	struct tm now;
	localtime_s(&now, &t);

	
	m_secondspointerangle = (now.tm_sec*6.0f);
	m_minutespointerangle = (now.tm_min*6.0f);
	int hrs = 0;
	if (now.tm_hour >= 12)
		hrs = (now.tm_hour - 12);
	else
		hrs = now.tm_hour;
	m_hourspointerangle = ((hrs * 60.0f) + now.tm_min) / 2.0f;
	return S_OK;
}

HRESULT CClockInstrument::PreRender(CInstrumentPanelContext* pContext)
{
	D2D1_SIZE_F size = pContext->m_pDeviceContext->GetSize();
	m_xPosition = INSTRUMENT_FAR_LEFT;
	m_yPosition = INSTRUMENT_BOTTOM;
	SetNudge();

	DrawInstrumentCase(pContext);
	//Card
	D2D1::Matrix3x2F translationMatrixX = D2D1::Matrix3x2F::Translation(m_xPosition, m_yPosition);
	pContext->m_pDeviceContext->SetTransform(translationMatrixX);
	pContext->m_pDeviceContext->DrawGeometry(m_pClockGraticule, m_pInstrumentBrush, 0.7f);
	pContext->m_pDeviceContext->DrawGeometry(m_pClockCard, m_pInstrumentBrush, 2.2f);
	//end card

	pContext->m_pText32ptFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	pContext->m_pText32ptFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pContext->m_pText18ptFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	pContext->m_pText18ptFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pContext->m_pText12ptFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	pContext->m_pText12ptFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

	float x1, y1, x2, y2;
	if (m_instrumentScale < 0.75f)
	{
		for (int i = 0; i < 12; i++)
		{
			float angle =  (30.0f * i);
			wchar_t c[4];
			swprintf_s(c, L"%d", (i ));
			x1 = -15.0f * m_instrumentScale;
			y1 = 0.0f * m_instrumentScale;
			x2 = 15.0f * m_instrumentScale;
			y2 = -230.0f * m_instrumentScale;
			RotateLine(x1, y1, x2, y2, angle);
			D2D1_RECT_F rc = D2D1::RectF(x1, y1, x2, y2);
			pContext->m_pDeviceContext->DrawTextW(c, (UINT32)wcslen(c), pContext->m_pText12ptFormat, rc, m_pInstrumentBrush);
		}
	}
	else {
		for (int i = 0; i < 12; i++)
		{
			float angle =  (30.0f * i);
			wchar_t c[4];
			swprintf_s(c, L"%d", (i));
			x1 = -15.0f * m_instrumentScale;
			y1 = 0.0f * m_instrumentScale;
			x2 = 15.0f * m_instrumentScale;
			y2 = -230.0f * m_instrumentScale;
			RotateLine(x1, y1, x2, y2, angle);
			D2D1_RECT_F rc = D2D1::RectF(x1, y1, x2, y2);
			pContext->m_pDeviceContext->DrawTextW(c, (UINT32)wcslen(c), pContext->m_pText18ptFormat, rc, m_pInstrumentBrush);
		}
	}


	DrawInstrumentTitle(pContext, L" ", m_pInstrumentBrush);
	return S_OK;
}


HRESULT CClockInstrument::Render(CInstrumentPanelContext* pContext)
{
	//pointer
	D2D1::Matrix3x2F translationMatrixX = D2D1::Matrix3x2F::Translation(m_xPosition, m_yPosition);
	D2D1::Matrix3x2F rotationMatrixX = D2D1::Matrix3x2F::Rotation((m_secondspointerangle), D2D1::Point2F(0, 0));
	pContext->m_pDeviceContext->SetTransform(rotationMatrixX * translationMatrixX);
	pContext->m_pDeviceContext->FillGeometry(m_pClockSecondsPointer, m_pInstrumentBrush);

	D2D1::Matrix3x2F rotationMatrixM = D2D1::Matrix3x2F::Rotation((m_minutespointerangle), D2D1::Point2F(0, 0));
	pContext->m_pDeviceContext->SetTransform(rotationMatrixM * translationMatrixX);
	pContext->m_pDeviceContext->FillGeometry(m_pClockMinutesPointer, m_pInstrumentBrush);

	D2D1::Matrix3x2F rotationMatrixH = D2D1::Matrix3x2F::Rotation((m_hourspointerangle), D2D1::Point2F(0, 0));
	pContext->m_pDeviceContext->SetTransform(rotationMatrixH * translationMatrixX);
	pContext->m_pDeviceContext->FillGeometry(m_pClockHoursPointer, m_pInstrumentBrush);
	//end pointer;

	pContext->m_pDeviceContext->SetTransform(translationMatrixX);
	pContext->m_pDeviceContext->FillEllipse(D2D1::Ellipse(
		D2D1::Point2F(-((GRATICULE_OUTER_CLIP)*m_instrumentScale), (GRATICULE_OUTER_CLIP)*m_instrumentScale),
		3,3), ((m_simstatus & 0x2) == 0x2)? m_pRedBrush : m_pGreenBrush);

	
	return CInstrument::Render(pContext);
}

HRESULT CClockInstrument::UnInitialise(CInstrumentPanelContext* pContext)
{
	CInstrument::UnInitialise(pContext);
	SafeRelease(&m_pInstrumentBrush);
	SafeRelease(&m_pRedBrush);
	SafeRelease(&m_pGreenBrush);
	SafeRelease(&m_pClockGraticule);
	SafeRelease(&m_pClockCard);
	SafeRelease(&m_pClockSecondsPointer);
	SafeRelease(&m_pClockMinutesPointer);
	SafeRelease(&m_pClockHoursPointer);
	return S_OK;
}
#include "stdafx.h"
#include "VSIInstrument.h"
#include "InstrumentPanelContext.h"


CVSIInstrument::CVSIInstrument()
	:CInstrument(0.85f,VSIIINSTRUMENT_ID)
{
	m_pInstrumentBrush = NULL;
	m_pInstrumentCaseBrush = NULL;

	m_pVSICard = NULL;
	m_pVSIGraticule = NULL;
	m_pVSIPointer = NULL;

	m_pointerangle = 0.0f;

	m_xCaseOffset = XCASE_OFFSET * m_instrumentScale;
	m_yCaseOffset = YCASE_OFFSET * m_instrumentScale;
}

CVSIInstrument::CVSIInstrument(float scale)
	:CInstrument(scale, VSIIINSTRUMENT_ID)
{
	m_pInstrumentBrush = NULL;
	m_pInstrumentCaseBrush = NULL;

	m_pVSICard = NULL;
	m_pVSIGraticule = NULL;
	m_pVSIPointer = NULL;

	m_pointerangle = 0.0f;

	m_xCaseOffset = XCASE_OFFSET * m_instrumentScale;
	m_yCaseOffset = YCASE_OFFSET * m_instrumentScale;
}

void CVSIInstrument::GetBounds(RECT* rect)
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

bool CVSIInstrument::PtInBounds(POINT pt)
{
	RECT r;
	GetBounds(&r);
	bool x = (pt.x >= r.left) && (pt.x < r.right);
	bool y = (pt.y >= r.top) && (pt.y < r.bottom);
	return (x == true && y == true) ? true : false;
}

CVSIInstrument::~CVSIInstrument()
{
}


HRESULT CVSIInstrument::Initialise(CInstrumentPanelContext* pContext)
{
	HRESULT	hr = pContext->m_pRenderTarget->CreateSolidColorBrush(GetInstrumentColor(), &m_pInstrumentBrush);
	if (SUCCEEDED(hr))
		hr = CInstrument::Initialise(pContext);

	return CreateVSI(pContext);
}

HRESULT CVSIInstrument::CreateVSI(CInstrumentPanelContext* pContext)
{
	HRESULT hr = CreateVSIGraticule(pContext);
	if (SUCCEEDED(hr))
	{
		hr = CreateVSICard(pContext);

		if (SUCCEEDED(hr))
			hr = CreateVSIPointer(pContext);
	}

	return hr;
}

HRESULT CVSIInstrument::CreateVSIGraticule(CInstrumentPanelContext* pContext)
{
	ID2D1GeometrySink *pSink = NULL;
	ID2D1PathGeometry* tempPath;
	HRESULT hr = pContext->m_pDirect2dFactory->CreatePathGeometry(&m_pVSIGraticule);
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
		double divisions = 360.0f / 100.0f;
		for (double angle = divisions; angle <= 360.0; angle += divisions)
		{
			if (angle > 236.0f && angle < 303.0f)
				continue;

				float x1 = 0, x2 = 0, y1 = 0 /*((GRATICULE_LENGTH* -1.0f) * m_instrumentScale)*/, y2 = (GRATICULE_LENGTH * m_instrumentScale);
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
		hr = m_pVSIGraticule->Open(&pSinkClip);
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

HRESULT CVSIInstrument::CreateVSICard(CInstrumentPanelContext* pContext)
{
	ID2D1GeometrySink *pSink = NULL;
	ID2D1PathGeometry* tempPath;
	HRESULT hr = pContext->m_pDirect2dFactory->CreatePathGeometry(&m_pVSICard);
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
		double divisions = 360.0f / 10.0f;
		for (double angle = divisions; angle <= 360.0; angle += divisions)
		{
			if (angle > 240.0f && angle < 269.0f)
				continue;
			float x1 = 0, x2 = 0, y1 = 0 /*((GRATICULE_LENGTH* -1.0f) * m_instrumentScale)*/, y2 = (GRATICULE_LENGTH * m_instrumentScale);
			RotateLine(x1, y1, x2, y2, angle + 18.0f);
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
		hr = m_pVSICard->Open(&pSinkClip);
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

HRESULT CVSIInstrument::CreateVSIPointer(CInstrumentPanelContext* pContext)
{
	HRESULT hr = S_OK;
	ID2D1GeometrySink *pSink = NULL;
	hr = pContext->m_pDirect2dFactory->CreatePathGeometry(&m_pVSIPointer);
	if (SUCCEEDED(hr))
	{

		hr = m_pVSIPointer->Open(&pSink);
	}
	if (SUCCEEDED(hr))
	{
		pSink->BeginFigure(
			D2D1::Point2F( ((POINTER_OVERRUN*-1.0f) * m_instrumentScale),(POINTER_HALF_WIDTH*-1.0f) * m_instrumentScale),
			D2D1_FIGURE_BEGIN_FILLED
			);
		pSink->AddLine(D2D1::Point2F( ((POINTER_LENGTH_ANGLE*-1.0f) * m_instrumentScale),((POINTER_HALF_WIDTH*-1.0f) * m_instrumentScale)));
		pSink->AddLine(D2D1::Point2F( ((POINTER_LENGTH*-1.0f) * m_instrumentScale),0));
		pSink->AddLine(D2D1::Point2F(((POINTER_LENGTH_ANGLE*-1.0f) * m_instrumentScale), (POINTER_HALF_WIDTH*1.0f) * m_instrumentScale));
		pSink->AddLine(D2D1::Point2F(((POINTER_OVERRUN*-1.0f) * m_instrumentScale), (POINTER_HALF_WIDTH*1.0f) * m_instrumentScale));

		pSink->EndFigure(D2D1_FIGURE_END_CLOSED);
		hr = pSink->Close();
	}
	SafeRelease(&pSink);
	return hr;
}

HRESULT CVSIInstrument::Update(FSX_SIM_DATA* fsxData)
{
	//wchar_t debugwStr[_MAX_PATH*8];
	//swprintf(debugwStr, L"Vertical Speed %5.5g\r\n", fsxData->vertical_speed);
	//OutputDebugString(debugwStr);

	if (fsxData->vertical_speed > 0)
	{  
		double d = fsxData->vertical_speed  >50.0 ? 50.0 : fsxData->vertical_speed;
		m_pointerangle = (float)((d/50)*(180));
	}
	else
	{
		double d = fsxData->vertical_speed < -50.0 ? -50.0 : fsxData->vertical_speed;
		m_pointerangle = (float)((d/50)*(180));
	}
	
	return S_OK;
}

HRESULT CVSIInstrument::PreRender(CInstrumentPanelContext* pContext)
{
	D2D1_SIZE_F size = pContext->m_pDeviceContext->GetSize();

	m_xPosition = INSTRUMENT_RIGHT;
	m_yPosition = INSTRUMENT_BOTTOM;
	SetNudge();

	DrawInstrumentCase(pContext);
	//Card
	D2D1::Matrix3x2F translationMatrixX = D2D1::Matrix3x2F::Translation(m_xPosition, m_yPosition);
	pContext->m_pDeviceContext->SetTransform(translationMatrixX);
	pContext->m_pDeviceContext->DrawGeometry(m_pVSIGraticule, m_pInstrumentBrush, 0.7f);
	pContext->m_pDeviceContext->DrawGeometry(m_pVSICard, m_pInstrumentBrush, 2.2f);
	//end card

	float x1, y1, x2, y2;
	wchar_t c[12];
	for (int i = 0; i <= 4; i++)
	{
		float angle = 270.0f + (36.0f * i);
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
	for (int i = 0; i <= 4; i++)
	{
		float angle = 270.0f - (36.0f * i);
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
	pContext->m_pText32ptFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
	pContext->m_pText18ptFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
	wcscpy_s(c, L"VERTICAL");
	pContext->m_pDeviceContext->DrawTextW(c, (UINT32)wcslen(c), (m_instrumentScale < 1.0f) ? pContext->m_pText18ptFormat : pContext->m_pText32ptFormat, D2D1::RectF(-165.0f, 0.0f, 165.0f, -90.0f* m_instrumentScale), m_pInstrumentBrush);
	wcscpy_s(c, L"SPEED");
	pContext->m_pDeviceContext->DrawTextW(c, (UINT32)wcslen(c), (m_instrumentScale < 1.0f) ? pContext->m_pText18ptFormat : pContext->m_pText32ptFormat, D2D1::RectF(-165.0f, 0.0f, 165.0f, -60.0f* m_instrumentScale), m_pInstrumentBrush);

	pContext->m_pText12ptFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
	wcscpy_s(c, L"FT/MIN");
	pContext->m_pDeviceContext->DrawTextW(c, (UINT32)wcslen(c), pContext->m_pText12ptFormat, D2D1::RectF(-165.0f, 45.0f, 165.0f, 55.0f), m_pInstrumentBrush);
	wcscpy_s(c, L"X 1000");
	pContext->m_pDeviceContext->DrawTextW(c, (UINT32)wcslen(c), pContext->m_pText12ptFormat, D2D1::RectF(-165.0f, 60.0f, 165.0f, 70.0f), m_pInstrumentBrush);

	pContext->m_pText32ptFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pContext->m_pText18ptFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pContext->m_pText12ptFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pContext->m_pText12ptFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	wcscpy_s(c, L"UP");
	pContext->m_pDeviceContext->DrawTextW(c, (UINT32)wcslen(c), pContext->m_pText12ptFormat, D2D1::RectF(-115.0f* m_instrumentScale, 0.0f, -55.0f, -60.0f), m_pInstrumentBrush);
	wcscpy_s(c, L"DOWN");
	pContext->m_pDeviceContext->DrawTextW(c, (UINT32)wcslen(c), pContext->m_pText12ptFormat, D2D1::RectF(-115.0f* m_instrumentScale, 0.0f, -55.0f, 60.0f), m_pInstrumentBrush);

	return S_OK;
}


HRESULT CVSIInstrument::Render(CInstrumentPanelContext* pContext)
{
	//pointer
	D2D1::Matrix3x2F translationMatrixX = D2D1::Matrix3x2F::Translation(m_xPosition, m_yPosition);
	D2D1::Matrix3x2F rotationMatrixX = D2D1::Matrix3x2F::Rotation(m_pointerangle, D2D1::Point2F(0, 0));
	pContext->m_pDeviceContext->SetTransform(rotationMatrixX * translationMatrixX);
	pContext->m_pDeviceContext->FillGeometry(m_pVSIPointer, m_pInstrumentBrush);
	//end pointer;

	return CInstrument::Render(pContext);
}

HRESULT CVSIInstrument::UnInitialise(CInstrumentPanelContext* pContext)
{
	CInstrument::UnInitialise(pContext);
	SafeRelease(&m_pInstrumentBrush);
	SafeRelease(&m_pVSIGraticule);
	SafeRelease(&m_pVSICard);
	SafeRelease(&m_pVSIPointer);
	return S_OK;
}

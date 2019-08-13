#include "stdafx.h"
#include "ASIInstrument.h"


CASIInstrument::CASIInstrument()
	:CInstrument(0.85f,ASIINSTRUMENT_ID)
{	
	m_pInstrumentBrush = NULL;
	m_pInstrumentCaseBrush = NULL;

	m_pASICard = NULL;
	m_pASIGraticule = NULL;
	m_pASIPointer = NULL;

	m_pointerangle = 0.0f;

	m_xCaseOffset = XCASE_OFFSET * m_instrumentScale;
	m_yCaseOffset = YCASE_OFFSET * m_instrumentScale;
}

CASIInstrument::CASIInstrument(float scale)
	:CInstrument(scale, ASIINSTRUMENT_ID)
{
	m_pInstrumentBrush = NULL;
	m_pInstrumentCaseBrush = NULL;

	m_pASICard = NULL;
	m_pASIGraticule = NULL;
	m_pASIPointer = NULL;

	m_pointerangle = 0.0f;

	m_xCaseOffset = XCASE_OFFSET * m_instrumentScale;
	m_yCaseOffset = YCASE_OFFSET * m_instrumentScale;
}


CASIInstrument::~CASIInstrument()
{
}

void CASIInstrument::GetBounds(RECT* rect)
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

bool CASIInstrument::PtInBounds(POINT pt)
{
	RECT r;
	GetBounds(&r);
	bool x = (pt.x >= r.left) && (pt.x < r.right);
	bool y = (pt.y >= r.top) && (pt.y < r.bottom);
	return (x == true && y == true) ? true : false;
}


HRESULT CASIInstrument::Initialise(CInstrumentPanelContext* pContext)
{
	HRESULT	hr = pContext->m_pRenderTarget->CreateSolidColorBrush(GetInstrumentColor(), &m_pInstrumentBrush);
	if (SUCCEEDED(hr))
			hr = CInstrument::Initialise(pContext);

	return CreateASI(pContext);
}

HRESULT CASIInstrument::CreateASI(CInstrumentPanelContext* pContext)
{
	HRESULT hr = CreateASIGraticule(pContext);
	if (SUCCEEDED(hr))
	{
		hr = CreateASICard(pContext);

		if (SUCCEEDED(hr))
			hr = CreateASIPointer(pContext);
	}

	return hr;
}

HRESULT CASIInstrument::CreateASIGraticule(CInstrumentPanelContext* pContext)
{
	ID2D1GeometrySink *pSink = NULL;
	ID2D1PathGeometry* tempPath;
	HRESULT hr = pContext->m_pDirect2dFactory->CreatePathGeometry(&m_pASIGraticule);
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
		double divisions = 360.0 / 50.0;
		for (double angle = divisions; angle <= 360.0; angle += divisions)
		{
			if (angle > ((360.0 / 10.0) * 0.9) && angle < ((360.0 / 10.0) * 9))
			{
				float x1 = 0, x2 = 0, y1 = 0 * m_instrumentScale, y2 = 150 * m_instrumentScale;
				RotateLine(x1, y1, x2, y2, angle);
				pSink->BeginFigure(D2D1::Point2F(x1, y1), D2D1_FIGURE_BEGIN_HOLLOW);
				pSink->AddLine(D2D1::Point2F(x2, y2));
				pSink->EndFigure(D2D1_FIGURE_END_OPEN);
			}
		}
		hr = pSink->Close();
		SafeRelease(&pSink);
	}
	ID2D1GeometrySink* pSinkClip;
	if (SUCCEEDED(hr))
	{
		hr = m_pASIGraticule->Open(&pSinkClip);
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

HRESULT CASIInstrument::CreateASICard(CInstrumentPanelContext* pContext)
{
	ID2D1GeometrySink *pSink = NULL;
	ID2D1PathGeometry* tempPath;
	HRESULT hr = pContext->m_pDirect2dFactory->CreatePathGeometry(&m_pASICard);
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

		double divisions = 360.0 / 10.0;
		for (double angle = divisions; angle < 360.0; angle += divisions)
		{
				float x1 = 0, x2 = 0, y1 = 0 * m_instrumentScale, y2 = 150 * m_instrumentScale;
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
		hr = m_pASICard->Open(&pSinkClip);
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

HRESULT CASIInstrument::CreateASIPointer(CInstrumentPanelContext* pContext)
{
	HRESULT hr = S_OK;
	ID2D1GeometrySink *pSink = NULL;
	hr = pContext->m_pDirect2dFactory->CreatePathGeometry(&m_pASIPointer);
	if (SUCCEEDED(hr))
	{

		hr = m_pASIPointer->Open(&pSink);
	}
	if (SUCCEEDED(hr))
	{
		pSink->BeginFigure(
			D2D1::Point2F((POINTER_HALF_WIDTH*-1.0f) * m_instrumentScale, ((POINTER_OVERRUN*-1.0f) * m_instrumentScale)),
			D2D1_FIGURE_BEGIN_FILLED
			);
		pSink->AddLine(D2D1::Point2F(((POINTER_HALF_WIDTH*-1.0f) * m_instrumentScale), POINTER_LENGTH_ANGLE * m_instrumentScale));
		pSink->AddLine(D2D1::Point2F(0, POINTER_LENGTH * m_instrumentScale));
		pSink->AddLine(D2D1::Point2F(POINTER_HALF_WIDTH * m_instrumentScale, POINTER_LENGTH_ANGLE * m_instrumentScale));
		pSink->AddLine(D2D1::Point2F(POINTER_HALF_WIDTH * m_instrumentScale, ((POINTER_OVERRUN*-1.0f) * m_instrumentScale)));

		pSink->EndFigure(D2D1_FIGURE_END_CLOSED);
		hr = pSink->Close();
	}
	return hr;
}

HRESULT CASIInstrument::Update(FSX_SIM_DATA* fsxData)
{
	double d = (fsxData->airspeed_true / 160.0) * 288.0;
	m_pointerangle =(float)d;
	return S_OK;
}

HRESULT CASIInstrument::PreRender(CInstrumentPanelContext* pContext)
{
	D2D1_SIZE_F size = pContext->m_pDeviceContext->GetSize();

	m_xPosition = INSTRUMENT_LEFT;
	m_yPosition = INSTRUMENT_TOP;
	SetNudge();

	DrawInstrumentCase(pContext);
	

	//Card
	D2D1::Matrix3x2F translationMatrixX = D2D1::Matrix3x2F::Translation(m_xPosition, m_yPosition);
	pContext->m_pDeviceContext->SetTransform(translationMatrixX);
	pContext->m_pDeviceContext->DrawGeometry(m_pASIGraticule, m_pInstrumentBrush, 0.7f);
	pContext->m_pDeviceContext->DrawGeometry(m_pASICard, m_pInstrumentBrush, 2.2f);
	//end card

	

	

	//IDWriteTextFormat* pLargeWriteFormat = (m_instrumentScale < 1.0f) ? pContext->m_pText18ptFormat : pContext->m_pText32ptFormat;
	//IDWriteTextFormat* pSmallWriteFormat = (m_instrumentScale < 1.0f) ? pContext->m_pText12ptFormat : pContext->m_pText18ptFormat;

	pContext->m_pText32ptFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	pContext->m_pText32ptFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pContext->m_pText18ptFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	pContext->m_pText18ptFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pContext->m_pText12ptFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	pContext->m_pText12ptFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

	float x1,y1,x2,y2;
	wchar_t c[12];
	for (int i = 0; i < 9; i++)
	{
		float angle = 216.0f +(36.0f * i);
		wchar_t c[4];
		swprintf_s(c, L"%d", (i*20));
		x1 = -15.0f * m_instrumentScale;
		y1 = 0.0f * m_instrumentScale;
		x2 = 15.0f * m_instrumentScale;
		y2 = -230.0f * m_instrumentScale;
		RotateLine(x1, y1, x2, y2, angle);
		D2D1_RECT_F rc = D2D1::RectF(x1, y1, x2, y2);
		pContext->m_pDeviceContext->DrawTextW(c, (UINT32)wcslen(c), pContext->m_pText18ptFormat, rc, m_pInstrumentBrush);
	}

	wcscpy_s(c, L"AIRSPEED");
	pContext->m_pDeviceContext->DrawTextW(c, (UINT32)wcslen(c), (m_instrumentScale < 1.0f) ? pContext->m_pText18ptFormat : pContext->m_pText32ptFormat, D2D1::RectF(-165.0f, 0.0f, 165.0f, -100.0f), m_pInstrumentBrush);
	wcscpy_s(c, L"KNOTS");
	pContext->m_pDeviceContext->DrawTextW(c, (UINT32)wcslen(c), (m_instrumentScale < 1.0f) ? pContext->m_pText12ptFormat : pContext->m_pText18ptFormat, D2D1::RectF(-165.0f, 0.0f, 165.0f, 100.0f), m_pInstrumentBrush);
	
	
	return S_OK;
}


HRESULT CASIInstrument::Render(CInstrumentPanelContext* pContext)
{
	//pointer
	D2D1::Matrix3x2F translationMatrixX = D2D1::Matrix3x2F::Translation(m_xPosition, m_yPosition);
	D2D1::Matrix3x2F rotationMatrixX = D2D1::Matrix3x2F::Rotation(m_pointerangle + 36.0f, D2D1::Point2F(0, 0));
	pContext->m_pDeviceContext->SetTransform(rotationMatrixX * translationMatrixX);
	pContext->m_pDeviceContext->FillGeometry(m_pASIPointer, m_pInstrumentBrush);
	//end pointer;

	return CInstrument::Render(pContext);
}

HRESULT CASIInstrument::UnInitialise(CInstrumentPanelContext* pContext)
{
	CInstrument::UnInitialise(pContext);
	SafeRelease(&m_pInstrumentBrush);
	SafeRelease(&m_pASIGraticule);
	SafeRelease(&m_pASICard);
	SafeRelease(&m_pASIPointer);
	return S_OK;
}

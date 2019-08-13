#include "stdafx.h"
#include "AltInstrument.h"

//The Alitmeter 

CAltInstrument::CAltInstrument()
	:CInstrument(0.85f,ALTINSTRUMENT_ID)
{
	m_pInstrumentBrush = NULL;
	m_pInstrumentCaseBrush = NULL;

	m_pAltCard = NULL;
	m_pAltGraticule = NULL;
	m_pAltHundredsPointer = NULL;
	m_pAltThousandsPointer = NULL;

	m_pointerthousandsangle = m_pointerhundredsangle = 0.0f;
	m_kohlsman = 1013;

	m_xCaseOffset = XCASE_OFFSET * m_instrumentScale;
	m_yCaseOffset = YCASE_OFFSET * m_instrumentScale;
}

CAltInstrument::CAltInstrument(float scale)
	:CInstrument(scale, ALTINSTRUMENT_ID)
{
	m_pInstrumentBrush = NULL;
	m_pInstrumentCaseBrush = NULL;

	m_pAltCard = NULL;
	m_pAltGraticule = NULL;
	m_pAltHundredsPointer = NULL;
	m_pAltThousandsPointer = NULL;

	m_pointerthousandsangle = m_pointerhundredsangle = 0.0f;
	m_kohlsman = 1013;

	m_xCaseOffset = XCASE_OFFSET * m_instrumentScale;
	m_yCaseOffset = YCASE_OFFSET * m_instrumentScale;
}

CAltInstrument::~CAltInstrument()
{
}

void CAltInstrument::GetBounds(RECT* rect)
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

bool CAltInstrument::PtInBounds(POINT pt)
{
	RECT r;
	GetBounds(&r);
	bool x = (pt.x >= r.left) && (pt.x < r.right);
	bool y = (pt.y >= r.top) && (pt.y < r.bottom);
	return (x == true && y == true) ? true : false;
}


HRESULT CAltInstrument::Initialise(CInstrumentPanelContext* pContext)
{
	HRESULT	hr = pContext->m_pRenderTarget->CreateSolidColorBrush(GetInstrumentColor(), &m_pInstrumentBrush);
	if (SUCCEEDED(hr))
		hr = CInstrument::Initialise(pContext);
	return CreateAlt(pContext);
}

HRESULT CAltInstrument::CreateAlt(CInstrumentPanelContext* pContext)
{
	HRESULT hr = CreateAltGraticule(pContext);
	if (SUCCEEDED(hr))
	{
		hr = CreateAltGraticuleHalfWay(pContext);
		if (SUCCEEDED(hr))
		{
			hr = CreateAltCard(pContext);



			if (SUCCEEDED(hr))
				hr = CreateAltPointers(pContext);
		}
	}
	return hr;
}

HRESULT CAltInstrument::CreateAltGraticule(CInstrumentPanelContext* pContext)
{
	ID2D1GeometrySink *pSink = NULL;
	ID2D1PathGeometry* tempPath;
	HRESULT hr = pContext->m_pDirect2dFactory->CreatePathGeometry(&m_pAltGraticule);
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
		
		double divisions = 360.0 / 100.0;
		for (double angle = divisions; angle <= 180.0; angle += divisions)
		{
				float x1, y1, x2, y2;
				x1 = 0.0f; 
				x2 = 0.0f;
				y1 = ((GRATICULE_LENGTH* -1.0f) * m_instrumentScale);
				y2 = ((GRATICULE_LENGTH*1.0f) * m_instrumentScale);
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
		hr = m_pAltGraticule->Open(&pSinkClip);
	}

	D2D1_ELLIPSE clip;
	ID2D1EllipseGeometry* clipEllipse;
	if (SUCCEEDED(hr))
	{
		//Create a clip ellipse.
		clip = D2D1::Ellipse(
			D2D1::Point2F(0.0f, 0.0f),
			GRATICULE_OUTER_CLIP*m_instrumentScale,
			GRATICULE_OUTER_CLIP*m_instrumentScale
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

HRESULT CAltInstrument::CreateAltGraticuleHalfWay(CInstrumentPanelContext* pContext)
{
	ID2D1GeometrySink *pSink = NULL;
	ID2D1PathGeometry* tempPath;
	HRESULT hr = pContext->m_pDirect2dFactory->CreatePathGeometry(&m_pAltHalfGraticule);
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

		double divisions = 360.0 / 20.0;
		for (double angle = divisions; angle <= 180.0; angle += divisions)
		{
			float x1, y1, x2, y2;
			x1 = 0, x2 = 0, y1 = ((GRATICULE_LENGTH* -1.0f) * m_instrumentScale), y2 = ((GRATICULE_LENGTH*1.0f) * m_instrumentScale);
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
		hr = m_pAltHalfGraticule->Open(&pSinkClip);
	}

	D2D1_ELLIPSE clip;
	ID2D1EllipseGeometry* clipEllipse;
	if (SUCCEEDED(hr))
	{
		//Create a clip ellipse.
		clip = D2D1::Ellipse(
			D2D1::Point2F(0.0f, 0.0f),
			(GRATICULE_INNER_CLIP+5.0f)*m_instrumentScale,
			(GRATICULE_INNER_CLIP + 5.0f)*m_instrumentScale
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

HRESULT CAltInstrument::CreateAltCard(CInstrumentPanelContext* pContext)
{
	ID2D1GeometrySink *pSink = NULL;
	ID2D1PathGeometry* tempPath;
	HRESULT hr = pContext->m_pDirect2dFactory->CreatePathGeometry(&m_pAltCard);
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
		/*
		pSink->BeginFigure(
			D2D1::Point2F(-10 * m_instrumentScale, 0),
			D2D1_FIGURE_BEGIN_FILLED
			);

		pSink->AddLine(D2D1::Point2F(0, -30 * m_instrumentScale));
		pSink->AddLine(D2D1::Point2F(10 * m_instrumentScale, 0));
		pSink->EndFigure(D2D1_FIGURE_END_CLOSED);
		*/


		pSink->BeginFigure(
			D2D1::Point2F(0, ((GRATICULE_LENGTH* -1.0f) * m_instrumentScale)),
			D2D1_FIGURE_BEGIN_FILLED
			);
		pSink->AddLine(D2D1::Point2F(0, (GRATICULE_LENGTH * m_instrumentScale)));
		pSink->EndFigure(D2D1_FIGURE_END_OPEN);

		double divisions = 360.0 / 10.0;
		for (double angle = divisions; angle <= 180.0; angle += divisions)
		{
			float x1 = 0, x2 = 0, y1 = ((GRATICULE_LENGTH* -1.0f) * m_instrumentScale), y2 = (GRATICULE_LENGTH * m_instrumentScale);
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
		hr = m_pAltCard->Open(&pSinkClip);
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

HRESULT CAltInstrument::CreateAltPointers(CInstrumentPanelContext* pContext)
{
	HRESULT hr = CreateAltHundredsPointer(pContext);
	if (SUCCEEDED(hr))
	{
		hr = CreateAltThousandsPointer(pContext);

	}
	return hr;
}	
	HRESULT CAltInstrument::CreateAltHundredsPointer(CInstrumentPanelContext* pContext)
	{

	HRESULT hr = S_OK;
	ID2D1GeometrySink *pSink = NULL;
	hr = pContext->m_pDirect2dFactory->CreatePathGeometry(&m_pAltHundredsPointer);
	if (SUCCEEDED(hr))
	{

		hr = m_pAltHundredsPointer->Open(&pSink);
	}
	if (SUCCEEDED(hr))
	{
		pSink->BeginFigure(
			D2D1::Point2F((POINTER_HALF_WIDTH*-1.0f) * m_instrumentScale, ((POINTER_OVERRUN*-1.0f) * m_instrumentScale)),
			D2D1_FIGURE_BEGIN_FILLED
			);
		pSink->AddLine(D2D1::Point2F(((POINTER_HALF_WIDTH*-1.0f) * m_instrumentScale), (POINTER_LENGTH_ANGLE*-1.0f) * m_instrumentScale));
		pSink->AddLine(D2D1::Point2F(0, (POINTER_LENGTH*-1.0f) * m_instrumentScale));
		pSink->AddLine(D2D1::Point2F(POINTER_HALF_WIDTH * m_instrumentScale, (POINTER_LENGTH_ANGLE*-1.0f) * m_instrumentScale));
		pSink->AddLine(D2D1::Point2F(POINTER_HALF_WIDTH * m_instrumentScale, ((POINTER_OVERRUN*-1.0f) * m_instrumentScale)));

		pSink->EndFigure(D2D1_FIGURE_END_CLOSED);
		hr = pSink->Close();
	}

	SafeRelease(&pSink);

	return hr;
}

	HRESULT CAltInstrument::CreateAltThousandsPointer(CInstrumentPanelContext* pContext)
	{

		HRESULT hr = S_OK;
		ID2D1GeometrySink *pSink = NULL;
		hr = pContext->m_pDirect2dFactory->CreatePathGeometry(&m_pAltThousandsPointer);
		if (SUCCEEDED(hr))
		{

			hr = m_pAltThousandsPointer->Open(&pSink);
		}
		if (SUCCEEDED(hr))
		{
			pSink->BeginFigure(
				D2D1::Point2F(((POINTER_HALF_WIDTH*1.2f)*-1.0f) * m_instrumentScale, ((POINTER_OVERRUN*-1.0f) * m_instrumentScale)),
				D2D1_FIGURE_BEGIN_FILLED
				);
			pSink->AddLine(D2D1::Point2F((((POINTER_HALF_WIDTH*2.2f)*-1.0f) * m_instrumentScale), ((POINTER_LENGTH_ANGLE/1.5f)*-0.75f) * m_instrumentScale));
			pSink->AddLine(D2D1::Point2F(0, (POINTER_LENGTH*-0.75f) * m_instrumentScale));
			pSink->AddLine(D2D1::Point2F((POINTER_HALF_WIDTH*2.2f) * m_instrumentScale, ((POINTER_LENGTH_ANGLE / 1.5f)*-0.75f) * m_instrumentScale));
			pSink->AddLine(D2D1::Point2F((POINTER_HALF_WIDTH*1.2f) * m_instrumentScale, ((POINTER_OVERRUN*-1.0f) * m_instrumentScale)));

			pSink->EndFigure(D2D1_FIGURE_END_CLOSED);
			hr = pSink->Close();
		}

		SafeRelease(&pSink);

		return hr;
	}

	HRESULT CAltInstrument::Update(FSX_SIM_DATA* fsxData)
{
	m_pointerhundredsangle = (float)((fmod(fsxData->indicated_altitude, 1000.0) / 1000.0)*360.0);
	m_pointerthousandsangle = (float)((fmod(fsxData->indicated_altitude, 10000.0) / 10000.0)*360.0);
	m_kohlsman = (int)(fsxData->kohlsman_setting);

	return S_OK;
}

HRESULT CAltInstrument::PreRender(CInstrumentPanelContext* pContext)
{
	D2D1_SIZE_F size = pContext->m_pDeviceContext->GetSize();

	m_xPosition = INSTRUMENT_RIGHT;
	m_yPosition = INSTRUMENT_TOP;
	SetNudge();

	DrawInstrumentCase(pContext);

	//Card
	D2D1::Matrix3x2F translationMatrixX = D2D1::Matrix3x2F::Translation(m_xPosition, m_yPosition);
	pContext->m_pDeviceContext->SetTransform(translationMatrixX);
	pContext->m_pDeviceContext->DrawGeometry(m_pAltGraticule, m_pInstrumentBrush, 0.7f);
	pContext->m_pDeviceContext->DrawGeometry(m_pAltHalfGraticule, m_pInstrumentBrush, 1.0f);
	pContext->m_pDeviceContext->DrawGeometry(m_pAltCard, m_pInstrumentBrush, 2.2f);
	//end card

	

	float x1;
	float y1;
	float x2;
	float y2;
	wchar_t c[16];

	for (int i = 0; i <= 9; i++)
	{
		float angle = (36.0f * i);
		swprintf_s(c, L"%d", i);
		x1 = -10.0f * m_instrumentScale;
		y1 = 0.0f * m_instrumentScale;
		x2 = 10.0f * m_instrumentScale;
		y2 = -230.0f * m_instrumentScale;
		RotateLine(x1, y1, x2, y2, angle);
		D2D1_RECT_F rc = D2D1::RectF(x1, y1, x2, y2);
		pContext->m_pDeviceContext->DrawTextW(c, (UINT32)wcslen(c), pContext->m_pText32ptFormat, rc, m_pInstrumentBrush);
	}
	
	//DrawInstrumentTitle(pContext, L"ALT", m_pInstrumentBrush);
	pContext->m_pText32ptFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
	pContext->m_pText18ptFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
	pContext->m_pText12ptFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);

	wcscpy_s(c, L"ALT");
	pContext->m_pDeviceContext->DrawTextW(c, (UINT32)wcslen(c), (m_instrumentScale < 1.0f) ? pContext->m_pText18ptFormat : pContext->m_pText32ptFormat, D2D1::RectF(-165.0f, 0.0f, 165.0f, -70.0f* m_instrumentScale), m_pInstrumentBrush);

	wcscpy_s(c, L"FEET");
	pContext->m_pDeviceContext->DrawTextW(c, (UINT32)wcslen(c), pContext->m_pText12ptFormat, D2D1::RectF(-165.0f, 40.0f*m_instrumentScale, 165.0f, 55.0f), m_pInstrumentBrush);

	wcscpy_s(c, L"X 1000");
	pContext->m_pDeviceContext->DrawTextW(c, (UINT32)wcslen(c), pContext->m_pText12ptFormat, D2D1::RectF(-165.0f, 55.0f* m_instrumentScale, 165.0f, 100.0f* m_instrumentScale), m_pInstrumentBrush);

	wcscpy_s(c, L"X 100");
	pContext->m_pDeviceContext->DrawTextW(c, (UINT32)wcslen(c), pContext->m_pText12ptFormat, D2D1::RectF(-165.0f, 70.0f* m_instrumentScale, 165.0f, 100.0f* m_instrumentScale), m_pInstrumentBrush);

	pContext->m_pText32ptFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pContext->m_pText18ptFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pContext->m_pText12ptFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	
	pContext->m_pDeviceContext->DrawRectangle(D2D1::RectF(79.0f* m_instrumentScale, -11.0f, (80.0f* m_instrumentScale) + 41.0f, 12.0f), m_pInstrumentBrush);


	return S_OK;
}


HRESULT CAltInstrument::Render(CInstrumentPanelContext* pContext)
{

	D2D1::Matrix3x2F translationMatrixX = D2D1::Matrix3x2F::Translation(m_xPosition, m_yPosition);
	pContext->m_pDeviceContext->SetTransform(translationMatrixX);
	//pointer
	wchar_t c[12];
	
	if (m_kohlsman > 800.0)
		swprintf_s(c, L"%d", m_kohlsman);
	else
		wcscpy_s(c, sizeof(c)/sizeof(WORD) ,L"1013");

	pContext->m_pText18ptFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pContext->m_pText18ptFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
	pContext->m_pDeviceContext->DrawTextW(c, (UINT32)wcslen(c), pContext->m_pText18ptFormat, D2D1::RectF(80.0f* m_instrumentScale, -10.0f, (80.0f* m_instrumentScale) + 39.5f, 10.0f), m_pInstrumentBrush);
	pContext->m_pText18ptFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);

	D2D1::Matrix3x2F rotationMatrix1000 = D2D1::Matrix3x2F::Rotation(m_pointerthousandsangle, D2D1::Point2F(0, 0));
	pContext->m_pDeviceContext->SetTransform(rotationMatrix1000 * translationMatrixX);
	pContext->m_pDeviceContext->FillGeometry(m_pAltThousandsPointer, m_pInstrumentBrush);

	D2D1::Matrix3x2F rotationMatrix100 = D2D1::Matrix3x2F::Rotation(m_pointerhundredsangle, D2D1::Point2F(0, 0));
	pContext->m_pDeviceContext->SetTransform(rotationMatrix100 * translationMatrixX);
	pContext->m_pDeviceContext->FillGeometry(m_pAltHundredsPointer, m_pInstrumentBrush);


	//end pointer;

	


	return CInstrument::Render(pContext);
}

HRESULT CAltInstrument::UnInitialise(CInstrumentPanelContext* pContext)
{
	CInstrument::UnInitialise(pContext);
	SafeRelease(&m_pInstrumentBrush);
	SafeRelease(&m_pAltGraticule);
	SafeRelease(&m_pAltHalfGraticule);
	SafeRelease(&m_pAltCard);
	SafeRelease(&m_pAltHundredsPointer);
	SafeRelease(&m_pAltThousandsPointer);
	return S_OK;
}

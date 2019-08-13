#include "stdafx.h"
#include "OilInstrument.h"


COilInstrument::COilInstrument()
	:CInstrument(0.6f,OILINSTRUMENT_ID)
{

	m_pInstrumentBrush = NULL;
	m_pOilCard = NULL;
	m_pOilGraticule = NULL;
	m_pOilPointer = NULL;

	m_pressureangle = 0.0f;
	m_temperatureangle = 0.0f;

	m_xCaseOffset = (XCASE_OFFSET * m_instrumentScale);
	m_yCaseOffset = (YCASE_OFFSET * m_instrumentScale);

}

COilInstrument::COilInstrument(float scale)
	:CInstrument(scale, OILINSTRUMENT_ID)
{

	m_pInstrumentBrush = NULL;
	m_pOilCard = NULL;
	m_pOilGraticule = NULL;
	m_pOilPointer = NULL;

	m_pressureangle = 0.0f;
	m_temperatureangle = 0.0f;

	m_xCaseOffset = (XCASE_OFFSET * m_instrumentScale);
	m_yCaseOffset = (YCASE_OFFSET * m_instrumentScale);

}

COilInstrument::~COilInstrument()
{
}

void COilInstrument::GetBounds(RECT* rect)
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

bool COilInstrument::PtInBounds(POINT pt)
{
	RECT r;
	GetBounds(&r);
	bool x = (pt.x >= r.left) && (pt.x < r.right);
	bool y = (pt.y >= r.top) && (pt.y < r.bottom);
	return (x == true && y == true) ? true : false;
}


HRESULT COilInstrument::Initialise(CInstrumentPanelContext* pContext)
{
	HRESULT hr = CInstrument::Initialise(pContext);
	if (SUCCEEDED(hr))
		HRESULT	hr = pContext->m_pRenderTarget->CreateSolidColorBrush(GetInstrumentColor(),&m_pInstrumentBrush);
	
	return CreateOil(pContext);
}

HRESULT COilInstrument::CreateOil(CInstrumentPanelContext* pContext)
{
	HRESULT hr = CreateOilGraticule(pContext);
	if (SUCCEEDED(hr))
	{
		hr = CreateOilCard(pContext);

		if (SUCCEEDED(hr))
			hr = CreateOilPointer(pContext);
	}

	return hr;
}

HRESULT COilInstrument::CreateOilGraticule(CInstrumentPanelContext* pContext)
{
	ID2D1GeometrySink *pSink = NULL;
	ID2D1PathGeometry* tempPath;
	HRESULT hr = pContext->m_pDirect2dFactory->CreatePathGeometry(&m_pOilGraticule);
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
		for (double angle = 3.6; angle <= 360.0; angle += 3.6)
		{
			//if (angle < 120.f || angle > 240.f)
			//{
			if (
				(angle > (324.f + 1.0f) && angle < (360.f))
				|| (angle > 0 && angle < (36.f))
				|| (angle >(144.f + 1.0f) && angle <= (180.f))
				|| (angle > 180 && angle < (216.0f))
				)

			//if ((angle > (288.f + 1.0f) && angle < (360.f)) || (angle > 0 && angle < (36.f)))
				continue;

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
		hr = m_pOilGraticule->Open(&pSinkClip);
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

HRESULT COilInstrument::CreateOilCard(CInstrumentPanelContext* pContext)
{
	ID2D1GeometrySink *pSink = NULL;
	ID2D1PathGeometry* tempPath;
	HRESULT hr = pContext->m_pDirect2dFactory->CreatePathGeometry(&m_pOilCard);
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

		for (double angle = 36.0; angle <= 360.0; angle += 36.0)
		{
			if (
				   (angle > (324.f + 1.0f) && angle <= (360.f))
				|| (angle > 0 && angle < (36.0f))
				|| (angle >(144.f + 1.0f) && angle <= (180.f))
				|| (angle > 180 && angle < (216.0f))
				)

//			if ((angle > (288.f + 1.0f) && angle <= (360.f)) || (angle > 0 && angle < (36.0f)))
				continue;
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
		hr = m_pOilCard->Open(&pSinkClip);
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

HRESULT COilInstrument::CreateOilPointer(CInstrumentPanelContext* pContext)
{
	HRESULT hr = S_OK;
	ID2D1GeometrySink *pSink = NULL;
	hr = pContext->m_pDirect2dFactory->CreatePathGeometry(&this->m_pOilPointer);
	if (SUCCEEDED(hr))
	{

		hr = m_pOilPointer->Open(&pSink);
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
	SafeRelease(&pSink);
	return hr;
}

HRESULT COilInstrument::Update(FSX_SIM_DATA* fsxData)
{ 
	m_pressureangle = (float)((fsxData->oilpressure / 12960.0) * (3.0 * 36.0));
	//m_pressureangle = (float)((fsxData->oilpressure/8640.0) * (2.0 * 36.0) );
	m_temperatureangle = (float)((RankineToCelsius(fsxData->oiltemp) / 120.0) * (3.0 * -36.0));
	return S_OK; 
}

double COilInstrument::RankineToCelsius(double rankine)
{
	return ((rankine - 491.67) * (5.0 / 9.0));
}

HRESULT COilInstrument::PreRender(CInstrumentPanelContext* pContext)
{
	D2D1_SIZE_F size = pContext->m_pDeviceContext->GetSize();
	m_xPosition = INSTRUMENT_FAR_LEFT;
	m_yPosition = INSTRUMENT_TOP;
	SetNudge();

	DrawInstrumentCase(pContext);
	//Card
	D2D1::Matrix3x2F translationMatrixX = D2D1::Matrix3x2F::Translation(m_xPosition,m_yPosition);
	pContext->m_pDeviceContext->SetTransform(translationMatrixX);
	pContext->m_pDeviceContext->DrawGeometry(m_pOilGraticule, m_pInstrumentBrush, 0.7f);
	pContext->m_pDeviceContext->DrawGeometry(m_pOilCard, m_pInstrumentBrush, 2.2f);
	//end card

	pContext->m_pText32ptFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	pContext->m_pText32ptFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pContext->m_pText18ptFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	pContext->m_pText18ptFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pContext->m_pText12ptFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	pContext->m_pText12ptFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	
	static wchar_t* pressurelabels[] = { L"0",L"30",L"60",L"90" };
	static wchar_t* templabels[] = { L"0",L"0",L"40",L"80",L"120" };
	
	float x1, y1, x2, y2;
	if (m_instrumentScale < 0.75f)
	{
		for (int i = 0; i < 4; i++)
		{
			float angle = 216.0f + (36.0f * i);
			wchar_t c[4];
			//swprintf_s(c, L"%d", (i * 4));
			wcscpy_s(c, pressurelabels[i]);
			x1 = -15.0f * m_instrumentScale;
			y1 = 0.0f * m_instrumentScale;
			x2 = 15.0f * m_instrumentScale;
			y2 = -230.0f * m_instrumentScale;
			RotateLine(x1, y1, x2, y2, angle);
			D2D1_RECT_F rc = D2D1::RectF(x1, y1, x2, y2);
			pContext->m_pDeviceContext->DrawTextW(c, (UINT32)wcslen(c), pContext->m_pText12ptFormat, rc, m_pInstrumentBrush);
		}

		for (int i = 4; i > 0; i--)
		{
			float angle = 216.0f + (36.0f * (9-i) );
			wchar_t c[6];
			//swprintf_s(c, L"%d", ( (i-1) * 4) );
			wcscpy_s(c, templabels[i]);
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
		for (int i = 0; i < 4; i++)
		{
			float angle = 216.0f + (36.0f * i);
			wchar_t c[4];
			swprintf_s(c, L"%d", (i * 4));
			x1 = -15.0f * m_instrumentScale;
			y1 = 0.0f * m_instrumentScale;
			x2 = 15.0f * m_instrumentScale;
			y2 = -230.0f * m_instrumentScale;
			RotateLine(x1, y1, x2, y2, angle);
			D2D1_RECT_F rc = D2D1::RectF(x1, y1, x2, y2);
			pContext->m_pDeviceContext->DrawTextW(c, (UINT32)wcslen(c), pContext->m_pText18ptFormat, rc, m_pInstrumentBrush);
		}

		for (int i = 4; i > 0; i--)
		{
			float angle = 216.0f + (36.0f * (9 - i));
			wchar_t c[4];
			swprintf_s(c, L"%d", ((i - 1) * 4));
			x1 = -15.0f * m_instrumentScale;
			y1 = 0.0f * m_instrumentScale;
			x2 = 15.0f * m_instrumentScale;
			y2 = -230.0f * m_instrumentScale;
			RotateLine(x1, y1, x2, y2, angle);
			D2D1_RECT_F rc = D2D1::RectF(x1, y1, x2, y2);
			pContext->m_pDeviceContext->DrawTextW(c, (UINT32)wcslen(c), pContext->m_pText18ptFormat, rc, m_pInstrumentBrush);
		}
	}

	wchar_t c[64];
	pContext->m_pText12ptFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
	wcscpy_s(c, L"OIL");
	pContext->m_pDeviceContext->DrawTextW(c, (UINT32)wcslen(c), pContext->m_pText12ptFormat, D2D1::RectF(-165.0f, -55.0f, 165.0f, -35.0f), m_pInstrumentBrush);
	pContext->m_pText32ptFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pContext->m_pText18ptFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pContext->m_pText12ptFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pContext->m_pText12ptFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	pContext->m_pText8ptFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	wcscpy_s(c, L"PRESSURE");
	pContext->m_pDeviceContext->DrawTextW(c, (UINT32)wcslen(c), pContext->m_pText8ptFormat, D2D1::RectF(-135.0f* m_instrumentScale, -8.0f, 0.0f, 8.0f), m_pInstrumentBrush);
	pContext->m_pText8ptFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
	wcscpy_s(c, L"TEMP");
	pContext->m_pDeviceContext->DrawTextW(c, (UINT32)wcslen(c), pContext->m_pText8ptFormat, D2D1::RectF(0.0f, -8.0f, 125.0f* m_instrumentScale, 8.0f), m_pInstrumentBrush);
	pContext->m_pText8ptFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);

	//wcscpy_s(c, L"psi");
	wcscpy_s(c, L"lbf/in?);
	pContext->m_pDeviceContext->DrawTextW(c, (UINT32)wcslen(c), pContext->m_pText12ptFormat, D2D1::RectF(-68.0f* m_instrumentScale, 0.0f, -5.0f, 135.0f), m_pInstrumentBrush);
	pContext->m_pText12ptFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);

	wcscpy_s(c, L"ºC");
	pContext->m_pDeviceContext->DrawTextW(c, (UINT32)wcslen(c), pContext->m_pText12ptFormat, D2D1::RectF(5.0f, 0.0f,68.0f* m_instrumentScale, 135.0f), m_pInstrumentBrush);
	pContext->m_pText12ptFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);

	//DrawInstrumentTitle(pContext, L"OIL", m_pInstrumentBrush);
	return S_OK;
}


HRESULT COilInstrument::Render(CInstrumentPanelContext* pContext)
{
		//pointer
		D2D1::Matrix3x2F translationMatrixX = D2D1::Matrix3x2F::Translation(m_xPosition, m_yPosition);
		D2D1::Matrix3x2F rotationMatrixX = D2D1::Matrix3x2F::Rotation((m_pressureangle+36.0f), D2D1::Point2F(0, 0));
		pContext->m_pDeviceContext->SetTransform(rotationMatrixX * translationMatrixX);
		pContext->m_pDeviceContext->FillGeometry(m_pOilPointer, m_pInstrumentBrush);
		//end pointer;

		D2D1::Matrix3x2F rotationMatrixT = D2D1::Matrix3x2F::Rotation((m_temperatureangle - 36.0f), D2D1::Point2F(0, 0));
		pContext->m_pDeviceContext->SetTransform(rotationMatrixT * translationMatrixX);
		pContext->m_pDeviceContext->FillGeometry(m_pOilPointer, m_pInstrumentBrush);

		//Pointer cover plate...
		pContext->m_pDeviceContext->SetTransform(translationMatrixX);
		pContext->m_pDeviceContext->FillEllipse(D2D1::Ellipse(
			D2D1::Point2F(0.0f, 0.0f),
			(GRATICULE_INNER_CLIP / 3.0f)*m_instrumentScale,
			(GRATICULE_INNER_CLIP / 3.0f)*m_instrumentScale
		), this->m_pInstrumentCardBrush);


		return CInstrument::Render(pContext);
}

HRESULT COilInstrument::UnInitialise(CInstrumentPanelContext* pContext)
{
	CInstrument::UnInitialise(pContext);
	SafeRelease(&m_pInstrumentBrush);
	SafeRelease(&m_pOilGraticule);
	SafeRelease(&m_pOilCard);
	SafeRelease(&m_pOilPointer);
	return S_OK;
}
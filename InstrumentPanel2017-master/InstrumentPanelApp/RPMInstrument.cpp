#include "stdafx.h"
#include "RPMInstrument.h"


CRPMInstrument::CRPMInstrument()
	:CInstrument(0.6f,RPMINSTRUMENT_ID)
{

	m_pInstrumentBrush = NULL;
	m_pRPMCard = NULL;
	m_pRPMGraticule = NULL;
	m_pRPMPointer = NULL;

	m_pointerangle = 0.0f;

	m_xCaseOffset = (XCASE_OFFSET * m_instrumentScale);
	m_yCaseOffset = (YCASE_OFFSET * m_instrumentScale);

}

CRPMInstrument::CRPMInstrument(float scale)
	:CInstrument(scale, RPMINSTRUMENT_ID)
{

	m_pInstrumentBrush = NULL;
	m_pRPMCard = NULL;
	m_pRPMGraticule = NULL;
	m_pRPMPointer = NULL;

	m_pointerangle = 0.0f;

	m_xCaseOffset = (XCASE_OFFSET * m_instrumentScale);
	m_yCaseOffset = (YCASE_OFFSET * m_instrumentScale);

}

CRPMInstrument::~CRPMInstrument()
{
}

void CRPMInstrument::GetBounds(RECT* rect)
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

bool CRPMInstrument::PtInBounds(POINT pt)
{
	RECT r;
	GetBounds(&r);
	bool x = (pt.x >= r.left) && (pt.x < r.right);
	bool y = (pt.y >= r.top) && (pt.y < r.bottom);
	return (x == true && y == true) ? true : false;
}

HRESULT CRPMInstrument::Initialise(CInstrumentPanelContext* pContext)
{
	HRESULT hr = CInstrument::Initialise(pContext);
	if (SUCCEEDED(hr))
		HRESULT	hr = pContext->m_pRenderTarget->CreateSolidColorBrush(GetInstrumentColor(),&m_pInstrumentBrush);
	
	return CreateRPM(pContext);
}

HRESULT CRPMInstrument::CreateRPM(CInstrumentPanelContext* pContext)
{
	HRESULT hr = CreateRPMGraticule(pContext);
	if (SUCCEEDED(hr))
	{
		hr = CreateRPMCard(pContext);

		if (SUCCEEDED(hr))
			hr = CreateRPMPointer(pContext);
	}

	return hr;
}

HRESULT CRPMInstrument::CreateRPMGraticule(CInstrumentPanelContext* pContext)
{
	ID2D1GeometrySink *pSink = NULL;
	ID2D1PathGeometry* tempPath;
	HRESULT hr = pContext->m_pDirect2dFactory->CreatePathGeometry(&m_pRPMGraticule);
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
			if ((angle > (324.f + 1.0f) && angle < (360.f)) || (angle > 0 && angle < (36.f)))

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
		hr = m_pRPMGraticule->Open(&pSinkClip);
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

HRESULT CRPMInstrument::CreateRPMCard(CInstrumentPanelContext* pContext)
{
	ID2D1GeometrySink *pSink = NULL;
	ID2D1PathGeometry* tempPath;
	HRESULT hr = pContext->m_pDirect2dFactory->CreatePathGeometry(&m_pRPMCard);
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
			if ((angle > (324.f + 1.0f) && angle <= (360.f)) || (angle > 0 && angle < (36.0f)))

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
		hr = m_pRPMCard->Open(&pSinkClip);
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

HRESULT CRPMInstrument::CreateRPMPointer(CInstrumentPanelContext* pContext)
{
	HRESULT hr = S_OK;
	ID2D1GeometrySink *pSink = NULL;
	hr = pContext->m_pDirect2dFactory->CreatePathGeometry(&this->m_pRPMPointer);
	if (SUCCEEDED(hr))
	{

		hr = m_pRPMPointer->Open(&pSink);
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

HRESULT CRPMInstrument::Update(FSX_SIM_DATA* fsxData)
{ 
	if (fsxData->rpm <= 0.0)
	{
		m_pointerangle = 0.0f;
	}
	else {
		
		double dlim = fsxData->rpm > 3240.0 ? 3240.0 : fsxData->rpm;
		double d = ((dlim/3240.0)* (289.0));
		if ( d > 324.0)
			m_pointerangle = 324.0f;
		else
			m_pointerangle = (float)d;
		
	}
	return S_OK; 
}

HRESULT CRPMInstrument::PreRender(CInstrumentPanelContext* pContext)
{
	D2D1_SIZE_F size = pContext->m_pDeviceContext->GetSize();
	m_xPosition = INSTRUMENT_FAR_LEFT;
	m_yPosition = INSTRUMENT_TOP;
	SetNudge();

	DrawInstrumentCase(pContext);
	//Card
	D2D1::Matrix3x2F translationMatrixX = D2D1::Matrix3x2F::Translation(m_xPosition,m_yPosition);
	pContext->m_pDeviceContext->SetTransform(translationMatrixX);
	pContext->m_pDeviceContext->DrawGeometry(m_pRPMGraticule, m_pInstrumentBrush, 0.7f);
	pContext->m_pDeviceContext->DrawGeometry(m_pRPMCard, m_pInstrumentBrush, 2.2f);
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
		for (int i = 0; i < 9; i++)
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
			pContext->m_pDeviceContext->DrawTextW(c, (UINT32)wcslen(c), pContext->m_pText12ptFormat, rc, m_pInstrumentBrush);
		}
	}
	else {
		for (int i = 0; i < 9; i++)
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
	}


	DrawInstrumentTitle(pContext, L"RPM", m_pInstrumentBrush);
	wchar_t c[64];
	pContext->m_pText12ptFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	wcscpy_s(c, L"x1000");
	pContext->m_pDeviceContext->DrawTextW(c, (UINT32)wcslen(c), pContext->m_pText12ptFormat, D2D1::RectF(-165.0f, 45.0f, 165.0f, 55.0f), m_pInstrumentBrush);

	return S_OK;
}


HRESULT CRPMInstrument::Render(CInstrumentPanelContext* pContext)
{
		//pointer
		D2D1::Matrix3x2F translationMatrixX = D2D1::Matrix3x2F::Translation(m_xPosition, m_yPosition);
		D2D1::Matrix3x2F rotationMatrixX = D2D1::Matrix3x2F::Rotation((m_pointerangle+36.0f), D2D1::Point2F(0, 0));
		pContext->m_pDeviceContext->SetTransform(rotationMatrixX * translationMatrixX);
		pContext->m_pDeviceContext->FillGeometry(m_pRPMPointer, m_pInstrumentBrush);
		//end pointer;

		return CInstrument::Render(pContext);
}

HRESULT CRPMInstrument::UnInitialise(CInstrumentPanelContext* pContext)
{
	CInstrument::UnInitialise(pContext);
	SafeRelease(&m_pInstrumentBrush);
	SafeRelease(&m_pRPMGraticule);
	SafeRelease(&m_pRPMCard);
	SafeRelease(&m_pRPMPointer);
	return S_OK;
}
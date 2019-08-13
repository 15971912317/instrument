#include "stdafx.h"
#include "TurnAndSlip.h"


CTurnAndSlip::CTurnAndSlip()
	:CInstrument(0.85f,TURNANDBANKINSTRUMENT_ID)
{

	m_pInstrumentBrush = NULL;
	m_pSlipCard = NULL;
	m_pGraticule = NULL;
	m_pTurnPointer = NULL;
	m_pSlipBall = NULL;

	m_pointerangle = 0.0f;

	m_xCaseOffset = XCASE_OFFSET * m_instrumentScale;
	m_yCaseOffset = YCASE_OFFSET * m_instrumentScale;

}

CTurnAndSlip::CTurnAndSlip(float scale)
	:CInstrument(scale, TURNANDBANKINSTRUMENT_ID)
{

	m_pInstrumentBrush = NULL;
	m_pSlipCard = NULL;
	m_pGraticule = NULL;
	m_pTurnPointer = NULL;
	m_pSlipBall = NULL;

	m_pointerangle = 0.0f;

	m_xCaseOffset = XCASE_OFFSET * m_instrumentScale;
	m_yCaseOffset = YCASE_OFFSET * m_instrumentScale;

}

	CTurnAndSlip::~CTurnAndSlip()
	{
	}

	void CTurnAndSlip::GetBounds(RECT* rect)
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

	bool CTurnAndSlip::PtInBounds(POINT pt)
	{
		RECT r;
		GetBounds(&r);
		bool x = (pt.x >= r.left) && (pt.x < r.right);
		bool y = (pt.y >= r.top) && (pt.y < r.bottom);
		return (x == true && y == true) ? true : false;
	}


HRESULT CTurnAndSlip::Initialise(CInstrumentPanelContext* pContext)
{
	HRESULT hr = CInstrument::Initialise(pContext);
	if (SUCCEEDED(hr))
		HRESULT	hr = pContext->m_pRenderTarget->CreateSolidColorBrush(GetInstrumentColor(), &m_pInstrumentBrush);

	return Create(pContext);
}

HRESULT CTurnAndSlip::Create(CInstrumentPanelContext* pContext)
{
	HRESULT hr = CreateGraticule(pContext);
	if (SUCCEEDED(hr))
	{
		hr = CreateSlipCard(pContext);;

		if (SUCCEEDED(hr)){
			hr = CreateTurnPointer(pContext);
		}

		if (SUCCEEDED(hr)){
			hr = CreateSlipBall(pContext);
		}
	}

	return hr;
}

HRESULT CTurnAndSlip::CreateGraticule(CInstrumentPanelContext* pContext)
{
	ID2D1GeometrySink *pSink = NULL;
	ID2D1PathGeometry* tempPath;
	HRESULT hr = pContext->m_pDirect2dFactory->CreatePathGeometry(&m_pGraticule);
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

		float x1 = 0, x2 = 0, y1 = 0, y2 = 0;

		x1 = 0;
		x2 = 0;
		y1 = 0;
		y2 = GRATICULE_LENGTH * -m_instrumentScale;
		pSink->BeginFigure(D2D1::Point2F(x1, y1), D2D1_FIGURE_BEGIN_FILLED);
		pSink->AddLine(D2D1::Point2F(x2, y2));
		pSink->EndFigure(D2D1_FIGURE_END_OPEN);

		x1 = 0;
		x2 = 0;
		y1 = 0;
		y2 = GRATICULE_LENGTH * -m_instrumentScale;
		RotateLine(x1, y1, x2, y2, 30);
		pSink->BeginFigure(D2D1::Point2F(x1, y1), D2D1_FIGURE_BEGIN_FILLED);
		pSink->AddLine(D2D1::Point2F(x2, y2));
		pSink->EndFigure(D2D1_FIGURE_END_OPEN);


		x1 = 0;
		x2 = 0;
		y1 = 0;
		y2 = GRATICULE_LENGTH * -m_instrumentScale;
		RotateLine(x1, y1, x2, y2, -30);
		pSink->BeginFigure(D2D1::Point2F(x1, y1), D2D1_FIGURE_BEGIN_FILLED);
		pSink->AddLine(D2D1::Point2F(x2, y2));
		pSink->EndFigure(D2D1_FIGURE_END_OPEN);
		

		hr = pSink->Close();
		SafeRelease(&pSink);
	}
	ID2D1GeometrySink* pSinkClip;
	if (SUCCEEDED(hr))
	{
		hr = m_pGraticule->Open(&pSinkClip);
	}

	D2D1_ELLIPSE clip;
	ID2D1EllipseGeometry* clipEllipse;
	if (SUCCEEDED(hr))
	{
		//Create a clip ellipse.
		clip = D2D1::Ellipse(
			D2D1::Point2F(0.0f, 0.0f),
			(GRATICULE_INNER_CLIP)*m_instrumentScale,
			(GRATICULE_INNER_CLIP)*m_instrumentScale
			);
		hr = pContext->m_pDirect2dFactory->CreateEllipseGeometry(&clip, &clipEllipse);
	}
	if (SUCCEEDED(hr))
	{
		//There's no direct support for clipping path in Direct2D. So we can intersect a path with its clip instead.
		hr = tempPath->CombineWithGeometry(clipEllipse, D2D1_COMBINE_MODE_EXCLUDE , NULL, 0, pSinkClip);
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

HRESULT CTurnAndSlip::CreateSlipCard(CInstrumentPanelContext* pContext)
{
	ID2D1GeometrySink *pSink = NULL;
	HRESULT hr = pContext->m_pDirect2dFactory->CreatePathGeometry(&m_pSlipCard);
	if (SUCCEEDED(hr))
	{
		hr = m_pSlipCard->Open(&pSink);
	}
	if (SUCCEEDED(hr))
	{

		float slipboxhalflen = ((POINTER_LENGTH / 2.0f));
		float cp = 5.f;
		float cpy = 5.f;
		float bottom = 100.f;
		float top = (bottom - 35.0f);
		pSink->BeginFigure(
			D2D1::Point2F((slipboxhalflen * -(m_instrumentScale)), bottom*m_instrumentScale),
			D2D1_FIGURE_BEGIN_FILLED
			);
		pSink->AddBezier(D2D1::BezierSegment(
			D2D1::Point2F((cp * -(m_instrumentScale)), bottom*m_instrumentScale + cpy),
			D2D1::Point2F((cp * (m_instrumentScale)), bottom*m_instrumentScale + cpy),
			D2D1::Point2F((slipboxhalflen * (m_instrumentScale)), bottom*m_instrumentScale)
			));
		pSink->AddLine(D2D1::Point2F((slipboxhalflen * (m_instrumentScale)), top*m_instrumentScale));
		pSink->AddBezier(D2D1::BezierSegment(
			D2D1::Point2F((cp * (m_instrumentScale)), top*m_instrumentScale + cpy),
			D2D1::Point2F((cp * -(m_instrumentScale)), top*m_instrumentScale + cpy),
			D2D1::Point2F((slipboxhalflen * -(m_instrumentScale)), top*m_instrumentScale)
			));
		pSink->EndFigure(D2D1_FIGURE_END_CLOSED);

		hr = pSink->Close();
		SafeRelease(&pSink);
	}
	return hr;
}

HRESULT CTurnAndSlip::CreateTurnPointer(CInstrumentPanelContext* pContext)
{
	HRESULT hr = S_OK;
	ID2D1GeometrySink *pSink = NULL;
	hr = pContext->m_pDirect2dFactory->CreatePathGeometry(&this->m_pTurnPointer);
	if (SUCCEEDED(hr))
	{

		hr = m_pTurnPointer->Open(&pSink);
	}
	if (SUCCEEDED(hr))
	{
		
		pSink->BeginFigure(
			D2D1::Point2F(POINTER_THREE_QUARTER_WIDTH * -(m_instrumentScale), 0),
			D2D1_FIGURE_BEGIN_FILLED
			);
		pSink->AddLine(D2D1::Point2F(POINTER_THREE_QUARTER_WIDTH * -(m_instrumentScale), (POINTER_LENGTH_ANGLE * -(m_instrumentScale))));
		pSink->AddLine(D2D1::Point2F(POINTER_THREE_QUARTER_WIDTH * (m_instrumentScale), (POINTER_LENGTH_ANGLE * -(m_instrumentScale))));
		pSink->AddLine(D2D1::Point2F(POINTER_THREE_QUARTER_WIDTH * (m_instrumentScale), 0));
		pSink->EndFigure(D2D1_FIGURE_END_CLOSED);
		hr = pSink->Close();

	}
	SafeRelease(&pSink);
	return hr;
}

HRESULT CTurnAndSlip::CreateSlipBall(CInstrumentPanelContext* pContext)
{
	HRESULT hr = pContext->m_pDirect2dFactory->CreateRoundedRectangleGeometry(
		D2D1::RoundedRect(
		D2D1::RectF(-9.0f, -13.0f, 9.0f, 13.0f), 8.0f, 5.0f),
		&m_pSlipBall);

	return hr;
}

HRESULT CTurnAndSlip::Update(FSX_SIM_DATA* fsxData)
{
	m_pointerangle = (float)(fsxData->heading_rate * (57.295779513082320876798154814105))* 6.282f;
	
	if (m_pointerangle < -45.0f)
		m_pointerangle = -45.0f;
	if (m_pointerangle > 45.0f)
		m_pointerangle = 45.0f;

	m_slipvalue = (float)((fsxData->turnball) * 40);  // +- 40  map from +- 128
	return S_OK;
}

HRESULT CTurnAndSlip::PreRender(CInstrumentPanelContext* pContext)
{
	D2D1_SIZE_F size = pContext->m_pDeviceContext->GetSize();
	m_xPosition = INSTRUMENT_LEFT;
	m_yPosition = INSTRUMENT_BOTTOM;
	SetNudge();

	DrawInstrumentCase(pContext);

	D2D1::Matrix3x2F translationMatrixX = D2D1::Matrix3x2F::Translation(m_xPosition, m_yPosition);
	pContext->m_pDeviceContext->SetTransform(translationMatrixX);
	pContext->m_pDeviceContext->DrawGeometry(m_pGraticule, m_pInstrumentBrush, 8.5f);
	pContext->m_pDeviceContext->DrawGeometry(m_pSlipCard, m_pInstrumentBrush, 1.0f);
	pContext->m_pDeviceContext->FillGeometry(m_pSlipCard, m_pInstrumentBrush);
	//end card


	pContext->m_pText32ptFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	pContext->m_pText32ptFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pContext->m_pText18ptFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	pContext->m_pText18ptFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pContext->m_pText12ptFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);

	pContext->m_pText8ptFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	pContext->m_pText8ptFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);

	float x1, y1, x2, y2;
	wchar_t c[32];

	x1 = (GRATICULE_LENGTH * -m_instrumentScale);
	x2 = (GRATICULE_LENGTH * m_instrumentScale);
	y1 = (GRATICULE_LENGTH - 40.0f) * m_instrumentScale;
	y2 = GRATICULE_LENGTH;

	D2D1_RECT_F rc = D2D1::RectF(x1, y1, x2, y2);
	/*
	wcscpy_s(c, L"NO PITCH");
	pContext->m_pDeviceContext->DrawTextW(c, wcslen(c), pContext->m_pText8ptFormat, rc, m_pInstrumentBrush);

	y1 += 12.0f;
	rc = D2D1::RectF(x1, y1, x2, y2);
	wcscpy_s(c, L"INFORMATION");
	pContext->m_pDeviceContext->DrawTextW(c, wcslen(c), pContext->m_pText8ptFormat, rc, m_pInstrumentBrush);
	*/



	y1 = 120.0f * m_instrumentScale;
	y2 = 140.0f * m_instrumentScale;
	rc = D2D1::RectF(x1, y1, x2, y2);
	wcscpy_s(c, L"D.C. ELEC");
	pContext->m_pDeviceContext->DrawTextW(c, (UINT32)wcslen(c), pContext->m_pText8ptFormat, rc, m_pInstrumentBrush);

	y1 = 50.0f * m_instrumentScale;
	y2 = 0;
	rc = D2D1::RectF(x1, y1, x2, y2);
	wcscpy_s(c, L"2 MIN TURN");
	pContext->m_pDeviceContext->DrawTextW(c, (UINT32)wcslen(c), pContext->m_pText18ptFormat, rc, m_pInstrumentBrush);

	pContext->m_pText8ptFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

	x1 = 80.0f  * -m_instrumentScale;
	x2 = 60.0f  * -m_instrumentScale;
	y1 = 100.0f * -m_instrumentScale;
	y2 = 80.0f * -m_instrumentScale;
	rc = D2D1::RectF(x1, y1, x2, y2);
	wcscpy_s(c, L"L");
	pContext->m_pDeviceContext->DrawTextW(c, (UINT32)wcslen(c), pContext->m_pText32ptFormat, rc, m_pInstrumentBrush);

	x1 = 80.0f  * m_instrumentScale;
	x2 = 60.0f  * m_instrumentScale;
	y1 = 100.0f * -m_instrumentScale;
	y2 = 80.0f * -m_instrumentScale;
	rc = D2D1::RectF(x1, y1, x2, y2);
	wcscpy_s(c, L"R");
	pContext->m_pDeviceContext->DrawTextW(c, (UINT32)wcslen(c), pContext->m_pText32ptFormat, rc, m_pInstrumentBrush);

	return S_OK;
}


HRESULT CTurnAndSlip::Render(CInstrumentPanelContext* pContext)
{
	float slipPosition = 86.0f*m_instrumentScale;
	D2D1::Matrix3x2F translationMatrixSlip = D2D1::Matrix3x2F::Translation(m_xPosition + m_slipvalue, (m_yPosition + slipPosition) - (fabs(m_slipvalue) / 18.0f));
	pContext->m_pDeviceContext->SetTransform(translationMatrixSlip);
	pContext->m_pDeviceContext->FillGeometry(m_pSlipBall, m_pInstrumentCaseBrush);

	D2D1::Matrix3x2F translationMatrixY = D2D1::Matrix3x2F::Translation(m_xPosition, m_yPosition + slipPosition);	/*pointer*/
	pContext->m_pDeviceContext->SetTransform(translationMatrixY);
	pContext->m_pDeviceContext->DrawLine(
		D2D1::Point2F(-10.5, -16.0f),
		D2D1::Point2F(-10.5, 16.0f), m_pInstrumentCardBrush);
	pContext->m_pDeviceContext->DrawLine(
		D2D1::Point2F(10.5, -16.0f),
		D2D1::Point2F(10.5, 16.0f), m_pInstrumentCardBrush);

	D2D1::Matrix3x2F translationMatrixX = D2D1::Matrix3x2F::Translation(m_xPosition, m_yPosition);
	D2D1::Matrix3x2F rotationMatrixX = D2D1::Matrix3x2F::Rotation((m_pointerangle), D2D1::Point2F(0, 0));
	pContext->m_pDeviceContext->SetTransform(rotationMatrixX * translationMatrixX);
	pContext->m_pDeviceContext->DrawGeometry(m_pTurnPointer, m_pInstrumentBrush, 1.0f);
	pContext->m_pDeviceContext->FillGeometry(m_pTurnPointer, m_pInstrumentBrush);

	return CInstrument::Render(pContext);
}

HRESULT CTurnAndSlip::UnInitialise(CInstrumentPanelContext* pContext)
{
	CInstrument::UnInitialise(pContext);
	SafeRelease(&m_pInstrumentBrush);
	SafeRelease(&m_pGraticule);
	SafeRelease(&m_pSlipCard);
	SafeRelease(&m_pTurnPointer);
	SafeRelease(&m_pSlipBall);
	return S_OK;
}
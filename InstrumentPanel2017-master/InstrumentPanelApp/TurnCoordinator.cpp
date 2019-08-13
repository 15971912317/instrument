#include "stdafx.h"
#include "TurnCoordinator.h"


CTurnCoordinator::CTurnCoordinator()
	:CInstrument(0.85f, TURNANDBANKINSTRUMENT_ID)
{

	m_pInstrumentBrush = NULL;
	m_pSlipCard = NULL;
	m_pGraticule = NULL;
	m_pTurnPlane = NULL;
	m_pSlipBall = NULL;

	m_pointerangle = 0.0f;

	m_xCaseOffset = XCASE_OFFSET * m_instrumentScale;
	m_yCaseOffset = YCASE_OFFSET * m_instrumentScale;

}

CTurnCoordinator::CTurnCoordinator(float scale)
	:CInstrument(scale, TURNANDBANKINSTRUMENT_ID)
{

	m_pInstrumentBrush = NULL;
	m_pSlipCard = NULL;
	m_pGraticule = NULL;
	m_pTurnPlane = NULL;
	m_pSlipBall = NULL;

	m_pointerangle = 0.0f;

	m_xCaseOffset = XCASE_OFFSET * m_instrumentScale;
	m_yCaseOffset = YCASE_OFFSET * m_instrumentScale;

}

CTurnCoordinator::~CTurnCoordinator()
{
}

void CTurnCoordinator::GetBounds(RECT* rect)
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

bool CTurnCoordinator::PtInBounds(POINT pt)
{
	RECT r;
	GetBounds(&r);
	bool x = (pt.x >= r.left) && (pt.x < r.right);
	bool y = (pt.y >= r.top) && (pt.y < r.bottom);
	return (x == true && y == true) ? true : false;
}


HRESULT CTurnCoordinator::Initialise(CInstrumentPanelContext* pContext)
{
	HRESULT hr = CInstrument::Initialise(pContext);
	if (SUCCEEDED(hr))
		HRESULT	hr = pContext->m_pRenderTarget->CreateSolidColorBrush(GetInstrumentColor(), &m_pInstrumentBrush);

	return Create(pContext);
}

HRESULT CTurnCoordinator::Create(CInstrumentPanelContext* pContext)
{

	HRESULT hr = CreateGraticule(pContext);
	if (SUCCEEDED(hr))
	{
		hr = CreateSlipCard(pContext);;

		if (SUCCEEDED(hr)){
			hr = CreateTurnPlane(pContext);
		}

		if (SUCCEEDED(hr)){
			hr = CreateSlipBall(pContext);
		}
	}

	return hr;

}

HRESULT CTurnCoordinator::CreateGraticule(CInstrumentPanelContext* pContext)
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

		x1 = (GRATICULE_LENGTH * -m_instrumentScale);
		x2 = (GRATICULE_LENGTH * m_instrumentScale);
		y1 = 0;
		y2 = 0;

		pSink->BeginFigure(D2D1::Point2F(x1, y1), D2D1_FIGURE_BEGIN_HOLLOW);
		pSink->AddLine(D2D1::Point2F(x2, y2));
		pSink->EndFigure(D2D1_FIGURE_END_OPEN);

		x1 = 0;
		x2 = (GRATICULE_LENGTH * m_instrumentScale);
		y1 = 0;
		y2 = 0;
		RotateLine(x1, y1, x2, y2, 30);
		pSink->BeginFigure(D2D1::Point2F(x1, y1), D2D1_FIGURE_BEGIN_HOLLOW);
		pSink->AddLine(D2D1::Point2F(x2, y2));
		pSink->EndFigure(D2D1_FIGURE_END_OPEN);

		x1 = 0;
		x2 = (GRATICULE_LENGTH * m_instrumentScale);
		y1 = 0;
		y2 = 0;
		RotateLine(x1, y1, x2, y2, 150);
		pSink->BeginFigure(D2D1::Point2F(x1, y1), D2D1_FIGURE_BEGIN_HOLLOW);
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




HRESULT CTurnCoordinator::CreateSlipCard(CInstrumentPanelContext* pContext)
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

HRESULT CTurnCoordinator::CreateTurnPlane(CInstrumentPanelContext* pContext)
{
	HRESULT hr = S_OK;
	ID2D1GeometrySink *pSink = NULL;
	hr = pContext->m_pDirect2dFactory->CreatePathGeometry(&this->m_pTurnPlane);
	if (SUCCEEDED(hr))
	{

		hr = m_pTurnPlane->Open(&pSink);
	}
	if (SUCCEEDED(hr))
	{
		D2D1_ARC_SEGMENT seg = D2D1::ArcSegment(
			D2D1::Point2F((POINTER_LENGTH / 8.0f) * (m_instrumentScale), -1.0f),
			D2D1::SizeF((POINTER_EIGHTH_WIDTH)* (m_instrumentScale), (POINTER_EIGHTH_WIDTH)* (m_instrumentScale)),
			180.0f,
			D2D1_SWEEP_DIRECTION_CLOCKWISE,
			D2D1_ARC_SIZE_SMALL
			);


		pSink->BeginFigure(
			D2D1::Point2F((POINTER_LENGTH / 8.0f) * -(m_instrumentScale), -1.0f),
			D2D1_FIGURE_BEGIN_FILLED
			);

		pSink->AddArc(seg);
		pSink->EndFigure(D2D1_FIGURE_END_CLOSED);

		//Left wing
		pSink->BeginFigure(
			D2D1::Point2F((POINTER_LENGTH * -(m_instrumentScale)), 0),
			D2D1_FIGURE_BEGIN_FILLED
			);
		pSink->AddLine(D2D1::Point2F(0, POINTER_EIGHTH_WIDTH * -(m_instrumentScale)));
		pSink->AddLine(D2D1::Point2F(0, 0));
		pSink->EndFigure(D2D1_FIGURE_END_CLOSED);

		//Right Wing
		pSink->BeginFigure(
			D2D1::Point2F(0, 0),
			D2D1_FIGURE_BEGIN_FILLED
			);
		pSink->AddLine(D2D1::Point2F(0, POINTER_EIGHTH_WIDTH * -(m_instrumentScale)));
		pSink->AddLine(D2D1::Point2F((POINTER_LENGTH * (m_instrumentScale)), 0));
		pSink->EndFigure(D2D1_FIGURE_END_CLOSED);

		// vertical stablizer
		pSink->BeginFigure(
			D2D1::Point2F((POINTER_EIGHTH_WIDTH)* -(m_instrumentScale), -16.f* m_instrumentScale),
			D2D1_FIGURE_BEGIN_FILLED
			);
		pSink->AddLine(D2D1::Point2F(0, (POINTER_LENGTH / 3.0f) * -(m_instrumentScale)));
		pSink->AddLine(D2D1::Point2F((POINTER_EIGHTH_WIDTH)* (m_instrumentScale), -16.f* m_instrumentScale));
		pSink->EndFigure(D2D1_FIGURE_END_CLOSED);


		pSink->BeginFigure(
			D2D1::Point2F(((POINTER_LENGTH / 3.0f) * -(m_instrumentScale)), (POINTER_LENGTH / 5.0f) * -(m_instrumentScale)),
			D2D1_FIGURE_BEGIN_FILLED
			);
		pSink->AddLine(D2D1::Point2F(0, ((POINTER_LENGTH / 5.0f) + POINTER_EIGHTH_WIDTH) * -(m_instrumentScale)));
		pSink->AddLine(D2D1::Point2F(((POINTER_LENGTH / 3.0f) * (m_instrumentScale)), (POINTER_LENGTH / 5.0f) * -(m_instrumentScale)));
		pSink->EndFigure(D2D1_FIGURE_END_CLOSED);




		hr = pSink->Close();

	}
	SafeRelease(&pSink);
	return hr;
}

HRESULT CTurnCoordinator::CreateSlipBall(CInstrumentPanelContext* pContext)
{
	HRESULT hr = pContext->m_pDirect2dFactory->CreateRoundedRectangleGeometry(
		D2D1::RoundedRect(
		  D2D1::RectF(-9.0f, -13.0f, 9.0f, 13.0f), 8.0f, 5.0f), 
		  &m_pSlipBall);

	return hr;
}

HRESULT CTurnCoordinator::Update(FSX_SIM_DATA* fsxData)
{

	//wchar_t debugwStr[_MAX_PATH * 8];
	//swprintf(debugwStr, L"Heading Rate  %5.5f Turn %5.5f\r\n", fsxData->heading_rate, fsxData->turnball );
	//OutputDebugString(debugwStr);

	m_pointerangle = (float)(fsxData->heading_rate * (57.295779513082320876798154814105) )* 6.282f;
	m_slipvalue = (float)((fsxData->turnball) * 40);  // +- 40  map from +- 128
	return S_OK;
}

HRESULT CTurnCoordinator::PreRender(CInstrumentPanelContext* pContext)
{
	D2D1_SIZE_F size = pContext->m_pDeviceContext->GetSize();
	m_xPosition = INSTRUMENT_LEFT;
	m_yPosition = INSTRUMENT_BOTTOM;
	SetNudge();

	DrawInstrumentCase(pContext);

	D2D1::Matrix3x2F translationMatrixX = D2D1::Matrix3x2F::Translation(m_xPosition, m_yPosition);
	pContext->m_pDeviceContext->SetTransform(translationMatrixX);
	pContext->m_pDeviceContext->DrawGeometry(m_pGraticule, m_pInstrumentBrush, 3.5f);
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
	wcscpy_s(c, L"NO PITCH");
	pContext->m_pDeviceContext->DrawTextW(c, (UINT32)wcslen(c), pContext->m_pText8ptFormat, rc, m_pInstrumentBrush);

	y1 += 12.0f;
	rc = D2D1::RectF(x1, y1, x2, y2);
	wcscpy_s(c, L"INFORMATION");
	pContext->m_pDeviceContext->DrawTextW(c, (UINT32)wcslen(c), pContext->m_pText8ptFormat, rc, m_pInstrumentBrush);

	y1 = 140.0f * -m_instrumentScale;
	y2 = 0;
	rc = D2D1::RectF(x1, y1, x2, y2);
	wcscpy_s(c, L"D.C.");
	pContext->m_pDeviceContext->DrawTextW(c, (UINT32)wcslen(c), pContext->m_pText8ptFormat, rc, m_pInstrumentBrush);

	y1 = 128.0f * -m_instrumentScale;
	y2 = 0;
	rc = D2D1::RectF(x1, y1, x2, y2);
	wcscpy_s(c, L"ELEC");
	pContext->m_pDeviceContext->DrawTextW(c, (UINT32)wcslen(c), pContext->m_pText8ptFormat, rc, m_pInstrumentBrush);

	y1 = 50.0f * m_instrumentScale;
	y2 = 0;
	rc = D2D1::RectF(x1, y1, x2, y2);
	wcscpy_s(c, L"TURN COORDINATOR");
	pContext->m_pDeviceContext->DrawTextW(c, (UINT32)wcslen(c), pContext->m_pText12ptFormat, rc, m_pInstrumentBrush);

	pContext->m_pText8ptFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

	return S_OK;
}


HRESULT CTurnCoordinator::Render(CInstrumentPanelContext* pContext)
{
	float slipPosition = 86.0f*m_instrumentScale;
	D2D1::Matrix3x2F translationMatrixSlip = D2D1::Matrix3x2F::Translation(m_xPosition + m_slipvalue, (m_yPosition + slipPosition) - (fabs(m_slipvalue) / 18.0f));
	pContext->m_pDeviceContext->SetTransform(translationMatrixSlip);
	pContext->m_pDeviceContext->FillGeometry(m_pSlipBall, m_pInstrumentCaseBrush);

	D2D1::Matrix3x2F translationMatrixY = D2D1::Matrix3x2F::Translation(m_xPosition, m_yPosition + slipPosition);	/*pointer*/
	pContext->m_pDeviceContext->SetTransform(translationMatrixY);
	pContext->m_pDeviceContext->DrawLine(
		D2D1::Point2F(-10.5,  -16.0f),
		D2D1::Point2F(-10.5, 16.0f), m_pInstrumentCardBrush);
	pContext->m_pDeviceContext->DrawLine(
		D2D1::Point2F(10.5, -16.0f),
		D2D1::Point2F(10.5, 16.0f), m_pInstrumentCardBrush);

	D2D1::Matrix3x2F translationMatrixX = D2D1::Matrix3x2F::Translation(m_xPosition, m_yPosition);
	D2D1::Matrix3x2F rotationMatrixX = D2D1::Matrix3x2F::Rotation((m_pointerangle), D2D1::Point2F(0, 0));
	pContext->m_pDeviceContext->SetTransform(rotationMatrixX * translationMatrixX);
	pContext->m_pDeviceContext->DrawGeometry(m_pTurnPlane, m_pInstrumentBrush,1.0f);
	pContext->m_pDeviceContext->FillGeometry(m_pTurnPlane, m_pInstrumentBrush);



	return CInstrument::Render(pContext);
}

HRESULT CTurnCoordinator::UnInitialise(CInstrumentPanelContext* pContext)
{
	CInstrument::UnInitialise(pContext);
	SafeRelease(&m_pInstrumentBrush);
	SafeRelease(&m_pGraticule);
	SafeRelease(&m_pSlipCard);
	SafeRelease(&m_pTurnPlane);
	SafeRelease(&m_pSlipBall);
	return S_OK;
}

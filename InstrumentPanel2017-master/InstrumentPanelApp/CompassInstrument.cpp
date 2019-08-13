#include "stdafx.h"
#include "CompassInstrument.h"

#pragma warning(disable:4244)


CCompassInstrument::CCompassInstrument()
	:CInstrument(0.90f,COMPASSINSTRUMENT_ID)
{
	m_pInstrumentBrush = NULL;
	m_pInstrumentCaseBrush = NULL;

	m_pCompassCard = NULL;
	m_pCompassGraticule = NULL;
	m_pCompassClipper = NULL;
	m_pCompassPlane = NULL;

	m_pointerangle = 0.0f;

	m_xCaseOffset = XCASE_OFFSET * m_instrumentScale;
	m_yCaseOffset = YCASE_OFFSET * m_instrumentScale;
}

CCompassInstrument::CCompassInstrument(float scale)
	:CInstrument(scale, COMPASSINSTRUMENT_ID)
{
	m_pInstrumentBrush = NULL;
	m_pInstrumentCaseBrush = NULL;

	m_pCompassCard = NULL;
	m_pCompassGraticule = NULL;
	m_pCompassClipper = NULL;
	m_pCompassPlane = NULL;

	m_pointerangle = 0.0f;

	m_xCaseOffset = XCASE_OFFSET * m_instrumentScale;
	m_yCaseOffset = YCASE_OFFSET * m_instrumentScale;
}


CCompassInstrument::~CCompassInstrument()
{
}


void CCompassInstrument::GetBounds(RECT* rect)
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

bool CCompassInstrument::PtInBounds(POINT pt)
{
	RECT r;
	GetBounds(&r);
	bool x = (pt.x >= r.left) && (pt.x < r.right);
	bool y = (pt.y >= r.top) && (pt.y < r.bottom);
	return (x == true && y == true) ? true : false;
}

HRESULT CCompassInstrument::Initialise(CInstrumentPanelContext* pContext)
{
	HRESULT	hr = pContext->m_pRenderTarget->CreateSolidColorBrush(GetInstrumentColor(), &m_pInstrumentBrush);
	
	if (SUCCEEDED(hr))
		hr = pContext->m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(((BYTE)(246.0 * m_brightness) & 0xFF) << 16 | ((BYTE)(84.0 * m_brightness) & 0xFF) << 8 | ((BYTE)(106.0 * m_brightness) & 0xFF)), &m_pCardinalBrush);

	hr = CInstrument::Initialise(pContext);

	return CreateCompass(pContext);
}

HRESULT CCompassInstrument::CreateCompass(CInstrumentPanelContext* pContext)
{
	HRESULT hr = CreateCompassGraticule(pContext);
	if (SUCCEEDED(hr))
	{
		hr = CreateCompassCard(pContext);

		if (SUCCEEDED(hr)){
			hr = CreateInstrumentBitmap(pContext);

			if (SUCCEEDED(hr)){
				hr = CreateCompassClipper(pContext);

				if (SUCCEEDED(hr)){
					hr = CreateCompassPlane(pContext);
				}
			}
		}
	}

	return hr;
}

HRESULT CCompassInstrument::CreateCompassGraticule(CInstrumentPanelContext* pContext)
{
	ID2D1GeometrySink *pSink = NULL;
	ID2D1PathGeometry* tempPath;
	HRESULT hr = pContext->m_pDirect2dFactory->CreatePathGeometry(&m_pCompassGraticule);
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
		for (double angle = 10.0; angle <= 180.0; angle += 10.0)
		{
			if (fmod(angle, 30.0) != 0.0)
			{

				float x1 = 0, x2 = 0, y1 = -150*m_instrumentScale, y2 = 150 * m_instrumentScale;
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
		hr = m_pCompassGraticule->Open(&pSinkClip);
	}

	D2D1_ELLIPSE clip;
	ID2D1EllipseGeometry* clipEllipse;
	if (SUCCEEDED(hr))
	{
		//Create a clip ellipse.
		clip = D2D1::Ellipse(
			D2D1::Point2F(0.0f, 0.0f),
			140.0f*m_instrumentScale,
			140.0f*m_instrumentScale
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

HRESULT CCompassInstrument::CreateCompassCard(CInstrumentPanelContext* pContext)
{
	ID2D1GeometrySink *pSink = NULL;
	ID2D1PathGeometry* tempPath;
	HRESULT hr = pContext->m_pDirect2dFactory->CreatePathGeometry(&m_pCompassCard);
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
		pSink->BeginFigure(
			D2D1::Point2F(-10, 0),
			D2D1_FIGURE_BEGIN_FILLED
			);

		pSink->AddLine(D2D1::Point2F(0, -30 * m_instrumentScale));
		pSink->AddLine(D2D1::Point2F(10 * m_instrumentScale, 0));
		pSink->EndFigure(D2D1_FIGURE_END_CLOSED);


		pSink->BeginFigure(
			D2D1::Point2F(0, -150 * m_instrumentScale),
			D2D1_FIGURE_BEGIN_FILLED
			);
		pSink->AddLine(D2D1::Point2F(0, 150 * m_instrumentScale));
		pSink->EndFigure(D2D1_FIGURE_END_OPEN);

		for (double angle = 30.0; angle <= 180.0; angle += 30.0)
		{
			float x1 = 0, x2 = 0, y1 = -150 * m_instrumentScale, y2 = 150 * m_instrumentScale;
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
		hr = m_pCompassCard->Open(&pSinkClip);
	}

	D2D1_ELLIPSE clip;
	ID2D1EllipseGeometry* clipEllipse;
	if (SUCCEEDED(hr))
	{
		//Create a clip ellipse.
		clip = D2D1::Ellipse(
			D2D1::Point2F(0.0f, 0.0f),
			130.0f*m_instrumentScale,
			130.0f*m_instrumentScale
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

HRESULT CCompassInstrument::CreateCompassClipper(CInstrumentPanelContext* pContext)
{
	D2D1_ELLIPSE clip;

	//Create a clip ellipse.
	clip = D2D1::Ellipse(
		D2D1::Point2F(0.0f,0.0f),
		150.0f*m_instrumentScale,
		150.0f*m_instrumentScale
		);
	return pContext->m_pDirect2dFactory->CreateEllipseGeometry(&clip, &m_pCompassClipper);
}

HRESULT CCompassInstrument::CreateCompassPlane(CInstrumentPanelContext* pContext)
{
	ID2D1GeometrySink *pSink = NULL;
	HRESULT hr = pContext->m_pDirect2dFactory->CreatePathGeometry(&m_pCompassPlane);

	if (SUCCEEDED(hr))
	{
		hr = m_pCompassPlane->Open(&pSink);
	}
	if (SUCCEEDED(hr))
	{
		int yoff = 6;
		pSink->BeginFigure(
		D2D1::Point2F(-60 * m_instrumentScale, (0 + yoff)),
			D2D1_FIGURE_BEGIN_FILLED
			);

		pSink->AddLine(D2D1::Point2F(-60 * m_instrumentScale, (-6 + yoff)*m_instrumentScale));
		pSink->AddLine(D2D1::Point2F(-8 * m_instrumentScale, (-30 + yoff)*m_instrumentScale));
		pSink->AddLine(D2D1::Point2F(-6 * m_instrumentScale, (-60 + yoff)*m_instrumentScale));
		pSink->AddLine(D2D1::Point2F(0, (-85 + yoff)*m_instrumentScale));
		pSink->AddLine(D2D1::Point2F(6 * m_instrumentScale, (-60 + yoff)*m_instrumentScale));
		pSink->AddLine(D2D1::Point2F(8 * m_instrumentScale, (-30 + yoff)*m_instrumentScale));
		pSink->AddLine(D2D1::Point2F(60 * m_instrumentScale, (-6 + yoff)*m_instrumentScale));
		pSink->AddLine(D2D1::Point2F(60 * m_instrumentScale, (0 + yoff)*m_instrumentScale));

		pSink->AddLine(D2D1::Point2F(8 * m_instrumentScale, (-6 + yoff)*m_instrumentScale));
		pSink->AddLine(D2D1::Point2F(6 * m_instrumentScale, (20 + yoff)*m_instrumentScale));

		pSink->AddLine(D2D1::Point2F(25 * m_instrumentScale, (34 + yoff)*m_instrumentScale));  //Right tail
		pSink->AddLine(D2D1::Point2F(25 * m_instrumentScale, (40 + yoff)*m_instrumentScale));
		pSink->AddLine(D2D1::Point2F(8 * m_instrumentScale, (38 + yoff)*m_instrumentScale));

		pSink->AddLine(D2D1::Point2F(6 * m_instrumentScale, (35 + yoff)*m_instrumentScale));
		pSink->AddLine(D2D1::Point2F(0, (60 + yoff)*m_instrumentScale));  //Tail point
		pSink->AddLine(D2D1::Point2F(-6 * m_instrumentScale, (35 + yoff)*m_instrumentScale));

		pSink->AddLine(D2D1::Point2F(-8 * m_instrumentScale, (38 + yoff)*m_instrumentScale)); //Left tail
		pSink->AddLine(D2D1::Point2F(-25 * m_instrumentScale, (40 + yoff)*m_instrumentScale));
		pSink->AddLine(D2D1::Point2F(-25 * m_instrumentScale, (34 + yoff)*m_instrumentScale));
		

		pSink->AddLine(D2D1::Point2F(-6 * m_instrumentScale, (20 + yoff)*m_instrumentScale));
		pSink->AddLine(D2D1::Point2F(-8 * m_instrumentScale, (-6 + yoff)*m_instrumentScale));

		pSink->EndFigure(D2D1_FIGURE_END_CLOSED);

		pSink->BeginFigure(
			D2D1::Point2F(-120 * m_instrumentScale, 0),
			D2D1_FIGURE_BEGIN_FILLED
			);

		pSink->AddLine(D2D1::Point2F(120 * m_instrumentScale, 0));
		pSink->EndFigure(D2D1_FIGURE_END_CLOSED);
		pSink->BeginFigure(
			D2D1::Point2F(0, -120 * m_instrumentScale),
			D2D1_FIGURE_BEGIN_FILLED
			);

		pSink->AddLine(D2D1::Point2F(0, 120 * m_instrumentScale));
		pSink->EndFigure(D2D1_FIGURE_END_CLOSED);
		hr = pSink->Close();
		SafeRelease(&pSink);
	}
	return hr;

}

HRESULT CCompassInstrument::CreateInstrumentBitmap(CInstrumentPanelContext* pContext)
{
	static CARDINALPOINTS cardinalpoints[] = 
	{
		{  0, L"N", 1 },
		{ 30, L"3", 0 },
		{ 60, L"6", 0 },
		{ 90, L"E", 1 },
		{ 120, L"12", 0 },
		{ 150, L"15", 0 },
		{ 180, L"S", 1 },
		{ 210, L"21", 0 },
		{ 240, L"24", 0 },
		{ 270, L"W", 1 },
		{ 300, L"30", 0 },
		{ 330, L"33", 0 },
		{ 0, NULL}
	};
	// Create a bitmap of for the PreRendered instruments - e.g the static bits like cards and graticules.
	ID2D1BitmapRenderTarget *pCompatibleRenderTarget = NULL;
	D2D1_SIZE_F drawSize = D2D1::Size((175.0f*2)*m_instrumentScale, (175.0f*2)*m_instrumentScale);
	HRESULT hr = pContext->m_pRenderTarget->CreateCompatibleRenderTarget(drawSize, &pCompatibleRenderTarget);
	if (SUCCEEDED(hr))
	{
		pCompatibleRenderTarget->BeginDraw();

		D2D1::Matrix3x2F translationMatrixX = D2D1::Matrix3x2F::Translation(175.0f*m_instrumentScale, 175.0f*m_instrumentScale);
		pCompatibleRenderTarget->SetTransform(translationMatrixX);
		pCompatibleRenderTarget->DrawGeometry(m_pCompassGraticule, m_pInstrumentBrush, 0.7f);
		pCompatibleRenderTarget->DrawGeometry(m_pCompassCard, m_pInstrumentBrush, 2.2f);

		D2D1_RECT_F rc = D2D1::RectF(-10.0f, 0.0f, 10.0f, -130.0f*m_instrumentScale);
		pContext->m_pText18ptFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		CARDINALPOINTS* cpoints = cardinalpoints;
		while (cpoints->label != NULL)
		{
			pCompatibleRenderTarget->SetTransform(D2D1::Matrix3x2F::Rotation(cpoints->angle, D2D1::Point2F(0.0f, 0.0f))*translationMatrixX);
			pCompatibleRenderTarget->DrawTextW(cpoints->label, (UINT32)wcslen(cpoints->label), pContext->m_pText18ptFormat, rc, (cpoints->brushindex > 0)?m_pCardinalBrush:m_pInstrumentBrush);
			cpoints++;
		}
		pCompatibleRenderTarget->SetTransform(translationMatrixX);
		
		pCompatibleRenderTarget->EndDraw();
		// Retrieve the bitmap from the render target.
		hr = pCompatibleRenderTarget->GetBitmap(&m_pInstrumentBitmap);
	}
	SafeRelease(&pCompatibleRenderTarget);
	return hr;
}

HRESULT CCompassInstrument::Update(FSX_SIM_DATA* fsxData)
{
	m_pointerangle = (fsxData->heading * -57.295779513082320876798154814105);
	return S_OK;
}

HRESULT CCompassInstrument::PreRender(CInstrumentPanelContext* pContext)
{
	D2D1_SIZE_F size = pContext->m_pDeviceContext->GetSize();

	m_xPosition = INSTRUMENT_CENTER;
	m_yPosition = INSTRUMENT_BOTTOM;
	SetNudge();

	DrawInstrumentCase(pContext);

	ID2D1GeometrySink *pSink = NULL;
	ID2D1PathGeometry* pCompassPointer;
	HRESULT hr = pContext->m_pDirect2dFactory->CreatePathGeometry(&pCompassPointer);

	if (SUCCEEDED(hr))
	{
		hr = pCompassPointer->Open(&pSink);
	}
	if (SUCCEEDED(hr))
	{

		pSink->BeginFigure(
			D2D1::Point2F(-8.f* m_instrumentScale, -12.0f* m_instrumentScale),
			D2D1_FIGURE_BEGIN_FILLED
			);
		pSink->AddLine(D2D1::Point2F(8.f* m_instrumentScale, -12.0f* m_instrumentScale));
		pSink->AddLine(D2D1::Point2F(0.0f, 0.0f));
		pSink->EndFigure(D2D1_FIGURE_END_CLOSED);
		hr = pSink->Close();
		SafeRelease(&pSink);
	}
	if (SUCCEEDED(hr) )
	{
		D2D1::Matrix3x2F translationMatrix = D2D1::Matrix3x2F::Translation(m_xPosition, (m_yPosition - (m_yCaseOffset + (56.f* m_instrumentScale))));
		pContext->m_pDeviceContext->SetTransform(translationMatrix);
		pContext->m_pDeviceContext->FillGeometry(pCompassPointer, m_pInstrumentBrush);

		SafeRelease(&pCompassPointer);
		return S_OK;
	}
	SafeRelease(&pCompassPointer);
	return hr;
}


HRESULT CCompassInstrument::Render(CInstrumentPanelContext* pContext)
{
	//pointer
	D2D1::Matrix3x2F translationMatrixX = D2D1::Matrix3x2F::Translation(m_xPosition - (175.0f*m_instrumentScale), m_yPosition - (175.0f*m_instrumentScale));
	D2D1::Matrix3x2F rotationMatrixX = D2D1::Matrix3x2F::Rotation(m_pointerangle, D2D1::Point2F(175.0f*m_instrumentScale, 175.0f*m_instrumentScale));

	pContext->m_pDeviceContext->SetTransform(translationMatrixX);  //Put the layer at the right spot
	pContext->m_pDeviceContext->PushLayer(D2D1::LayerParameters(D2D1::InfiniteRect(), m_pCompassClipper, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE, D2D1::Matrix3x2F::Translation(175.0*m_instrumentScale, 175.0f*m_instrumentScale)), NULL);
	pContext->m_pDeviceContext->SetTransform(rotationMatrixX*translationMatrixX);  //rotate the compass card
	pContext->m_pDeviceContext->DrawBitmap(m_pInstrumentBitmap); //draw the compass card
	pContext->m_pDeviceContext->PopLayer();

	//Restore translation place and draw the plane
	pContext->m_pDeviceContext->SetTransform(D2D1::Matrix3x2F::Translation(m_xPosition, m_yPosition));  //Put the layer at the right spot
	pContext->m_pDeviceContext->DrawGeometry(m_pCompassPlane, m_pInstrumentBrush, 0.25f);
	pContext->m_pDeviceContext->FillGeometry(m_pCompassPlane, m_pInstrumentBrush);
	
	return CInstrument::Render(pContext);
}

HRESULT CCompassInstrument::UnInitialise(CInstrumentPanelContext* pContext)
{
	CInstrument::UnInitialise(pContext);
	SafeRelease(&m_pInstrumentBrush);
	SafeRelease(&m_pCardinalBrush);
	SafeRelease(&m_pCompassGraticule);
	SafeRelease(&m_pCompassCard);
	SafeRelease(&m_pCompassClipper);
	SafeRelease(&m_pCompassPlane);

	SafeRelease(&m_pInstrumentBitmap);
	
	return S_OK;
}

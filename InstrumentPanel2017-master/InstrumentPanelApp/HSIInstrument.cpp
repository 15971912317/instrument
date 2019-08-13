#include "stdafx.h"
#include "HSIInstrument.h"


CHSIInstrument::CHSIInstrument()
	:CInstrument(0.90f, HSIINSTRUMENT_ID)
{
	m_pInstrumentBrush = NULL;
	m_pInstrumentCaseBrush = NULL;

	m_pHSIInnerClipper = NULL;

	m_pInstrumentBitmap = NULL;

	m_pHSISkyBrush = NULL;
	m_pHSIGroundBrush = NULL;

	m_pointerangle = 0.0f;
	m_pointerpitch = 0.0f;

	m_xCaseOffset = XCASE_OFFSET * m_instrumentScale;
	m_yCaseOffset = YCASE_OFFSET * m_instrumentScale;

}

CHSIInstrument::CHSIInstrument(float scale)
	:CInstrument(scale, HSIINSTRUMENT_ID)
{
	m_pInstrumentBrush = NULL;
	m_pInstrumentCaseBrush = NULL;

	m_pHSIInnerClipper = NULL;

	m_pInstrumentBitmap = NULL;

	m_pHSISkyBrush = NULL;
	m_pHSIGroundBrush = NULL;

	m_pointerangle = 0.0f;
	m_pointerpitch = 0.0f;

	m_xCaseOffset = XCASE_OFFSET * m_instrumentScale;
	m_yCaseOffset = YCASE_OFFSET * m_instrumentScale;

}

void CHSIInstrument::GetBounds(RECT* rect)
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

bool CHSIInstrument::PtInBounds(POINT pt)
{
	RECT r;
	GetBounds(&r);
	bool x = (pt.x >= r.left) && (pt.x < r.right);
	bool y = (pt.y >= r.top) && (pt.y < r.bottom);
	return (x == true && y == true) ? true : false;
}


CHSIInstrument::~CHSIInstrument()
{
}


HRESULT CHSIInstrument::Initialise(CInstrumentPanelContext* pContext)
{

	HRESULT	hr = CInstrument::Initialise(pContext);
	if (SUCCEEDED(hr)) {
		SafeRelease(&m_pInstrumentCardBrush);

		hr = pContext->m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(((BYTE)(153.0 * m_brightness) & 0xFF) << 16 | ((BYTE)(153.0 * m_brightness) & 0xFF) << 8 | ((BYTE)(153.0 * m_brightness) & 0xFF)), &m_pInstrumentCardBrush);
	}

	if (SUCCEEDED(hr))
		hr = pContext->m_pRenderTarget->CreateSolidColorBrush(GetInstrumentColor(), &m_pInstrumentBrush);
	if (SUCCEEDED(hr))
		hr = pContext->m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(((BYTE)(242.0 * m_brightness) & 0xFF) << 16 | ((BYTE)(238.0 * m_brightness) & 0xFF) << 8 | ((BYTE)(5.0 * m_brightness) & 0xFF)), &m_pGullWingBrush);

	//hr = pContext->m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(0xF2EE05), &m_pGullWingBrush);
	return CreateHSI(pContext);
}

HRESULT CHSIInstrument::CreateHSI(CInstrumentPanelContext* pContext)
{
	HRESULT hr = CreateHSIBackgroundBrushes(pContext);
	if (SUCCEEDED(hr))
	{
		hr = CreateInstrumentBitmap(pContext);
	}
	if (SUCCEEDED(hr))
	{
		hr = CreateHSIInnerClipper(pContext);
	}

	if (SUCCEEDED(hr))
	{
		hr = CreateHSIRollPointer(pContext);
	}

	if (SUCCEEDED(hr))
	{
		hr = CreateHSIGullWing(pContext);
	}
	return hr;
}

HRESULT CHSIInstrument::CreateHSIRollPointer(CInstrumentPanelContext* pContext)
{
	HRESULT hr = S_OK;
	ID2D1GeometrySink *pSink = NULL;
	hr = pContext->m_pDirect2dFactory->CreatePathGeometry(&m_pHSIRollPointer);
	if (SUCCEEDED(hr))
	{

		hr = m_pHSIRollPointer->Open(&pSink);
	}
	if (SUCCEEDED(hr))
	{
		pSink->BeginFigure(
			D2D1::Point2F(165 * m_instrumentScale, 50 * m_instrumentScale),
			D2D1_FIGURE_BEGIN_FILLED
		);

		pSink->AddLine(D2D1::Point2F(175 * m_instrumentScale, 37 * m_instrumentScale));
		pSink->AddLine(D2D1::Point2F(185 * m_instrumentScale, 50 * m_instrumentScale));
		pSink->EndFigure(D2D1_FIGURE_END_CLOSED);
		hr = pSink->Close();
	}
	return hr;
}

HRESULT CHSIInstrument::CreateHSIGullWing(CInstrumentPanelContext* pContext)
{
	HRESULT hr = S_OK;
	ID2D1GeometrySink *pSink = NULL;
	hr = pContext->m_pDirect2dFactory->CreatePathGeometry(&m_pHSIGullWing);
	if (SUCCEEDED(hr))
	{

		hr = m_pHSIGullWing->Open(&pSink);
	}
	if (SUCCEEDED(hr))
	{
		pSink->BeginFigure(D2D1::Point2F(-100 * m_instrumentScale, 0), D2D1_FIGURE_BEGIN_FILLED);

		pSink->AddLine(D2D1::Point2F(-40 * m_instrumentScale, 0));
		pSink->AddLine(D2D1::Point2F(-20 * m_instrumentScale, 20 * m_instrumentScale));

		pSink->AddLine(D2D1::Point2F(0, 0));
		pSink->AddLine(D2D1::Point2F(20 * m_instrumentScale, 20 * m_instrumentScale));

		pSink->AddLine(D2D1::Point2F(40 * m_instrumentScale, 0));
		pSink->AddLine(D2D1::Point2F(100 * m_instrumentScale, 0));
		pSink->EndFigure(D2D1_FIGURE_END_OPEN);
		hr = pSink->Close();
	}
	return hr;
}


HRESULT CHSIInstrument::CreateHSIBackgroundBrushes(CInstrumentPanelContext* pContext)
{



	HRESULT hr = pContext->m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(((BYTE)(0.0 * m_brightness) & 0xFF) << 16 | ((BYTE)(180.0 * m_brightness) & 0xFF) << 8 | ((BYTE)(255.0 * m_brightness) & 0xFF)), &m_pHSISkyBrush);

	if (SUCCEEDED(hr))
	{
		hr = pContext->m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(((BYTE)(180.0 * m_brightness) & 0xFF) << 16 | ((BYTE)(108.0 * m_brightness) & 0xFF) << 8 | ((BYTE)(3.0 * m_brightness) & 0xFF)), &m_pHSIGroundBrush);

	}
	return hr;
}


HRESULT CHSIInstrument::CreateHSIInnerClipper(CInstrumentPanelContext* pContext)
{
	D2D1_ELLIPSE clip;

	//This is used in the Render method to clip the moving gimbal inside the case
	clip = D2D1::Ellipse(
		D2D1::Point2F(0.0f, 0.0f),
		138.0f*m_instrumentScale,
		138.0f*m_instrumentScale
	);
	return pContext->m_pDirect2dFactory->CreateEllipseGeometry(&clip, &m_pHSIInnerClipper);
}


HRESULT CHSIInstrument::CreateInstrumentBitmap(CInstrumentPanelContext* pContext)
{

	// Create a bitmap of for the PreRendered instruments - e.g the static bits like cards and graticules.
	ID2D1BitmapRenderTarget *pCompatibleRenderTarget = NULL;
	D2D1_SIZE_F drawSize = D2D1::Size(700.0f*m_instrumentScale, 700.0f*m_instrumentScale);
	HRESULT hr = pContext->m_pRenderTarget->CreateCompatibleRenderTarget(drawSize, &pCompatibleRenderTarget);
	if (SUCCEEDED(hr))
	{
		pCompatibleRenderTarget->BeginDraw();
		pCompatibleRenderTarget->Clear(GetInstrumentColor());
		pCompatibleRenderTarget->FillRectangle(D2D1::RectF(0.0f, 0.0f, 700.0f*m_instrumentScale, 349.3f*m_instrumentScale), m_pHSISkyBrush);
		pCompatibleRenderTarget->FillRectangle(D2D1::RectF(0.0f, 350.8f*m_instrumentScale, 700.0f*m_instrumentScale, 700.0f*m_instrumentScale), m_pHSIGroundBrush);

		pContext->m_pText18ptFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		D2D1_RECT_F rectPush = D2D1::RectF((350.0f - 50.0f)* m_instrumentScale, (350.0f - (79.0f*pitch_ratio))* m_instrumentScale, (350.0f + 50.0f)* m_instrumentScale, (350.0f - (79.0f*pitch_ratio))* m_instrumentScale);
		pCompatibleRenderTarget->DrawTextW(L"PUSH", 4, pContext->m_pText18ptFormat, rectPush, m_pInstrumentBrush);
		D2D1_RECT_F rectPull = D2D1::RectF((350.0f - 50.0f)* m_instrumentScale, (350.0f - (-79.0f*pitch_ratio))* m_instrumentScale, (350.0f + 50.0f)* m_instrumentScale, (350.0f - (-79.0f*pitch_ratio))* m_instrumentScale);
		pCompatibleRenderTarget->DrawTextW(L"PULL", 4, pContext->m_pText18ptFormat, rectPull, m_pInstrumentBrush);


		//Draw pitch graticule
		for (float pitch = 5.0f; pitch <= 25.0f; pitch += 5.0f)
		{
			float barwidth = 15.0f;
			if (fmod(pitch, 10) == 0) { barwidth = 25.0f; }
			pCompatibleRenderTarget->DrawLine(D2D1::Point2F((350.0f * m_instrumentScale) - barwidth, (350.0f - (pitch*pitch_ratio))* m_instrumentScale), D2D1::Point2F((350.0f* m_instrumentScale) + barwidth, (350.0f - (pitch*pitch_ratio))* m_instrumentScale), m_pInstrumentBrush, 1.2f);
			pCompatibleRenderTarget->DrawLine(D2D1::Point2F((350.0f * m_instrumentScale) - barwidth, (350.0f - (-pitch)*pitch_ratio)* m_instrumentScale), D2D1::Point2F((350.0f* m_instrumentScale) + barwidth, (350.0f - ((-pitch)*pitch_ratio))* m_instrumentScale), m_pInstrumentBrush, 1.2f);

		}
		pCompatibleRenderTarget->EndDraw();
		// Retrieve the bitmap from the render target.
		hr = pCompatibleRenderTarget->GetBitmap(&m_pInstrumentBitmap);
	}
	SafeRelease(&pCompatibleRenderTarget);
	return hr;
}



HRESULT CHSIInstrument::Update(FSX_SIM_DATA* fsxData)
{
	m_pointerangle = (float)(fsxData->bank_degrees * (57.295779513082320876798154814105));
	m_pointerpitch = (float)(fsxData->pitch_degrees * (-57.295779513082320876798154814105)); //5.0f;
	return S_OK;
}


HRESULT CHSIInstrument::PreRender(CInstrumentPanelContext* pContext)
{
	D2D1_SIZE_F size = pContext->m_pDeviceContext->GetSize();

	m_xPosition = INSTRUMENT_CENTER;
	m_yPosition = INSTRUMENT_TOP;
	SetNudge();



	//Clipping Ellipse for out glass
	D2D1_ELLIPSE instrumentCard = D2D1_ELLIPSE();
	instrumentCard.point = D2D1::Point2F(0, 0);
	instrumentCard.radiusX = 153.f * m_instrumentScale;
	instrumentCard.radiusY = 153.f * m_instrumentScale;
	ID2D1EllipseGeometry* pHSIOuterClipper;
	HRESULT hr = pContext->m_pDirect2dFactory->CreateEllipseGeometry(&instrumentCard, &pHSIOuterClipper);


	if (SUCCEEDED(hr))
	{
		//Position the case
		D2D1::Matrix3x2F translationMatrix = D2D1::Matrix3x2F::Translation(m_xPosition - m_xCaseOffset, m_yPosition - m_yCaseOffset);
		pContext->m_pDeviceContext->SetTransform(translationMatrix);

		//Draw the case
		D2D1_ROUNDED_RECT instrumentCase = D2D1::RoundedRect(D2D1::RectF(-70.0f* m_instrumentScale, -70.0f* m_instrumentScale, 260.0f* m_instrumentScale, 260.0f* m_instrumentScale), 20.f* m_instrumentScale, 20.f* m_instrumentScale);
		pContext->m_pDeviceContext->FillRoundedRectangle(instrumentCase, m_pInstrumentCaseBrush);


		//Position the card viewport
		D2D1::Matrix3x2F translationMatrixY = D2D1::Matrix3x2F::Translation(m_xPosition - (175.0f* m_instrumentScale), m_yPosition - (175.0f* m_instrumentScale));
		pContext->m_pDeviceContext->SetTransform(translationMatrixY);

		//Clip outer card
		pContext->m_pDeviceContext->PushLayer(D2D1::LayerParameters1(D2D1::InfiniteRect(), pHSIOuterClipper, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE, D2D1::Matrix3x2F::Translation(175.0f* m_instrumentScale, 175.0f* m_instrumentScale), 1.0f, NULL, D2D1_LAYER_OPTIONS1_IGNORE_ALPHA), NULL);

		//Translate outercard bitmap to center of the case
		D2D1_RECT_F destRect = D2D1::RectF(0.0F, 0.0F, 350.0f* m_instrumentScale, 350.0f* m_instrumentScale);
		D2D1_RECT_F srcRect = D2D1::RectF(175.0f* m_instrumentScale, 175.0f* m_instrumentScale, (350.0f* m_instrumentScale) + 175.0f * m_instrumentScale, (350.0f* m_instrumentScale) + 175.0f * m_instrumentScale);
		pContext->m_pDeviceContext->DrawBitmap(m_pInstrumentBitmap, &destRect, 1.0F, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, &srcRect);


		//Draw the roll bars
		float x, y, x1, y1;
		y = x1 = x = 0;
		y1 = -350.0f* m_instrumentScale;
		pContext->m_pDeviceContext->DrawLine(D2D1::Point2F(x + (175.0f* m_instrumentScale), y + (175.0f* m_instrumentScale)), D2D1::Point2F(x1 + (175.0f* m_instrumentScale), y1 + (175.0f* m_instrumentScale)), m_pInstrumentBrush, 2.0f);
		for (float rot = 10.0f; rot <= 30.0f; rot += 10.0f)
		{
			y = x1 = x = 0;	y1 = -350.0f* m_instrumentScale;
			RotateLine(x, y, x1, y1, rot);
			pContext->m_pDeviceContext->DrawLine(D2D1::Point2F(x + 175.0f* m_instrumentScale, y + 175.0f* m_instrumentScale), D2D1::Point2F(x1 + 175.0f* m_instrumentScale, y1 + 175.0f* m_instrumentScale), m_pInstrumentBrush, 1.5f);
			y = x1 = x = 0;	y1 = -350.0f* m_instrumentScale;
			RotateLine(x, y, x1, y1, (-rot));
			pContext->m_pDeviceContext->DrawLine(D2D1::Point2F(x + 175.0f* m_instrumentScale, y + 175.0f* m_instrumentScale), D2D1::Point2F(x1 + 175.0f* m_instrumentScale, y1 + 175.0f* m_instrumentScale), m_pInstrumentBrush, 1.5f);

		}
		y = x1 = x = 0;	y1 = -350.0f* m_instrumentScale;
		RotateLine(x, y, x1, y1, 60);
		pContext->m_pDeviceContext->DrawLine(D2D1::Point2F(x + 175.0f* m_instrumentScale, y + 175.0f* m_instrumentScale), D2D1::Point2F(x1 + 175.0f* m_instrumentScale, y1 + 175.0f* m_instrumentScale), m_pInstrumentBrush, 1.5f);
		y = x1 = x = 0;	y1 = -350.0f* m_instrumentScale;
		RotateLine(x, y, x1, y1, (-60));
		pContext->m_pDeviceContext->DrawLine(D2D1::Point2F(x + 175.0f* m_instrumentScale, y + 175.0f* m_instrumentScale), D2D1::Point2F(x1 + 175.0f* m_instrumentScale, y1 + 175.0f* m_instrumentScale), m_pInstrumentBrush, 1.5f);

		//Draw thin line at the inner edge of the outer card to diffentiate the pitch/roll card from the outer
		D2D1_ELLIPSE shadow = D2D1_ELLIPSE();
		shadow.point = D2D1::Point2F(175.0f* m_instrumentScale, 175.0f* m_instrumentScale);
		shadow.radiusX = 139 * m_instrumentScale;
		shadow.radiusY = 139 * m_instrumentScale;
		pContext->m_pDeviceContext->DrawEllipse(shadow, m_pInstrumentCardBrush, 1.2f);

		pContext->m_pDeviceContext->PopLayer();
	}

	SafeRelease(&pHSIOuterClipper);
	//pContext->m_pDeviceContext->SetTransform(translationMatrixX);



	return S_OK;
}


HRESULT CHSIInstrument::Render(CInstrumentPanelContext* pContext)
{
	D2D1::Matrix3x2F translationMatrixX = D2D1::Matrix3x2F::Translation(m_xPosition - (175.0f* m_instrumentScale), m_yPosition - (175.0f* m_instrumentScale));
	D2D1::Matrix3x2F rotationMatrixX = D2D1::Matrix3x2F::Rotation(m_pointerangle, D2D1::Point2F(175.0f* m_instrumentScale, 175.0f* m_instrumentScale));
	pContext->m_pDeviceContext->SetTransform(translationMatrixX);
	pContext->m_pDeviceContext->PushLayer(D2D1::LayerParameters1(D2D1::InfiniteRect(), m_pHSIInnerClipper, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE, D2D1::Matrix3x2F::Translation(175.0f* m_instrumentScale, 175.0f* m_instrumentScale), 1.0f, NULL, D2D1_LAYER_OPTIONS1_IGNORE_ALPHA), NULL);
	pContext->m_pDeviceContext->SetTransform(rotationMatrixX*translationMatrixX);  //rotate the compass card

	//Compute bitmap translation between -45 to +45 degress - where -45 = 87.5 and +45 = 262.5 (limits)
	float pointerpitchangle = m_pointerpitch;
	if (pointerpitchangle > 89.0f)
		pointerpitchangle = 89.0f;
	if (pointerpitchangle < -89.0f)
		pointerpitchangle = -89.0f;
	float y = ((350.0f*m_instrumentScale) - (pointerpitchangle*(3.8888f* m_instrumentScale)));


	D2D1_RECT_F destRect = D2D1::RectF(0.0F, 0.0F, 350.0f* m_instrumentScale, 350.0f* m_instrumentScale);
	D2D1_RECT_F srcRect = D2D1::RectF(175.0f* m_instrumentScale, y - 175.5f* m_instrumentScale, (350.0f* m_instrumentScale) + 175.0f* m_instrumentScale, y + 175.0f* m_instrumentScale);
	pContext->m_pDeviceContext->DrawBitmap(m_pInstrumentBitmap, &destRect, 1.0F, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, &srcRect);


	//Draw the roll pointer;
	pContext->m_pDeviceContext->FillGeometry(m_pHSIRollPointer, m_pInstrumentBrush);
	pContext->m_pDeviceContext->PopLayer();

	D2D1::Matrix3x2F translationMatrixY = D2D1::Matrix3x2F::Translation(m_xPosition /*- m_xCaseOffset*/, m_yPosition /* - m_xCaseOffset*/);

	pContext->m_pDeviceContext->SetTransform(translationMatrixY);
	pContext->m_pDeviceContext->DrawGeometry(m_pHSIGullWing, m_pGullWingBrush, 4.0f);

	//FOCUS
	D2D1::Matrix3x2F notranslationMatrix = D2D1::Matrix3x2F::Translation(m_xPosition - (175.0f* m_instrumentScale), m_yPosition - (175.0f* m_instrumentScale));
	pContext->m_pDeviceContext->SetTransform(notranslationMatrix);
	D2D1_ELLIPSE shadow = D2D1_ELLIPSE();
	shadow.point = D2D1::Point2F(175.0f* m_instrumentScale, 175.0f* m_instrumentScale);
	shadow.radiusX = 139 * m_instrumentScale;
	shadow.radiusY = 139 * m_instrumentScale;

	return CInstrument::Render(pContext);
}

HRESULT CHSIInstrument::UnInitialise(CInstrumentPanelContext* pContext)
{
	CInstrument::UnInitialise(pContext);

	SafeRelease(&m_pHSIGullWing);
	SafeRelease(&m_pHSIRollPointer);
	SafeRelease(&m_pHSIInnerClipper);
	SafeRelease(&m_pHSISkyBrush);
	SafeRelease(&m_pHSIGroundBrush);
	SafeRelease(&m_pGullWingBrush);
	SafeRelease(&m_pInstrumentBitmap);
	SafeRelease(&m_pInstrumentBrush);
	return S_OK;
}

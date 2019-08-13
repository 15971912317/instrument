#include "stdafx.h"
#include "InstrumentPanel.h"
#include "Instrument.h"

extern UINT nBackgroundCard;

double CInstrument::m_brightness = 0.0;


CInstrument::CInstrument(float Scale, int instrumentId)
{
	m_instrumentId = instrumentId;
	m_pInstrumentCaseBrush = NULL;
	m_pInstrumentCardBrush = NULL;
	m_instrumentScale = Scale;
	this->m_nudge_x = 0;
	this->m_nudge_y = 0;
	m_bHasFocus = false;
	
}


CInstrument::~CInstrument()
{
}


void CInstrument::Nudge(float x, float y)
{
	this->m_nudge_x = x;
	this->m_nudge_y = y;
}

void CInstrument::SetNudge()
{
	m_xPosition += m_nudge_x;
	m_yPosition += m_nudge_y;
}

void CInstrument::GetBounds(RECT* rect)
{
}

bool CInstrument::PtInBounds(POINT pt)
{
	return false;
}

HRESULT CInstrument::Initialise(CInstrumentPanelContext* pContext)
{
	HRESULT hr = pContext->m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF(0x000000)), &m_pInstrumentCaseBrush);
	if (SUCCEEDED(hr))
		hr = pContext->m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF(nBackgroundCard)), &m_pInstrumentCardBrush);
	if (SUCCEEDED(hr))
		hr = pContext->m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF(0xAA0000)), &m_pInstrumentFocusBrush);

	return hr;
}

D2D1::ColorF CInstrument::GetInstrumentColor()
{
	return D2D1::ColorF(((BYTE)(255.0 * m_brightness) & 0xFF) << 16 | ((BYTE)(255.0 * m_brightness) & 0xFF) << 8 | ((BYTE)(255.0 * m_brightness) & 0xFF));
}


HRESULT CInstrument::Update(FSX_SIM_DATA* fsxData)
{
	return S_OK;
}


HRESULT CInstrument::PreRender(CInstrumentPanelContext* pContext)
{
	return S_OK;
}

HRESULT CInstrument::Render(CInstrumentPanelContext* pContext)
{
	if (m_bHasFocus == true)
	{
		pContext->m_pDeviceContext->SetTransform(D2D1::IdentityMatrix());
		RECT r;
		this->GetBounds(&r);
		D2D1_RECT_F recttt = D2D1::RectF((float)r.left, (float)r.top, (float)r.right, (float)r.bottom);
		pContext->m_pDeviceContext->DrawRectangle(recttt, m_pInstrumentFocusBrush, 5.0f);
	}
	return S_OK;
}
HRESULT CInstrument::UnInitialise(CInstrumentPanelContext* pContext)
{
	SafeRelease(&m_pInstrumentCaseBrush);
	SafeRelease(&m_pInstrumentCardBrush);
	SafeRelease(&m_pInstrumentFocusBrush);
	return S_OK;
}


void CInstrument::RotateLine(float& x1, float& y1, float& x2, float& y2, double deg)
{
	int r, c, k;
	double rad, a[2][2], m[2][2], rot[2][2];
	a[0][0] = x1;
	a[0][1] = y1;
	a[1][0] = x2;
	a[1][1] = y2;
	rad = (deg*(3.14159265358979323846)) / 180;
	rot[0][0] = cos(rad);
	rot[0][1] = sin(rad);
	rot[1][0] = -sin(rad);
	rot[1][1] = rot[0][0];  //cos(rad)
	for (r = 0; r<2; r++)
		for (c = 0; c<2; c++)
		{
			m[r][c] = 0;
			for (k = 0; k<2; k++)
				m[r][c] = m[r][c] + a[r][k] * rot[k][c];
		}
	x1 = (float)m[0][0], y1 = (float)m[0][1], x2 = (float)m[1][0], y2 = (float)m[1][1];

}


void CInstrument::DrawInstrumentTitle(CInstrumentPanelContext* pContext, LPCWSTR szTitle, ID2D1SolidColorBrush* titleBrush)
{
	IDWriteTextFormat* pWriteFormat = (m_instrumentScale < 1.0f) ? pContext->m_pText18ptFormat : pContext->m_pText32ptFormat;
	pWriteFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	pWriteFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	pContext->m_pDeviceContext->DrawTextW(szTitle, (UINT32)wcslen(szTitle), pWriteFormat, D2D1::RectF(-35.0f, 10.0f, 35.0f, 60.0f), titleBrush);
}

void CInstrument::DrawInstrumentCase(CInstrumentPanelContext* pContext)
{
	D2D1_ROUNDED_RECT instrumentCase = D2D1::RoundedRect(D2D1::RectF(-70.0f*m_instrumentScale, -70.0f*m_instrumentScale, 260.0f*m_instrumentScale, 260.0f*m_instrumentScale), 20.f, 20.f);
	D2D1_ELLIPSE instrumentCard = D2D1_ELLIPSE();
	instrumentCard.point = D2D1::Point2F(m_xCaseOffset, m_yCaseOffset);
	instrumentCard.radiusX = 150.0f * m_instrumentScale;
	instrumentCard.radiusY = 150.0f * m_instrumentScale;

	D2D1::Matrix3x2F translationMatrix = D2D1::Matrix3x2F::Translation(m_xPosition - m_xCaseOffset, m_yPosition - m_yCaseOffset);
	pContext->m_pDeviceContext->SetTransform(translationMatrix);
	pContext->m_pDeviceContext->FillRoundedRectangle(instrumentCase, m_pInstrumentCaseBrush);
	pContext->m_pDeviceContext->FillEllipse(&instrumentCard, m_pInstrumentCardBrush);
}
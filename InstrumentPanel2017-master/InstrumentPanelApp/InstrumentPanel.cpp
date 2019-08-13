#include "stdafx.h"
#include "Nudge.h"
#include "InstrumentPanel.h"

#include "RPMInstrument.h"
#include "ASIInstrument.h"
#include "HSIInstrument.h"
#include "CompassInstrument.h"
#include "AltInstrument.h"
#include "VSIInstrument.h"
#include "TurnAndSlip.h"
#include "TurnCoordinator.h"
#include "ClockInstrument.h"
#include "OilInstrument.h"
#include "FuelInstrument.h"

extern UINT nBackgroundColor;

CInstrumentPanel::CInstrumentPanel(BOOL useTurnAndSlip, CNudgeCollection* nudges, double dBrightness)
{
	m_pSceneBitmap = NULL;

	m_context.m_pDirect2dFactory = NULL;
	m_context.m_pDWriteFactory = NULL;
	m_context.m_pRenderTarget = NULL;

	float xNudge = 30.0f;
	float yNudgeLower = 25.0f;
	float yNudgeUpper = 25.0f;
	float yNudgeCenter = 20.0f;

	CInstrument::m_brightness = dBrightness;

	CNudge* pNudge = nudges->Find(RPMINSTRUMENT_ID);
	CRPMInstrument* pRPM;
	if (pNudge != NULL) {
		pRPM = new CRPMInstrument(pNudge->scale());
		pRPM->Nudge((float)pNudge->m_x, (float)pNudge->m_y);
	}
	else {
		pRPM = new CRPMInstrument(0.55f);
		pRPM->Nudge(xNudge - 10, 65.0f);
	}
	m_instrumentlist.push_back(pRPM);


	// CLOCKINSTRUMENT_ID 8
	CClockInstrument* pClockX;
	pNudge = nudges->Find(CLOCKINSTRUMENT_ID);
	if (pNudge != NULL) {
		pClockX = new CClockInstrument(pNudge->scale());
		pClockX->Nudge((float)pNudge->m_x, (float)pNudge->m_y);
	}
	else {
		pClockX = new CClockInstrument(0.55f);
		pClockX->Nudge(xNudge - 10, -65.0f);
	}

	m_instrumentlist.push_back(pClockX);

	/* RHS instruments*/
// OILINSTRUMENT_ID 6
	COilInstrument* pOil;
	pNudge = nudges->Find(OILINSTRUMENT_ID);
	if (pNudge != NULL) {
		pOil = new COilInstrument(pNudge->scale());
		pOil->Nudge((float)pNudge->m_x, (float)pNudge->m_y);
	}
	else {
		pOil = new COilInstrument(0.55f);
		pOil->Nudge(xNudge + 1120, 65.0f);
	}
	m_instrumentlist.push_back(pOil);




#//FUELINSTRUMENT_ID 7
	CFuelInstrument* pFuel;
	pNudge = nudges->Find(FUELINSTRUMENT_ID);
	if (pNudge != NULL) {
		pFuel = new CFuelInstrument(pNudge->scale());
		pFuel->Nudge((float)pNudge->m_x, (float)pNudge->m_y);
	}
	else {
		pFuel = new CFuelInstrument(0.55f);
		pFuel->Nudge(xNudge + 1120, -65.0f);
	}
	m_instrumentlist.push_back(pFuel);


	if (useTurnAndSlip == TRUE)
	{
		// TURNANDBANKINSTRUMENT_ID 4
		CTurnAndSlip* pTurnAndSlip;
		pNudge = nudges->Find(TURNANDBANKINSTRUMENT_ID);
		if (pNudge != NULL) {
			pTurnAndSlip = new CTurnAndSlip(pNudge->scale());
			pTurnAndSlip->Nudge((float)pNudge->m_x, (float)pNudge->m_y);
		}
		else {
			pTurnAndSlip = new CTurnAndSlip(0.9f);
			pTurnAndSlip->Nudge(xNudge, -yNudgeUpper);
		}
		m_instrumentlist.push_back(pTurnAndSlip);
	}
	else
	{
		// TURNANDBANKINSTRUMENT_ID 4
		CTurnCoordinator* pTurnCoordinator;
		pNudge = nudges->Find(TURNANDBANKINSTRUMENT_ID);
		if (pNudge != NULL) {
			pTurnCoordinator = new CTurnCoordinator(pNudge->scale());
			pTurnCoordinator->Nudge((float)pNudge->m_x, (float)pNudge->m_y);
		}
		else {
			pTurnCoordinator = new CTurnCoordinator(0.9f);
			pTurnCoordinator->Nudge(xNudge, -yNudgeUpper);
		}
		m_instrumentlist.push_back(pTurnCoordinator);
	}

	// ASIINSTRUMENT_ID 9
	CASIInstrument* pASIInstrument = new CASIInstrument(0.9f);
	pNudge = nudges->Find(ASIINSTRUMENT_ID);
	if (pNudge != NULL) {
		pASIInstrument = new CASIInstrument(pNudge->scale());
		pASIInstrument->Nudge((float)pNudge->m_x, (float)pNudge->m_y);
	}
	else
	{
		pASIInstrument = new CASIInstrument(0.9f);
		pASIInstrument->Nudge(xNudge, yNudgeLower);
	}
	m_instrumentlist.push_back(pASIInstrument);


	// HSIINSTRUMENT_ID 1
	CHSIInstrument* pHSIInstrument;
	pNudge = nudges->Find(HSIINSTRUMENT_ID);
	if (pNudge != NULL) {
		pHSIInstrument = new CHSIInstrument(pNudge->scale());
		pHSIInstrument->Nudge((float)pNudge->m_x, (float)pNudge->m_y);
	}
	else {
		pHSIInstrument = new CHSIInstrument(1.0f);
		pHSIInstrument->Nudge(0.f, yNudgeCenter);
	}
	m_instrumentlist.push_back(pHSIInstrument);

	// COMPASSINSTRUMENT_ID 2
	CCompassInstrument* pCompassInstrument;
	pNudge = nudges->Find(COMPASSINSTRUMENT_ID);
	if (pNudge != NULL)
	{
		pCompassInstrument = new CCompassInstrument(pNudge->scale());
		pCompassInstrument->Nudge((float)pNudge->m_x, (float)pNudge->m_y);
	}
	else
	{
		pCompassInstrument = new CCompassInstrument(1.0f);
		pCompassInstrument->Nudge(0, -yNudgeCenter);
	}
	m_instrumentlist.push_back(pCompassInstrument);

	// ALTINSTRUMENT_ID 10
	CAltInstrument* pAltInstrument;
	pNudge = nudges->Find(ALTINSTRUMENT_ID);
	if (pNudge != NULL)
	{
		pAltInstrument = new CAltInstrument(pNudge->scale());
		pAltInstrument->Nudge((float)pNudge->m_x, (float)pNudge->m_y);
	}
	else {
		pAltInstrument = new CAltInstrument(0.9f);
		pAltInstrument->Nudge(-xNudge, yNudgeLower);
	}
	m_instrumentlist.push_back(pAltInstrument);

	// VSIIINSTRUMENT_ID 3
	CVSIInstrument* pVSIInstrument;
	pNudge = nudges->Find(VSIIINSTRUMENT_ID);
	if (pNudge != NULL){
		pVSIInstrument = new CVSIInstrument(pNudge->scale());
		pVSIInstrument->Nudge((float)pNudge->m_x, (float)pNudge->m_y);
	}
	else {
		pVSIInstrument = new CVSIInstrument(0.9f);
		pVSIInstrument->Nudge(-xNudge, -yNudgeUpper);
	}
	m_instrumentlist.push_back(pVSIInstrument);



}


CInstrumentPanel::~CInstrumentPanel()
{
	UnInitialise();
	for (auto instrument : m_instrumentlist)
	{
		delete instrument;
	}
	m_instrumentlist.clear();
}

void CInstrumentPanel::UpdateNudges(CNudgeCollection* nudges)
{
	for (auto instrument : m_instrumentlist)
	{
		CNudge* pNudge = nudges->Find(instrument->m_instrumentId);
		if (pNudge != NULL)
		{
			pNudge->m_x = (int)instrument->m_nudge_x;
			pNudge->m_y = (int)instrument->m_nudge_y;
		}
		else {
			nudges->Add(new CNudge(instrument->m_instrumentId, (int)instrument->m_nudge_x, (int)instrument->m_nudge_y, (int)(instrument->m_instrumentScale*100.0f)));
		}
	}

}

HRESULT CInstrumentPanel::Initialise()
{
	return CreateDeviceIndependentResources();
}


HRESULT CInstrumentPanel::Update(FSX_SIM_DATA* pSimData)
{
	for (auto instrument : m_instrumentlist)
	{
		instrument->Update(pSimData);
	}
	return S_OK;
}

bool CInstrumentPanel::GetDesktopDpi(float& dpiX, float& dpiY)
{
	if (m_context.m_pDirect2dFactory != NULL)
	{
		m_context.m_pDirect2dFactory->GetDesktopDpi(&dpiX, &dpiY);
		return true;
	}
	return false;
}

CInstrument* CInstrumentPanel::FindBoundedInstrument(POINT pt)
{

	for (auto instrument : m_instrumentlist)
	{
		if (instrument->PtInBounds(pt) == true)
			return instrument;
	}

	return NULL;
}

void CInstrumentPanel::SetAllInstrumentFocus(bool hasFocus)
{

	for (auto instrument : m_instrumentlist)
	{
		instrument->m_bHasFocus = hasFocus;
	}
}

CInstrument* CInstrumentPanel::FindFocussedInstrument()
{

	for (auto instrument : m_instrumentlist)
	{
		if (instrument->m_bHasFocus == true)
			return instrument;
	}
	return NULL;
}


HRESULT CInstrumentPanel::Render(bool bDesign)
{
	HRESULT hr = S_OK;
	hr = this->CreateDeviceResources();
	if (SUCCEEDED(hr))
	{
		ID2D1DeviceContext* device;
		hr = m_context.m_pRenderTarget->QueryInterface<ID2D1DeviceContext>(&device);
		m_context.m_pDeviceContext = device;

		m_context.m_pDeviceContext->BeginDraw();
		m_context.m_pDeviceContext->SetTransform(D2D1::Matrix3x2F::Identity());

		if (!bDesign)
			m_context.m_pDeviceContext->DrawBitmap(m_pSceneBitmap); //Draws the background with cases and graticules.
		else
		{
			D2D1_SIZE_F f = m_context.m_pDeviceContext->GetSize();
			D2D1_RECT_F r = D2D1::RectF(0.0f, 0.0f, f.width, f.height);
			m_context.m_pDeviceContext->FillRectangle(r, m_pInstrumentDesignBackgroundBrush);
		}

		for (auto instrument : m_instrumentlist)
		{
			if (bDesign)
				instrument->PreRender(&m_context);

			instrument->Render(&m_context);
		}
		m_context.m_pDeviceContext->EndDraw();
		SafeRelease(&device);
		m_context.m_pDeviceContext = NULL;
	}

	if (hr == D2DERR_RECREATE_TARGET)
	{
		hr = S_OK;
		DiscardResources();
	}
	return hr;
}

HRESULT CInstrumentPanel::UnInitialise()
{
	DiscardResources();
	return S_OK;
}

void CInstrumentPanel::SetWindow(HWND hWnd)
{
	m_hWnd = hWnd;
}

#pragma warning(disable:4305)
// Device independent resources are not specific to a particular device. For example, factory and font.
HRESULT CInstrumentPanel::CreateDeviceIndependentResources()
{
	HRESULT hr = S_OK;

#if defined(DXDEBUG) || defined(_DXDEBUG)
	D2D1_FACTORY_OPTIONS options;
	options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, options, &m_pDirect2dFactory);
	// Create Direct 2D factory.
#else
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_context.m_pDirect2dFactory);
#endif

	if (SUCCEEDED(hr))
	{

		// Create a DirectWrite factory.
		hr = DWriteCreateFactory(
			DWRITE_FACTORY_TYPE_SHARED,
			__uuidof(m_context.m_pDWriteFactory),
			reinterpret_cast<IUnknown **>(&m_context.m_pDWriteFactory)
		);
	}
	return hr;
}

HRESULT CInstrumentPanel::CreateDeviceResources()
{
	HRESULT hr = S_OK;
	if (!m_context.m_pRenderTarget)
	{
		// Create an HwndRenderTarget to draw to the hwnd.
		RECT rc;
		//GetClientRect(m_hWnd, &rc);
		rc.left = 0; rc.right = 1366; rc.top = 0; rc.bottom = 768;

		D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);
		hr = m_context.m_pDirect2dFactory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(this->m_hWnd, size),
			&m_context.m_pRenderTarget
		);


		if (SUCCEEDED(hr) && m_context.m_pDWriteFactory != NULL)

			hr = m_context.m_pDWriteFactory->CreateTextFormat(
				L"Segoe UI",                // Font family name.
				NULL,                       // Font collection (NULL sets it to use the system font collection).
				DWRITE_FONT_WEIGHT_REGULAR,
				DWRITE_FONT_STYLE_NORMAL,
				DWRITE_FONT_STRETCH_NORMAL,
				28.0f,
				L"en-us",
				&m_context.m_pText32ptFormat
			);



		hr = m_context.m_pDWriteFactory->CreateTextFormat(
			L"Segoe UI",                // Font family name.
			NULL,                       // Font collection (NULL sets it to use the system font collection).
			DWRITE_FONT_WEIGHT_REGULAR,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			18.0f,
			L"en-us",
			&m_context.m_pText18ptFormat
		);

		hr = m_context.m_pDWriteFactory->CreateTextFormat(
			L"Segoe UI",                // Font family name.
			NULL,                       // Font collection (NULL sets it to use the system font collection).
			DWRITE_FONT_WEIGHT_REGULAR,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			12.0f,
			L"en-us",
			&m_context.m_pText12ptFormat
		);

		hr = m_context.m_pDWriteFactory->CreateTextFormat(
			L"Segoe UI",                // Font family name.
			NULL,                       // Font collection (NULL sets it to use the system font collection).
			DWRITE_FONT_WEIGHT_REGULAR,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			9.0f,
			L"en-us",
			&m_context.m_pText8ptFormat
		);

		//Call each instrument to CreateDeviceResources
		if (SUCCEEDED(hr)) {
			for (auto instrument : m_instrumentlist)
			{
				instrument->Initialise(&m_context);
			}
		}

		m_context.m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF(0x0000F0)), &m_pInstrumentDesignBackgroundBrush);

		hr = CreateBackgroundBitmap();
	}
	return hr;
}

HRESULT CInstrumentPanel::CreateBackgroundBitmap()
{
	// Create a bitmap of for the PreRendered instruments - e.g the static bits like cards and graticules.
	ID2D1BitmapRenderTarget *pCompatibleRenderTarget = NULL;
	HRESULT hr = m_context.m_pRenderTarget->CreateCompatibleRenderTarget(&pCompatibleRenderTarget);
	if (SUCCEEDED(hr))
	{
		ID2D1DeviceContext* device;
		hr = pCompatibleRenderTarget->QueryInterface<ID2D1DeviceContext>(&device);

		if (SUCCEEDED(hr))
		{
			m_context.m_pDeviceContext = device;
			m_context.m_pDeviceContext->BeginDraw();
			m_context.m_pDeviceContext->SetTransform(D2D1::Matrix3x2F::Identity());
			//m_context.m_pDeviceContext->Clear(D2D1::ColorF(D2D1::ColorF(0x555555)));
			m_context.m_pDeviceContext->Clear(D2D1::ColorF(D2D1::ColorF(nBackgroundColor)));
			for (auto instrument : m_instrumentlist)
			{
				instrument->PreRender(&m_context);
			}
			m_context.m_pDeviceContext->EndDraw();
			// Retrieve the bitmap from the render target.
			hr = pCompatibleRenderTarget->GetBitmap(&m_pSceneBitmap);
			m_context.m_pDeviceContext = NULL;
		}
		SafeRelease(&device);
	}
	SafeRelease(&pCompatibleRenderTarget);
	return hr;
}

void CInstrumentPanel::DiscardResources()
{

	for (auto instrument : m_instrumentlist)
	{
		instrument->UnInitialise(&m_context);

	}
	//Release instrument list;

	SafeRelease(&m_context.m_pText32ptFormat);
	SafeRelease(&m_context.m_pText18ptFormat);
	SafeRelease(&m_context.m_pText12ptFormat);
	SafeRelease(&m_context.m_pText8ptFormat);

	SafeRelease(&m_pInstrumentDesignBackgroundBrush);
	SafeRelease(&m_pSceneBitmap);
	SafeRelease(&m_context.m_pRenderTarget);
	SafeRelease(&m_context.m_pDWriteFactory);
	SafeRelease(&m_context.m_pDirect2dFactory);
}
#pragma once

#include "InstrumentPanelContext.h"
class CInstrument;
class CNudgeCollection;

class CInstrumentPanel
{
public:
	CInstrumentPanel(BOOL useTurnAndSlip,CNudgeCollection* nudges, double dBrightness);
	~CInstrumentPanel();

public:
	HRESULT Initialise();
	HRESULT Update(FSX_SIM_DATA* fsxData);
	HRESULT Render(bool bDesign);
	HRESULT UnInitialise();
	void SetWindow(HWND hWnd);
	bool GetDesktopDpi(float& dpiX, float& dpiY);
	CInstrument* FindBoundedInstrument(POINT pt);
	CInstrument* FindFocussedInstrument();
	void SetAllInstrumentFocus(bool hasFocus);
	void UpdateNudges(CNudgeCollection* nudges);

public:
	CInstrumentPanelContext m_context;
private:
	HWND m_hWnd;
	
	HRESULT CreateDeviceIndependentResources();
	HRESULT CreateDeviceResources();
	void DiscardResources();

	HRESULT CreateBackgroundBitmap();
	ID2D1Bitmap* m_pSceneBitmap;
	ID2D1SolidColorBrush* m_pInstrumentDesignBackgroundBrush;
	
	std::list<CInstrument*> m_instrumentlist;
};


#pragma once
#include "Instrument.h"
#include "InstrumentPanelContext.h"

class CFuelInstrument :
	public CInstrument
{
public:
	CFuelInstrument();
	CFuelInstrument(float scale);
	~CFuelInstrument();
	HRESULT Initialise(CInstrumentPanelContext* pContext);
	HRESULT Update(FSX_SIM_DATA* fsxData);
	HRESULT PreRender(CInstrumentPanelContext* pContext);
	HRESULT Render(CInstrumentPanelContext* pContext);
	HRESULT UnInitialise(CInstrumentPanelContext* pContext);
public:
	virtual void GetBounds(RECT* rect);
	virtual bool PtInBounds(POINT pt);
private:
	ID2D1SolidColorBrush* m_pInstrumentBrush;

	ID2D1PathGeometry* m_pFuelCard;
	ID2D1PathGeometry* m_pFuelGraticule;
	ID2D1PathGeometry* m_pFuelPointer;

private:
	HRESULT CreateFuel(CInstrumentPanelContext* pContext);
	HRESULT CreateFuelGraticule(CInstrumentPanelContext* pContext);
	HRESULT CreateFuelCard(CInstrumentPanelContext* pContext);
	HRESULT CreateFuelPointer(CInstrumentPanelContext* pContext);

	float m_fuelleftangle;
	float m_fuelrightangle;

};


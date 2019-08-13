#pragma once
#include "Instrument.h"
#include "InstrumentPanelContext.h"

class COilInstrument :
	public CInstrument
{
public:
	COilInstrument();
	COilInstrument(float scale);
	~COilInstrument();
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

	ID2D1PathGeometry* m_pOilCard;
	ID2D1PathGeometry* m_pOilGraticule;
	ID2D1PathGeometry* m_pOilPointer;

private:
	HRESULT CreateOil(CInstrumentPanelContext* pContext);
	HRESULT CreateOilGraticule(CInstrumentPanelContext* pContext);
	HRESULT CreateOilCard(CInstrumentPanelContext* pContext);
	HRESULT CreateOilPointer(CInstrumentPanelContext* pContext);
private:
	double RankineToCelsius(double rankine);

	float m_pressureangle;
	float m_temperatureangle;

};


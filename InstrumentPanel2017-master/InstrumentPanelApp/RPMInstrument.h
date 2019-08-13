#pragma once
#include "Instrument.h"
#include "InstrumentPanelContext.h"

class CRPMInstrument :
	public CInstrument
{
public:
	CRPMInstrument();
	CRPMInstrument(float scale);
	~CRPMInstrument();
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

	ID2D1PathGeometry* m_pRPMCard;
	ID2D1PathGeometry* m_pRPMGraticule;
	ID2D1PathGeometry* m_pRPMPointer;

private:
	HRESULT CreateRPM(CInstrumentPanelContext* pContext);
	HRESULT CreateRPMGraticule(CInstrumentPanelContext* pContext);
	HRESULT CreateRPMCard(CInstrumentPanelContext* pContext);
	HRESULT CreateRPMPointer(CInstrumentPanelContext* pContext);

	float m_pointerangle;

};


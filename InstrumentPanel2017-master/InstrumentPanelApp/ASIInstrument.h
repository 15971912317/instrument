#pragma once
#include "Instrument.h"
#include "InstrumentPanelContext.h"

class CASIInstrument :
	public CInstrument
{
public:
	CASIInstrument();
	CASIInstrument(float scale);
	~CASIInstrument();
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

	ID2D1PathGeometry* m_pASICard;
	ID2D1PathGeometry* m_pASIGraticule;
	ID2D1PathGeometry* m_pASIPointer;

private:
	HRESULT CreateASI(CInstrumentPanelContext* pContext);
	HRESULT CreateASIGraticule(CInstrumentPanelContext* pContext);
	HRESULT CreateASICard(CInstrumentPanelContext* pContext);
	HRESULT CreateASIPointer(CInstrumentPanelContext* pContext);

	float m_pointerangle;
};


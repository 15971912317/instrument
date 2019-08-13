#pragma once
#include "Instrument.h"
#include "InstrumentPanelContext.h"

class CTurnCoordinator :
	public CInstrument
{
public:
	CTurnCoordinator();
	CTurnCoordinator(float scale);
	~CTurnCoordinator();
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

	ID2D1PathGeometry* m_pSlipCard;
	ID2D1PathGeometry* m_pGraticule;
	ID2D1PathGeometry* m_pTurnPlane;
	ID2D1RoundedRectangleGeometry* m_pSlipBall;

private:
	HRESULT Create(CInstrumentPanelContext* pContext);
	HRESULT CreateGraticule(CInstrumentPanelContext* pContext);
	HRESULT CreateSlipCard(CInstrumentPanelContext* pContext);
	HRESULT CreateSlipBall(CInstrumentPanelContext* pContext);
	HRESULT CreateTurnPlane(CInstrumentPanelContext* pContext);

	float m_pointerangle;
	float m_slipvalue;

};



#pragma once
#include "Instrument.h"
class CVSIInstrument :
	public CInstrument
{
public:
	CVSIInstrument();
	CVSIInstrument(float scale);
	~CVSIInstrument();

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

	ID2D1PathGeometry* m_pVSICard;
	ID2D1PathGeometry* m_pVSIGraticule;
	ID2D1PathGeometry* m_pVSIPointer;

private:
	HRESULT CreateVSI(CInstrumentPanelContext* pContext);
	HRESULT CreateVSIGraticule(CInstrumentPanelContext* pContext);
	HRESULT CreateVSICard(CInstrumentPanelContext* pContext);
	HRESULT CreateVSIPointer(CInstrumentPanelContext* pContext);

	float m_pointerangle;
};



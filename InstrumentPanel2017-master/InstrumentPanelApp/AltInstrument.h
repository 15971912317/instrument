#pragma once
#include "Instrument.h"
#include "InstrumentPanelContext.h"

class CAltInstrument :
	public CInstrument
{
public:
	CAltInstrument();
	CAltInstrument(float scale);
	~CAltInstrument();
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
	ID2D1PathGeometry* m_pAltCard;
	ID2D1PathGeometry* m_pAltGraticule;
	ID2D1PathGeometry* m_pAltHalfGraticule;
	ID2D1PathGeometry* m_pAltHundredsPointer;
	ID2D1PathGeometry* m_pAltThousandsPointer;
private:
	HRESULT CreateAlt(CInstrumentPanelContext* pContext);
	HRESULT CreateAltGraticule(CInstrumentPanelContext* pContext);
	HRESULT CreateAltGraticuleHalfWay(CInstrumentPanelContext* pContext);
	HRESULT CreateAltCard(CInstrumentPanelContext* pContext);
	HRESULT CreateAltPointers(CInstrumentPanelContext* pContext);
	HRESULT CreateAltHundredsPointer(CInstrumentPanelContext* pContext);
	HRESULT CreateAltThousandsPointer(CInstrumentPanelContext* pContext);

	float m_pointerhundredsangle;
	float m_pointerthousandsangle;
	int m_kohlsman;
};



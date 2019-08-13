#pragma once
#include "Instrument.h"
#include "InstrumentPanelContext.h"

class CClockInstrument :
	public CInstrument
{
public:
	CClockInstrument();
	CClockInstrument(float scale);
	~CClockInstrument();

public:
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

	ID2D1SolidColorBrush* m_pRedBrush;
	ID2D1SolidColorBrush* m_pGreenBrush;

	ID2D1PathGeometry* m_pClockCard;
	ID2D1PathGeometry* m_pClockGraticule;

	ID2D1PathGeometry* m_pClockHoursPointer;
	ID2D1PathGeometry* m_pClockMinutesPointer;
	ID2D1PathGeometry* m_pClockSecondsPointer;


private:
	HRESULT CreateClock(CInstrumentPanelContext* pContext);
	HRESULT CreateClockGraticule(CInstrumentPanelContext* pContext);
	HRESULT CreateClockCard(CInstrumentPanelContext* pContext);
	HRESULT CreateClockPointers(CInstrumentPanelContext* pContext);

	float m_hourspointerangle;
	float m_minutespointerangle;
	float m_secondspointerangle;

	int m_simstatus;

};


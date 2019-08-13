#pragma once
#include "Instrument.h"
#include "InstrumentPanelContext.h"

class CHSIInstrument :
	public CInstrument
{
public:
	CHSIInstrument();
	CHSIInstrument(float scale);
	~CHSIInstrument();
	HRESULT Initialise(CInstrumentPanelContext* pContext);
	HRESULT Update(FSX_SIM_DATA* fsxData);
	HRESULT PreRender(CInstrumentPanelContext* pContext);
	HRESULT Render(CInstrumentPanelContext* pContext);
	HRESULT UnInitialise(CInstrumentPanelContext* pContext);

	virtual void GetBounds(RECT* rect);
	virtual bool PtInBounds(POINT pt);

private:
	ID2D1SolidColorBrush* m_pInstrumentBrush;
	ID2D1SolidColorBrush* m_pHSISkyBrush;
	ID2D1SolidColorBrush* m_pHSIGroundBrush;
	ID2D1SolidColorBrush* m_pGullWingBrush;

	ID2D1EllipseGeometry* m_pHSIInnerClipper;
	ID2D1PathGeometry* m_pHSIRollPointer;
	ID2D1PathGeometry* m_pHSIGullWing;



	ID2D1Bitmap* m_pInstrumentBitmap;

private:
	HRESULT CreateHSI(CInstrumentPanelContext* pContext);
	HRESULT CreateHSICard(CInstrumentPanelContext* pContext);
	HRESULT CreateHSIRollPointer(CInstrumentPanelContext* pContext);
	HRESULT CreateHSIGullWing(CInstrumentPanelContext* pContext);


	HRESULT CreateHSIBackgroundBrushes(CInstrumentPanelContext* pContext);
	HRESULT CreateInstrumentBitmap(CInstrumentPanelContext* pContext);
	HRESULT CreateHSIInnerClipper(CInstrumentPanelContext* pContext);

	float m_pointerangle;
	float m_pointerpitch;

	const float pitch_ratio = 3.88888f;
};



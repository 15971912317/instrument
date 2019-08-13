#pragma once
#include "Instrument.h"
#include "InstrumentPanelContext.h"

typedef struct _tagCardinals
{
	float angle;
	LPCWSTR label;
	int brushindex;
}CARDINALPOINTS;

class CCompassInstrument :
	public CInstrument
{
public:
	CCompassInstrument();
	CCompassInstrument(float scale);
	~CCompassInstrument();

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
	ID2D1SolidColorBrush* m_pCardinalBrush;

	ID2D1PathGeometry* m_pCompassCard;
	ID2D1PathGeometry* m_pCompassGraticule;
	ID2D1PathGeometry* m_pCompassPlane;

	ID2D1EllipseGeometry* m_pCompassClipper;
	
	ID2D1Bitmap* m_pInstrumentBitmap;

private:
	HRESULT CreateCompass(CInstrumentPanelContext* pContext);
	HRESULT CreateCompassGraticule(CInstrumentPanelContext* pContext);
	HRESULT CreateCompassCard(CInstrumentPanelContext* pContext);
	//HRESULT CreateCompassPointer(CInstrumentPanelContext* pContext);
	HRESULT CreateCompassClipper(CInstrumentPanelContext* pContext);
	HRESULT CreateCompassPlane(CInstrumentPanelContext* pContext);

	HRESULT CCompassInstrument::CreateInstrumentBitmap(CInstrumentPanelContext* pContext);
	

	float m_pointerangle;


};


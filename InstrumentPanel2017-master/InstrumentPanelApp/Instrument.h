#pragma once

//#define INSTRUMENT_HORIZ_SPACE 50
#define INSTRUMENT_VERT_SPACE 50

/*
#define INSTRUMENT_LEFT ((size.width / 4) -(INSTRUMENT_HORIZ_SPACE))
#define INSTRUMENT_CENTER (size.width / 2)
#define INSTRUMENT_RIGHT (((size.width /4)*3) +INSTRUMENT_HORIZ_SPACE)
*/
#define INSTRUMENT_HORIZ_SPACE 340

#define INSTRUMENT_LEFT ((size.width / 2) -(INSTRUMENT_HORIZ_SPACE))
#define INSTRUMENT_FAR_LEFT ((size.width / 2) -(INSTRUMENT_HORIZ_SPACE+((INSTRUMENT_HORIZ_SPACE/4)*3)-10) )
#define INSTRUMENT_FAR_RIGHT ((size.width / 2) + (INSTRUMENT_HORIZ_SPACE+((INSTRUMENT_HORIZ_SPACE/4)*3) - 70 ) )
#define INSTRUMENT_CENTER (size.width / 2)
#define INSTRUMENT_RIGHT (((size.width /2)) +INSTRUMENT_HORIZ_SPACE)

#define INSTRUMENT_TOP ((size.height / 3) -(INSTRUMENT_VERT_SPACE) )
#define INSTRUMENT_MIDLINE ((size.height / 2) -(INSTRUMENT_VERT_SPACE/4) )
#define INSTRUMENT_BOTTOM ((size.height / 3) + 300)

#define HSIINSTRUMENT_ID 1
#define COMPASSINSTRUMENT_ID 2
#define VSIIINSTRUMENT_ID 3
#define TURNANDBANKINSTRUMENT_ID 4
#define RPMINSTRUMENT_ID 5
#define OILINSTRUMENT_ID 6
#define FUELINSTRUMENT_ID 7
#define CLOCKINSTRUMENT_ID 8
#define ASIINSTRUMENT_ID 9 
#define ALTINSTRUMENT_ID 10





class CInstrumentPanelContext;

class CInstrument
{
public:
	CInstrument(float Scale = 1.0f, int instrumentId  = 0);
	~CInstrument();
public:
	virtual HRESULT Initialise(CInstrumentPanelContext* pContext);
	virtual HRESULT Update(FSX_SIM_DATA* fsxData);
	virtual HRESULT PreRender(CInstrumentPanelContext* pContext);
	virtual HRESULT Render(CInstrumentPanelContext* pContext);
	virtual HRESULT UnInitialise(CInstrumentPanelContext* pContext);

	virtual void Nudge(float x, float y);
	virtual void GetBounds(RECT* rect);
	virtual bool PtInBounds(POINT pt);

protected:
	void SetNudge();
	void RotateLine(float& x1, float& y1, float& x2, float& y2, double deg);
	virtual void DrawInstrumentTitle(CInstrumentPanelContext* pContext, LPCWSTR szTitle, ID2D1SolidColorBrush* titleBrush);	
	virtual void DrawInstrumentCase(CInstrumentPanelContext* pContext);

	
	D2D1::ColorF GetInstrumentColor();

	ID2D1SolidColorBrush* m_pInstrumentCaseBrush;
	ID2D1SolidColorBrush* m_pInstrumentCardBrush;
	ID2D1SolidColorBrush* m_pInstrumentFocusBrush;
public:
	float m_xPosition;
	float m_yPosition;
	float m_xCaseOffset;
	float m_yCaseOffset;

	float m_instrumentScale;

	int m_instrumentId;
	bool m_bHasFocus;
	float m_nudge_x;
	float m_nudge_y;
	
	static double m_brightness;
};


#pragma once

#include "stdafx.h"

#define XCASE_OFFSET 95
#define YCASE_OFFSET 95

#define GRATICULE_LENGTH 150.0f
#define GRATICULE_OUTER_CLIP (GRATICULE_LENGTH-10.0f)
#define GRATICULE_INNER_CLIP (GRATICULE_LENGTH-20.0f)

#define POINTER_LENGTH 130.0f
#define POINTER_LENGTH_ANGLE 120.0f
#define POINTER_OVERRUN -10.0f
#define POINTER_HALF_WIDTH 5.0f
#define POINTER_QUARTER_WIDTH 4.0f
#define POINTER_THREE_QUARTER_WIDTH 5.0f
#define POINTER_EIGHTH_WIDTH 2.0f

class CInstrumentPanelContext
{
public:
	ID2D1Factory* m_pDirect2dFactory;
	IDWriteFactory* m_pDWriteFactory;
	ID2D1HwndRenderTarget* m_pRenderTarget;

	IDWriteTextFormat* m_pText32ptFormat;
	IDWriteTextFormat* m_pText18ptFormat;
	IDWriteTextFormat* m_pText12ptFormat;
	IDWriteTextFormat* m_pText8ptFormat;

	ID2D1DeviceContext* m_pDeviceContext;

public:
	CInstrumentPanelContext() { m_pDeviceContext = NULL;  }
};
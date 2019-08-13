// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

// Windows Header Files:
#include <windows.h>
#include <Windowsx.h>
#include <time.h>
//#include <d2d1.h>
#include <D2d1_1.h>
#include <dwrite.h>
#include <wincodec.h>
#include <wincodecsdk.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <process.h>
#include <Shellapi.h>
#include <Shlwapi.h>

#include <atltrace.h>

#include "CkSocketW.h"
#include "CkByteData.h"
#include "CkJsonObjectW.h"
#include "CkJsonArrayW.h"
#include <CkFileAccessW.h>


#include <list>
using namespace std;

#include "FsxDataSet.h"


// TODO: reference additional headers your program requires here
//Standard SafeRelease function.
template<class Interface>
inline void SafeRelease(
	Interface **ppInterfaceToRelease
	)
{
	if (*ppInterfaceToRelease != NULL)
	{
		(*ppInterfaceToRelease)->Release();

		(*ppInterfaceToRelease) = NULL;
	}
}



// TODO: reference additional headers your program requires here

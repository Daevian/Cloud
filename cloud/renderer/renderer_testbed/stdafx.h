// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>


// TODO: reference additional headers your program requires here

#include <string>
#include <sstream>
#include <fstream>

#include <d3d11.h>
#include <d3dx11.h>

#include <math.h>

#include "../../core_libraries/utilities/Windows.h"

#include "../../core_libraries/utilities/DefinesAsserts.h"
#include "../../core_libraries/utilities/DefinesMacros.h"
#include "../../core_libraries/utilities/DefinesTypes.h"

#include "../../core_libraries/debugging/DebugLog.h"

#include "../../core_libraries/utilities/IntrinsicFunctions.h"
#include "../../core_libraries/utilities/StaticArray.h"
#include "../../core_libraries/utilities/DynamicArray.h"

#include "../../core_libraries/utilities/Float4.h"
#include "../../core_libraries/utilities/Float3.h"
#include "../../core_libraries/utilities/Float2.h"


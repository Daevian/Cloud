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

#include <d3d11_1.h>

#include <vector>
#include <array>
#include <map>
#include <math.h>
#include <algorithm>
#include <functional>
#include <memory>

#include "utilities/DefinesAsserts.h"
#include "utilities/DefinesMacros.h"
#include "utilities/DefinesTypes.h"

#include "utilities/Timer.h"
#include "utilities/ClString.h"
#include "utilities/ClStringStream.h"

#include "utilities/IntrinsicFunctions.h"
#include "utilities/StaticArray.h"
#include "utilities/DynamicArray.h"

#include "utilities/MathUtilities.h"
#include "utilities/Float4.h"
#include "utilities/Float3.h"
#include "utilities/Float2.h"
#include "utilities/Matrix4.h"

#include "debugging/DebugLog.h"

#include "renderer/DirectxHelpers.h"

#include "jsoncpp-src-0.5.0/include/json/json.h"
#include "lua-5.3.2/LuaWrapper.h"
#include "WinConsoleLogHandler.h"

#include "../utilities/windows.h"

void Cloud::Debug::WinConsoleLogHandler::HandleTrace(const CLchar* output) const
{
    OutputDebugStringA(output);
    OutputDebugStringA("\n");
}
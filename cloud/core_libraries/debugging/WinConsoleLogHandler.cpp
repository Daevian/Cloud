#include "WinConsoleLogHandler.h"

#include "../utilities/windows.h"

void Cloud::Debug::WinConsoleLogHandler::HandleTrace(const t_char* output) const
{
    OutputDebugStringA(output);
    OutputDebugStringA("\n");
}
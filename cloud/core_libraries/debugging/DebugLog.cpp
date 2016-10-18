#include "DebugLog.h"
#include "BaseLogHandler.h"
#include "WinConsoleLogHandler.h"
#include "../utilities/DefinesAsserts.h"
#include "../utilities/DefinesMacros.h"

Cloud::Debug::DebugLog* Cloud::Debug::DebugLog::s_instance = 0;

Cloud::Debug::DebugLog::DebugLog()
{
    for (CLint i = 0; i < MAX_HANDLERS; ++i)
    {
        m_logHandlers[i] = 0;
    }    
}

Cloud::Debug::DebugLog::~DebugLog()
{
    for (CLint i = 0; i < MAX_HANDLERS; ++i)
    {
        if (m_logHandlers[i] != 0)
        {
            SAFE_DELETE(m_logHandlers[i]);
        }
    }
}

CLbool Cloud::Debug::DebugLog::Create()
{
    CL_ASSERT(s_instance == 0, "DebugLog already created. Can't re-create!");

    if (s_instance == 0)
    {
        s_instance = new DebugLog();
        return s_instance->Initialise();
    }

    return false;
}

void Cloud::Debug::DebugLog::Destroy()
{
    CL_ASSERT(s_instance != 0, "DebugLog not created yet. Can't destroy!");

    if (s_instance != 0)
    {
        s_instance->Shutdown();
        SAFE_DELETE(s_instance);
    }
}

CLbool Cloud::Debug::DebugLog::Initialise()
{
    AddLogHandler(new Cloud::Debug::WinConsoleLogHandler());

    Debug::DebugLog::Instance().Trace("INIT", "[DebugLog] Initialised!");
    return true;
}

void Cloud::Debug::DebugLog::Shutdown()
{
}

void Cloud::Debug::DebugLog::Trace(const CLchar* output, ...)
{
    va_list argList;
    va_start(argList, output);

    WriteToChannel("GLOBAL", output, argList);

    va_end(argList);    
}

void Cloud::Debug::DebugLog::TraceChannel(const CLchar* channel, const CLchar* output, ...)
{
    va_list argList;
    va_start(argList, output);

    WriteToChannel(channel, output, argList);

    va_end(argList);
}

void Cloud::Debug::DebugLog::WriteToChannel(const CLchar* channel, const CLchar* output, va_list args)
{
    std::array<CLchar, MAX_CHARS> buffer;
    WriteToBuffer(buffer, output, args);

    ChannelIterator i = m_channelList.find(channel);
    if (i == m_channelList.end())
    {
        m_channelList.insert(Channel(channel, true));
        PushBufferToHandlers(&buffer[0]);
        return;
    }

    if (i->second)
    {
        PushBufferToHandlers(&buffer[0]);
    }
}

void Cloud::Debug::DebugLog::WriteToBuffer(std::array<CLchar, MAX_CHARS>& buffer, const CLchar* output, va_list args)
{
    vsnprintf_s(&buffer[0], MAX_CHARS, MAX_CHARS, output, args);
    buffer[MAX_CHARS - 1] = '\0';
}

void Cloud::Debug::DebugLog::PushBufferToHandlers(const CLchar* buffer)
{
    for (CLint i = 0; i < MAX_HANDLERS; ++i)
    {
        if (m_logHandlers[i] != 0)
        {
            m_logHandlers[i]->HandleTrace(buffer);
        }
    }
}

void Cloud::Debug::DebugLog::AddLogHandler(BaseLogHandler* log)
{
    for (CLint i = 0; i < MAX_HANDLERS; ++i)
    {
        if (m_logHandlers[i] == 0)
        {
            m_logHandlers[i] = log;
            return;
        }
    }

    CL_ASSERT_MSG("CAN'T ADD ANOTHER LOG HANDLER!");
}
#ifndef CLOUD_DEBUG_DEBUGLOG_HEADER
#define CLOUD_DEBUG_DEBUGLOG_HEADER

#include <stdarg.h>
#include <map>
#include "../utilities/DefinesTypes.h"
#include "../utilities/StaticArray.h"

#define CL_TRACE_CHANNEL Cloud::Debug::DebugLog::Instance().TraceChannel
#define CL_TRACE Cloud::Debug::DebugLog::Instance().Trace

namespace Cloud
{
    namespace Debug
    {
        class BaseLogHandler;   

        class DebugLog
        {
        public:
            static CLbool Create();
            static void Destroy();

            static DebugLog& Instance() { return *s_instance; }

            void Trace(const CLchar* output, ...);
            void TraceChannel(const CLchar* channel, const CLchar* output, ...);

            void EnableChannel(const CLchar * channel) { m_channelList[channel] = true; }
            void DisableChannel(const CLchar * channel) { m_channelList[channel] = false; }

            void AddLogHandler(BaseLogHandler* log);

        private:
            DebugLog();
            ~DebugLog();

            CLbool Initialise();
            void Shutdown();

            void WriteToChannel(const CLchar* channel, const CLchar* output, va_list args);
            void WriteToBuffer(CLchar* buffer, const CLchar* output, va_list args);
            void PushBufferToHandlers(const CLchar* buffer);

            typedef std::pair<const CLchar*, bool> Channel;
            typedef std::map<const CLchar*, bool> ChannelList;
            typedef ChannelList::iterator ChannelIterator;

            static DebugLog* s_instance;

            static const CLint MAX_CHARS = 1023;
            static const CLint MAX_HANDLERS = 4;

            Utils::StaticArray<BaseLogHandler*, MAX_HANDLERS> m_logHandlers;
            ChannelList m_channelList;
        };
    }
}

#endif // CLOUD_DEBUG_DEBUGLOG_HEADER
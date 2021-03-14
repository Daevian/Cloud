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
            static bool Create();
            static void Destroy();

            static DebugLog& Instance() { return *s_instance; }

            void Trace(const t_char* output, ...);
            void TraceChannel(const t_char* channel, const t_char* output, ...);

            void EnableChannel(const t_char * channel) { m_channelList[channel] = true; }
            void DisableChannel(const t_char * channel) { m_channelList[channel] = false; }

            void AddLogHandler(BaseLogHandler* log);

        private:
            DebugLog();
            ~DebugLog();

            static const int MAX_CHARS = 1024;
            static const int MAX_HANDLERS = 4;

            bool Initialise();
            void Shutdown();

            void WriteToChannel(const t_char* channel, const t_char* output, va_list args);
            void WriteToBuffer(std::array<t_char, MAX_CHARS>& buffer, const t_char* output, va_list args);
            void PushBufferToHandlers(const t_char* buffer);

            typedef std::pair<const t_char*, bool> Channel;
            typedef std::map<const t_char*, bool> ChannelList;
            typedef ChannelList::iterator ChannelIterator;

            static DebugLog* s_instance;

            Utils::StaticArray<BaseLogHandler*, MAX_HANDLERS> m_logHandlers;
            ChannelList m_channelList;
        };
    }
}

#endif // CLOUD_DEBUG_DEBUGLOG_HEADER
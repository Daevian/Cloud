#ifndef CLOUD_DEBUG_WINCONSOLELOG_HEADER
#define CLOUD_DEBUG_WINCONSOLELOG_HEADER

#include "BaseLogHandler.h"

namespace Cloud
{
    namespace Debug
    {
        class WinConsoleLogHandler : public BaseLogHandler
        {
        public:
            virtual void HandleTrace(const CLchar* output) const;
        };
    }
}

#endif // CLOUD_DEBUG_WINCONSOLELOG_HEADER
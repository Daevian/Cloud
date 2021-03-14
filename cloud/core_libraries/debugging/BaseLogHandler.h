#ifndef CLOUD_DEBUG_BASELOG_INCLUDED
#define CLOUD_DEBUG_BASELOG_INCLUDED

#include <cstdarg>
#include <map>
#include "../utilities/DefinesTypes.h"

namespace Cloud
{
    namespace Debug
    {
        class BaseLogHandler
        {
        public:
            virtual ~BaseLogHandler() {};

            virtual void HandleTrace(const t_char* output) const = 0;
        };
    }
}

#endif // CLOUD_DEBUG_BASELOG_INCLUDED
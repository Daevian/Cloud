#ifndef CLOUD_UTILITIES_TIMER_HEADER
#define CLOUD_UTILITIES_TIMER_HEADER

#include "DefinesTypes.h"
#include <windows.h>

namespace Cloud
{
namespace Utils
{
    class Timer
    {
    public:
        Timer();
        
        void Initialise();
        void Update();

        CLdouble GetTotalTime() const       { return m_totalTime; }
        CLdouble GetTimeStep() const        { return m_timeStep; }

    private:
        CLdouble m_totalTime;
        CLdouble m_timeStep;

        LARGE_INTEGER m_performanceTimercalibrationData;
        LARGE_INTEGER m_clockStarted;
        LARGE_INTEGER m_frameStarted;
        LARGE_INTEGER m_callOverhead;
    };
}
}

#endif // CLOUD_UTILITIES_TIMER_HEADER
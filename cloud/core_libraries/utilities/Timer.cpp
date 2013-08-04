#include "Timer.h"

Cloud::Utils::Timer::Timer()
    : m_totalTime(0.0)
    , m_timeStep(0.0)
{
}

void Cloud::Utils::Timer::Initialise()
{
    m_totalTime = 0.0;
    m_timeStep = 0.0;

    m_callOverhead.QuadPart = 0;
    for (int i = 0; i < 10000; ++i)
    {
        LARGE_INTEGER start,end;
        QueryPerformanceCounter(&start);
        QueryPerformanceCounter(&end);
        m_callOverhead.QuadPart = end.QuadPart - start.QuadPart;
    }
    
    m_callOverhead.QuadPart += 5000;
    m_callOverhead.QuadPart /= 10000;

    QueryPerformanceFrequency(&m_performanceTimercalibrationData);
    QueryPerformanceCounter(&m_clockStarted);
    m_frameStarted = m_clockStarted;
}

void Cloud::Utils::Timer::Update()
{
    LARGE_INTEGER frameEnd;
    QueryPerformanceCounter(&frameEnd);
    m_clockStarted.QuadPart += m_callOverhead.QuadPart;

    CLdouble lastFrameTime = m_totalTime;
    m_totalTime = static_cast<CLdouble>((frameEnd.QuadPart - m_clockStarted.QuadPart)) / static_cast<CLdouble>(m_performanceTimercalibrationData.QuadPart);
    m_timeStep = m_totalTime - lastFrameTime;
    
    QueryPerformanceCounter(&m_frameStarted);
}
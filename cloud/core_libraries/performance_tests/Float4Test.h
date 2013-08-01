#ifndef FLOAT4TEST_HEADER
#define FLOAT4TEST_HEADER

#include "../utilities/Float4.h"
#include "../utilities/StaticArray.h"

namespace Tests
{
    class Float4Test
    {
    public:
        Float4Test();

        void Init();
        void Run();
        void OutputResults();

    private:
        static const unsigned int m_vectorAmount = 10000;

        Cloud::Utils::StaticArray<Cloud::Math::Float4, m_vectorAmount> m_vectors;

        
    };
}

#endif
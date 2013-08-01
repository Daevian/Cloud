#include "stdafx.h"
#include "Float4Test.h"

Tests::Float4Test::Float4Test()
{
}

void Tests::Float4Test::Init()
{
    for (int i = 0; i < m_vectorAmount; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            m_vectors[i].a[j] = 1.0f;
        }

        m_vectors[i].a[3] = 0.0f;
    }

    std::cout << "Float4 Test Initialising..." << std::endl;
}

void Tests::Float4Test::Run()
{
    std::cout << "Running test..." << std::endl;

    for (int loop = 0; loop < 10; ++loop)
    {
        for (int i = 0; i < m_vectorAmount - 1; ++i)
        {
            m_vectors[i] += m_vectors[i + 1];
        }
    }

    m_vectors[0].Normalize();
    std::cout << "Test finished!" << std::endl;
}

void Tests::Float4Test::OutputResults()
{
    std::cout << "Results:" << std::endl;
}


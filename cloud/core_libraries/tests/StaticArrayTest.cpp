#include "StdAfx.h"
#include <cfixcc.h>

#include "../utilities/StaticArray.h"

#define TEST_ARRAY_SIZE 1000

class StaticArrayTest : public cfixcc::TestFixture
{
public:
    void AddTest()
    {
        Cloud::Utils::StaticArray<CLint, TEST_ARRAY_SIZE> staticArray;
        AddItems(staticArray);

        for (int i = 0; i < TEST_ARRAY_SIZE; ++i)
        {
            CFIXCC_ASSERT_EQUALS(i, staticArray[i]);
        }
    }

	void InsertTest()
	{
        Cloud::Utils::StaticArray<CLint, TEST_ARRAY_SIZE> staticArray;
        AddItems(staticArray);

        staticArray.Insert(500, 10000);

        for (int i = 0; i < 500; ++i)
        {
            CFIXCC_ASSERT_EQUALS(i, staticArray[i]);
        }

        CFIXCC_ASSERT_EQUALS(10000, staticArray[500]);

        for (int i = 501; i < TEST_ARRAY_SIZE; ++i)
        {
            CFIXCC_ASSERT_EQUALS(i-1, staticArray[i]);
        }
	}

    void CopyTest()
    {
        Cloud::Utils::StaticArray<CLint, TEST_ARRAY_SIZE> staticArray1;
        AddItems(staticArray1);

        Cloud::Utils::StaticArray<CLint, TEST_ARRAY_SIZE> staticArray2;
        staticArray2 = staticArray1;

        for (int i = 0; i < TEST_ARRAY_SIZE; ++i)
        {
            CFIXCC_ASSERT_EQUALS(staticArray1[i], staticArray2[i]);
        }
    }

private:
    void AddItems(Cloud::Utils::StaticArray<CLint, TEST_ARRAY_SIZE>& staticArray)
    {
        for (CLint i = 0; i < TEST_ARRAY_SIZE; ++i)
        {
            staticArray[i] = i;
        }
    }
};

CFIXCC_BEGIN_CLASS(StaticArrayTest)
    CFIXCC_METHOD(AddTest)
	CFIXCC_METHOD(InsertTest)
    CFIXCC_METHOD(CopyTest)
CFIXCC_END_CLASS()


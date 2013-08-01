#include "StdAfx.h"
#include <cfixcc.h>

#include "../utilities/DynamicArray.h"

class DynamicArrayTest : public cfixcc::TestFixture
{
public:
	void InitTest()
	{
		Cloud::Utils::DynamicArray<CLint> dynamicArray1;
        dynamicArray1.Init(16, 16);
        Cloud::Utils::DynamicArray<CLint> dynamicArray2(16, 16);
        Cloud::Utils::DynamicArray<CLint> dynamicArray3(dynamicArray2);
	}

    void AddTest()
    {
        Cloud::Utils::DynamicArray<CLint> dynamicArray(2, 2);

        AddStuff(dynamicArray);
        CFIXCC_ASSERT_EQUALS(1000, dynamicArray.Count());
        CFIXCC_ASSERT_EQUALS(999, *(dynamicArray.Last()));

        for (int i = 0; i < 1000; ++i)
        {
            CFIXCC_ASSERT_EQUALS(i, dynamicArray[i]);
        }
    }

    void InsertTest()
    {
        Cloud::Utils::DynamicArray<CLint> dynamicArray(2, 2);
        AddStuff(dynamicArray);

        InsertStuff(dynamicArray);
        CFIXCC_ASSERT_EQUALS(1100, dynamicArray.Count());
        CFIXCC_ASSERT_EQUALS(999, *(dynamicArray.Last()));

        for (int i = 0; i < 500; ++i)
        {
            CFIXCC_ASSERT_EQUALS(i, dynamicArray[i]);
        }

        for (int i = 0; i < 100; ++i)
        {
            CFIXCC_ASSERT_EQUALS(i * 1000, dynamicArray[i + 500]);
        }

        for (int i = 500; i < 1000; ++i)
        {
            CFIXCC_ASSERT_EQUALS(i, dynamicArray[i + 100]);
        }
    }

    void RemoveTest()
    {
        Cloud::Utils::DynamicArray<CLint> dynamicArray(2, 2);
        AddStuff(dynamicArray);

        RemoveStuff(dynamicArray, 500, 500);
        CFIXCC_ASSERT_EQUALS(500, dynamicArray.Count());
        CFIXCC_ASSERT_EQUALS(499, *(dynamicArray.Last()));

        RemoveStuff(dynamicArray, 300, 100);
        CFIXCC_ASSERT_EQUALS(400, dynamicArray.Count());
        CFIXCC_ASSERT_EQUALS(499, *(dynamicArray.Last()));
    }

    void RemoveCyclicTest()
    {
        Cloud::Utils::DynamicArray<CLint> dynamicArray(2, 2);
        AddStuff(dynamicArray);

        RemoveStuffCyclic(dynamicArray, 500, 500);
        CFIXCC_ASSERT_EQUALS(500, dynamicArray.Count());
        CFIXCC_ASSERT_EQUALS(499, *(dynamicArray.Last()));

        RemoveStuffCyclic(dynamicArray, 300, 100);
        CFIXCC_ASSERT_EQUALS(400, dynamicArray.Count());
        CFIXCC_ASSERT_EQUALS(399, *(dynamicArray.Last()));
        CFIXCC_ASSERT_EQUALS(400, dynamicArray[300]);
    }

    void ReAddTest()
    {
        Cloud::Utils::DynamicArray<CLint> dynamicArray(2, 2);
        AddStuff(dynamicArray);

        RemoveStuff(dynamicArray, 0, 1000);
        CFIXCC_ASSERT_EQUALS(0, dynamicArray.Count());

        AddStuff(dynamicArray);

        CFIXCC_ASSERT_EQUALS(1000, dynamicArray.Count());
        CFIXCC_ASSERT_EQUALS(999, *(dynamicArray.Last()));

        for (int i = 0; i < 1000; ++i)
        {
            CFIXCC_ASSERT_EQUALS(i, dynamicArray[i]);
        }
    }

    void CopyTest()
    {
        Cloud::Utils::DynamicArray<CLint> dynamicArray1(2, 2);
        AddStuff(dynamicArray1);
        Cloud::Utils::DynamicArray<CLint> dynamicArray2(dynamicArray1);
        Cloud::Utils::DynamicArray<CLint> dynamicArray3;
        dynamicArray3 = dynamicArray1;

        for (CLint i = 0; i < dynamicArray1.Count(); ++i)
        {
            CFIXCC_ASSERT_EQUALS(dynamicArray1[i], dynamicArray2[i]);
            CFIXCC_ASSERT_EQUALS(dynamicArray1[i], dynamicArray3[i]);
        }
    }


private:
    void AddStuff(Cloud::Utils::DynamicArray<CLint>& dynamicArray)
    {
        for (CLint i = 0; i < 1000; ++i)
        {
            dynamicArray.Add(i);
        }
    }

    void InsertStuff(Cloud::Utils::DynamicArray<CLint>& dynamicArray)
    {
        CL_ASSERT(dynamicArray.Count() >= 500, "Need more then 500 items already added to use InsertStuff().");
        for (CLint i = 0; i < 100; ++i)
        {
            dynamicArray.Insert(i + 500, i * 1000);
        }
    }

    void RemoveStuff(Cloud::Utils::DynamicArray<CLint>& dynamicArray, CLint startIndex, CLint range)
    {
        CL_ASSERT(startIndex >= 0 && startIndex < dynamicArray.Count(), "startIndex out of bounds!");
        CL_ASSERT(range > 0 && startIndex + range <= dynamicArray.Count(), "range too high!");

        for (CLint i = startIndex; i < startIndex + range; ++i)
        {
            dynamicArray.RemoveAtIndex(startIndex);
        }
    }

    void RemoveStuffCyclic(Cloud::Utils::DynamicArray<CLint>& dynamicArray, CLint startIndex, CLint range)
    {
        CL_ASSERT(startIndex >= 0 && startIndex < dynamicArray.Count(), "startIndex out of bounds!");
        CL_ASSERT(range > 0 && startIndex + range <= dynamicArray.Count(), "range too high!");

        for (CLint i = startIndex; i < startIndex + range; ++i)
        {
            dynamicArray.RemoveCyclicAtIndex(startIndex);
        }
    }
};

CFIXCC_BEGIN_CLASS(DynamicArrayTest)
	CFIXCC_METHOD(InitTest)
    CFIXCC_METHOD(AddTest)
    CFIXCC_METHOD(InsertTest)
    CFIXCC_METHOD(RemoveTest)
    CFIXCC_METHOD(RemoveCyclicTest)
    CFIXCC_METHOD(ReAddTest)
    CFIXCC_METHOD(CopyTest)
CFIXCC_END_CLASS()
#ifndef CLOUD_DEFINESASSERTS_HEADER
#define CLOUD_DEFINESASSERTS_HEADER

#include <assert.h>

#ifdef _DEBUG
    #define CL_ASSERT(expression, message)  assert(expression && message)
    #define CL_ASSERT_MSG(message)          assert(false && message)
    #define CL_ASSERT_NULL(variable)        assert(variable != 0 && "variable isn't allowed to be null!")
    #define USING_ASSERT
#else
    #define CL_ASSERT(expression, message) 
    #define CL_ASSERT_MSG(message)
    #define CL_ASSERT_NULL(variable)
#endif

#endif // CLOUD_DEFINESASSERTS_HEADER
#ifndef CLOUD_DEFINES_MACROS_HEADER
#define CLOUD_DEFINES_MACROS_HEADER

#define SAFE_DELETE(item) if (item != 0) { delete item; item = 0; }
#define SAFE_DELETE_ARRAY(item) if (item != 0) { delete [] item; item = 0; }

#define CL_UNUSED(variable) (variable)

#define CL_ALIGN(alignment) __declspec(align(alignment))

#define CL_ARRAY_SIZE(a, type) sizeof(a) / sizeof(type)

#define NULL 0

#endif // CLOUD_DEFINES_MACROS_HEADER
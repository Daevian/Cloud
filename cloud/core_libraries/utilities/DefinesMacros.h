#ifndef CLOUD_DEFINES_MACROS_HEADER
#define CLOUD_DEFINES_MACROS_HEADER

#define SAFE_DELETE(item) if (item != 0) { delete item; item = 0; }
#define SAFE_DELETE_ARRAY(item) if (item != 0) { delete [] item; item = 0; }

#define CL_UNUSED(variable) (variable)

#define CL_ALIGN(alignment) __declspec(align(alignment))

#define CL_ALIGN_TO(value, alignment) ((value + (alignment - 1)) & ~(alignment - 1))

#define CL_ARRAY_SIZE(a, type) sizeof(a) / sizeof(type)

#define NULL 0


#define CL_DISABLE_WARNING_UNUSED_VAR __pragma(warning( disable : 4101 ))


#define FLOAT4_BLACK(alpha)     ClFloat4(0.0f, 0.0f, 0.0f, alpha)
#define FLOAT4_WHITE(alpha)     ClFloat4(1.0f, 1.0f, 1.0f, alpha)
#define FLOAT4_RED(alpha)       ClFloat4(1.0f, 0.0f, 0.0f, alpha)
#define FLOAT4_GREEN(alpha)     ClFloat4(0.0f, 1.0f, 0.0f, alpha)
#define FLOAT4_BLUE(alpha)      ClFloat4(0.0f, 0.0f, 1.0f, alpha)
#define FLOAT4_YELLOW(alpha)    ClFloat4(1.0f, 1.0f, 0.0f, alpha)
#define FLOAT4_MAGENTA(alpha)   ClFloat4(1.0f, 0.0f, 1.0f, alpha)
#define FLOAT4_CYAN(alpha)      ClFloat4(0.0f, 1.0f, 1.0f, alpha)

#endif // CLOUD_DEFINES_MACROS_HEADER
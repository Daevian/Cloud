#ifndef CLOUD_UTILITIES_FILE_HEADER
#define CLOUD_UTILITIES_FILE_HEADER

#include "Windows.h"
#include <memory>
#include "ClString.h"
#include "DefinesTypes.h"

namespace Cloud
{
    namespace Utils
    {
        namespace File
        {
            struct ScopedHandleCloser
            {
                void operator()(HANDLE handle)
                {
                    if (handle)
                        CloseHandle(handle);
                }
            };

            typedef public std::unique_ptr<void, ScopedHandleCloser> ScopedHandle;
            inline HANDLE SafeHandle(HANDLE handle) { return (handle == INVALID_HANDLE_VALUE) ? 0 : handle; }

            ScopedHandle    LoadFile(const Cloud::String& fileName);
            bool          Read(const ScopedHandle& handle, std::unique_ptr<uint8[]>& buffer, uint readSize);
            int64         GetFileSize(const ScopedHandle& handle);
        }
    }
}

#endif // CLOUD_UTILITIES_FILE_HEADER
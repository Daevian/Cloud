#include "ClFile.h"
#include "DefinesAsserts.h"

using namespace Cloud::Utils::File;

ScopedHandle Cloud::Utils::File::LoadFile(const ClString& fileName)
{
#if (_WIN32_WINNT >= _WIN32_WINNT_WIN8)
    ScopedHandle hFile(SafeHandle(CreateFile2(
        ClWString(fileName.begin(), fileName.end()).c_str(),
        GENERIC_READ,
        FILE_SHARE_READ,
        OPEN_EXISTING,
        nullptr)));
#else
    ScopedHandle hFile(SafeHandle(CreateFileA(
        fileName.c_str(),
        GENERIC_READ,
        FILE_SHARE_READ,
        nullptr,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        nullptr)));
#endif

    auto result = HRESULT_FROM_WIN32(GetLastError());
    if (FAILED(result))
    {
        CL_ASSERT_MSG(("Failed to load the file" + fileName + "!").c_str());
    }

    return hFile;
}

CLbool Cloud::Utils::File::Read(const ScopedHandle& handle, std::unique_ptr<CLuint8[]>& buffer, CLuint readSize)
{
    DWORD bytesRead = 0;
    if (!ReadFile(
        handle.get(),
        buffer.get(),
        readSize,
        &bytesRead,
        nullptr))
    {
        auto result = HRESULT_FROM_WIN32(GetLastError());
        if (FAILED(result))
        {
            CL_ASSERT_MSG("Failed to read the file!");
        }

        return false;
    }

    return true;
}

CLint64 Cloud::Utils::File::GetFileSize(const ScopedHandle& handle)
{
    LARGE_INTEGER fileSize = { 0 };

#if (_WIN32_WINNT >= _WIN32_WINNT_VISTA)
    FILE_STANDARD_INFO fileInfo;
    if (!GetFileInformationByHandleEx( handle.get(), FileStandardInfo, &fileInfo, sizeof(fileInfo)))
    {
        auto result = HRESULT_FROM_WIN32(GetLastError());
        if (FAILED(result))
        {
            CL_ASSERT_MSG("Failed to get file information!");
        }
    }

    fileSize = fileInfo.EndOfFile;
#else
    GetFileSizeEx( handle.get(), &fileSize );
#endif

    return fileSize.QuadPart;
}

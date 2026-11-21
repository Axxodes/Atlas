#pragma once
#include <windows.h>

#include <malloc_handler.hpp>

struct FileBuffer
{
    size_t capacity;
    size_t used;
    char* memory;
};

FileBuffer fileBuffer {};

#define RESET_FILEBUFFER() do { \
fileBuffer.memory = nullptr; \
fileBuffer.capacity = 0; \
fileBuffer.used = 0;} \
while (false) 

bool allocateFileBuffer(size_t size)
{
    // round UP for the input

    RESET_FILEBUFFER();

    fileBuffer.memory = (char*)std::malloc(size);
    if(!fileBuffer.memory)
    {
        OUTPUT_ERROR("FileBuffer failed to allocated... aborting.");
        return false;
    }
    fileBuffer.capacity = size;
    memset(fileBuffer.memory,0, size);
    return true;
}

bool openFile(const wchar_t* filePath)
{
    // you can create a wide char by putting L infront of the first quotation mark

    HANDLE file = CreateFileW(filePath,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
    if (file == INVALID_HANDLE_VALUE)
    {
        DWORD error = GetLastError();
        OUTPUT_ERROR("Tried to open file, returning error: " << error);

        CloseHandle(file);
        return false;
    }

    LARGE_INTEGER filesize {};
    
    if(GetFileSizeEx(file,&filesize) == 0)
    {
        OUTPUT_ERROR("Filesize acquired below 0... aborting.");

        CloseHandle(file);
        return false;
    }

    DWORD bytesRead = 0;

    if(!allocateFileBuffer(filesize.QuadPart))
    {
        CloseHandle(file);
        return false;
    }

    if(!ReadFile(file,fileBuffer.memory,static_cast<DWORD>(fileBuffer.capacity),&bytesRead,nullptr))
    {
        DWORD error = GetLastError();
        OUTPUT_ERROR("Tried to fill FileBuffer with file data, (hint: it failed) returning error: " << error);

        CloseHandle(file);
        std::free(fileBuffer.memory);
        fileBuffer.capacity = 0;
        fileBuffer.used = 0;
        return false;
    }

    fileBuffer.used = bytesRead;

    CloseHandle(file);
    return true;
}

bool createFile(const wchar_t* filePath)
{
    HANDLE file = CreateFileW(filePath,GENERIC_WRITE,FILE_SHARE_READ,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
    if (file == INVALID_HANDLE_VALUE)
    {
        DWORD error = GetLastError();
        OUTPUT_ERROR("File creation failed, returning error: " << error);

        CloseHandle(file);
        return false;
    }

    CloseHandle(file);
    return true;
}

bool checkFileSize(const wchar_t* filePath, int* fileSizeOut)
{
    HANDLE file = CreateFileW(filePath,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
    if (file == INVALID_HANDLE_VALUE)
    {
        DWORD error = GetLastError();
        OUTPUT_ERROR("File creation failed, returning error: " << error);

        CloseHandle(file);
        return false;
    }

    LARGE_INTEGER fileSize {};
    
    if(GetFileSizeEx(file,&fileSize) == 0)
    {
        OUTPUT_ERROR("Filesize acquired below 0...");

        CloseHandle(file);
        return false;
    }

    fileSizeOut[0] = fileSize.QuadPart;

    CloseHandle(file);
    return true;
}

bool writeToFile(const wchar_t* filePath,CharMemory memoryBlock)
{
    HANDLE file = CreateFileW(filePath,GENERIC_WRITE,FILE_SHARE_READ,NULL,CREATE_ALWAYS ,FILE_ATTRIBUTE_NORMAL,NULL);
    if (file == INVALID_HANDLE_VALUE)
    {
        DWORD error = GetLastError();
        OUTPUT_ERROR("File creation failed, returning error: " << error);

        CloseHandle(file);
        return false;
    }

    DWORD bytesWritten = 0;

    if(!WriteFile(file,memoryBlock.memory,memoryBlock.capacity,&bytesWritten,NULL))
    {
        DWORD error = GetLastError();
        OUTPUT_ERROR(error);

        if(error == ERROR_INVALID_USER_BUFFER)
        {
            OUTPUT_ERROR("Critical Error, (hint: User-buffer invalid)");
        }

        CloseHandle(file);
        return false;
    }

    CloseHandle(file);
    return true;
}
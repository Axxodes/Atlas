#include <iostream>

#include <cstring>

#define OUTPUT_ERROR(...) (std::cerr << __VA_ARGS__ << '\n')

#include <malloc_handler.hpp>
#include <file_handler.hpp>

int wmain(int argc, const wchar_t* argv[])
{
    if(argc < 2)
    {
        OUTPUT_ERROR("Filename missing, please rerun the .exe with the following args: atlas.exe fileExample.test");
        return 1;
    }

    bool decompress = false;

    if (argc >= 3) 
    {
        if (!wcscmp(argv[2],L"1")) 
        {
            decompress = true;
        }
        else 
        {
            if (!wcscmp(argv[2],L"0"))
            {
                OUTPUT_ERROR("WARNING: Mode is optional, and should only be used when decompressing. Continueing...");
            }
            else
            {
                OUTPUT_ERROR("Failed, (hint: incorrect third arg, needs to be either 0 or 1) please rerun the .exe with the following args adjusted: atlas.exe fileExample.test (mode)");
                return 1;
            }
        }
    }

    LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);

    LARGE_INTEGER a, b;
	QueryPerformanceCounter(&a);

    const wchar_t* fileName = argv[1];

    if(!openFile(fileName))
    {
        return 1;
    }

    if(decompress == false)
    {
        std::wstring outputFileName = fileName;
        outputFileName += L'c';

        if(!createFile(outputFileName.c_str()))
        {
            OUTPUT_ERROR("Stopped compressor,");
            return 1;
        }

        if(!allocateCharMemory(fileBuffer.used))
        {
            OUTPUT_ERROR("Aborting...");
            return 1;
        }

        for(size_t i{0};i<fileBuffer.used;i++)
        {
            charMemory.memory[i] = fileBuffer.memory[i];
        }

        // do some shit to this memory

        if(!writeToFile(outputFileName.c_str(),charMemory))
        {
            OUTPUT_ERROR("Aborting...");
            return 1;
        }   
    }

    if(decompress == true)
    {
        wprintf(L"Didn't perform task on %ls due to decompressing not being implemented on current patch.\n", fileName);
    }

    QueryPerformanceCounter(&b);

    double ticks = static_cast<double>(b.QuadPart - a.QuadPart);

    wprintf(L"Performed task on %ls\n", fileName);
    printf("\nFinished within %3.0f ticks on a %3.0f Hz clock\n",ticks,static_cast<double>(freq.QuadPart));

    RESET_FILEBUFFER();
    return 0;
}
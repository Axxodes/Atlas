#include <iostream>

#include <cstring>
#include <algorithm>

#define OUTPUT_ERROR(...) (std::cerr << __VA_ARGS__ << '\n')

#define FREE_MEMORY() do { \
RESET_FILEBUFFER(); \
RESET_CHARMEMORY(); \
} while(false)

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
                OUTPUT_ERROR("WARNING: Mode is an optional arg, and should only be used when decompressing. Continueing...");
            }
            else
            {
                OUTPUT_ERROR("Failed, (hint: incorrect third arg, needs to be either 0 or 1) please rerun the .exe with the following args adjusted: atlas.exe fileExample.test (mode)");
                return 1;
            }
        }
    }

    const wchar_t* patch {L"1.0.0"};

    LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);

    LARGE_INTEGER a, b;

    const wchar_t* fileName = argv[1];

    if(!openFile(fileName))
    {
        return 1;
    }

    if(decompress == false)
    {
        // timer start

        QueryPerformanceCounter(&a);

        // main logic

        std::wstring outputFileName = fileName;
        outputFileName += L'c';

        if(!createFile(outputFileName.c_str()))
        {
            OUTPUT_ERROR("Aborting...");
            return 1;
        }

        if(!allocateCharMemory(fileBuffer.used))
        {
            OUTPUT_ERROR("Aborting...");
            return 1;
        }

        // the following code is the compressor

        size_t out {0};

        for(size_t i{0};i<fileBuffer.used;i++)
        {
            size_t count {0};
            for(size_t u{i+1};u<(i+std::min(fileBuffer.used,size_t(32)));u++)
            {
                if(fileBuffer.memory[i] == fileBuffer.memory[u-1])
                {
                    count++;
                    continue;
                }

                break;
            }

            if(count<3)
            {
                charMemory.memory[out] = fileBuffer.memory[i];
                out++;
                continue;
            }

            charMemory.memory[i] = '%';
            charMemory.memory[i+1] = '0' + count;
            charMemory.memory[i+2] = fileBuffer.memory[i];

            out+=3;
            i+=count-1;
        }

        // end of the compressor

        charMemory.used = out;
        RESET_FILEBUFFER();

        CharMemory returnCharMemory{};
        returnCharMemory.memory = allocateCharMemory(charMemory.used,true);
        returnCharMemory.capacity = charMemory.used;

        int currentReturnChar{0};
        for(size_t i{0};i<charMemory.used;i++)
        {
            if(charMemory.memory[i] && charMemory.memory[i] != 0)
            {
                returnCharMemory.memory[currentReturnChar] = charMemory.memory[i];
                currentReturnChar++;
            }
        }

        RESET_CHARMEMORY();


        if(!writeToFile(outputFileName.c_str(),returnCharMemory))
        {
            OUTPUT_ERROR("Aborting...");
            return 1;
        }

        // end of main logic
        // and timer end

        QueryPerformanceCounter(&b);
    }

    if(decompress == true)
    {
        // start timer

        QueryPerformanceCounter(&a);

        // main logic

        wprintf(L"Didn't perform task on %ls due to decompressing not being implemented on current patch.\n", fileName);

        // end of main logic
        // and timer end

        QueryPerformanceCounter(&b);
    }

    double ticks = static_cast<double>(b.QuadPart - a.QuadPart);

    auto outputFileName = std::wstring(fileName)+L"c";

    if(decompress == false)
    {
        // remove this if check, once decompressing is implemented.

        wprintf(L"Performed task on '%ls'.\nSaved as '%ls'.\n",fileName,outputFileName.c_str());
        printf("\nFinished within %3.0f ticks on a %3.0f Hz clock\n",ticks,static_cast<double>(freq.QuadPart));
    }

    wprintf(L"Current patch: %ls\n", patch);

    FREE_MEMORY();
    return 0;
}
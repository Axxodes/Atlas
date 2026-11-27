#include <iostream>
#define OUTPUT_ERROR(...) (std::cerr << __VA_ARGS__ << '\n')

#define FREE_MEMORY() do { \
RESET_FILEBUFFER(); \
RESET_CHARMEMORY(); \
} while(false)

#define ABORT_ERROR(...) do { \
OUTPUT_ERROR(__VA_ARGS__); \
FREE_MEMORY(); \
} \
while(false)

#include <cstring>
#include <algorithm>

#include <malloc_handler.hpp>
#include <file_handler.hpp>


int wmain(int argc, const wchar_t* argv[])
{
    if(argc < 2)
    {
        ABORT_ERROR("Failed, (hint: filename missing) please rerun the .exe with the following args: atlas.exe file.example (mode)");
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
                ABORT_ERROR("Failed, (hint: incorrect third arg, needs to be either 0 or 1) please rerun the .exe with the following args adjusted: atlas.exe file.example (mode)");
                return 1;
            }
        }
    }

    const wchar_t* patch {L"1.1.0"};

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

        size_t out {0}; // used in every compressor (don't remove)

        for(size_t i{0};i<fileBuffer.used;i++)
        {
            size_t count {0};
            for(size_t u{i+1};u<(i+std::min(fileBuffer.used,size_t(9)));u++)
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

            charMemory.memory[out] = '%';
            charMemory.memory[out+1] = '0' + count;
            charMemory.memory[out+2] = fileBuffer.memory[i];

            out+=3;
            i+=count-1;
        }

        // end of the compressor

        charMemory.used = out;
        RESET_FILEBUFFER();

        CharMemory returnCharMemory{};
        returnCharMemory.memory = allocateCharMemory(charMemory.used,true);
        returnCharMemory.capacity = charMemory.used;

        if(!returnCharMemory.memory)
        {
            OUTPUT_ERROR("Allocation failed, aborting...");
            FREE_MEMORY();
            return 1;
        }

        int currentReturnChar{0};
        for(size_t i{0};i<charMemory.used;i++)
        {
            if(charMemory.memory[i] && charMemory.memory[i] != 0)
            {
                returnCharMemory.memory[currentReturnChar] = charMemory.memory[i];
                currentReturnChar++;
            }
        }

        returnCharMemory.used = static_cast<size_t>(currentReturnChar);

        RESET_CHARMEMORY();

        if(!writeToFile(outputFileName.c_str(),returnCharMemory))
        {
            OUTPUT_ERROR("Aborting...");
            return 1;
        }

        std::free(returnCharMemory.memory);

        // end of main logic
        // and timer end

        QueryPerformanceCounter(&b);

        double ticks = static_cast<double>(b.QuadPart - a.QuadPart);
        auto printFileName = std::wstring(fileName)+L"c";

        wprintf(L"Performed task on '%ls'.\nSaved as '%ls'.\n",fileName,printFileName.c_str());
        printf("\nFinished within %3.0f ticks on a %3.0f Hz clock\n",ticks,static_cast<double>(freq.QuadPart));
    }

    if(decompress == true)
    {
        // start timer

        QueryPerformanceCounter(&a);

        // main logic

        std::wstring inputFileName = fileName;

        if(!openFile(inputFileName.c_str()))
        {
            ABORT_ERROR("Aborting...");
            return 1;
        }

        CharMemory returnCharMemory{};
        returnCharMemory.memory = allocateCharMemory(fileBuffer.capacity*2,true);
        returnCharMemory.capacity = fileBuffer.used;

        if(!returnCharMemory.memory)
        {
            ABORT_ERROR("Aborting...");
            return 1;
        }

        // start decompressor

        size_t out {0};

        for(size_t i{0};i<fileBuffer.used;i++)
        {
            if(fileBuffer.memory[i] == '%')
            {
                char nextChar = fileBuffer.memory[i+1];
                int amount = nextChar - '0';

                char repeatedChar = fileBuffer.memory[i+2];

                if(amount<3)
                {
                    ABORT_ERROR("Decompressing failed, (hint: compressed file invalid) aborting...");
                    return 1;
                }
                else
                {
                    for(int u{0};u<amount;u++)
                    {
                        returnCharMemory.memory[out] = repeatedChar;
                        out++;
                    }
                }
                i+=2;
            }
            else
            {
                returnCharMemory.memory[out] = fileBuffer.memory[i];
                out++;
            }
        }

        // end decompressor

        returnCharMemory.used = out;
        RESET_FILEBUFFER();

        if(!writeToFile(inputFileName.c_str(),returnCharMemory))
        {
            OUTPUT_ERROR("Aborting...");
            return 1;
        }

        std::free(returnCharMemory.memory);

        QueryPerformanceCounter(&b);

        // end of main logic

        double ticks = static_cast<double>(b.QuadPart - a.QuadPart);

        wprintf(L"Performed task on '%ls'.\n",fileName);
        printf("\nFinished within %3.0f ticks on a %3.0f Hz clock\n",ticks,static_cast<double>(freq.QuadPart));
    }

    wprintf(L"Current patch: %ls\n", patch);

    FREE_MEMORY();
    return 0;
}
#include <iostream>

#include <cstring>

#define OUTPUT_ERROR(...) (std::cerr << __VA_ARGS__ << '\n')
#include <filehandler.hpp>

int wmain(int argc, const wchar_t* argv[])
{
    if(argc < 2)
    {
        OUTPUT_ERROR("Filename missing, please rerun the .exe with hyphen-marked args added: atlas.exe -fileExample.test-");
        return 1;
    }

    bool decompress = false;

    if (argc >= 3) 
    {
        if (wcscmp(argv[2],L"1")) 
        {
            decompress = true;
        }
        else 
        {
            if (wcscmp(argv[2],L"0")) {}
            else
            {
                OUTPUT_ERROR("Failed, (hint: incorrect third arg, needs to be either 0 or 1) please rerun the .exe with hyphen-marked args adjusted: atlas.exe -fileExample.test-");
                return 1;
            }
        }
    }

    const wchar_t* fileName = argv[1];
    printf("%s\n",fileName);

    openFile(fileName);
    
    for(size_t i{0};i<fileBuffer.capacity;i++)
    {
        printf("%s ",fileBuffer.memory[i]);
    }

    RESET_FILEBUFFER();
    return 0;
}
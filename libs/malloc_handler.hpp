#pragma once

struct CharMemory
{
    size_t capacity;
    char* memory;
    size_t used;
};

CharMemory charMemory {};

#define RESET_CHARMEMORY() do { \
std::free(charMemory.memory); \
charMemory.memory = nullptr; \
charMemory.capacity = 0; \
charMemory.used = 0;} \
while (false)

inline bool allocateCharMemory(size_t size)
{
    charMemory.memory = static_cast<char*>(malloc(sizeof(char)*size));
    if(!charMemory.memory)
    {
        printf("Failed to allocate CharMemory...\n");
        return false;
    }

    charMemory.capacity = size;

    return true;
}

inline char* allocateCharMemory(size_t size, bool functionChooser)
{
    if(functionChooser == false)
    {
        printf("Warning: The functionChooser param in allocateCharMemory is supposed to be true. (Internal code warning, make an issue on GitHub)\n");
    }
    CharMemory tempCharMemory {};

    tempCharMemory.memory = static_cast<char*>(malloc(sizeof(char)*size));
    if(!tempCharMemory.memory)
    {
        printf("Failed to allocate CharMemory... (hint: %lld)\n",size);
        return nullptr;
    }

    return tempCharMemory.memory;
}
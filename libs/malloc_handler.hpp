#pragma once

struct CharMemory
{
    size_t capacity;
    char* memory;
};

CharMemory charMemory {};

inline bool allocateCharMemory(size_t size)
{
    charMemory.memory = static_cast<char*>(malloc(sizeof(char)*size));
    if(!charMemory.memory)
    {
        OUTPUT_ERROR("Failed to allocate CharMemory...");
        return false;
    }

    charMemory.capacity = size;

    return true;
}
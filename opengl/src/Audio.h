#pragma once
#include <string>

class Audio
{
public:
    bool Init();
    void Shutdown();
    unsigned int LoadWAV(const std::string &path); // returns buffer id
    unsigned int PlaySound(unsigned int buffer, bool loop = false);
    void Stop(unsigned int source);
};

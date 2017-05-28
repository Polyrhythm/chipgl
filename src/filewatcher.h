#ifndef FILEWATCHER_H
#define FILEWATCHER_H

#include <stdio.h>
#include <ctime>

class Filewatcher
{
public:
    Filewatcher(const char* filepath);
    ~Filewatcher();
    bool fileChanged();
    const char* getFilepath() const;
    const char* readFile();

private:
    time_t _fileTimestamp;
    FILE* _file;
    const char* _filepath;
};

#endif

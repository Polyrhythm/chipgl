
#include <iostream>
#include <fstream>
#include <vector>
#include <sys/stat.h>
#include <errno.h>

#include "filewatcher.h"

Filewatcher::Filewatcher(const char* filepath)
    : _filepath(filepath)
{
    _file = fopen(_filepath, "rb");

    if (!_file)
    {
        std::cerr << "Could not open file!" << std::endl;
    }

    fileChanged();
}

Filewatcher::~Filewatcher()
{
    _file = nullptr;
}

const char* Filewatcher::readFile()
{
    long length;
    char* buf;
    int result;
    _file = fopen(_filepath, "r");

    if (_file == NULL)
    {
        printf("could not open file\n");
        return NULL;
    }

    result = fseeko(_file, 0L, SEEK_END);
    length = ftello(_file);
    buf = (char*)malloc(length + 1);
    result = fseeko(_file, 0, SEEK_SET);
    result = fread(buf, length, 1, _file);
    fclose(_file);
    buf[length] = 0;

    return (const char*)buf;
}

bool Filewatcher::fileChanged()
{
    struct stat fileStat;
    bool isChanged;

    int err = stat(_filepath, &fileStat);
    if (err != 0)
    {
        std::cerr << "Uhh file error." << std::endl;
    }

    time_t newTimestamp = fileStat.st_mtime;
    isChanged = newTimestamp > _fileTimestamp;
    _fileTimestamp = newTimestamp;

    return isChanged;
}

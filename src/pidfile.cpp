#include "pidfile.h"

#include <fstream>
#include <cerrno>
#include <cstring>

#include <unistd.h> // getpid

using Caster::PIDFile;

PIDFile::PIDFile(const std::string& fileName)
    : m_fileName(fileName)
{
    if (m_fileName.empty())
        return;

    std::ofstream file(m_fileName.c_str());

    if (!file.is_open())
        throw Error("Failed to create pid-file '" + m_fileName + "': " + strerror(errno));

    file << getpid();
}

PIDFile::~PIDFile()
{
    if (m_fileName.empty())
        return;

    unlink(m_fileName.c_str());
}

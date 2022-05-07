#pragma once

#include "error.h"

#include <string>

namespace Caster
{

class PIDFile
{
    public:
        struct Error : Caster::Error
        {
            explicit Error(const std::string& message) noexcept : Caster::Error("PIDFile", message) {}
        };

        explicit PIDFile(const std::string& file);
        ~PIDFile();

    private:
        const std::string m_fileName;

        PIDFile(const PIDFile&) = delete;
        PIDFile& operator=(const PIDFile&) = delete;
};

}

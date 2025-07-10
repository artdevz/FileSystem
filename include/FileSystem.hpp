#pragma once
#include <string>

class FileSystem {

public:
    virtual ~FileSystem() = default;

    virtual void Mkdir(const std::string& path) = 0;
    virtual void Touch(const std::string& path) = 0;
    virtual void Ls(const std::string& path) const = 0;
    virtual void Cd(const std::string& path) = 0;
    virtual void Mv(const std::string& sourcePath, const std::string& destPath) = 0;
    virtual void Echo(const std::string& path, const std::string& content, bool overwrite) = 0;
    virtual void Cat(const std::string& path) = 0;
    virtual void Rm(const std::string& path, bool recursive) = 0;

    virtual std::string GetCurrentPath() const = 0;

};
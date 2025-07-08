#include "FileSystem.hpp"

FileSystem::FileSystem() {}

FileSystem::~FileSystem() {}

// CMD

void FileSystem::Mkdir(const std::string& name) {}

void FileSystem::Touch(const std::string& name) {}

void FileSystem::Ls() const {}

void FileSystem::Cd(const std::string& path) {}

void FileSystem::Move(const std::string& fileName, const std::string& targetDir) {}

void FileSystem::WriteToFile(const std::string& fileName, const std::string& content) {}

void FileSystem::ReadFromFile(const std::string& fileName) {}

void FileSystem::Rm(const std::string& name) {}

// AUX

Inode* FileSystem::FindInode(const std::string& name) const {
    return nullptr;
}

void FileSystem::DeleteInode(Inode* node) {}

void FileSystem::RecursiveDelete(Inode* node) {}
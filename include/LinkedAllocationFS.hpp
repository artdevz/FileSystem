#pragma once
#include "Inode.hpp"
#include <unordered_map>
#include <list>

// Segunda Abordagem
class LinkedAllocationFS {

public:
    LinkedAllocationFS();
    ~LinkedAllocationFS();

    void mkdir(const std::string& name);
    void touch(const std::string& name);
    void ls() const;
    void cd(const std::string& path);
    void move(const std::string& fileName, const std::string& targetDir);
    void writeToFile(const std::string& fileName, const std::string& content);
    void readFromFile(const std::string& fileName);
    void rm(const std::string& name);

private:
    Inode* root;
    Inode* currentDir;

    int nextBlockIndex;

    std::unordered_map<int, std::pair<std::string, int>> linkedBlocks;

};
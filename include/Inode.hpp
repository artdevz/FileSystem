#pragma once
#include <string>
#include <vector>

class Inode {

public:
    enum class Type { FILE, DIRECTORY };

    Inode(std::string name, Type type);

    std::string name;
    Type type;
    size_t size;

    std::vector<int> dataBlocks;
    std::vector<Inode*> children;

    Inode* parent;

    std::string Read();
    void Write(const std::string& data);

    bool IsDirectory() const;

};
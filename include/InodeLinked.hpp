#pragma once
#include <string>
#include <vector>

struct DataBlock {
    std::string data = "null";
    int next = -1;
};

class InodeLinked {
public:
    enum class Type { FILE, DIRECTORY };

    InodeLinked(std::string name, Type type);

    std::string name;
    Type type;
    size_t size;
    int firstBlock;

    std::vector<InodeLinked*> children;
    InodeLinked* parent;

    std::string Read(const std::vector<DataBlock>& blocks) const;
    void Write(const std::string& data, std::vector<DataBlock>& blocks, bool overwrite);

    bool IsDirectory() const;
};

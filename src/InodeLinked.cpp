#include "InodeLinked.hpp"

InodeLinked::InodeLinked(std::string name, Type type)
    : name(name), type(type), size(0), firstBlock(-1), parent(nullptr) {}

std::string InodeLinked::Read(const std::vector<DataBlock>& blocks) const {
    std::string result;
    int current = firstBlock;

    while (current != -1 && current < static_cast<int>(blocks.size())) {
        result += blocks[current].data;
        current = blocks[current].next;
    }

    return result;
}

void InodeLinked::Write(const std::string& data, std::vector<DataBlock>& blocks, bool overwrite) {
    const int blockSize = 8;

    if (overwrite && firstBlock != -1) {
        int current = firstBlock;
        while (current != -1) {
            int next = blocks[current].next;
            blocks[current].data = "null";
            blocks[current].next = -1;
            current = next;
        }
        firstBlock = -1;
        size = 0;
    }

    int prevBlock = -1;
    int i = 0;

    while (i < static_cast<int>(data.size())) {
        std::string chunk = data.substr(i, blockSize);
        i += blockSize;

        int newBlock = -1;
        for (int j = 0; j < static_cast<int>(blocks.size()); ++j) {
            if (blocks[j].data == "null") {
                newBlock = j;
                break;
            }
        }

        if (newBlock == -1) {
            blocks.push_back({ "null", -1 });
            newBlock = blocks.size() - 1;
        }

        blocks[newBlock].data = chunk;
        blocks[newBlock].next = -1;

        if (firstBlock == -1)
            firstBlock = newBlock;
        else
            blocks[prevBlock].next = newBlock;

        prevBlock = newBlock;
        size += chunk.size();
    }
}

bool InodeLinked::IsDirectory() const {
    return type == Type::DIRECTORY;
}

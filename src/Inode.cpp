#include "Inode.hpp"
#include <iostream>

Inode::Inode(std::string name, Type type) :
    name(name), type(type), size(0) {}

std::string Inode::Read(const std::vector<std::string>& blockStorage) const {
    std::string result;
    for (int index : dataBlocks) {
        if (index >= 0 && index < static_cast<int>(blockStorage.size())) result += blockStorage[index];
    }
    return result;
}

void Inode::Write(const std::string& data, std::vector<std::string>& blockStorage, std::vector<int>& freeBlocks, bool overwrite) {
    const int blockSize = 8;

    if (overwrite) {
        for (int i : dataBlocks) {
            if (i >= 0 && i < static_cast<int>(blockStorage.size())) {
                blockStorage[i] = "";
                freeBlocks.push_back(i); 
            }
        }
        dataBlocks.clear();
        size = 0;
    }

    int i = 0;
    while (i < static_cast<int>(data.size())) {
        std::string chunk = data.substr(i, blockSize);
        i += blockSize;

        int index = -1;

        if (!freeBlocks.empty()) {
            index = freeBlocks.back();
            freeBlocks.pop_back();
            blockStorage[index] = chunk;
        } 
        else {
            index = blockStorage.size();
            blockStorage.push_back(chunk);
        }
        dataBlocks.push_back(index);
        size += chunk.size();
    }    
}

bool Inode::IsDirectory() const { return type == Type::DIRECTORY; }

void Inode::DebugPrintBlocks(const std::vector<std::string>& blockStorage) const {
    std::cout << "Blocos de \"" << name << "\": ";
    for (int index : dataBlocks) {
        if (index >= 0 && index < static_cast<int>(blockStorage.size())) {
            std::cout << "[" << index << ": \"" << blockStorage[index] << "\" ";
            continue;
        }
        std::cout << "[" << index << ": inválido] ";
    }
    std::cout << "\n";
}
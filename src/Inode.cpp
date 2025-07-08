#include "Inode.hpp"

Inode::Inode(std::string name, Type type) :
    name(name), type(type) {}

// Concatena blocos referenciados em dataBlocks e retorna o conteúdo
std::string Inode::Read() {
    std::string s = "I-Node lido";
    return s;
}

void Inode::Write(const std::string& data, std::vector<std::string>& blockStorage) {
    blockStorage.push_back(data);
    dataBlocks.push_back(blockStorage.size() - 1);
    size += data.size();
}

bool Inode::IsDirectory() const { return type == Type::DIRECTORY; }
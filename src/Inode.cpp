#include "Inode.hpp"

Inode::Inode(std::string name, Type type) :
    name(name), type(type) {}

std::string Inode::Read() {
    std::string s = "I-Node lido";
    return s;
}

void Inode::Write(const std::string& data) {}

bool Inode::IsDirectory() const { return type == Type::DIRECTORY; }
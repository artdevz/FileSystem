#include "FileSystem.hpp"
#include <iostream>
#include <algorithm>

FileSystem::FileSystem() {
    root = new Inode("/", Inode::Type::DIRECTORY);
    root->parent = nullptr;
    currentDir = root;
    nextBlockIndex = 0;
}

FileSystem::~FileSystem() {}

// CMD

void FileSystem::Mkdir(const std::string& name) {
    if (name.empty()) {
        std::cout << "\033[1;31m[Erro]\033[0m Nome de diretório não pode ser vazio\n";
        return;
    }

    if (FindInode(name)) {
        std::cout << "\033[1;31m[Erro]\033[0m Já existe um arquivo com '" << name << "'\n";
        return;
    }
    
    Inode* newInode = new Inode(name, Inode::Type::DIRECTORY);
    newInode->parent = currentDir;
    currentDir->children.push_back(newInode);
}

void FileSystem::Touch(const std::string& name) {
    if (name.empty()) {
        std::cout << "\033[1;31m[Erro]\033[0m Nome de arquivo não pode ser vazio\n";
        return;
    }

    if (FindInode(name)) {
        std::cout << "\033[1;31m[Erro]\033[0m Já existe um arquivo com '" << name << "'\n";
        return;
    }

    Inode* newInode = new Inode(name, Inode::Type::FILE);
    newInode->parent = currentDir;
    currentDir->children.push_back(newInode);
}

void FileSystem::Ls() const {
    if (!currentDir->children.empty()) {
        for (Inode* child : currentDir->children) {
            std::string name = child->name;
            if (child->IsDirectory()) name = "\033[1;33m" + name + "\033[0m";
            std::cout << name << "   ";
        }
        std::cout << "\n";
    }
}

void FileSystem::Cd(const std::string& path) {
    if (path.empty()) {
        currentDir = root;
        return;
    }

    if (path == "..") { // TO-DO: Fazer com que volte usando cd ../../../
        if (currentDir == root) return;
        currentDir = currentDir->parent;
        return;
    }

    Inode* target = FindInode(path);
    if (!target) {
        std::cout << "\033[1;31m[Erro]\033[0m não há nenhum diretório ou arquivo com esse '"<< path <<"' caminho\n";
        return;
    }

    if (!target->IsDirectory()) {
        std::cout << "\033[1;31m[Erro]\033[0m "<< path <<" não é um diretório\n";
        return;
    }

    currentDir = target;    
}

void FileSystem::Move(const std::string& fileName, const std::string& targetDir) {}

void FileSystem::Echo(const std::string& fileName, const std::string& content, bool overwrite) {
    if (fileName.empty()) {
        std::cout << "\033[1;31m[Erro]\033[0m Nome de arquivo não pode ser vazio\n";
        return;
    }

    Inode* target = FindInode(fileName);
    if (!target) Touch(fileName);

    if (target->IsDirectory()) {
        std::cout << "\033[1;31m[Erro]\033[0m '"<< fileName <<"' é um diretório\n";
        return;
    }
    
    target->Write(content, blockStorage, overwrite);
}

void FileSystem::Cat(const std::string& fileName) {
    Inode* target = FindInode(fileName);
    if (!target) {
        std::cout << "\033[1;31m[Erro]\033[0m não há nenhum diretório ou arquivo com esse '"<< fileName <<"' caminho\n";
        return;
    }
    
    if (target->IsDirectory()) {
        std::cout << "\033[1;31m[Erro]\033[0m '"<< fileName <<"' é um diretório\n";
        return;
    }

    std::cout << target->Read(blockStorage) << "\n";
}

void FileSystem::Rm(const std::string& name, bool recursive) {
    Inode* target = FindInode(name);
    if (!target) {
        std::cout << "\033[1;31m[Erro]\033[0m não há nenhum diretório ou arquivo com esse '"<< name <<"' nome\n";
        return;
    }
    
    if (target->IsDirectory() && !target->children.empty() && !recursive) {
        std::cout << "\033[1;31m[Erro]\033[0m Diretório não está vazio. Use -r para remoção recursiva\n";
        return;
    }

    currentDir->children.erase(
        std::remove(currentDir->children.begin(), currentDir->children.end(), target),
        currentDir->children.end()
    );

    if (recursive) RecursiveDelete(target);
    else DeleteInode(target);
}

// AUX

Inode* FileSystem::FindInode(const std::string& name) const {
    for (Inode* child : currentDir->children) {
        if (child->name == name) return child;
    }
    return nullptr;
}

void FileSystem::DeleteInode(Inode* node) {
    for (int i : node->dataBlocks) {
        if (i >= 0 && i < static_cast<int>(blockStorage.size())) blockStorage[i] = "null";
    }
    delete node;
}

void FileSystem::RecursiveDelete(Inode* node) {}

// GET

std::string FileSystem::GetCurrentPath() const {
    if (currentDir == root) return "/";

    std::string path;
    Inode* node = currentDir;

    while (node != nullptr && node != root) {
        path = "/" + node->name + path;
        node = node->parent;
    }

    return path.empty()? "/" : path;
}
#include "FileSystemLinked.hpp"
#include <iostream>
#include <algorithm>

FileSystemLinked::FileSystemLinked() {
    root = new InodeLinked("/", InodeLinked::Type::DIRECTORY);
    root->parent = nullptr;
    currentDir = root;
    nextBlockIndex = 0;

    blockStorage.resize(100);  // DataBlock default: data = "", next = -1
}

FileSystemLinked::~FileSystemLinked() {
    RecursiveDelete(root);
}

// CMD

void FileSystemLinked::Mkdir(const std::string& name) {
    if (name.empty()) {
        std::cout << "\033[1;31m[Erro]\033[0m Nome de diretório não pode ser vazio\n";
        return;
    }

    if (FindInode(name)) {
        std::cout << "\033[1;31m[Erro]\033[0m Já existe um arquivo com '" << name << "'\n";
        return;
    }

    InodeLinked* newInode = new InodeLinked(name, InodeLinked::Type::DIRECTORY);
    newInode->parent = currentDir;
    currentDir->children.push_back(newInode);
}

void FileSystemLinked::Touch(const std::string& name) {
    if (name.empty()) {
        std::cout << "\033[1;31m[Erro]\033[0m Nome de arquivo não pode ser vazio\n";
        return;
    }

    if (FindInode(name)) {
        std::cout << "\033[1;31m[Erro]\033[0m Já existe um arquivo com '" << name << "'\n";
        return;
    }

    InodeLinked* newInode = new InodeLinked(name, InodeLinked::Type::FILE);
    newInode->parent = currentDir;
    currentDir->children.push_back(newInode);
}

void FileSystemLinked::Ls() const {
    for (InodeLinked* child : currentDir->children) {
        std::string name = child->name;
        if (child->IsDirectory()) name = "\033[1;33m" + name + "\033[0m";
        std::cout << name << "   ";
    }
    std::cout << "\n";
}

void FileSystemLinked::Cd(const std::string& path) {
    if (path.empty()) {
        currentDir = root;
        return;
    }

    if (path == "..") {
        if (currentDir != root) currentDir = currentDir->parent;
        return;
    }

    InodeLinked* target = FindInode(path);
    if (!target) {
        std::cout << "\033[1;31m[Erro]\033[0m não há nenhum diretório ou arquivo com esse '" << path << "' caminho\n";
        return;
    }

    if (!target->IsDirectory()) {
        std::cout << "\033[1;31m[Erro]\033[0m '" << path << "' não é um diretório\n";
        return;
    }

    currentDir = target;
}

void FileSystemLinked::Move(const std::string& fileName, const std::string& targetDir) {
    InodeLinked* file = FindInode(fileName);
    if (!file) {
        std::cout << "\033[1;31m[Erro]\033[0m Arquivo '" << fileName << "' não encontrado\n";
        return;
    }

    InodeLinked* target = FindInode(targetDir);
    if (target && target->IsDirectory()) {
        auto& children = currentDir->children;
        children.erase(std::remove(children.begin(), children.end(), file), children.end());
        file->parent = target;
        target->children.push_back(file);
        return;
    }

    if (!target) {
        file->name = targetDir;
        return;
    }

    std::cout << "\033[1;31m[Erro]\033[0m Já existe '" << targetDir << "' no diretório\n";
}

void FileSystemLinked::Echo(const std::string& fileName, const std::string& content, bool overwrite) {
    if (fileName.empty()) {
        std::cout << "\033[1;31m[Erro]\033[0m Nome de arquivo não pode ser vazio\n";
        return;
    }

    InodeLinked* target = FindInode(fileName);
    if (!target) {
        Touch(fileName);
        target = FindInode(fileName); // recupere após criar
    }

    if (target->IsDirectory()) {
        std::cout << "\033[1;31m[Erro]\033[0m '" << fileName << "' é um diretório\n";
        return;
    }

    target->Write(content, blockStorage, overwrite);
}

void FileSystemLinked::Cat(const std::string& fileName) {
    InodeLinked* target = FindInode(fileName);
    if (!target) {
        std::cout << "\033[1;31m[Erro]\033[0m Arquivo '" << fileName << "' não encontrado\n";
        return;
    }

    if (target->IsDirectory()) {
        std::cout << "\033[1;31m[Erro]\033[0m '" << fileName << "' é um diretório\n";
        return;
    }

    std::cout << target->Read(blockStorage) << "\n";
}

void FileSystemLinked::Rm(const std::string& name, bool recursive) {
    InodeLinked* target = FindInode(name);
    if (!target) {
        std::cout << "\033[1;31m[Erro]\033[0m Nenhum arquivo ou diretório chamado '" << name << "'\n";
        return;
    }

    if (target->IsDirectory() && !target->children.empty() && !recursive) {
        std::cout << "\033[1;31m[Erro]\033[0m Diretório não está vazio. Use -r\n";
        return;
    }

    currentDir->children.erase(
        std::remove(currentDir->children.begin(), currentDir->children.end(), target),
        currentDir->children.end()
    );

    if (recursive) RecursiveDelete(target);
    else DeleteInode(target);
}

// AUXILIARES

InodeLinked* FileSystemLinked::FindInode(const std::string& name) const {
    for (InodeLinked* child : currentDir->children) {
        if (child->name == name) return child;
    }
    return nullptr;
}

void FileSystemLinked::DeleteInode(InodeLinked* node) {
    int current = node->firstBlock;
    while (current != -1) {
        int next = blockStorage[current].next;
        blockStorage[current].data = "";
        blockStorage[current].next = -1;
        current = next;
    }
    delete node;
}

void FileSystemLinked::RecursiveDelete(InodeLinked* node) {
    if (node->IsDirectory()) {
        for (InodeLinked* child : node->children) {
            RecursiveDelete(child);
        }
        node->children.clear();
    }
    DeleteInode(node);
}

// GET

std::string FileSystemLinked::GetCurrentPath() const {
    if (currentDir == root) return "/";

    std::string path;
    InodeLinked* node = currentDir;
    while (node != root) {
        path = "/" + node->name + path;
        node = node->parent;
    }

    return path.empty() ? "/" : path;
}

#include "FileSystem.hpp"
#include <iostream>
#include <algorithm>
#include <sstream>

FileSystem::FileSystem() {
    root = new Inode("/", Inode::Type::DIRECTORY);
    root->parent = nullptr;
    currentDir = root;
    nextBlockIndex = 0;
}

FileSystem::~FileSystem() { 
    RecursiveDelete(root);
}

// CMD

void FileSystem::Mkdir(const std::string& path) {
    if (path.empty()) {
        PRINT_ERROR("Nome de diretório não pode ser vazio");
        return;
    }
    
    Inode* parent = FindInode(path, true);
    if (!parent || !parent->IsDirectory()) {
        PRINT_ERROR("Criação de Diretório não permitida, pois '" << path << "' não é um diretório");
        return;
    }

    std::string name = path.substr(path.find_last_of('/') + 1);

    if (FindChild(parent, name)) {
        PRINT_ERROR("Já existe um arquivo ou diretório com '" << name << "'");
        return;
    }
    
    Inode* newInode = new Inode(name, Inode::Type::DIRECTORY);
    newInode->parent = parent;
    parent->children.push_back(newInode);
}



void FileSystem::Touch(const std::string& path) {
    if (path.empty()) {
        PRINT_ERROR("Nome de arquivo não pode ser vazio");
        return;
    }

    Inode* parent = FindInode(path, true);
    if (!parent || !parent->IsDirectory()) {
        PRINT_ERROR("Criação de Arquivo não permitida, pois '" << path << "' não é um diretório");
        return;
    }

    std::string name = path.substr(path.find_last_of('/') + 1);

    if (FindChild(parent, name)) {
        PRINT_ERROR("Já existe um arquivo ou diretório com '" << name << "'");
        return;
    }

    Inode* newInode = new Inode(name, Inode::Type::FILE);
    newInode->parent = parent;
    parent->children.push_back(newInode);
}



void FileSystem::Ls(const std::string& path) const {
    Inode* dir = path.empty()? currentDir : FindInode(path);

    if (!dir) {
        PRINT_ERROR("Caminho inválido: '" << path << "'");
        return;
    }

    if (!dir->IsDirectory()) {
        PRINT_ERROR("'" << path << "' não é um Diretório");
        return;
    }

    if (!dir->children.empty()) {
        for (Inode* child : dir->children) {
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

    Inode* target = FindInode(path);
    if (!target) {
        PRINT_ERROR("Não há nenhum diretório ou arquivo com esse '"<< path <<"' caminho");
        return;
    }

    if (!target->IsDirectory()) {
        PRINT_ERROR("'"<< path <<"' Não é um diretório");
        return;
    }

    currentDir = target;    
}



void FileSystem::Mv(const std::string& sourcePath, const std::string& destPath) {
    Inode* sourceNode = FindInode(sourcePath);
    if (!sourceNode) {
        PRINT_ERROR("Não há nenhum diretório ou arquivo com esse '"<< sourcePath <<"' nome");
        return;
    }

    if (sourceNode == root) {
        PRINT_ERROR("Não é permitido mover o diretório raiz");
        return;
    }

    Inode* destNode = FindInode(destPath);

    if (destNode && destNode->IsDirectory()) {
        if (FindChild(destNode, sourceNode->name)) {
            PRINT_ERROR("Já existe arquivo com nome '" << sourceNode->name << "' no destino");
            return;
        }

        auto& siblings = sourceNode->parent->children;
        siblings.erase(std::remove(siblings.begin(), siblings.end(), sourceNode), siblings.end());

        sourceNode->parent = destNode;
        destNode->children.push_back(sourceNode);
        return;
    }

    auto splitPath = [](const std::string& path) -> std::pair<std::string, std::string> {
        size_t pos = path.find_last_of('/');
        if (pos == std::string::npos)
            return { "", path };
        return { path.substr(0, pos), path.substr(pos + 1) };
    };

    std::string destParentPath, destName;
    std::tie(destParentPath, destName) = splitPath(destPath);
    
    Inode* destParent = destParentPath.empty()? currentDir : FindInode(destParentPath);

    if (!destParent || !destParent->IsDirectory()) {
        PRINT_ERROR("Diretório pai do destino '" << destParentPath << "' não existe ou não é diretório");
        return;
    }

    if (destName.empty()) destName = sourceNode->name;

    if (FindChild(destParent, destName)) {
        PRINT_ERROR("Já existe arquivo ou diretório com nome '" << destName << "' no destino");
        return;
    }

    if (sourceNode->parent) {
        auto& siblings = sourceNode->parent->children;
        siblings.erase(std::remove(siblings.begin(), siblings.end(), sourceNode), siblings.end());
    }

    sourceNode->parent = destParent;
    sourceNode->name = destName;
    destParent->children.push_back(sourceNode);    
}



void FileSystem::Echo(const std::string& path, const std::string& content, bool overwrite) {
    if (path.empty()) {
        PRINT_ERROR("Nome de arquivo não pode ser vazio");
        return;
    }

    Inode* target = FindInode(path);
    if (!target) {
        Touch(path);
        target = FindInode(path);
        if (!target) {
            PRINT_ERROR("Arquivo ainda não encontrado após Touch");
            return;
        }
    }

    if (target->IsDirectory()) {
        PRINT_ERROR("'"<< path <<"' é um diretório");
        return;
    }
    
    target->Write(content, blockStorage, overwrite);
}



void FileSystem::Cat(const std::string& path) {
    Inode* target = FindInode(path);
    if (!target) {
        PRINT_ERROR("Não há nenhum diretório ou arquivo com esse '"<< path <<"' caminho");
        return;
    }
    
    if (target->IsDirectory()) {
        PRINT_ERROR("'"<< path <<"' é um diretório");
        return;
    }

    std::cout << target->Read(blockStorage) << "\n";
}



void FileSystem::Rm(const std::string& path, bool recursive) {
    Inode* target = FindInode(path);
    if (!target) {
        PRINT_ERROR("Não há nenhum diretório ou arquivo com esse '"<< path <<"' nome");
        return;
    }

    if (target == root) {
        PRINT_ERROR("Não é permitido remover o diretório raiz");
        return;
    }
    
    if (target->IsDirectory() && !target->children.empty() && !recursive) {
        PRINT_ERROR("Diretório não está vazio. Use -r para remoção recursiva");
        return;
    }

    Inode* parent = target->parent;
    parent->children.erase(
        std::remove(parent->children.begin(), parent->children.end(), target),
        parent->children.end()
    );

    if (recursive) RecursiveDelete(target);
    else DeleteInode(target);
}

// AUX

Inode* FileSystem::FindChild(Inode* parent, const std::string& name) const {
    if (!parent) return nullptr;
    for (Inode* child : parent->children) {
        if (child->name == name) return child;
    }
    return nullptr;
}



Inode* FileSystem::FindInode(const std::string& path, bool resolveToParent) const {
    if (path.empty()) return nullptr;

    Inode* current = (path[0] == '/')? root : currentDir;
    std::istringstream ss(path);
    std::string token;
    std::vector<std::string> parts;

    while (std::getline(ss, token, '/')) {
        if (token.empty() || token == ".") continue;
        if (token == "..") {
            if (!parts.empty()) {
                parts.pop_back();
                continue;
            }
            if (current->parent) current = current->parent;
            continue;
        }
        parts.push_back(token);
    }

    if (resolveToParent && !parts.empty()) parts.pop_back();

    for (size_t i = 0; i < parts.size(); i++) {
        const std::string& part = parts[i];
        bool found = false;

        for (Inode* child : current->children) {
            if (child->name == part && (i < parts.size() - 1? child->IsDirectory() : true)) {
                current = child;
                found = true;
                break;
            }
        }
        if (!found) return nullptr;
    }

    return current;
}



void FileSystem::DeleteInode(Inode* node) {
    for (int i : node->dataBlocks) {
        if (i >= 0 && i < static_cast<int>(blockStorage.size())) blockStorage[i] = "null";
    }
    delete node;
}



void FileSystem::RecursiveDelete(Inode* node) {
    if (node->IsDirectory()) {
        for (Inode* child : node->children) RecursiveDelete(child);
        node->children.clear();
    }
    DeleteInode(node);
}

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
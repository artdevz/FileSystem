#include "FileSystemLinked.hpp"
#include <iostream>
#include <algorithm>
#include <tuple>
#include <sstream> 

FileSystemLinked::FileSystemLinked() {
    root = new InodeLinked("/", InodeLinked::Type::DIRECTORY);
    root->parent = nullptr;
    currentDir = root;
    nextBlockIndex = 0;

    blockStorage.resize(100000);  
}

FileSystemLinked::~FileSystemLinked() {
    RecursiveDelete(root);
}

// CMD

void FileSystemLinked::Mkdir(const std::string& path) {
    if (path.empty()) {
        std::cout << "\033[1;31m[Erro]\033[0m Nome de diretório não pode ser vazio\n";
        return;
    }

    InodeLinked* parent = FindInode(path, true);
    if (FindInode(path)) {
        std::cout << "\033[1;31m[Erro]\033[0m Já existe um arquivo com '" << path << "'\n";
        return;
    }

    std::string name = path.substr(path.find_last_of('/') + 1);

    if (FindChild(parent, name)) {
        PRINT_ERROR("Já existe um arquivo ou diretório com '" << name << "'");
        return;
    }

    InodeLinked* newInode = new InodeLinked(name, InodeLinked::Type::DIRECTORY);
    newInode->parent = parent;
    parent->children.push_back(newInode);
}

void FileSystemLinked::Touch(const std::string& path) {
    if (path.empty()) {
        std::cout << "\033[1;31m[Erro]\033[0m Nome de arquivo não pode ser vazio\n";
        return;
    }

    InodeLinked* parent = FindInode(path, true);
    if (!parent || !parent->IsDirectory()) {
        PRINT_ERROR("Criação de Arquivo não permitida, pois '" << path << "' não é um diretório");
        return;
    }

    std::string name = path.substr(path.find_last_of('/') + 1);
    if (FindChild(parent, name)) {
        std::cout << "\033[1;31m[Erro]\033[0m Já existe um arquivo com '" << name << "'\n";
        return;
    }

    InodeLinked* newInode = new InodeLinked(path, InodeLinked::Type::FILE);
    newInode->parent = parent;
    parent->children.push_back(newInode);
}

void FileSystemLinked::Ls(const std::string& path) const {
    InodeLinked* dir = path.empty()? currentDir : FindInode(path);

    if (!dir) {
        PRINT_ERROR("Caminho inválido: '" << path << "'");
        return;
    }

    if (!dir->IsDirectory()) {
        PRINT_ERROR("'" << path << "' não é um Diretório");
        return;
    }

    if (!dir->children.empty()) {
        for (InodeLinked* child : dir->children) {
            std::string name = child->name;
            if (child->IsDirectory()) name = "\033[1;33m" + name + "\033[0m";
            std::cout << name << "   ";
        }
        std::cout << "\n";
    }
}

void FileSystemLinked::Cd(const std::string& path) {
    if (path.empty()) {
        currentDir = root;
        return;
    }

    InodeLinked* target = FindInode(path);
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

void FileSystemLinked::Move(const std::string& sourcePath, const std::string& destPath) {
    InodeLinked* sourceNode = FindInode(sourcePath);
    if (!sourceNode) {
        std::cout << "\033[1;31m[Erro]\033[0m Não há nenhum diretório ou arquivo com esse '" 
                  << sourcePath << "' nome\n";
        return;
    }
    
    if (sourceNode == root) {
        std::cout << "\033[1;31m[Erro]\033[0m Não é permitido mover o diretório raiz\n";
        return;
    }
    
    InodeLinked* destNode = FindInode(destPath);
    if (destNode && destNode->IsDirectory()) {
        if (FindChild(destNode, sourceNode->name)) {
            std::cout << "\033[1;31m[Erro]\033[0m Já existe arquivo com nome '" 
                      << sourceNode->name << "' no destino\n";
            return;
        }
        
        if (sourceNode->parent) {
            auto& siblings = sourceNode->parent->children;
            siblings.erase(std::remove(siblings.begin(), siblings.end(), sourceNode), siblings.end());
        }
        
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
    
    InodeLinked* destParent = destParentPath.empty() ? currentDir : FindInode(destParentPath);
    if (!destParent || !destParent->IsDirectory()) {
        std::cout << "\033[1;31m[Erro]\033[0m Diretório pai do destino '" 
                  << destParentPath << "' não existe ou não é diretório\n";
        return;
    }
    
    if (destName.empty()) destName = sourceNode->name;
    
    if (FindChild(destParent, destName)) {
        std::cout << "\033[1;31m[Erro]\033[0m Já existe arquivo ou diretório com nome '" 
                  << destName << "' no destino\n";
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

void FileSystemLinked::Echo(const std::string& path, const std::string& content, bool overwrite) {
    if (path.empty()) {
        PRINT_ERROR("Nome de arquivo não pode ser vazio");
        return;
    }

    InodeLinked* target = FindInode(path);
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

void FileSystemLinked::Cat(const std::string& path) {
    InodeLinked* target = FindInode(path);
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

void FileSystemLinked::Rm(const std::string& path, bool recursive) {
    InodeLinked* target = FindInode(path);
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

    InodeLinked* parent = target->parent;
    parent->children.erase(
        std::remove(parent->children.begin(), parent->children.end(), target),
        parent->children.end()
    );

    if (recursive) RecursiveDelete(target);
    else DeleteInode(target);
}

// AUXILIARES

InodeLinked* FileSystemLinked::FindChild(InodeLinked* parent, const std::string& name) const {
    if (!parent) return nullptr;
    for (InodeLinked* child : parent->children) {
        if (child->name == name) return child;
    }
    return nullptr;
}

InodeLinked* FileSystemLinked::FindInode(const std::string& path, bool resolveToParent) const {
    if (path.empty()) return nullptr;
    
    InodeLinked* current = (path[0] == '/') ? root : currentDir;
    
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
    
    if (resolveToParent && !parts.empty()) {
        parts.pop_back();
    }
    
    for (size_t i = 0; i < parts.size(); i++) {
        const std::string& part = parts[i];
        bool found = false;
        
        for (InodeLinked* child : current->children) {
            if (child->name == part && (i < parts.size() - 1 ? child->IsDirectory() : true)) {
                current = child;
                found = true;
                break;
            }
        }
        
        if (!found) return nullptr;
    }
    
    return current;
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

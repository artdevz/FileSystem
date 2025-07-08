#pragma once
#include "Inode.hpp"
#include <string>

class FileSystem {

public:
    FileSystem();
    ~FileSystem();

    void Mkdir(const std::string& name);                                        // Cria diretório
    void Touch(const std::string& name);                                        // Cria arquivo simples
    void Ls() const;                                                            // Lista filhos do diretório atual
    void Cd(const std::string& path);                                           // Muda para subdiretório ou ..
    void Move(const std::string& fileName, const std::string& targetDir);       // Adiciona blocos
    void WriteToFile(const std::string& fileName, const std::string& content);  // Imprime conteúdo dos blocos
    void ReadFromFile(const std::string& fileName);                             // Remove de um e adiciona em outro (Ctrl X)
    void Rm(const std::string& name);                                           // Remove arquivo/diretório

    std::string GetCurrentPath() const;

private:
    Inode* root;
    Inode* currentDir;

    int nextBlockIndex;
    std::vector<std::string> blockStorage;

    Inode* FindInode(const std::string& name) const;
    void DeleteInode(Inode* node);
    void RecursiveDelete(Inode* node);

};
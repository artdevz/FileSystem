#pragma once
#include "Inode.hpp"
#include <string>

#define PRINT_ERROR(msg) std::cout << "\033[1;36mBrunOS\033[0m\033[1;35m » \033[0m\033[1;31m[Erro]\033[0m\033[1;35m » \033[0m" << msg << "\n"

class FileSystem {

public:
    FileSystem();
    ~FileSystem();

    void Mkdir(const std::string& path);                                        
    void Touch(const std::string& path);                                        
    void Ls(const std::string& path) const;                                                            
    void Cd(const std::string& path);                                          
    void Mv(const std::string& sourcePath, const std::string& destPath);       
    void Echo(const std::string& path, const std::string& content, bool overwrite);         
    void Cat(const std::string& path);                                      
    void Rm(const std::string& path, bool recursive);                                           

    std::string GetCurrentPath() const;

private:
    Inode* root;
    Inode* currentDir;

    std::vector<std::string> blockStorage;
    std::vector<int> freeBlocks;

    Inode* FindChild(Inode* parent, const std::string& name) const;
    Inode* FindInode(const std::string& path, bool resolveToParent = false) const;
    void DeleteInode(Inode* node);
    void RecursiveDelete(Inode* node);

};
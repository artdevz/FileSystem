#pragma once
#include "FileSystem.hpp"
#include "Inode.hpp"
#include <string>

#define PRINT_ERROR(msg) std::cout << "\033[1;36mBrunOS\033[0m\033[1;35m » \033[0m\033[1;31m[Erro]\033[0m\033[1;35m » \033[0m" << msg << "\n"

class FileSystemIndexed : public FileSystem {

public:
    FileSystemIndexed();
    ~FileSystemIndexed();

    void Mkdir(const std::string& path) override;                                        
    void Touch(const std::string& path) override;                                        
    void Ls(const std::string& path) const override;                                                            
    void Cd(const std::string& path) override;                                          
    void Mv(const std::string& sourcePath, const std::string& destPath) override;       
    void Echo(const std::string& path, const std::string& content, bool overwrite) override;         
    void Cat(const std::string& path) override;                                      
    void Rm(const std::string& path, bool recursive) override;                                           

    std::string GetCurrentPath() const override;

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
#pragma once
#include "Inode.hpp"
#include <string>

class FileSystem {

public:
    FileSystem();
    ~FileSystem();

    void Mkdir(const std::string& name);                                        
    void Touch(const std::string& name);                                        
    void Ls() const;                                                            
    void Cd(const std::string& path);                                          
    void Move(const std::string& fileName, const std::string& targetDir);       
    void Echo(const std::string& fileName, const std::string& content, bool overwrite);         
    void Cat(const std::string& fileName);                                      
    void Rm(const std::string& name);                                           

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
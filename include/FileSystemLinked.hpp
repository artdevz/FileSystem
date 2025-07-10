#pragma once
#include "InodeLinked.hpp"
#include <string>
#include <vector>

#define PRINT_ERROR(msg) std::cout << "\033[1;36mBrunOS\033[0m\033[1;35m » \033[0m\033[1;31m[Erro]\033[0m\033[1;35m » \033[0m" << msg << "\n"

struct DataBlock {
    std::string data;
    int next = -1; 
};

class FileSystemLinked {

public:
    FileSystemLinked();
    ~FileSystemLinked();

    void Mkdir(const std::string& name);                                        
    void Touch(const std::string& name);                                        
    void Ls(const std::string& path) const;                                                          
    void Cd(const std::string& path);                                           
    void Move(const std::string& fileName, const std::string& targetDir);       
    void Echo(const std::string& fileName, const std::string& content, bool overwrite = true);         
    void Cat(const std::string& fileName);                                      
    void Rm(const std::string& name, bool recursive = false);                                           

    std::string GetCurrentPath() const;

private:
    InodeLinked* root;
    InodeLinked* currentDir;

    int nextBlockIndex;
    std::vector<DataBlock> blockStorage;  

    InodeLinked* FindChild(InodeLinked* parent, const std::string& name) const;
    InodeLinked* FindInode(const std::string& path, bool resolveToParent= false) const;
    void DeleteInode(InodeLinked* node);
    void RecursiveDelete(InodeLinked* node);
};

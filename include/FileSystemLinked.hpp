#pragma once
#include "InodeLinked.hpp"
#include <string>
#include <vector>

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
    void Ls() const;                                                            
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

    InodeLinked* FindInode(const std::string& name) const;
    void DeleteInode(InodeLinked* node);
    void RecursiveDelete(InodeLinked* node);
};

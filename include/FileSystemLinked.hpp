#pragma once
#include "FileSystem.hpp"
#include "InodeLinked.hpp"
#include <string>
#include <vector>

#define PRINT_ERROR(msg) std::cout << "\033[1;36mBrunOS\033[0m\033[1;35m » \033[0m\033[1;31m[Erro]\033[0m\033[1;35m » \033[0m" << msg << "\n"


class FileSystemLinked : public FileSystem {
public:
    FileSystemLinked();
    ~FileSystemLinked();

    void Mkdir(const std::string& name) override;                                        
    void Touch(const std::string& name) override;                                        
    void Ls(const std::string& path) const override;                                                          
    void Cd(const std::string& path) override;                                           
    void Mv(const std::string& fileName, const std::string& targetDir) override;       
    void Echo(const std::string& fileName, const std::string& content, bool overwrite = true) override;         
    void Cat(const std::string& fileName) override;                                      
    void Rm(const std::string& name, bool recursive = false) override;                                           
    void PrintBlockStatus() const;    
    void ShowBlocks(const std::string& path) ;
    std::string GetCurrentPath() const override;
    std::vector<DataBlock> blockStorage;  


private:
    InodeLinked* root;
    InodeLinked* currentDir;

    int nextBlockIndex;

    InodeLinked* FindChild(InodeLinked* parent, const std::string& name) const;
    InodeLinked* FindInode(const std::string& path, bool resolveToParent= false) const;
    void DeleteInode(InodeLinked* node);
    void RecursiveDelete(InodeLinked* node);
};

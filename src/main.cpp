#include "FileSystemIndexed.hpp"
#include "FileSystemLinked.hpp"
#include <iostream>
#include <sstream>
#include <string>
#include <memory>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Uso: " << argv[0] << " < 0 | 1 >\n[0] -> Indexado\n[1] -> Encadeado\n";
        return 1;
    }

    bool mode = std::stoi(argv[1]);
    std::unique_ptr<FileSystem> fs;
    if (mode) {
        fs = std::make_unique<FileSystemLinked>();
    }
    else fs = std::make_unique<FileSystemIndexed>();

    std::string input;

    std::cout << "Sistema de Arquivos (I-Node Simulado)\n";

    while(true) {
        std::cout 
        << "\033[1;36m" << "BrunOS" << "\033[0m"
        << "\033[1;35m" << " » " << "\033[1m"
        << "\033[1;33m" << fs->GetCurrentPath() << "\033[0m"
        << "\033[1;35m" << " » " << "\033[0m";
        
        std::getline(std::cin, input);
        std::istringstream iss(input);

        std::string cmd;
        iss >> cmd;

        if (cmd == "mkdir") {
            std::string path;
            iss >> path;
            fs->Mkdir(path);
            continue;
        }

        if (cmd == "touch") {
            std::string path;
            iss >> path;
            fs->Touch(path);
            continue;
        }

        if (cmd == "ls") {
            std::string path;
            iss >> path;
            fs->Ls(path);
            continue;
        }

        if (cmd == "cd") {
            std::string path;
            iss >> path;
            fs->Cd(path);
            continue;
        }

        if (cmd == "echo") {
            std::string content;
            char ch;

            iss >> std::ws;
            iss >> ch;
            if (ch == '"') std::getline(iss, content, '"');
            else {
                PRINT_ERROR("Conteúdo precisa estar entre aspas");
                continue;
            }

            std::string redirect;
            iss >> redirect;
            if (!(redirect == ">" || redirect == ">>")) {
                PRINT_ERROR("Esperado > ou >>");
                continue;
            }

            bool overwrite = (redirect == ">")? true : false;

            std::string path;
            iss >> path;
            fs->Echo(path, content, overwrite);
            continue;
        }

        if (cmd == "cat") {
            std::string path;
            iss >> path;
            fs->Cat(path);
            continue;
        }

        if (cmd == "mv") {
            std::string source;
            std::string destiny;
            iss >> source;
            iss >> destiny;
            fs->Mv(source, destiny);
            continue;
        }

        if (cmd == "rm") {
            std::string path;
            bool recursive = false;
            
            std::string token;
            if (!(iss >> token)) {
                PRINT_ERROR("Nome não informado");
                continue;
            }

            if (token == "-r") {
                recursive = true;
                if (!(iss >> path)) {
                    PRINT_ERROR("Nome não informado após -r");
                    continue;
                }
            }
            else path = token;

            fs->Rm(path, recursive);
            continue;
        }

        if (cmd == "status") {
            if (auto linked = dynamic_cast<FileSystemLinked*>(fs.get())) linked->PrintBlockStatus();
            else PRINT_ERROR("Comando exclusivo do modo encadeado");
            continue;
        }

        if (cmd == "blocos") {
            std::string path;
            iss >> path;
            if (auto linked = dynamic_cast<FileSystemLinked*>(fs.get())) linked->ShowBlocks(path);
            else PRINT_ERROR("Comando exclusivo do modo encadeado");
            continue;
        }

        if (cmd == "exit") {
            std::cout << "\033[1;36mBrunOS\033[0m\033[1;35m » \033[0m\033[1;33mSaindo...\033[0m\n";
            break;
        }

        std::cout << "Comando Inválido.\n";
    }

    return 0;
}
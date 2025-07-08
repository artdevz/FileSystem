#include "FileSystem.hpp"
#include <iostream>
#include <sstream>
#include <string>

int main() {
    FileSystem fs;
    std::string input;

    std::cout << "Sistema de Arquivos (I-Node Simulado)\n";

    while(true) {
        std::cout 
        << "\033[1;32m" << "abgr@PC" << "\033[0m"
        << "\033[37m" << ":" << "\033[1m"
        << "\033[1;34m" << "~" << fs.GetCurrentPath() << "\033[0m"
        << "\033[37m" << "$ " << "\033[0m";
        
        std::getline(std::cin, input);
        std::istringstream iss(input);

        std::string cmd;
        iss >> cmd;

        if (cmd == "mkdir") {
            std::string name;
            iss >> name;
            fs.Mkdir(name);
            continue;
        }

        if (cmd == "touch") {
            std::string name;
            iss >> name;
            fs.Touch(name);
            continue;
        }

        if (cmd == "ls") {
            fs.Ls();
            continue;
        }

        if (cmd == "cd") {
            std::string path;
            iss >> path;
            fs.Cd(path);
            continue;
        }

        if (cmd == "write") {
            std::cout << "Adicionar blocos\n";
            continue;
        }

        if (cmd == "read") {
            std::cout << "Imprimir conteúdo dos blocos\n";
            continue;
        }

        if (cmd == "mv") {
            std::cout << "Mover de diretório\n";
            continue;
        }

        if (cmd == "rm") {
            std::cout << "Remove arquivo/diretório\n";
            continue;
        }

        if (cmd == "exit") {
            std::cout << "Saindo...\n";
            break;
        }

        std::cout << "Comando Inválido.\n";
    }

    return 0;
}
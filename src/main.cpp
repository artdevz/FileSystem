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
        << "\033[1;32m" << "BrunOS" << "\033[0m"
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

        if (cmd == "echo") {
            std::string content;
            char ch;

            iss >> std::ws;
            iss >> ch;
            if (ch == '"') std::getline(iss, content, '"');
            else {
                std::cout << "\033[1;31m[Erro]\033[0m Conteúdo precisa estar entre aspas\n";
                continue;
            }

            std::string redirect;
            iss >> redirect;
            if (!(redirect == ">" || redirect == ">>")) {
                std::cout << "\033[1;31m[Erro]\033[0m Esperado > ou >>\n";
                continue;
            }

            bool overwrite = (redirect == ">")? true : false;

            std::string fileName;
            iss >> fileName;
            fs.Echo(fileName, content, overwrite);
            continue;
        }

        if (cmd == "cat") {
            std::string name;
            iss >> name;
            fs.Cat(name);
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
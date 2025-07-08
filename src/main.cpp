#include "FileSystem.hpp"
#include <iostream>
#include <sstream>
#include <string>

int main() {
    FileSystem fs;
    std::string input;

    std::cout << "Sistema de Arquivos (I-Node Simulado)\n";

    while(true) {
        std::cout << "> ";
        std::getline(std::cin, input);
        std::istringstream iss(input);

        std::string cmd;
        iss >> cmd;

        if (cmd == "mkdir") {
            std::cout << "Criar Diretório\n";
            continue;
        }

        if (cmd == "touch") {
            std::cout << "Criar Arquivo\n";
            continue;
        }

        if (cmd == "ls") {
            std::cout << "Listar filhos\n";
            continue;
        }

        if (cmd == "cd") {
            std::cout << "Navegar pra sub-diretório ou ..\n";
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
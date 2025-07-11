#include "FileSystemIndexed.hpp"
#include "FileSystemLinked.hpp"
#include "GnuplotWrapper.hpp"
#include <benchmark.hpp>
#include <iostream>
#include <sstream>
#include <string>
#include <memory>
#include <filesystem>

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

        if (cmd == "benchmark") {
            std::string operation;
            iss >> operation;

            if (operation == "cd") {
                std::string path;
                iss >> path;
                double time_taken = benchmark_cd(fs.get(), path);
                std::cout << "Tempo de navegação para o diretório '" << path << "': " << time_taken*1000000 << " micro-segundos" << std::endl;
                continue;
            }

            if (operation == "random_access") {
                int numBlocks = 100;     // Número de blocos no arquivo
                int numAccesses = 1000;  // Número de acessos aleatórios

                Inode inodeFile("benchmarkInode", Inode::Type::FILE);
                inodeFile.dataBlocks.resize(numBlocks);
                std::vector<std::string> inodeBlocks(numBlocks);
                for (int i = 0; i < numBlocks; ++i) {
                    inodeFile.dataBlocks[i] = i;
                    inodeBlocks[i] = "data" + std::to_string(i);
                }

                InodeLinked linkedFile("benchmarkLinked", InodeLinked::Type::FILE);
                linkedFile.firstBlock = 0;
                std::vector<DataBlock> linkedBlocks(numBlocks);
                for (int i = 0; i < numBlocks; ++i) {
                    linkedBlocks[i].data = "data" + std::to_string(i);
                    linkedBlocks[i].next = (i == numBlocks - 1) ? -1 : i + 1;
                }

                int countBlocks = CountBlocksInChain(47, linkedBlocks);
                std::cout << "Número de blocos na cadeia a partir do bloco 47: " << countBlocks << "\n";

                BenchmarkResult result;
                BenchmarkRandomAccess(&inodeFile, inodeBlocks, &linkedFile, linkedBlocks, numAccesses, result);
            
                // Preparando os dados para o gráfico
                std::vector<int> x = result.blockIndex;
                std::vector<double> inodeTimes(result.inodeTimes.begin(), result.inodeTimes.end());
                std::vector<double> linkedTimes(result.linkedTimes.begin(), result.linkedTimes.end());
            
                // Plotando o gráfico com o GnuplotWrapper
                GnuplotWrapper plotter;
                plotter.Plot(
                    x, 
                    inodeTimes, 
                    "Inode", 
                    linkedTimes, 
                    "Linked", 
                    "Índice de bloco acessado", 
                    "Tempo em µs", 
                    "Benchmark de Acesso Aleatório - Inode vs Linked", 
                    "benchmark_access.png", 
                    2 // Escala do gráfico no eixo Y
                );
            
                std::cout << "Benchmark concluído. Resultados em benchmark_access.png\n";
                continue;
            }

            if (operation == "read") {
                std::string path;
                iss >> path;
                double time_taken = benchmark_read(fs.get(), path);
                std::cout << "Tempo de leitura do arquivo '" << path << "': " << time_taken*1000000 << " micro-segundos" << std::endl;
                continue;
            }

            if (operation == "write") {
                std::string path, content;
                bool overwrite;
                iss >> path;
                iss >> std::ws; // Ignora qualquer espaço extra
                std::getline(iss, content, '"'); // Lê o conteúdo entre aspas
                std::string redirect;
                iss >> redirect;
        
                overwrite = (redirect == ">"); // Se for '>', sobrescreve, senão faz append
        
                double time_taken = benchmark_write(fs.get(), path, content, overwrite);
                std::cout << "Tempo de escrita no arquivo '" << path << "': " << time_taken*1000000 << " micro-segundos" << std::endl;
                continue;
            }

            if (operation == "mv") {
                std::string source, destiny;
                iss >> source >> destiny;
                double time_taken = benchmark_move(fs.get(), source, destiny);
                std::cout << "Tempo de movimentação do arquivo '" << source << "' para '" << destiny << "': " << time_taken*1000000 << " micro-segundos" << std::endl;
                continue;
            }

            if (operation == "remove") {
                std::string path;
                iss >> path;
                double time_taken = benchmark_remove(fs.get(), path);
                std::cout << "Tempo de remoção do arquivo '" << path << "': " << time_taken*1000000 << " micro-segundos" << std::endl;
                continue;
            }

            PRINT_ERROR("Operação não reconhecida. Use 'cd', 'random_access', 'read', 'write', 'mv' ou 'remove'.");
        }

        if (cmd == "exit") {
            std::cout << "\033[1;36mBrunOS\033[0m\033[1;35m » \033[0m\033[1;33mSaindo...\033[0m\n";
            break;
        }

        std::cout << "Comando Inválido.\n";
    }

    return 0;
}
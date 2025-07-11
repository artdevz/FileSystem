#pragma once

#include <vector>
#include <string>

class GnuplotWrapper {

public:
    void Plot(
        const std::vector<int>& x,               // Índices de blocos
        const std::vector<double>& inodeTimes,   // Tempos de acesso para Inode Indexado
        const std::string& label1,               // Rótulo para Inode Indexado
        const std::vector<double>& linkedTimes,  // Tempos de acesso para Lista Encadeada
        const std::string& label2,               // Rótulo para Lista Encadeada
        const std::string& xlabel,               // Rótulo do eixo X
        const std::string& ylabel,               // Rótulo do eixo Y
        const std::string& title,                // Título do gráfico
        const std::string& fileName,             // Nome do arquivo para salvar o gráfico
        const int scale   
    );

private:
    void RunGnuPlot(const std::string& scriptFile);
    void OpenGnuPlot(const std::string& plotFile);

};
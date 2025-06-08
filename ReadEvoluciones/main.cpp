#include <iostream>
#include <mutex>
#include <thread>
#include <set>
#include "Sources/Utilities/REUtilities.h"

std::mutex datos_mutex;

int main() {
    try {
        auto archivos = buscarArchivosClasificados();
        std::vector<std::string> requeridos = {"UGP1", "JURIDICA", "VIGENTE", "CASTIGADA"};

        std::set<std::string> encontrados;
        for (const auto& [tipo, _] : archivos) {
            encontrados.insert(tipo);
        }

        std::vector<std::string> faltantes;
        for (const auto& r : requeridos) {
            if (!encontrados.count(r)) {
                faltantes.push_back(r);
            }
        }

        if (!faltantes.empty()) {
            std::string msg = "Faltan archivos requeridos: ";
            for (const auto& f : faltantes) msg += f + " ";
            throw std::logic_error(msg);
        }

        std::vector<RowData> todosLosDatos;
        std::vector<std::thread> hilos;

        for (const auto& [tipo, ruta] : archivos) {
            hilos.emplace_back([&todosLosDatos, tipo, ruta]() {
                try {
                    const std::string basePath = "Constants/" + tipo + "/";
                    const auto esperadas = cargarDiccionarioDesdeCSV(basePath + "ESPERADAS.txt");
                    const auto canonicas = cargarDiccionarioDesdeCSV(basePath + "CANONICAL.txt");

                    auto datos = read_excel_file(ruta, tipo, esperadas, canonicas);

                    std::lock_guard<std::mutex> lock(datos_mutex);
                    todosLosDatos.insert(todosLosDatos.end(), datos.begin(), datos.end());

                    std::cout << "[✔] " << ruta << " (" << datos.size() << " filas)\n";
                } catch (const std::exception& e) {
                    std::cerr << "[✖] Error en " << ruta << ": " << e.what() << "\n";
                }
            });
        }

        for (auto& h : hilos) if (h.joinable()) h.join();

        std::cout << "\nTotal filas: " << todosLosDatos.size() << "\n";

    } catch (const std::exception& e) {
        std::cerr << "Error general: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
#include <iostream>
#include <mutex>
#include <thread>
#include <set>
#include "Sources/Utilities/REUtilities.h"

std::mutex datos_mutex;

int main() {
    try {
        const std::vector<std::string> requeridos = {"UGP1", "JURIDICA", "VIGENTE", "CASTIGADA"};
        auto archivos = buscarArchivosClasificados("Datos/", requeridos);
        
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
                    
                    std::vector<RowData> datos;

                    if (ruta.ends_with(".xlsx")) {
                        datos = read_excel_file(ruta, tipo, esperadas, canonicas);
                    } else if (ruta.ends_with(".txt") || ruta.ends_with(".csv")) {
                        datos = read_csv_file(ruta, tipo, esperadas, canonicas);
                    } else {
                        throw std::runtime_error("Extensión no soportada: " + ruta);
                    }
                    

                    std::lock_guard<std::mutex> lock(datos_mutex);
                    todosLosDatos.insert(todosLosDatos.end(), datos.begin(), datos.end());

                    std::cout << "[Success] " << ruta << " (" << datos.size() << " filas)\n";
                } catch (const std::exception& e) {
                    std::cerr << "[Error] Generado en " << ruta << ": " << e.what() << "\n";
                }
            });
        }

        for (auto& h : hilos) if (h.joinable()) h.join();

        std::cout << "\nTotal filas: " << todosLosDatos.size() << "\n";
        // --- Exportar a un archivo .csv ---
        std::ofstream out("resultado_unico.txt");
        if (!out.is_open()) {
            throw std::runtime_error("No se pudo abrir el archivo de salida");
        }

        // Obtener encabezados únicos de todos los datos
        std::set<std::string> columnas;
        for (const auto& fila : todosLosDatos) {
            for (const auto& [col, _] : fila.values) {
                columnas.insert(col);
            }
        }

        // Escribir encabezados
        bool first = true;
        for (const auto& col : columnas) {
            if (!first) out << ",";
            out << col;
            first = false;
        }
        out << "\n";

        // Función auxiliar para imprimir std::variant
       auto convertirAVariableTexto = [](const ValorDato& val) -> std::string {
            return std::visit([](auto&& arg) -> std::string {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    return "\"" + arg + "\"";
                } else if constexpr (std::is_same_v<T, bool>) {
                    return arg ? "true" : "false";
                } else if constexpr (std::is_same_v<T, __int128_t>) {
                    // Conversión manual de __int128_t a string
                    std::string result;
                    __int128_t value = arg;
                    bool negative = false;

                    if (value == 0) return "0";

                    if (value < 0) {
                        negative = true;
                        value = -value;
                    }

                    while (value > 0) {
                        result = static_cast<char>('0' + (value % 10)) + result;
                        value /= 10;
                    }

                    if (negative) result = "-" + result;
                    return result;
                } else {
                    return std::to_string(arg);
                }
            }, val);
        };

        // Escribir cada fila
        for (const auto& fila : todosLosDatos) {
            first = true;
            for (const auto& col : columnas) {
                if (!first) out << ",";
                auto it = fila.values.find(col);
                if (it != fila.values.end()) {
                    out << convertirAVariableTexto(it->second);
                } else {
                    out << "";  // valor vacío si falta
                }
                first = false;
            }
            out << "\n";
        }

        out.close();
        std::cout << "Archivo exportado: resultado_unico.csv\n";

    } catch (const std::exception& e) {
        std::cerr << "Error general: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
#include "Utilities/REUtilities.h"

//Funcion para cargar los diccionarios de nombres canonicos y columnas esperadas
std::unordered_map<std::string, std::string> cargarDiccionarioDesdeCSV(const std::string& path) {
    std::ifstream file(path);
    std::unordered_map<std::string, std::string> dict;
    std::string line;

    if (!file.is_open()) {
        throw std::runtime_error("No se pudo abrir el archivo: " + path);
    }

    // Leer primera línea para detectar separador
    std::getline(file, line);
    const std::vector<char> separadores = {'\t', ',', ';', '|', '-', ':', '.'};
    char separador = '\0';

    for (char sep : separadores) {
        size_t count = std::count(line.begin(), line.end(), sep);
        if (count >= 1) {
            separador = sep;
            break;
        }
    }

    if (separador == '\0') {
        throw std::runtime_error("No se pudo detectar un separador valido en: " + path);
    }

    // Procesar la primera línea
    std::stringstream ss1(line);
    std::string original, canonical;
    if (std::getline(ss1, original, separador) && std::getline(ss1, canonical)) {
        std::transform(original.begin(), original.end(), original.begin(), ::toupper);
        dict[original] = canonical;
    }

    // Procesar las demás líneas
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        if (std::getline(ss, original, separador) && std::getline(ss, canonical)) {
            std::transform(original.begin(), original.end(), original.begin(), ::toupper);
            dict[original] = canonical;
        }
    }

    return dict;
}
#include "Utilities/REUtilities.h"

namespace fs = std::filesystem;

std::vector<std::pair<std::string, std::string>> buscarArchivosClasificados(const std::string& carpeta,
                                                                            const std::vector<std::string>& palabrasClave) {
    std::vector<std::pair<std::string, std::string>> archivos;

    if (!fs::exists(carpeta) || !fs::is_directory(carpeta)) {
        throw std::runtime_error("La carpeta '" + carpeta + "' no existe o no es un directorio.");
    }

    for (const auto& entry : fs::directory_iterator(carpeta)) {
        if (!entry.is_regular_file()) continue;

        std::string filename = entry.path().filename().string();
        std::string filename_upper = filename;
        std::transform(filename_upper.begin(), filename_upper.end(), filename_upper.begin(), ::toupper);

        for (const std::string& palabraClave: palabrasClave){
            if (filename_upper.find(palabraClave) != std::string::npos){
                archivos.emplace_back(palabraClave, entry.path().string());
            }
        }
    }

    return archivos;
}
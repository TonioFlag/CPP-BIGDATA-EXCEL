#include "Utilities/REUtilities.h"

namespace fs = std::filesystem;

std::vector<std::pair<std::string, std::string>> buscarArchivosClasificados(const std::string& carpeta) {
    std::vector<std::pair<std::string, std::string>> archivos;

    if (!fs::exists(carpeta) || !fs::is_directory(carpeta)) {
        throw std::runtime_error("La carpeta '" + carpeta + "' no existe o no es un directorio.");
    }

    for (const auto& entry : fs::directory_iterator(carpeta)) {
        if (!entry.is_regular_file()) continue;

        std::string filename = entry.path().filename().string();
        std::string filename_lower = filename;
        std::transform(filename_lower.begin(), filename_lower.end(), filename_lower.begin(), ::tolower);

        if (filename_lower.ends_with(".xlsx")) {
            if (filename_lower.find("ugp1") != std::string::npos) {
                archivos.emplace_back("UGP1", entry.path().string());
            } else if (filename_lower.find("juridica") != std::string::npos) {
                archivos.emplace_back("JURIDICA", entry.path().string());
            } else if (filename_lower.find("vigente") != std::string::npos) {
                archivos.emplace_back("VIGENTE", entry.path().string());
            } else if (filename_lower.find("castigada") != std::string::npos) {
                archivos.emplace_back("CASTIGADA", entry.path().string());
            }
        }
    }

    return archivos;
}
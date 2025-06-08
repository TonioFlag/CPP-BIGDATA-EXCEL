#include "Utilities/REUtilities.h"
#include "Utilities/dictionary.h"

void replaceAll(std::string& str, const std::string& from, const std::string& to) {
    if (from.empty()) return;
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
}

std::string limpiarCadena(std::string& texto) {
    static const auto sorted_reemplazos = getSortedReemplazos();  // Asume que getSortedReemplazos() está en dictionary.h
    for (const auto& [from, to] : sorted_reemplazos) {
        replaceAll(texto, from, to);
    }
    return texto;
}
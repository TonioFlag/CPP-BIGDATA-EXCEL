#include "Utilities/REUtilities.h"

// Detecta separador a partir de la primera línea
char detectarSeparador(const std::string& header_line) {
    const std::vector<char> separadores = {',', ';', '\t', '|'};
    size_t max_count = 0;
    char mejor_sep = ',';

    for (char sep : separadores) {
        size_t count = std::count(header_line.begin(), header_line.end(), sep);
        if (count > max_count) {
            max_count = count;
            mejor_sep = sep;
        }
    }
    return mejor_sep;
}

// Conversión de valor
ValorDato convertirValor(std::string& valor, const std::string& tipo) {
    std::string upper = valor;
    std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);

    if (upper == "NA" || upper == "N/A" || upper == "NULL" || upper.empty()) return "";

    try {
        if (tipo == "string") return limpiarCadena(valor);
        if (tipo == "int64") return static_cast<int64_t>(std::stoll(valor));
        if (tipo == "int32") return static_cast<int32_t>(std::stoi(valor));
        if (tipo == "int16") return static_cast<int16_t>(std::stoi(valor));
        if (tipo == "int8")  return static_cast<int8_t>(std::stoi(valor));
        if (tipo == "float64" || tipo == "float32") return std::stof(valor);
        if (tipo == "bool") {
            std::transform(valor.begin(), valor.end(), valor.begin(), ::tolower);
            if (valor == "true" || valor == "1") return "true";
            if (valor == "false" || valor == "0") return "false";
            throw std::invalid_argument("Valor bool inválido: " + valor);
        }
    } catch (...) {
        std::cerr << "Error al convertir valor: '" << valor << "' a tipo: " << tipo << "\n";
    }

    return valor;
}

// Procesa CSV/TXT
std::vector<RowData> read_csv_file(const std::string& file_path,
                                   const std::string& name_evol,
                                   const std::unordered_map<std::string, std::string>& columnasEsperadas,
                                   const std::unordered_map<std::string, std::string>& nombreCanonical) {
    std::ifstream file(file_path);
    if (!file.is_open()) throw std::runtime_error("No se pudo abrir el archivo: " + file_path);

    std::string line;
    std::getline(file, line);
    char sep = detectarSeparador(line);

    // Leer cabeceras
    std::istringstream header_stream(line);
    std::string header;
    std::vector<std::string> headers;
    while (std::getline(header_stream, header, sep)) {
        std::string lim = limpiarCadena(header);
        std::transform(lim.begin(), lim.end(), lim.begin(), ::toupper);
        if (nombreCanonical.count(lim)) headers.push_back(nombreCanonical.at(lim));
        else headers.push_back(lim);
    }

    // Crear mapa de índice
    std::unordered_map<std::string, size_t> headerMap;
    for (size_t i = 0; i < headers.size(); ++i) headerMap[headers[i]] = i;

    for (const auto& [colName, _] : columnasEsperadas) {
        if (!headerMap.count(colName)) throw std::runtime_error("Falta columna: " + colName);
    }

    std::vector<RowData> rows;

    while (std::getline(file, line)) {
        std::istringstream row_stream(line);
        std::string cell;
        std::vector<std::string> cells;

        while (std::getline(row_stream, cell, sep)) {
            cells.push_back(cell);
        }

        RowData r;
        for (const auto& [colName, tipo] : columnasEsperadas) {
            size_t idx = headerMap[colName];
            if (idx < cells.size()) {
                r.values[colName] = convertirValor(cells[idx], tipo);
            } else {
                r.values[colName] = "";
            }
        }

        if (!name_evol.empty()) {
            r.values["EVOLUCION_BETA"] = name_evol;
        }

        rows.push_back(r);
    }

    return rows;
}
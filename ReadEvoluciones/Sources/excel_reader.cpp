#include "Utilities/REUtilities.h"

// Identifica hoja con nombre relacionado a "base"
std::string getSheetName(const std::vector<std::string>& hojas) {
    if (hojas.size() == 1) return hojas[0];
    const std::regex base_name(".*base.*|.*bas.*", std::regex_constants::icase);
    for (const auto& hoja : hojas) {
        if (std::regex_search(hoja, base_name)) return hoja;
    }
    throw std::runtime_error("No se encontró una hoja válida con nombre 'base'");
}

// Construye mapa de cabeceras
std::unordered_map<std::string, uint64_t> getHeaderMap(const OpenXLSX::XLWorksheet& sheet, const std::unordered_map<std::string, std::string>& nombreCanonical) {
    std::unordered_map<std::string, uint64_t> headerMap;

    for (uint64_t col = 1; col <= sheet.columnCount(); ++col) {
        std::string name = sheet.cell(1, col).value().get<std::string>();
        std::transform(name.begin(), name.end(), name.begin(), ::toupper);
        name.erase(name.find_last_not_of(" \n\r\t") + 1);
        name.erase(0, name.find_first_not_of(" \n\r\t"));

        if (nombreCanonical.count(name)) {
            headerMap[nombreCanonical.at(name)] = col;
        } else {
            headerMap[name] = col;
        }
    }
    return headerMap;
}

// Obtiene el valor de celda y lo convierte al tipo correspondiente
ValorDato getColumnValue(OpenXLSX::XLCell& cell, const std::string& tipo) {

    std::string string_representation_of_value;

    switch (cell.value().type()) {
        case OpenXLSX::XLValueType::Empty:
            string_representation_of_value = std::string("");
        case OpenXLSX::XLValueType::String: { // Usar bloque para ámbito de variables temporales
            string_representation_of_value = cell.value().get<std::string>();
            std::string temp_val_upper = string_representation_of_value;
            std::transform(temp_val_upper.begin(), temp_val_upper.end(), temp_val_upper.begin(), ::toupper);
            // Si es una cadena vacía o valores "nulos", retornamos una cadena vacía inmediatamente.
            // Esto evita intentar convertir "NA" o "NULL" a números, por ejemplo.
            if (temp_val_upper == "NA" || temp_val_upper == "N/A" || temp_val_upper == "NULL" || string_representation_of_value.empty()) {
                string_representation_of_value = std::string(""); // Devuelve una cadena vacía como ValorDato
            }
            // Si no es un valor "nulo" y es una cadena, la dejamos para limpieza y posible conversión.
            break;
        }
        case OpenXLSX::XLValueType::Integer:
            string_representation_of_value = std::to_string(cell.value().get<int64_t>());
            break;
        case OpenXLSX::XLValueType::Float:
            string_representation_of_value = std::to_string(cell.value().get<double>());
            break;
        case OpenXLSX::XLValueType::Boolean:
            string_representation_of_value = cell.value().get<bool>() ? "true" : "false";
            break;
        case OpenXLSX::XLValueType::Error:
        default:
            // Si hay un error o un tipo desconocido, se devuelve una cadena vacía.
            string_representation_of_value = "";
            break;
    }

    try {
        if (tipo == "string") return limpiarCadena(string_representation_of_value);
        if (tipo == "int128") return static_cast<__int128_t>(std::stoll(string_representation_of_value));
        if (tipo == "int64")  return static_cast<int64_t>(std::stoll(string_representation_of_value));
        if (tipo == "int32")  return static_cast<int32_t>(std::stoi(string_representation_of_value));
        if (tipo == "int16")  return static_cast<int16_t>(std::stoi(string_representation_of_value));
        if (tipo == "int8")   return static_cast<int8_t>(std::stoi(string_representation_of_value));
        if (tipo == "float64" || tipo == "float32") return static_cast<float_t>(std::stof(string_representation_of_value));
        if (tipo == "bool") { // manejar "true"/"false" a bool
            std::string temp_bool_val = string_representation_of_value;
            std::transform(temp_bool_val.begin(), temp_bool_val.end(), temp_bool_val.begin(), ::tolower);
            if (temp_bool_val == "true" || temp_bool_val == "1") return true;
            if (temp_bool_val == "false" || temp_bool_val == "0") return false;
            // Si no es "true"/"false"/"1"/"0", lanza un error.
            throw std::invalid_argument("No se pudo convertir a booleano: " + string_representation_of_value);
        }
    } catch (const std::exception& e) {
        std::cerr << "[Error] Fallo conversión '" << string_representation_of_value << "' a tipo '" << tipo << "': " << e.what() << "\n";
    }

    return string_representation_of_value;  // Valor sin transformar si falla
}

// Lee un archivo Excel y devuelve las filas procesadas
std::vector<RowData> read_excel_file(const std::string& file_path,
                                     const std::string& name_evol,
                                     const std::unordered_map<std::string, std::string>& columnasEsperadas,
                                     const std::unordered_map<std::string, std::string>& nombreCanonical) {
    std::vector<RowData> rows;
    OpenXLSX::XLDocument doc;
    doc.open(file_path);

    std::string hoja = getSheetName(doc.workbook().sheetNames());
    auto sheet = doc.workbook().worksheet(hoja);
    const auto headerMap = getHeaderMap(sheet, nombreCanonical);

    for (const auto& [colName, _] : columnasEsperadas) {
        if (headerMap.find(colName) == headerMap.end()) {
            throw std::runtime_error("Falta columna requerida: " + colName + " en el excel " + file_path + " en la hoja " + hoja);
        }
    }

    if (sheet.rowCount() <= 1) {
        throw std::length_error("La hoja " + hoja + " no trae datos para procesar.");
    }

    for (uint64_t row = 2; row <= sheet.rowCount(); ++row) {
        RowData r;
        for (const auto& [colName, tipo] : columnasEsperadas) {
            auto cell = sheet.cell(row, headerMap.at(colName));
            r.values[colName] = getColumnValue(cell, tipo);
        }
        if (!name_evol.empty()) {
            r.values["EVOLUCION_BETA"] = name_evol;
        }
        rows.push_back(r);
    }

    doc.close();
    return rows;
}
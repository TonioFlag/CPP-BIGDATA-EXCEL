#pragma once

// ---- Includes estándar necesarios por los headers originales ----
#include <string>
#include <vector>
#include <unordered_map>
#include <variant>
#include <cstdint>
#include <filesystem>
#include <algorithm>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <regex>
#include <OpenXLSX.hpp>

// ---- Declaraciones de tipos ----
using ValorDato = std::variant<std::string, __int128_t, int64_t, int32_t, int16_t, int8_t, float_t>;

struct RowData {
    std::unordered_map<std::string, ValorDato> values;
};

// ---- archivos.h ----
std::vector<std::pair<std::string, std::string>> buscarArchivosClasificados(const std::string& carpeta = "Datos/");

// ---- diccionario.h ----
std::unordered_map<std::string, std::string> cargarDiccionarioDesdeCSV(const std::string& path);

// ---- limpieza.h ----
void replaceAll(std::string& str, const std::string& from, const std::string& to);
std::string limpiarCadena(std::string& texto);

// ---- excel_reader.h ----
std::vector<RowData> read_excel_file(const std::string& file_path,
                                     const std::string& name_evol,
                                     const std::unordered_map<std::string, std::string>& columnasEsperadas,
                                     const std::unordered_map<std::string, std::string>& nombreCanonical);
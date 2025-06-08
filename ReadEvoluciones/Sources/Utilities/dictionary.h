#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <string>
#include <unordered_map>
#include <vector> // Para la versión ordenada
#include <algorithm> // Para std::sort

// Definición del diccionario de reemplazos.
// Declarado como 'const' para asegurar que no se modifique.
// Declarado como 'inline' para evitar problemas de "multiple definition"
// si este .h se incluye en varios .cpp, ya que 'inline' permite que la definición
// exista en múltiples unidades de traducción siempre y cuando sean idénticas.
const inline std::unordered_map<std::string, std::string> REEMPLAZOS_RAW = {
    // --- 1. Caracteres con tilde/diéresis a su versión sin tilde (básicos y mayúsculas) ---
    {"á", "a"}, {"é", "e"}, {"í", "i"}, {"ó", "o"}, {"ú", "u"},
    {"Á", "A"}, {"É", "E"}, {"Í", "I"}, {"Ó", "O"}, {"Ú", "U"},
    {"ü", "u"}, {"Ü", "U"},

    // --- 2. Ñ y sus variantes "rotas" y normalización a 'n' ---
    {"ñ", "n"}, {"Ñ", "N"},
    {"n~", "n"}, {"~n", "n"},
    {"nÂ", "n"}, {"NÂ", "N"},
    {"nÃ", "n"}, {"NÃ", "N"},
    {"ni", "n"}, {"Ni", "N"},
    {"nh", "n"}, {"Nh", "N"},

    // --- 3. Caracteres especiales y símbolos que a menudo se "rompen" o se quieren normalizar ---
    {"ç", "c"}, {"Ç", "C"},
    {"º", "o"}, {"ª", "a"},
    {"©", "c"}, {"®", "r"}, {"™", "tm"},
    {"€", "e"}, {"\u20ac", "e"},
    {"\u00a3", "libras"}, {"\u00a5", "yenes"},

    // --- 4. Entidades HTML y XML comunes (normalizadas) ---
    {"&amp;", "&"}, {"&lt;", "<"}, {"&gt;", ">"},
    {"&quot;", "\""}, {"&apos;", "'"}, {"&#039;", "'"},
    {"&ntilde;", "n"}, {"&Ntilde;", "N"},
    {"&aacute;", "a"}, {"&eacute;", "e"}, {"&iacute;", "i"}, {"&oacute;", "o"}, {"&uacute;", "u"},
    {"&Aacute;", "A"}, {"&Eacute;", "E"}, {"&Iacute;", "I"}, {"&Oacute;", "O"}, {"&Uacute;", "U"},
    {"&uuml;", "u"}, {"&Uuml;", "U"},
    {"&nbsp;", " "},

    // --- 5. Guiones, comillas y apóstrofos tipográficos (normalizados a ASCII estándar) ---
    {"â€“", "-"}, {"â€”", "-"}, {"â€", "-"},
    {"\u2013", "-"}, {"\u2014", "-"}, {"\u2010", "-"}, {"\u2011", "-"}, {"\u2012", "-"},
    {"â€œ", "\""}, {"â€", "\""},
    {"\u201c", "\""}, {"\u201d", "\""}, {"\u201e", "\""}, {"\u00ab", "\""}, {"\u00bb", "\""},
    {"â€˜", "'"}, {"â€™", "'"},
    {"\u2018", "'"}, {"\u2019", "'"}, {"\u201a", "'"},

    // --- 6. Espacios y caracteres invisibles no estándar (normalizados a espacio regular o eliminado) ---
    {"\xc2\xa0", " "}, {"\u00A0", " "},
    {"\t", " "}, {"\r", ""}, {"\n", " "},
    {"\u200b", ""}, {"\uFEFF", ""},

    // --- 7. Casos de "rotura" específicos y errores comunes de codificación/OCR (ej. tu "afa~") ---
    {"Ã­", "i"}, {"Ã¬", "i"}, {"Ã¯", "i"}, {"Ã®", "i"},
    {"Ã‰", "E"}, {"ÃŒ", "I"}, {"Ã?", "i"}, {"ÃI", "I"},
    {"afa~", "i"}, {"AfA~", "I"},
    {"i'", "i"}, {"I'", "I"}, {"i`", "i"}, {"I`", "I"}, {"i?", "i"}, {"I?", "I"},
    {"î", "i"}, {"ï", "i"}, {"l", "i"}, {"1", "i"}, {"í-", "i"}, {"I-", "I"},
    {"ì", "i"}, {"î", "i"}, {"ï", "i"}, {"Ì", "I"}, {"Î", "I"}, {"Ï", "I"},

    // --- 8. Errores de codificación de dos bytes a un carácter simple (mojibake) ---
    {"Ã±", "n"}, {"Ã‘", "N"},
    {"Ã¡", "a"}, {"Ã©", "e"}, {"Ã³", "o"}, {"Ãº", "u"},
    {"Ã", "a"}, {"Ãš", "U"},
    {"Ã¶", "o"}, {"Ãœ", "U"},
    {"Â¡", ""}, {"Â¿", ""}, {"â€“", "-"}, {"â€”", "-"},
    {"Ã", "I"},
    {"Ä±", "i"}, {"ÄŸ", "g"}, {"Ä°", "I"}, {"Äœ", "g"},
    {"Ä‚", "a"}, {"Äƒ", "a"}, {"Åž", "s"}, {"ÅŸ", "s"},
    {"Ä‚", "A"}, {"Å›", "s"}, {"Å¼", "z"}, {"Å£", "t"},
    {"Ä†", "c"}, {"Ä‡", "c"}, {"ÄŽ", "d"}, {"Ä‘", "d"},
    {"Ä˜", "e"}, {"Ä™", "e"}, {"Ä›", "e"}, {"Äš", "s"},
    {"Å½", "z"}, {"Å¾", "z"},

    // --- 9. Puntos suspensivos, asteriscos, etc. ---
    {"…", "..."}, {"\u2026", "..."},
    {"*", ""}, {"#", ""},

    // --- 10. Caracteres invisibles o de control adicionales ---
    {"\u200C", ""}, {"\u200D", ""},
    {"\u00ED\u200B", "i"}, {"\u00CD\u200B", "I"},
    {"Ã\u00AD", "i"},

    // --- 11. Errores de OCR comunes (ej. números que parecen letras o viceversa) ---
    {"l", "1"}, {"O", "0"}, {"o", "0"}, {"S", "5"}, {"B", "8"}, {"Z", "2"},

    // --- 12. Normalización de puntuación compleja a simple ---
    {"!", ""}, {"?", ""}, {"(", ""}, {")", ""},
    {"[", ""}, {"]", ""}, {"{", ""}, {"}", ""},

    // --- 13. Secuencias de caracteres que resultan de la "rotura" de caracteres con acentos graves o circunflejos (menos comunes en español pero posibles) ---
    {"Ã¨", "e"}, {"Ã¬", "i"}, {"Ã²", "o"}, {"Ã¹", "u"},
    {"Ã¢", "a"}, {"Ãª", "e"}, {"Ã®", "i"}, {"Ã´", "o"}, {"Ã»", "u"},
    {"Ä…", "a"}, {"Ä™", "e"}, {"Ä’", "e"}, {"Ä«", "i"}, {"ÅŒ", "o"}, {"Åª", "u"},

    // --- Adiciones sugeridas ---
    {"\u00b0", ""}, // Símbolo de grado
    {"â€™", "'"}, // Otro apóstrofo rizado
    {"â€œ", "\""}, {"â€", "\""} // Otro par de comillas rizadas
};

// Se recomienda tener una versión "ordenada" de los reemplazos
// para asegurar que las secuencias más largas se reemplacen primero.
// Esto se inicializa una vez y se usa en la función de limpieza.
const inline std::vector<std::pair<std::string, std::string>> getSortedReemplazos() {
    std::vector<std::pair<std::string, std::string>> sorted_reemplazos;
    for (const auto& pair : REEMPLAZOS_RAW) {
        sorted_reemplazos.push_back(pair);
    }
    std::sort(sorted_reemplazos.begin(), sorted_reemplazos.end(),
              [](const auto& a, const auto& b) {
                  return a.first.length() > b.first.length(); // Orden descendente por longitud
              });
    return sorted_reemplazos;
}

#endif // DICTIONARY_H
#pragma once
#include <cstdint>
#include <string>

namespace hf2 {

// Comprime con Huffman a formato HF2 **solo si** el resultado queda más pequeño.
// Si no reduce, lanza std::runtime_error (NO genera salida).
// Solo se admite .txt por ahora (se valida por extensión).
void compress_autosafe_hf2(const std::string& inPath, const std::string& outPath);

// Descomprime archivo HF2 (Huffman). Valida encabezado.
// Para .txt comprimidos con esta utilidad.
void decompress_hf2(const std::string& inPath, const std::string& outPath);

} // namespace hf2

#pragma once
#include <string>
#include <vector>
#include <cstdint>

namespace futil {

// Lee todo el archivo a memoria (syscalls).
std::vector<uint8_t> read_all(const std::string& path);

// Escribe todo el buffer a 'path' (syscalls).
void write_all(const std::string& path, const std::vector<uint8_t>& data);

// Tamaño (bytes) usando stat.
std::uint64_t file_size(const std::string& path);

// Copia permisos (modo) de src a dst.
void copy_mode(const std::string& src, const std::string& dst);

// Aceptamos .txt por ahora.
bool is_txt(const std::string& path);

// Endianness helpers (little-endian).
void write_u32_le(std::vector<uint8_t>& out, std::uint32_t v);
void write_u64_le(std::vector<uint8_t>& out, std::uint64_t v);
std::uint32_t read_u32_le(const uint8_t* p);
std::uint64_t read_u64_le(const uint8_t* p);

} // namespace futil

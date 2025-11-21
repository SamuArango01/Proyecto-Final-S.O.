#include "crypto.hpp"
#include <vector>
#include <cstdint>
#include <string>
#include <cstddef>

namespace crypto {

    // Hash sencillo tipo FNV-1a para derivar una semilla a partir del password.
    static std::uint32_t simpleHash(const std::string& s) {
        std::uint32_t h = 2166136261u;
        for (unsigned char c : s) {
            h ^= static_cast<std::uint8_t>(c);
            h *= 16777619u;
        }
        return h;
    }

    // Generador lineal congruencial para producir un keystream
    // pseudoaleatorio a partir de la contraseña.
    std::vector<std::uint8_t> keyStream(const std::string& password,
                                        std::size_t length) {
        std::vector<std::uint8_t> ks(length);
        std::uint32_t state = simpleHash(password);

        for (std::size_t i = 0; i < length; ++i) {
            state = state * 1664525u + 1013904223u;
            ks[i] = static_cast<std::uint8_t>(state >> 24);
        }

        return ks;
    }

    // Encriptación: XOR con keystream (stream cipher sencillo y 100% reversible).
    void encryptBuffer(std::vector<std::uint8_t>& data,
                       const std::string& password) {
        if (data.empty()) return;

        auto ks = keyStream(password, data.size());
        for (std::size_t i = 0; i < data.size(); ++i) {
            data[i] ^= ks[i];
        }
    }

    // Desencriptación: exactamente la misma operación (XOR con el mismo keystream).
    void decryptBuffer(std::vector<std::uint8_t>& data,
                       const std::string& password) {
        if (data.empty()) return;

        auto ks = keyStream(password, data.size());
        for (std::size_t i = 0; i < data.size(); ++i) {
            data[i] ^= ks[i];
        }
    }

} // namespace crypto

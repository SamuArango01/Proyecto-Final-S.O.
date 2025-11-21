#ifndef CRYPTO_HPP
#define CRYPTO_HPP

#include <string>
#include <vector>
#include <cstdint>

namespace crypto {

    // Genera un flujo de bytes pseudoaleatorio a partir de la clave
    std::vector<uint8_t> keyStream(const std::string& password, size_t length);

    // Cifra un buffer en memoria (in-place)
    void encryptBuffer(std::vector<uint8_t>& data, const std::string& password);

    // Descifra un buffer en memoria (in-place)
    void decryptBuffer(std::vector<uint8_t>& data, const std::string& password);

}

#endif // CRYPTO_HPP

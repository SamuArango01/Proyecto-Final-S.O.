#include "huffman.hpp"
#include "file_util.hpp"
#include "crypto.hpp"

#include <iostream>
#include <string>
#include <stdexcept>
#include <vector>
#include <cstdint>

// Compara dos archivos byte a byte y dice si son idénticos.
static bool archivosIguales(const std::string& a, const std::string& b) {
    std::vector<std::uint8_t> da = futil::read_all(a);
    std::vector<std::uint8_t> db = futil::read_all(b);
    if (da.size() != db.size()) return false;
    for (std::size_t i = 0; i < da.size(); ++i) {
        if (da[i] != db[i]) return false;
    }
    return true;
}

int main() {
    const std::string baseFile         = "base.txt";
    const std::string compressedFile   = "textocomprimido.txt";
    const std::string decompressedFile = "textodescomprimido.txt";
    const std::string encryptedFile    = "textoencriptado.txt";
    const std::string decryptedFile    = "textodesencriptado.txt";

    const std::uint64_t MIN_SIZE_FOR_COMPRESSION = 100; // bytes mínimos

    try {
        // Comprobar que base.txt existe y tiene tamaño suficiente
        std::uint64_t size = futil::file_size(baseFile);

        if (size < MIN_SIZE_FOR_COMPRESSION) {
            std::cout << "El archivo '" << baseFile << "' tiene " << size
                      << " bytes.\n";
            std::cout << "Archivo demasiado ligero y la compresion no es posible.\n";
            return 0;
        }

        // MENÚ PERSISTENTE
        while (true) {
            std::cout << "\n=============================================\n";
            std::cout << "  Bienvenido al sistema de compresion 007\n";
            std::cout << "  Archivo base: " << baseFile << " (" << size << " bytes)\n";
            std::cout << "=============================================\n";
            std::cout << "Selecciona la operacion a realizar:\n";
            std::cout << "  -c  Comprimir base.txt -> textocomprimido.txt\n";
            std::cout << "  -d  Descomprimir textocomprimido.txt -> textodescomprimido.txt\n";
            std::cout << "      (y comparar con base.txt)\n";
            std::cout << "  -e  Encriptar base.txt -> textoencriptado.txt\n";
            std::cout << "  -u  Desencriptar textoencriptado.txt -> textodesencriptado.txt\n";
            std::cout << "      (y comparar con base.txt)\n";
            std::cout << "  -s  Salir\n";
            std::cout << "\nOperacion: ";

            std::string op;
            std::cin >> op;

            if (op == "-s" || op == "s") {
                std::cout << "Saliendo del programa...\n";
                break;
            }

            if (op == "-c" || op == "c") {
                try {
                    // COMPRIMIR base.txt -> textocomprimido.txt
                    hf2::compress_autosafe_hf2(baseFile, compressedFile);

                    std::uint64_t originalSize   = futil::file_size(baseFile);
                    std::uint64_t compressedSize = futil::file_size(compressedFile);

                    double tc = static_cast<double>(compressedSize) /
                                static_cast<double>(originalSize);

                    std::cout << "\n>> Se ha generado el archivo comprimido: "
                              << compressedFile << "\n";
                    std::cout << "Tasa de compresion (TC): " << tc
                              << " | Reduccion: " << (1.0 - tc) * 100.0 << "%\n";
                } catch (const std::exception& e) {
                    std::cerr << "ERROR al comprimir: " << e.what() << "\n";
                }
            }
            else if (op == "-d" || op == "d") {
                try {
                    // DESCOMPRIMIR textocomprimido.txt -> textodescomprimido.txt
                    hf2::decompress_hf2(compressedFile, decompressedFile);
                    std::cout << "\n>> Se ha generado el archivo descomprimido: "
                              << decompressedFile << "\n";

                    // Comparar con base.txt
                    bool iguales = archivosIguales(baseFile, decompressedFile);
                    if (iguales) {
                        std::cout << "Verificacion: El archivo DESCOMPRIMIDO es "
                                     "IDENTICO a base.txt\n";
                    } else {
                        std::cout << "Verificacion: El archivo DESCOMPRIMIDO NO "
                                     "coincide con base.txt\n";
                    }
                } catch (const std::exception& e) {
                    std::cerr << "ERROR al descomprimir: " << e.what() << "\n";
                }
            }
           else if (op == "-e" || op == "e") {
    try {
        // ENCRIPTAR base.txt -> textoencriptado.txt
        std::string password;
        std::cout << "Clave para encriptar: ";
        std::cin >> password;

        std::vector<std::uint8_t> data = futil::read_all(baseFile); // 👈 base.txt
        crypto::encryptBuffer(data, password);
        futil::write_all(encryptedFile, data);
        futil::copy_mode(baseFile, encryptedFile);

        std::cout << "\n>> Se ha generado el archivo encriptado: "
                  << encryptedFile << "\n";
    } catch (const std::exception& e) {
        std::cerr << "ERROR al encriptar: " << e.what() << "\n";
    }
}
            else if (op == "-u" || op == "u") {
    try {
        // DESENCRIPTAR textoencriptado.txt -> textodesencriptado.txt
        std::string password;
        std::cout << "Clave para desencriptar: ";
        std::cin >> password;

        std::vector<std::uint8_t> data = futil::read_all(encryptedFile);
        crypto::decryptBuffer(data, password);
        futil::write_all(decryptedFile, data);
        futil::copy_mode(encryptedFile, decryptedFile);

        std::cout << "\n>> Se ha generado el archivo desencriptado: "
                  << decryptedFile << "\n";

        // Comparar con base.txt
        bool iguales = archivosIguales(baseFile, decryptedFile);
        if (iguales) {
            std::cout << "Verificacion: El archivo DESENCRIPTADO es IDENTICO a '"
                      << baseFile << "'.\n";
        } else {
            std::cout << "Verificacion: El archivo DESENCRIPTADO NO coincide con '"
                      << baseFile << "'.\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "ERROR al desencriptar: " << e.what() << "\n";
    }
}

            else {
                std::cout << "Operacion no reconocida. Intente de nuevo.\n";
            }
        }

    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << "\n";
        return 1;
    }

    return 0;
}

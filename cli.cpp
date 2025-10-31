#include "huffman.hpp"
#include "file_util.hpp"
#include <iostream>
#include <string>
#include <stdexcept>

// CLI mínima (centrada en .txt + Huffman):
// -c  : comprimir (Huffman HF2, autosafe estricto: si no reduce, error)
// -d  : descomprimir (HF2)
// -i  : entrada
// -o  : salida
// --comp-alg huffman (opcional, por defecto huffman)

int main(int argc, char** argv) {
    bool doCompress=false, doDecompress=false;
    std::string inPath, outPath;
    std::string compAlg = "huffman";

    for (int i=1; i<argc; ++i) {
        std::string a = argv[i];
        if (a == "-c") doCompress = true;
        else if (a == "-d") doDecompress = true;
        else if (a == "-i" && i+1<argc) inPath = argv[++i];
        else if (a == "-o" && i+1<argc) outPath = argv[++i];
        else if (a == "--comp-alg" && i+1<argc) compAlg = argv[++i];
        else {
            std::cerr << "Uso:\n"
                      << "  " << argv[0] << " -c -i <input.txt> -o <output.hf2> [--comp-alg huffman]\n"
                      << "  " << argv[0] << " -d -i <input.hf2> -o <output.txt>\n";
            return 2;
        }
    }

    if ((doCompress && doDecompress) || (!doCompress && !doDecompress)) {
        std::cerr << "Especifique una sola operacion: -c o -d\n";
        return 2;
    }
    if (inPath.empty() || outPath.empty()) {
        std::cerr << "Faltan -i o -o\n";
        return 2;
    }
    if (doCompress && compAlg != "huffman") {
        std::cerr << "Por ahora solo se admite --comp-alg huffman\n";
        return 2;
    }

    try {
        if (doCompress) {
            hf2::compress_autosafe_hf2(inPath, outPath);
            std::uint64_t a = futil::file_size(inPath);
            std::uint64_t b = futil::file_size(outPath);
            double tc = (double)b / (double)a;
            std::cout << "OK: comprimido " << b << " / " << a << " bytes (TC=" << tc
                      << ", reduccion=" << (1.0 - tc)*100.0 << "%)\n";
        } else {
            hf2::decompress_hf2(inPath, outPath);
            std::cout << "OK: descomprimido\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << "\n";
        return 1;
    }
    return 0;
}

#include "huffman.hpp"
#include "file_util.hpp"

#include <array>
#include <queue>
#include <vector>
#include <stdexcept>
#include <cstdint>

namespace hf2 {

// Formato HF2:
//  [0..2]  : 'H' 'F' '2'
//  [3]     : method (1 = Huffman)
//  [4..11] : uint64_t original_size (LE)
//  [12..12+256*4-1] : 256 uint32_t frecuencias (LE)
//  [resto] : bitstream Huffman

struct Node {
    uint8_t  byte;
    bool     leaf;
    uint64_t freq;
    Node*    left;
    Node*    right;

    Node(uint8_t b, uint64_t f, bool isLeaf)
        : byte(b), leaf(isLeaf), freq(f), left(nullptr), right(nullptr) {}
};

struct NodeCmp {
    bool operator()(const Node* a, const Node* b) const {
        return a->freq > b->freq;
    }
};

struct Code {
    uint64_t bits;
    uint8_t  len;
};

static void freeTree(Node* n) {
    if (!n) return;
    freeTree(n->left);
    freeTree(n->right);
    delete n;
}

static Node* buildTree(const std::array<uint64_t,256>& freq) {
    std::priority_queue<Node*, std::vector<Node*>, NodeCmp> pq;
    for (int i = 0; i < 256; ++i) {
        if (freq[i] == 0) continue;
        pq.push(new Node(static_cast<uint8_t>(i), freq[i], true));
    }

    if (pq.empty()) {
        return nullptr;
    }

    if (pq.size() == 1) {
        Node* a = pq.top(); pq.pop();
        Node* p = new Node(0, a->freq, false);
        p->left = a;
        pq.push(p);
    }

    while (pq.size() > 1) {
        Node* a = pq.top(); pq.pop();
        Node* b = pq.top(); pq.pop();
        Node* p = new Node(0, a->freq + b->freq, false);
        p->left  = a;
        p->right = b;
        pq.push(p);
    }

    return pq.top();
}

static void genCodes(Node* n,
                     std::array<Code,256>& table,
                     uint64_t path,
                     uint8_t depth)
{
    if (!n) return;
    if (n->leaf) {
        table[n->byte].bits = path;
        table[n->byte].len  = depth;
        return;
    }
    genCodes(n->left,  table, (path << 1) | 0u, static_cast<uint8_t>(depth + 1));
    genCodes(n->right, table, (path << 1) | 1u, static_cast<uint8_t>(depth + 1));
}

// Construye el buffer HF2 (header + bitstream) en memoria.
static std::vector<uint8_t> buildHF2(const std::vector<uint8_t>& data) {
    const uint64_t n = static_cast<uint64_t>(data.size());

    std::array<uint64_t,256> freq{};
    for (uint8_t b : data) {
        freq[b]++;
    }

    std::vector<uint8_t> out;
    out.reserve(3 + 1 + 8 + 256*4 + (n ? n/2 : 0));

    // Magic + metodo
    out.push_back('H');
    out.push_back('F');
    out.push_back('2');
    out.push_back(1); // method = 1 (Huffman)

    // Tamaño original
    futil::write_u64_le(out, n);

    // Tabla de frecuencias (uint32 LE saturada)
    for (int i = 0; i < 256; ++i) {
        uint64_t f = freq[i];
        uint32_t f32 = (f > 0xFFFFFFFFull) ? 0xFFFFFFFFu
                                           : static_cast<uint32_t>(f);
        futil::write_u32_le(out, f32);
    }

    if (n == 0) {
        // Archivo vacío: solo header, sin bitstream.
        return out;
    }

    Node* root = buildTree(freq);
    if (!root) {
        throw std::runtime_error("Arbol Huffman nulo con datos no vacios");
    }

    std::array<Code,256> table{};
    genCodes(root, table, 0, 0);

    // Codificar a bitstream
    std::vector<uint8_t> bitbuf;
    bitbuf.reserve(n / 2 + 16);

    uint8_t  buf  = 0;
    int      used = 0;

    auto pushByte = [&](uint8_t x) {
        bitbuf.push_back(x);
    };

    for (uint8_t b : data) {
        const Code& c = table[b];
        for (int i = c.len - 1; i >= 0; --i) {
            uint8_t bit = static_cast<uint8_t>((c.bits >> i) & 1u);
            buf = static_cast<uint8_t>((buf << 1) | bit);
            used++;
            if (used == 8) {
                pushByte(buf);
                buf  = 0;
                used = 0;
            }
        }
    }

    if (used > 0) {
        buf <<= (8 - used);
        pushByte(buf);
    }

    freeTree(root);

    // Adjuntar bitstream al header
    out.insert(out.end(), bitbuf.begin(), bitbuf.end());

    // Aquí ya no hacemos verificación de "autosafe".
    // La tasa de compresión se reporta en el CLI.
    return out;
}

void compress_autosafe_hf2(const std::string& inPath, const std::string& outPath) {
    // Restriccion original: solo .txt
    if (!futil::is_txt(inPath)) {
        throw std::runtime_error("Solo se admiten archivos .txt como entrada (" + inPath + ")");
    }

    std::vector<uint8_t> data = futil::read_all(inPath);
    std::vector<uint8_t> hf2  = buildHF2(data);

    futil::write_all(outPath, hf2);
    futil::copy_mode(inPath, outPath);
}

void decompress_hf2(const std::string& inPath, const std::string& outPath) {
    std::vector<uint8_t> in = futil::read_all(inPath);
    const std::size_t minHeader = 3 + 1 + 8 + 256*4;

    if (in.size() < minHeader) {
        throw std::runtime_error("Archivo demasiado pequeño para HF2");
    }

    if (!(in[0] == 'H' && in[1] == 'F' && in[2] == '2')) {
        throw std::runtime_error("Magic invalido (no HF2)");
    }
    if (in[3] != 1) {
        throw std::runtime_error("Metodo HF2 no soportado");
    }

    const uint8_t* p = in.data() + 4;

    uint64_t original = futil::read_u64_le(p);
    p += 8;

    std::array<uint64_t,256> freq{};
    for (int i = 0; i < 256; ++i) {
        uint32_t f32 = futil::read_u32_le(p);
        p += 4;
        freq[i] = f32;
    }

    const uint8_t* bitstart = p;
    const uint8_t* bitend   = in.data() + in.size();

    if (original == 0) {
        std::vector<uint8_t> empty;
        futil::write_all(outPath, empty);
        futil::copy_mode(inPath, outPath);
        return;
    }

    Node* root = buildTree(freq);
    if (!root) {
        throw std::runtime_error("Arbol Huffman nulo en decompress");
    }

    std::vector<uint8_t> out;
    out.reserve(original);

    uint8_t curByte = 0;
    int     left    = 0;

    auto nextBit = [&](int& outBit) -> bool {
        if (left == 0) {
            if (bitstart >= bitend) {
                return false;
            }
            curByte = *bitstart++;
            left = 8;
        }
        outBit = (curByte >> 7) & 1u;
        curByte <<= 1;
        --left;
        return true;
    };

    Node*    cur      = root;
    uint64_t written  = 0;

    while (written < original) {
        int b;
        if (!nextBit(b)) {
            freeTree(root);
            throw std::runtime_error("Bitstream demasiado corto");
        }

        cur = (b == 0) ? cur->left : cur->right;
        if (!cur) {
            freeTree(root);
            throw std::runtime_error("Bitstream invalido (ruta nula)");
        }

        if (cur->leaf) {
            out.push_back(cur->byte);
            ++written;
            cur = root;
        }
    }

    freeTree(root);

    futil::write_all(outPath, out);
    futil::copy_mode(inPath, outPath);
}

} // namespace hf2

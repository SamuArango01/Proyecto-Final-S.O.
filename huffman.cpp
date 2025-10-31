#include "huffman.hpp"
#include "bitio.hpp"
#include "file_util.hpp"
#include <array>
#include <queue>
#include <vector>
#include <stdexcept>
#include <cstdint>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

namespace hf2 {

// ======= Encabezado HF2 =======
// [3]  'H' 'F' '2'
// [1]  method = 1 (Huffman)
// [8]  original_size (u64 LE)
// [256*4] frecuencias (u32 LE)
// [..] bitstream (MSB-first) de Huffman

struct Node {
    uint8_t b; uint64_t f; Node* l; Node* r; bool leaf;
    Node(uint8_t B, uint64_t F, bool L): b(B), f(F), l(nullptr), r(nullptr), leaf(L) {}
};
struct Cmp {
    bool operator()(const Node* a, const Node* b) const {
        if (a->f != b->f) return a->f > b->f;
        return a > b;
    }
};

static void freeTree(Node* n){ if(!n) return; freeTree(n->l); freeTree(n->r); delete n; }

struct Code { uint64_t bits=0; uint8_t len=0; };

static Node* buildTree(const std::array<uint64_t,256>& fr) {
    std::priority_queue<Node*, std::vector<Node*>, Cmp> pq;
    size_t nz=0;
    for (int i=0;i<256;++i) if (fr[i]) { pq.push(new Node((uint8_t)i, fr[i], true)); ++nz; }
    if (nz==0) return nullptr;
    if (pq.size()==1) { // caso borde: solo un símbolo
        Node* only=pq.top(); pq.pop();
        Node* root = new Node(0, only->f, false);
        root->l = only;
        root->r = new Node(only->b, 0, true); // rama dummy
        return root;
    }
    while (pq.size()>1) {
        Node* a=pq.top(); pq.pop();
        Node* b=pq.top(); pq.pop();
        Node* p=new Node(0, a->f+b->f, false);
        p->l=a; p->r=b;
        pq.push(p);
    }
    return pq.top();
}

static void genCodes(Node* n, std::array<Code,256>& T, uint64_t path, uint8_t depth) {
    if (!n) return;
    if (n->leaf) { T[n->b].bits=path; T[n->b].len=depth; return; }
    genCodes(n->l, T, (path<<1)|0ULL, (uint8_t)(depth+1));
    genCodes(n->r, T, (path<<1)|1ULL, (uint8_t)(depth+1));
}

// Construye el archivo *completo* HF2 en memoria (vector<uint8_t>).
// Lanza si input no reduce tamaño.
static std::vector<uint8_t> buildHF2orThrow(const std::vector<uint8_t>& data) {
    const uint64_t n = (uint64_t)data.size();
    std::array<uint64_t,256> fr{}; for (uint8_t b : data) fr[b]++;

    // Header provisional en 'out'
    std::vector<uint8_t> out;
    out.reserve(3 + 1 + 8 + 256*4 + (n? n/2 : 0));
    out.push_back('H'); out.push_back('F'); out.push_back('2');
    out.push_back((uint8_t)1); // method = 1 (Huffman)
    futil::write_u64_le(out, n);
    for (int i=0;i<256;++i) futil::write_u32_le(out, (uint32_t)(fr[i] > 0xFFFFFFFFULL ? 0xFFFFFFFFULL : fr[i]));

    if (n==0) {
        // archivo vacío → HF2 cabe (solo header) y es más chico que 0? No; pero está ok generar 12+ bytes.
        // Para este proyecto, tratamos archivo vacío como "comprimido" (tiene sentido).
        return out;
    }

    Node* root = buildTree(fr);
    if (!root) throw std::runtime_error("Árbol nulo inesperado");

    // Bitstream en un fd temporal (memoria → pipe/archivo?), aquí usamos un vector y luego lo escribimos con syscalls
    // Para cumplir "solo syscalls", convertimos el bitstream directamente a bytes en un vector extra.
    std::vector<uint8_t> bitbuf;
    {
        // Simula un BitWriter a un vector usando un byte acumulador.
        uint8_t buf=0; int used=0;
        std::array<Code,256> T{}; genCodes(root, T, 0ULL, 0);
        auto pushByte = [&](uint8_t x){ bitbuf.push_back(x); };

        for (uint8_t b : data) {
            Code c = T[b];
            // MSB-first
            for (int i = c.len - 1; i >= 0; --i) {
                uint8_t bit = (c.bits >> i) & 1U;
                buf = (uint8_t)((buf << 1) | bit);
                used++;
                if (used == 8) { pushByte(buf); buf=0; used=0; }
            }
        }
        if (used>0) { buf <<= (8-used); pushByte(buf); }
    }

    freeTree(root);

    // Adjuntar bitstream
    out.insert(out.end(), bitbuf.begin(), bitbuf.end());

    // Verificación AUTOSAFE (estricta): tamaño HF2 debe ser < tamaño original
    if (out.size() >= data.size()) {
        throw std::runtime_error("Compresion no reduce tamaño (AutoSafe estricto).");
    }
    return out;
}

void compress_autosafe_hf2(const std::string& inPath, const std::string& outPath) {
    // Solo .txt por ahora
    if (!futil::is_txt(inPath)) {
        throw std::runtime_error("Solo se admiten .txt por ahora (entrada: " + inPath + ")");
    }

    // Lee archivo completo con syscalls
    std::vector<uint8_t> data = futil::read_all(inPath);

    // Construye HF2 o lanza si no reduce
    std::vector<uint8_t> hf2 = buildHF2orThrow(data);

    // Escribe HF2
    futil::write_all(outPath, hf2);

    // Copiar permisos del original
    futil::copy_mode(inPath, outPath);
}

void decompress_hf2(const std::string& inPath, const std::string& outPath) {
    // Abrimos con syscalls y leemos a memoria para simplicidad + validación
    std::vector<uint8_t> in = futil::read_all(inPath);
    if (in.size() < 3+1+8+256*4) throw std::runtime_error("Archivo demasiado pequeño para HF2");

    if (!(in[0]=='H' && in[1]=='F' && in[2]=='2')) throw std::runtime_error("Magic invalido (no HF2)");
    if (in[3] != 1) throw std::runtime_error("Metodo HF2 no soportado");
    const uint8_t* p = in.data() + 4;
    uint64_t original = futil::read_u64_le(p); p += 8;

    std::array<uint64_t,256> fr{};
    for (int i=0;i<256;++i) { fr[i] = futil::read_u32_le(p); p += 4; }

    const uint8_t* bitstart = p;
    const uint8_t* bitend   = in.data() + in.size();

    // Caso borde
    if (original == 0) {
        std::vector<uint8_t> empty;
        futil::write_all(outPath, empty);
        futil::copy_mode(inPath, outPath);
        return;
    }

    Node* root = buildTree(fr);
    if (!root) throw std::runtime_error("Árbol nulo con original>0");

    // Decodificar bits
    std::vector<uint8_t> out;
    out.reserve(original);

    // Usamos BitReaderFD sobre un fd temporal para ceñirnos a la interfaz, pero
    // aquí ya tenemos todo en memoria; decodificamos manualmente desde el buffer.
    uint8_t curByte = 0; int left = 0;
    auto nextBit = [&](int& outBit)->bool {
        if (left == 0) {
            if (bitstart >= bitend) return false;
            curByte = *bitstart++;
            left = 8;
        }
        outBit = (curByte >> 7) & 1U;
        curByte <<= 1;
        --left;
        return true;
    };

    Node* cur = root;
    uint64_t written = 0;
    while (written < original) {
        int bit;
        if (!nextBit(bit)) { freeTree(root); throw std::runtime_error("EOF prematuro en bitstream"); }
        cur = (bit==0) ? cur->l : cur->r;
        if (!cur) { freeTree(root); throw std::runtime_error("Bitstream invalido"); }
        if (cur->leaf) {
            out.push_back(cur->b);
            ++written;
            cur = root;
        }
    }
    freeTree(root);

    futil::write_all(outPath, out);
    futil::copy_mode(inPath, outPath);
}

} // namespace hf2

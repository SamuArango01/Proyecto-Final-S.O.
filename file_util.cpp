#include "file_util.hpp"
#include <stdexcept>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>

namespace futil {

std::vector<uint8_t> read_all(const std::string& path) {
    int fd = ::open(path.c_str(), O_RDONLY);
    if (fd < 0) throw std::runtime_error("open() fallo al leer: " + path);

    std::vector<uint8_t> buf;
    uint8_t tmp[1<<15]; // 32KB
    for (;;) {
        ssize_t r = ::read(fd, tmp, sizeof(tmp));
        if (r == 0) break;              // EOF
        if (r < 0) { ::close(fd); throw std::runtime_error("read() fallo: " + path); }
        buf.insert(buf.end(), tmp, tmp + r);
    }
    ::close(fd);
    return buf;
}

void write_all(const std::string& path, const std::vector<uint8_t>& data) {
    int fd = ::open(path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) throw std::runtime_error("open() fallo al escribir: " + path);
    size_t off = 0;
    while (off < data.size()) {
        ssize_t w = ::write(fd, data.data() + off, data.size() - off);
        if (w < 0) { ::close(fd); throw std::runtime_error("write() fallo: " + path); }
        off += (size_t)w;
    }
    ::close(fd);
}

std::uint64_t file_size(const std::string& path) {
    struct stat st{};
    if (::stat(path.c_str(), &st) != 0) throw std::runtime_error("stat() fallo: " + path);
    return static_cast<std::uint64_t>(st.st_size);
}

void copy_mode(const std::string& src, const std::string& dst) {
    struct stat st{};
    if (::stat(src.c_str(), &st) != 0) return;
    ::chmod(dst.c_str(), st.st_mode & 0777);
}

bool is_txt(const std::string& path) {
    const std::string suf = ".txt";
    if (path.size() < suf.size()) return false;
    return path.compare(path.size() - suf.size(), suf.size(), suf) == 0;
}

void write_u32_le(std::vector<uint8_t>& out, std::uint32_t v) {
    for (int i=0;i<4;++i) out.push_back((uint8_t)((v>>(8*i)) & 0xFF));
}
void write_u64_le(std::vector<uint8_t>& out, std::uint64_t v) {
    for (int i=0;i<8;++i) out.push_back((uint8_t)((v>>(8*i)) & 0xFF));
}
std::uint32_t read_u32_le(const uint8_t* p) {
    return (uint32_t)p[0] | ((uint32_t)p[1]<<8) | ((uint32_t)p[2]<<16) | ((uint32_t)p[3]<<24);
}
std::uint64_t read_u64_le(const uint8_t* p) {
    std::uint64_t v=0;
    for(int i=0;i<8;++i) v |= ((uint64_t)p[i]) << (8*i);
    return v;
}

} // namespace futil

#pragma once
#include <cstdint>
#include <unistd.h>
#include <stdexcept>

// Escritor/lector de bits sobre un file descriptor (syscalls).
// No usa stdio ni iostream.

class BitWriterFD {
    int fd_;
    uint8_t buf_;
    int used_; // bits usados en buf_
public:
    explicit BitWriterFD(int fd) : fd_(fd), buf_(0), used_(0) {}

    // Escribe 'len' bits de 'bits' (más significativo primero).
    void writeBits(uint64_t bits, uint8_t len) {
        for (int i = len - 1; i >= 0; --i) {
            uint8_t bit = (bits >> i) & 1U;
            buf_ = (uint8_t)((buf_ << 1) | bit);
            used_++;
            if (used_ == 8) {
                if (::write(fd_, &buf_, 1) != 1) {
                    throw std::runtime_error("write() fallo en BitWriterFD");
                }
                buf_ = 0; used_ = 0;
            }
        }
    }

    void flush() {
        if (used_ > 0) {
            buf_ <<= (8 - used_);
            if (::write(fd_, &buf_, 1) != 1) {
                throw std::runtime_error("write() fallo en BitWriterFD::flush");
            }
            buf_ = 0; used_ = 0;
        }
    }
};

class BitReaderFD {
    int fd_;
    uint8_t buf_;
    int left_; // bits restantes en buf_
public:
    explicit BitReaderFD(int fd) : fd_(fd), buf_(0), left_(0) {}

    // Retorna 0/1 o -1 si EOF duro.
    int readBit() {
        if (left_ == 0) {
            uint8_t tmp;
            ssize_t r = ::read(fd_, &tmp, 1);
            if (r == 0) return -1;           // EOF
            if (r < 0) throw std::runtime_error("read() fallo en BitReaderFD");
            buf_ = tmp;
            left_ = 8;
        }
        int bit = (buf_ >> 7) & 1U;
        buf_ <<= 1;
        --left_;
        return bit;
    }
};

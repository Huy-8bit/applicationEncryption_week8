#include <iostream>
#include <string>

using namespace std;

// AES round constant table
const unsigned char Rcon[11] = {
    0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1B, 0x36, 0x6C
};


static const uint8_t sbox[256] = {
    // 0     1     2     3     4     5     6     7
    // 8     9     A     B     C     D     E     F
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5,
    0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76, // 0
    0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0,
    0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0, // 1
    0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc,
    0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15, // 2
    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a,
    0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75, // 3
    0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0,
    0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84, // 4
    0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b,
    0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf, // 5
    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85,
    0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8, // 6
    0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5,
    0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2, // 7
    0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17,
    0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73, // 8
    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88,
    0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb, // 9
    0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c,
    0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79, // A
    0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9,
    0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08, // B
    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6,
    0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a, // C
    0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e,
    0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e, // D
    0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94,
    0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf, // E
    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68,
    0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16 }; // F

// SubBytes: substitution using the S-box
void subBytes(unsigned char* state) {
    for (int i = 0; i < 16; i++) {
        state[i] = sbox[state[i]];
    }
}

// ShiftRows: cyclically shift each row
void shiftRows(unsigned char* state) {
    unsigned char tmp[16];
    memcpy(tmp, state, 16);
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            tmp[4 * r + c] = state[4 * r + (c + r) % 4];
        }
    }
    memcpy(state, tmp, 16);
}


// MixColumns: matrix multiplication over GF(2^8)
uint8_t gf_mul(uint8_t value) {
    if (value < 0x80) {
        return static_cast<uint8_t>((value << 1) & 0xFF);
    }
    else {
        return static_cast<uint8_t>((value << 1) ^ 0x1B);
    }
}

void mixColumns(unsigned char* state) {
    unsigned char tmp[16];
    for (int c = 0; c < 4; c++) {
        tmp[4 * c + 0] = gf_mul(state[4 * c + 0]) ^ gf_mul(state[4 * c + 1]) ^ state[4 * c + 2] ^ state[4 * c + 3];
        tmp[4 * c + 1] = state[4 * c + 0] ^ gf_mul(state[4 * c + 1]) ^ gf_mul(state[4 * c + 2]) ^ state[4 * c + 3];
        tmp[4 * c + 2] = state[4 * c + 0] ^ state[4 * c + 1] ^ gf_mul(state[4 * c + 2]) ^ gf_mul(state[4 * c + 3]);
        tmp[4 * c + 3] = gf_mul(state[4 * c + 0]) ^ state[4 * c + 1] ^ state[4 * c + 2] ^ gf_mul(state[4 * c + 3]);
    }
    memcpy(state, tmp, 16);
}

// AddRoundKey: XOR with the round key
void addRoundKey(unsigned char* state, unsigned char* key) {
    for (int i = 0; i < 16; i++) {
        state[i] ^= key[i];
    }
}

// KeyExpansion: generate the round keys
void keyExpansion(unsigned char* key, unsigned char* roundKey) {
    unsigned char tmp[4], t;
    const int Nk = 4, Nr = 10;
    const int Nb = 4;
    unsigned char Rcon[11] = { 0x00, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36 };
    for (int i = 0; i < Nk; i++) {
        roundKey[4 * i + 0] = key[4 * i + 0];
        roundKey[4 * i + 1] = key[4 * i + 1];
        roundKey[4 * i + 2] = key[4 * i + 2];
        roundKey[4 * i + 3] = key[4 * i + 3];
    }

    for (int i = Nk; i < Nb * (Nr + 1); i++) {
        tmp[0] = roundKey[4 * (i - 1) + 0];
        tmp[1] = roundKey[4 * (i - 1) + 1];
        tmp[2] = roundKey[4 * (i - 1) + 2];
        tmp[3] = roundKey[4 * (i - 1) + 3];

        if (i % Nk == 0)
        {
            t = tmp[0];
            tmp[0] = sbox[tmp[1]] ^ Rcon[i / Nk];
            tmp[1] = sbox[tmp[2]];
            tmp[2] = sbox[tmp[3]];
            tmp[3] = sbox[t];
        }
        else if (Nk > 6 && i % Nk == 4) {
            tmp[0] = sbox[tmp[0]];
            tmp[1] = sbox[tmp[1]];
            tmp[2] = sbox[tmp[2]];
            tmp[3] = sbox[tmp[3]];
        }
        roundKey[4 * i + 0] = roundKey[4 * (i - Nk) + 0] ^ tmp[0];
        roundKey[4 * i + 1] = roundKey[4 * (i - Nk) + 1] ^ tmp[1];
        roundKey[4 * i + 2] = roundKey[4 * (i - Nk) + 2] ^ tmp[2];
        roundKey[4 * i + 3] = roundKey[4 * (i - Nk) + 3] ^ tmp[3];
    }
}

// AES encryption function
void aes_encrypt(unsigned char* message, unsigned char* key, unsigned char* ciphertext) {
    unsigned char state[16];
    unsigned char roundKey[176];
    memcpy(state, message, 16);
    keyExpansion(key, roundKey);
    addRoundKey(state, key);
    for (int i = 1; i <= 10; i++) {
        subBytes(state);
        shiftRows(state);
        if (i < 10) {
            mixColumns(state);
        }
        addRoundKey(state, roundKey + 16 * i);
    }
    memcpy(ciphertext, state, 16);
}

int main() {
    unsigned char message[] = "Two One Nine Two";
    unsigned char key[] = "Thats my Kung Fu";
    unsigned char ciphertext[16];
    unsigned char decrypted[16];
    aes_encrypt(message, key, ciphertext);
    // aes_decrypt(ciphertext, key, decrypted);

    printf("Original message: %s\n", message);
    printf("Encrypted message: ");
    for (int i = 0; i < 16; i++) {
        printf("%02x", ciphertext[i]);
        cout << " ";
    }
    printf("\n");

    // printf("Decrypted message: %s\n", decrypted);

    return 0;
}
#include<iostream>
#include<iomanip>
#include<string>
#include<cstring>
using namespace std;

const unsigned char rcon[11] = {
        0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1B, 0x36, 0x6C
};

const int BLOCK_SIZE = 16;  // AES block size is always 16 bytes
const int NUM_ROUNDS = 10;  // AES-128 has 10 rounds

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

// tinh nghich dao
uint8_t gf_mul(uint8_t value) {
    if (value < 0x80) {
        return static_cast<uint8_t>((value << 1) & 0xFF);
    }
    else {
        return static_cast<uint8_t>((value << 1) ^ 0x1B);
    }
}
//sub bytes
void sub_bytes(unsigned char* block) {
    for (int i = 0; i < BLOCK_SIZE; i++) {
        block[i] = sbox[block[i]];
    }
}

void inv_sub_bytes(unsigned char* block) {
    for (int i = 0; i < BLOCK_SIZE; i++) {
        gf_mul(block[i] = sbox[block[i]]);
    }
}
// xor
void xor_blocks(unsigned char* dst, const unsigned char* a, const unsigned char* b, int len) {
    for (int i = 0; i < len; i++) {
        dst[i] = a[i] ^ b[i];
    }
}

// shift row
void shift_rows(unsigned char* block) {
    // shift row 1 by 1 byte
    unsigned char tmp = block[1];
    block[1] = block[5];
    block[5] = block[9];
    block[9] = block[13];
    block[13] = tmp;

    // shift row 2 by 2 bytes
    tmp = block[2];
    block[2] = block[10];
    block[10] = tmp;
    tmp = block[6];
    block[6] = block[14];
    block[14] = tmp;

    // shift row 3 by 3 bytes
    tmp = block[15];
    block[15] = block[11];
    block[11] = block[7];
    block[7] = block[3];
    block[3] = tmp;
}

void inv_shift_rows(unsigned char* block) {
    // shift row 1 by 1 byte
    unsigned char tmp = block[1];
    block[1] = block[5];
    block[5] = block[9];
    block[9] = block[13];
    block[13] = tmp;

    // shift row 2 by 2 bytes
    tmp = block[2];
    block[2] = block[10];
    block[10] = tmp;
    tmp = block[6];
    block[6] = block[14];
    block[14] = tmp;

    // shift row 3 by 3 bytes
    tmp = block[15];
    block[15] = block[11];
    block[11] = block[7];
    block[7] = block[3];
    block[3] = tmp;
}

// mixed columns
void mix_columns(unsigned char* block) {
    for (int i = 0; i < BLOCK_SIZE; i += 4) {
        unsigned char a = block[i], b = block[i + 1], c = block[i + 2], d = block[i + 3];
        (block[i] = (unsigned char)(0x02 * a ^ 0x03 * b ^ c ^ d));
        block[i + 1] = (unsigned char)(a ^ 0x02 * b ^ 0x03 * c ^ d);
        block[i + 2] = (unsigned char)(a ^ b ^ 0x02 * c ^ 0x03 * d);
        block[i + 3] = (unsigned char)(0x03 * a ^ b ^ c ^ 0x02 * d);
    }
}

void inv_mix_columns(unsigned char* block) {
    // nghich dao
    for (int i = 0; i < BLOCK_SIZE; i += 4) {
        unsigned char a = block[i], b = block[i + 1], c = block[i + 2], d = block[i + 3];
        gf_mul(block[i] = (unsigned char)(0x02 * a ^ 0x03 * b ^ c ^ d));
        gf_mul(block[i + 1] = (unsigned char)(a ^ 0x02 * b ^ 0x03 * c ^ d));
        gf_mul(block[i + 2] = (unsigned char)(a ^ b ^ 0x02 * c ^ 0x03 * d));
        gf_mul(block[i + 3] = (unsigned char)(0x03 * a ^ b ^ c ^ 0x02 * d));
    }
}

// Expand key
void keyExpansion(unsigned char* key, unsigned char* expanded_key) {
    memcpy(expanded_key, key, BLOCK_SIZE);
    unsigned char temp[4];
    int rcon_idx = 1;
    for (int i = BLOCK_SIZE; i < BLOCK_SIZE * (NUM_ROUNDS + 1); i = i + 4) {
        for (int j = 0; j < 4; j++) {
            temp[j] = expanded_key[i - 4 + j];
        }
        if (i % BLOCK_SIZE == 0) {
            // Rotate word
            unsigned char tmp = temp[0];
            temp[0] = temp[1];
            temp[1] = temp[2];
            temp[2] = temp[3];
            temp[3] = tmp;

            // substitute word using Sbox
            for (int j = 0; j < 4; j++) {
                temp[j] = sbox[temp[j]];
            }

            // apply Rcon
            temp[0] ^= rcon[rcon_idx++];
        }

        // expand key by XORing with 4-byte block n bytes back
        for (int j = 0; j < 4; j++) {
            expanded_key[i + j] = expanded_key[i - BLOCK_SIZE + j] ^ temp[j];
        }
    }
}

void aes_encrypt(unsigned char* plaintext, int plaintext_len, unsigned char* key, unsigned char* iv, unsigned char* ciphertext) {
    unsigned char expanded_key[BLOCK_SIZE * (NUM_ROUNDS + 1)];
    keyExpansion(key, expanded_key);
    unsigned char prev_block[BLOCK_SIZE];
    // prev_block = copy of iv
    memcpy(prev_block, iv, BLOCK_SIZE);
    for (int i = 0; i < plaintext_len; i += BLOCK_SIZE) {
        // XOR plaintext block with previous ciphertext block or IV
        unsigned char block[BLOCK_SIZE];
        int block_len = min(BLOCK_SIZE, plaintext_len - i);
        memcpy(block, plaintext + i, block_len);
        if (block_len < BLOCK_SIZE) {
            // pad last block with zeros
            memset(block + block_len, 0, BLOCK_SIZE - block_len);
        }
        xor_blocks(block, block, prev_block, BLOCK_SIZE);

        // perform AES encryption
        for (int round = 0; round < NUM_ROUNDS; round++) {
            sub_bytes(block);
            shift_rows(block);
            if (round < NUM_ROUNDS - 1) {
                mix_columns(block);
            }
            xor_blocks(block, block, expanded_key + round * BLOCK_SIZE, BLOCK_SIZE);
        }

        // copy ciphertext block to output buffer and update previous block
        memcpy(ciphertext + i, block, BLOCK_SIZE);
        memcpy(prev_block, block, BLOCK_SIZE);
    }

}

void aes_decrypt(unsigned char* ciphertext, int ciphertext_len, unsigned char* key, unsigned char* iv, unsigned char* plaintext) {
    unsigned char expanded_key[BLOCK_SIZE * (NUM_ROUNDS + 1)];
    keyExpansion(key, expanded_key);
    unsigned char prev_block[BLOCK_SIZE];
    // prev_block = copy of iv
    memcpy(prev_block, iv, BLOCK_SIZE);
    for (int i = 0; i < ciphertext_len; i += BLOCK_SIZE) {
        // perform AES decryption
        unsigned char block[BLOCK_SIZE];
        memcpy(block, ciphertext + i, BLOCK_SIZE);
        unsigned char temp_block[BLOCK_SIZE];
        memcpy(temp_block, block, BLOCK_SIZE);
        for (int round = NUM_ROUNDS - 1; round >= 0; round--) {
            xor_blocks(block, block, expanded_key + round * BLOCK_SIZE, BLOCK_SIZE);
            if (round < NUM_ROUNDS - 1) {
                inv_mix_columns(block);
            }
            //inv_shift_rows(block);
            //inv_sub_bytes(block);
        }

        // XOR decrypted block with previous ciphertext block or IV
        xor_blocks(block, block, prev_block, BLOCK_SIZE);

        // copy decrypted block to output buffer and update previous block
        int block_len = min(BLOCK_SIZE, ciphertext_len - i);
        memcpy(plaintext + i, block, block_len);
        memcpy(prev_block, temp_block, BLOCK_SIZE);
    }
}



int main() {
    unsigned char message[] = "Two One Nine Two";
    unsigned char key[] = "Thats my Kung Fu";
    // iv
    unsigned char iv[] = "00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00";
    int message_len = strlen((const char*)message);

    // allocate buffer for ciphertext (including padding)
    int padded_len = (message_len / BLOCK_SIZE + 1) * BLOCK_SIZE;
    unsigned char ciphertext[padded_len];

    // encrypt message with CBC mode
    aes_encrypt(message, message_len, key, iv, ciphertext);

    // output the ciphertext
    printf("Ciphertext: ");
    for (int i = 0; i < padded_len; i++) {
        printf("%02x", ciphertext[i]);
        cout << " ";
    }
    printf("\n");

    return 0;
}
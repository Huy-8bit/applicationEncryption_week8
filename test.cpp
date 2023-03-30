#include <iostream>
#include <cstring>
#include <cstdint>
#include <string>
#include <vector>
#include <algorithm>
#include <iomanip>


using namespace std;


const uint8_t rcon[] = {
0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36
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

// Define the AES block size
const int AES_BLOCK_SIZE = 16;

// Define the AES key size
const int AES_KEY_SIZE = 16;

// Define the number of rounds in AES-128
const int AES_ROUNDS = 10;

const uint8_t inv_sbox[] = {
};

// Define the key schedule function to generate round keys
void key_schedule(const uint8_t* key, uint8_t* round_keys)
{
    uint8_t temp[4];
    uint8_t i, j, k;
    // Copy the original key to the first 16 bytes of round_keys
    for (i = 0; i < 16; i++) {
        round_keys[i] = key[i];
    }

    // Generate the remaining round keys
    for (i = 1, j = 0; i < 11; i++) {
        // Copy the previous 4 bytes to temp
        for (k = 0; k < 4; k++) {
            temp[k] = round_keys[(i - 1) * 16 + 12 + k];
        }

        // Apply the key schedule core
        if (i % 4 == 0) {
            // Rotate the temp word by 1 byte
            uint8_t t = temp[0];
            for (k = 0; k < 3; k++) {
                temp[k] = temp[k + 1];
            }
            temp[3] = t;

            // Apply the S-box to each byte
            for (k = 0; k < 4; k++) {
                temp[k] = sbox[temp[k]];
            }

            // XOR the first byte with the round constant
            temp[0] ^= rcon[(i / 4) - 1];
        }

        // XOR temp with the 4-byte block at the end of the previous round key
        for (k = 0; k < 4; k++) {
            round_keys[i * 16 + k] = round_keys[(i - 1) * 16 + k] ^ temp[k];
        }

        // XOR the remaining 12 bytes with the corresponding bytes in the previous round key
        for (k = 4; k < 16; k++) {
            round_keys[i * 16 + k] = round_keys[(i - 1) * 16 + k] ^ round_keys[i * 16 + k - 4];
        }
    }
}
uint8_t gf_mul(uint8_t a, uint8_t b) {
    uint8_t p = 0;
    for (int i = 0; i < 8; i++) {
        if (b & 1) {
            p ^= a;
        }
        uint8_t high_bit = a & 0x80;
        a <<= 1;
        if (high_bit) {
            a ^= 0x1b;
        }
        b >>= 1;
    }
    return p;
}

// Define the encryption function
void aes_encrypt(const uint8_t* plaintext, const uint8_t* key, uint8_t* ciphertext)
{
    uint8_t state[4][4];
    uint8_t round_keys[176];
    uint8_t i, j, k, round;
    // Generate the round keys from the key
    key_schedule(key, round_keys);

    // Copy the plaintext into the state array
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            state[j][i] = plaintext[i * 4 + j];
        }
    }

    // Add the first round key to the state
    for (i = 0; i < 16; i++) {
        state[i % 4][i / 4] ^= round_keys[i];
    }

    // Perform the 9 main rounds
    for (round = 1; round <= 9; round++) {
        // Substitute each byte in the state with the corresponding byte in the S-box
        for (i = 0; i < 4; i++) {
            for (j = 0; j < 4; j++) {
                state[i][j] = sbox[state[i][j]];
            }
        }
        // Shift the rows of the state
        for (i = 1; i < 4; i++) {
            for (j = 0; j < i; j++) {
                uint8_t t = state[i][0];
                for (k = 0; k < 3; k++) {
                    state[i][k] = state[i][k + 1];
                }
                state[i][3] = t;
            }
        }

        // Mix the columns of the state using the Galois field multiplication
        for (i = 0; i < 4; i++) {
            uint8_t s0 = state[0][i];
            uint8_t s1 = state[1][i];
            uint8_t s2 = state[2][i];
            uint8_t s3 = state[3][i];

            state[0][i] = gf_mul(s0, 0x02) ^ gf_mul(s1, 0x03) ^ s2 ^ s3;
            state[1][i] = s0 ^ gf_mul(s1, 0x02) ^ gf_mul(s2, 0x03) ^ s3;
            state[2][i] = s0 ^ s1 ^ gf_mul(s2, 0x02) ^ gf_mul(s3, 0x03);
            state[3][i] = gf_mul(s0, 0x03) ^ s1 ^ s2 ^ gf_mul(s3, 0x02);
        }

        // Add the round key to the state
        for (i = 0; i < 16; i++) {
            state[i % 4][i / 4] ^= round_keys[round * 16 + i];
        }
    }

    // Perform the final round
    // Substitute each byte in the state with the corresponding byte in the S-box
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            state[i][j] = sbox[state[i][j]];
        }
    }

    // Shift the rows of the state
    for (i = 1; i < 4; i++) {
        for (j = 0; j < i; j++) {
            uint8_t t = state[i][0];
            for (k = 0; k < 3; k++) {
                state[i][k] = state[i][k + 1];
            }
            state[i][3] = t;
        }
    }

    // Add the final round key to the state
    for (i = 0; i < 16; i++) {
        state[i % 4][i / 4] ^= round_keys[160 + i];
    }

    // Copy the state into the ciphertext array
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            ciphertext[i * 4 + j] = state[j][i];
        }
    }
}


// int main()
// {
//     // Define the plaintext and key
//     uint8_t plaintext[16] = {
//         0x32, 0x88, 0x31, 0xe0,
//         0x43, 0x5a, 0x31, 0x37,
//         0xf6, 0x30, 0x98, 0x07,
//         0xa8, 0x8d, 0xa2, 0x34
//     };
//     uint8_t key[16] = {
//         0x2b, 0x7e, 0x15, 0x16,
//         0x28, 0xae, 0xd2, 0xa6,
//         0xab, 0xf7, 0x15, 0x88,
//         0x09, 0xcf, 0x4f, 0x3c
//     };

//     // Encrypt the plaintext using AES-128
//     uint8_t ciphertext[16];
//     aes_encrypt(plaintext, key, ciphertext);

//     // Print the ciphertext
//     cout << "Ciphertext: ";
//     for (int i = 0; i < 16; i++) {
//         cout << hex << setw(2) << setfill('0') << (int)ciphertext[i] << " ";
//     }
//     cout << endl;

//     return 0;
// }


int main()
{
    // Define the plaintext and key as strings
    string plaintext_str = "This is a plaintext message.";
    string key_str = "This is a secret key.";
    
    // Convert the plaintext and key to uint8_t arrays
    const uint8_t* plaintext = reinterpret_cast<const uint8_t*>(plaintext_str.data());
    const uint8_t* key = reinterpret_cast<const uint8_t*>(key_str.data());
    
    // Print the plaintext and key as uint8_t arrays
    cout << "Plaintext: ";
    for (int i = 0; i < plaintext_str.size(); i++) {
        cout << hex << (int)plaintext[i] << " ";
    }
    cout << endl;
    
    cout << "Key: ";
    for (int i = 0; i < key_str.size(); i++) {
        cout << hex << (int)key[i] << " ";
    }
    cout << endl;
    
    // Encrypt the plaintext using AES-128
    uint8_t ciphertext[16];
    aes_encrypt(plaintext, key, ciphertext);
    
    // Print the ciphertext
    cout << "Ciphertext: ";
    for (int i = 0; i < 16; i++) {
        cout << hex << setw(2) << setfill('0') << (int)ciphertext[i];
    }
    cout << endl;
    
    return 0;
}
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <cstdint>
#include <cstring>

using namespace std;

// S-box
const unsigned char sbox[256] = {
    // 0     1     2     3     4     5     6     7
    // 8     9     A     B     C     D     E     F
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5,
    0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76, //0
    0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0,
    0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0, //1
    0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc,
    0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15, //2
    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a,
    0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75, //3
    0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0,
    0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84, //4
    0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b,
    0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf, //5
    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85,
    0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8, //6
    0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5,
    0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2, //7
    0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17,
    0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73, //8
    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88,
    0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb, //9
    0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c,
    0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79, //A
    0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9,
    0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08, //B
    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6,
    0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a, //C
    0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e,
    0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e, //D
    0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94,
    0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf, //E
    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68,
    0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16 //F

};

const unsigned char rcon[11] = {
  0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1B, 0x36, 0x00
};

//void shiftRow(unsigned char state[4][4]) {
//    for (int i = 0; i < 4; i++)
//    {
//        unsigned char temp = state[i][0];
//        state[i][0] = state[i][1];
//        state[i][1] = state[i][2];
//        state[i][2] = state[i][3];
//        state[i][3] = temp;
//    }
//   /* unsigned char temp = state[3][0];
//    state[3][0] = state[3][1];
//    state[3][1] = state[3][2];
//    state[3][2] = state[3][3];
//    state[3][3] = temp;*/
//}

void shiftRow(unsigned char state[][4])
{
    unsigned char temp = state[1][0];
    state[1][0] = state[1][1];
    state[1][1] = state[1][2];
    state[1][2] = state[1][3];
    state[1][3] = temp;

    temp = state[2][0];
    state[2][0] = state[2][2];
    state[2][2] = temp;
    temp = state[2][1];
    state[2][1] = state[2][3];
    state[2][3] = temp;

    temp = state[3][0];
    state[3][0] = state[3][3];
    state[3][3] = state[3][2];
    state[3][2] = state[3][1];
    state[3][1] = temp;
}


void s_box(unsigned char word[][4]) {
    for (int i = 0; i < 4; i++) {
        word[i][0] = sbox[word[i][0]];
        word[i][1] = sbox[word[i][1]];
        word[i][2] = sbox[word[i][2]];
        word[i][3] = sbox[word[i][3]];
    }
}


void rotword(unsigned char* word) {
    unsigned char temp = word[0];
    word[0] = word[1];
    word[1] = word[2];
    word[2] = word[3];
    word[3] = temp;
}

void subword(unsigned char* word) {
    for (int i = 0; i < 4; i++) {
        word[i] = sbox[word[i]];
    }
}

vector<unsigned char> keyexpansion(const string& key) {
    const int nk = 4;
    const int nb = 4;
    const int nr = 10;
    const int key_length = key.size();

    if (key_length != 16) {
        cerr << "error: key must be 16 bytes (128 bits) long." << endl;
        exit(1);
    }

    unsigned char key_bytes[nk * 4];
    memcpy(key_bytes, key.c_str(), key_length);

    vector<unsigned char> expanded_key(key_bytes, key_bytes + nk * 4);

    unsigned char temp[4];

    for (int i = nk; i < nb * (nr + 1); i++) {
        for (int j = 0; j < 4; j++) {
            temp[j] = expanded_key[(i - 1) * 4 + j];
        }

        if (i % nk == 0) {
            rotword(temp);
            subword(temp);
            temp[0] ^= rcon[i / nk - 1];
        }

        for (int j = 0; j < 4; j++) {
            expanded_key.push_back(expanded_key[(i - nk) * 4 + j] ^ temp[j]);
        }
    }
    return expanded_key;
}

void addroundkey(unsigned char state[][4], vector<unsigned char>& round_key, int round) {
    for (int c = 0; c < 4; c++) {
        unsigned char roundkey_column[4];
        for (int i = 0; i < 4; i++) {
            roundkey_column[i] = round_key[round * 16 + c * 4 + i];
        }
        for (int r = 0; r < 4; r++) {
            state[r][c] ^= roundkey_column[r];
        }
    }
}

unsigned char gmul(unsigned char a, unsigned char b) {
    unsigned char p = 0;
    unsigned char counter;
    unsigned char hi_bit_set;
    for (counter = 0; counter < 8; counter++) {
        if (b & 1) {
            p ^= a;
        }
        hi_bit_set = (a & 0x80);
        a <<= 1;
        if (hi_bit_set) {
            a ^= 0x1b; /* x^8 + x^4 + x^3 + x + 1 */
        }
        b >>= 1;
    }
    return p;
}


void mixcolumns(unsigned char state[][4]) {
    unsigned char temp[4];
    for (int i = 0; i < 4; i++) {
        temp[0] = gmul(state[0][i], 2) ^ gmul(state[1][i], 3) ^ state[2][i] ^ state[3][i];
        temp[1] = state[0][i] ^ gmul(state[1][i], 2) ^ gmul(state[2][i], 3) ^ state[3][i];
        temp[2] = state[0][i] ^ state[1][i] ^ gmul(state[2][i], 2) ^ gmul(state[3][i], 3);
        temp[3] = gmul(state[0][i], 3) ^ state[1][i] ^ state[2][i] ^ gmul(state[3][i], 2);
        state[0][i] = temp[0];
        state[1][i] = temp[1];
        state[2][i] = temp[2];
        state[3][i] = temp[3];
    }
}


vector<unsigned char> aes128(string m, string key)
{
    vector<unsigned char> ciphertext;
    vector<unsigned char> expanded_key = keyexpansion(key);
    int nk = key.size() / 4;
    int nr = nk + 6;
    unsigned char state[4][4];
    // copy the plaintext into the state variable
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            state[j][i] = m[i * 4 + j];
        }
    }
    addroundkey(state, expanded_key, 0);
    for (int i = 1; i < 10; i++)
    {
        s_box(state);
        shiftRow(state);
        mixcolumns(state);
        addroundkey(state, expanded_key, i);

    }
    // the last round is slightly different from the previous rounds
    s_box(state);
    shiftRow(state);
    addroundkey(state, expanded_key, nr);
    ciphertext.resize(16);
    // copy the ciphertext from the state variable
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            ciphertext[i * 4 + j] = state[j][i];
        }
    }
    return ciphertext;
}

int main() {

    string key = "0123456789abcdef";
    string plaintext = "0123456789abcdef";
    unsigned char encryptedData[16];

    vector<unsigned char> ciphertext = aes128(plaintext, key);

    for (int i = 0; i < 16; i++) {
        cout << hex << (int)encryptedData[i] << " ";
    }


    return 0;
}


#include<iostream>
#include<string>
#include<iomanip>
using namespace std;

void printHex(string str) {
    for (char c : str) {
        cout << hex << setw(2) << setfill('0') << static_cast<int>(c) << " ";
    }
    cout << endl;
}



int main() {
    system("cls");
    string m = "12345678", c = "", k = "1234567";
    // in ra chuoi m (so thap luc phan)
    printHex(m);
    // in ra chuoi c (so thap luc phan)

    c = CBCRandomEncrypt(m, k);
    printHex(c);

    // in ra chuoi m (so thap luc phan)
    // m = CBCRandomDecrypt(c, k);
    // printHex(m);


    return 0;
}

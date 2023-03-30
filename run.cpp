#include<iostream>
#include<string>
#include<cstring>
#include<iomanip>

using namespace std;


void printHex(string str) {
    for (char c : str) {
        cout << hex << setw(2) << setfill('0') << static_cast<int>(c) << " ";
    }
    cout << endl;
}






int main()
{


    return 0;
}
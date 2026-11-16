#include <iostream>
#include <cstdlib>
using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 2) { cerr << "Usage: ./path n\n"; return 1; }
    int n = atoi(argv[1]);              
    cout << n << " " << n - 1 << "\n";  
    for (int i = 1; i < n; i++)
        cout << i << " " << i + 1 << "\n";
    return 0;
}
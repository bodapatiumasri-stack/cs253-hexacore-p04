#include <iostream>
#include <cstdlib>
using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 2) { cerr << "Usage: ./empty n\n"; return 1; }
    int n = atoi(argv[1]);
    if (n < 1) { cerr << "n must be at least 1\n"; return 1; }

    cout << n << " " << 0 << "\n";
    return 0;
}
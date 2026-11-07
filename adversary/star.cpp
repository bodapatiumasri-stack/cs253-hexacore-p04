#include <iostream>
#include <cstdlib>
using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 2) { cerr << "Usage: ./star n\n"; return 1; }
    int n = atoi(argv[1]);
    if (n < 3) { cerr << "n must be at least 3\n"; return 1; }

    cout << n << " " << n - 1 << "\n";
    for (int i = 2; i <= n; i++)
        cout << 1 << " " << i << "\n";    
    return 0;
}
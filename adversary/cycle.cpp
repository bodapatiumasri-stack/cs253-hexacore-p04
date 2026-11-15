#include <iostream>
#include <cstdlib>
using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 2) { cerr << "Usage: ./cycle n\n"; return 1; }
    int n = atoi(argv[1]);
    if (n < 3) { cerr << "n must be at least 3\n"; return 1; }

    cout << n << " " << n << "\n";        
    for (int i = 1; i < n; i++)
        cout << i << " " << i + 1 << "\n";
    cout << n << " " << 1 << "\n";        
    return 0;
}
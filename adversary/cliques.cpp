#include <iostream>
#include <cstdlib>
#include <vector>
using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 3) { cerr << "Usage: ./cliques k s\n"; return 1; }
    int k = atoi(argv[1]);
    int s = atoi(argv[2]);
    if (k < 2 || s < 3) { cerr << "need k >= 2 and s >= 3\n"; return 1; }

    long long n = 1 + (long long)k * (s - 1);
    long long m = (long long)k * s * (s - 1) / 2;
    cout << n << " " << m << "\n";

    for (int j = 0; j < k; j++) {
        vector<int> vs;
        vs.push_back(1);                         
        for (int t = 0; t < s - 1; t++)
            vs.push_back(2 + j * (s - 1) + t);  
        for (int a = 0; a < s; a++)
            for (int b = a + 1; b < s; b++)
                cout << vs[a] << " " << vs[b] << "\n";
    }
    return 0;
}
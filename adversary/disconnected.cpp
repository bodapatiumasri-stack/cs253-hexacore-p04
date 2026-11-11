#include <iostream>
#include <cstdlib>
using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 2) { cerr << "Usage: ./disconnected n\n"; return 1; }
    int n = atoi(argv[1]);
    if (n < 10) { cerr << "n must be at least 10\n"; return 1; }

    int a = (n - 1) / 3;
    int b = (n - 1) / 3;
    int c = n - 1 - a - b;

    cout << n << " " << n - 3 << "\n";

    for (int i = 1; i < a; i++)                    
        cout << i << " " << i + 1 << "\n";

    for (int i = a + 1; i < a + b; i++)            
        cout << i << " " << i + 1 << "\n";
    cout << a + b << " " << a + 1 << "\n";         

    int centre = a + b + 1;                        
    for (int i = centre + 1; i <= a + b + c; i++)
        cout << centre << " " << i << "\n";

    return 0;                                      
}
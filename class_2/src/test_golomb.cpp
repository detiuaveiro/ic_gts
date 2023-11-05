#include <string>
#include <cmath>
#include "golomb.h"

using namespace std;

int main(){
    Golomb golomb(5); // m = 5
    
    // Exemplo 5.7 do livro
    for (int i = 0; i <= 15; i++){
        string s = golomb.encode(i);
        cout << i << " -> " << s << endl;
    }

    return 0;
}
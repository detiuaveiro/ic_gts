#include <string>
#include <cmath>
#include "golomb.h"

using namespace std;

int main(){
    // m é potência de 2
    cout << "m=4 (Potência de 2)" << endl;
    Golomb golomb1(4); // m = 4
    for (int i = 0; i <= 15; i++){
        string s = golomb1.encode(i);
        cout << i << " -> " << s << endl;
    }

    cout << endl;
    cout << "------------------------" << endl;
    cout << "m=5 (Não é potência de 2)" << endl;
    Golomb golomb2(5); // m = 5    
    // Exemplo 5.7 do livro (m não é potência de 2)
    for (int i = 0; i <= 15; i++){
        string s = golomb2.encode(i);
        cout << i << " -> " << s << endl;
    }



    return 0;
}
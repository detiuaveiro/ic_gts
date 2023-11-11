#include <string>
#include <cmath>
#include "golomb.h"

using namespace std;

int main(){

    BitStream writer('w', "test_golomb.bin");    
    Golomb golomb1(4, writer); 
    golomb1.encode(2);
    int bits_to_read = golomb1.getTotalBits();
    
    BitStream reader('r', "test_golomb.bin");
    Golomb golomb2(4, reader, 1, bits_to_read);
    int value = golomb2.decode();
    cout << "value: " << value << endl;

    BitStream writer0('w', "test_golomb0.bin");    
    Golomb golomb0(4, writer0); 
    golomb0.encode(-2);
    bits_to_read = golomb0.getTotalBits();
    
    BitStream reader0('r', "test_golomb0.bin");
    Golomb golomb0_2(4, reader0, 1, bits_to_read);
    value = golomb0_2.decode();
    cout << "value: " << value << endl;

//--------------------------------------------------------------------------------//
    // test value interleaving
    BitStream writer2('w', "test_golomb2.bin");
    Golomb golomb3(5, writer2, 2); // m = 5, interleaving
    golomb3.encode(11); // Como 11 é positivo, vai ser dado encode de 2*11
    bits_to_read = golomb3.getTotalBits();

    BitStream reader2('r', "test_golomb2.bin");   
    Golomb golomb4(5, reader2, 2, bits_to_read);
    value = golomb4.decode();
    cout << "value: " << value << endl;

    BitStream writer3('w', "test_golomb3.bin");
    Golomb golomb5(5, writer3, 2);
    golomb5.encode(-11); // vai ser dado encode de 2*11 - 1
    bits_to_read = golomb3.getTotalBits();
    
    BitStream reader3('r', "test_golomb3.bin");   
    Golomb golomb6(5, reader3, 2, bits_to_read);
    value = golomb6.decode();
    cout << "value: " << value << endl;
    
    
    // BitStream reader('r', "test_golomb.bin");
    // Golomb golomb2(5, reader, 2, bits_to_read);
    // int value = golomb2.decode();
    // cout << "value: " << value << endl;


    //Golomb golomb2(5, reader); 
    // for (int i = 0; i <= 15; i++){
    //     golomb2.decode();
    //     //cout << i << " -> " << s << endl;
    //     cout << endl;
    // }
    /*
    cout << endl;
    cout << "------------------------" << endl;
    cout << "m=5 (Não é potência de 2)" << endl;
    Golomb golomb2(5, bs); // m = 5    
    // Exemplo 5.7 do livro (m não é potência de 2)
    for (int i = 0; i <= 15; i++){
        string s = golomb2.encode(i);
        cout << i << " -> " << s << endl;
    }

    cout << "------------------------" << endl;
    cout << "m=5, números negativos" << endl;
    for (int i = 0; i >= -15; i--){
        string s = golomb2.encode(i);
        cout << i << " -> " << s << endl;
    }
    */

    return 0;
}
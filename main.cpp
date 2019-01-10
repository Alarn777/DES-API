#include <iostream>
#include <vector>
#include <stdio.h>
#include <bitset>
#include<stdio.h>
#include<stdint.h>
typedef struct {
    unsigned char TwoBits:2;
} tTwoBits;
typedef struct {
    unsigned char FourBits:4;
} tFourBits;

typedef struct {
    unsigned char SixBits:6;
} tSixBits;

typedef struct {
    unsigned char FortyEightBits:48;
} tFortyEightBitsBits;

using namespace std;
static unsigned char Positions[8] = {1,2,4,8,16,32,64,128};
vector<unsigned char> encryptBlock(const vector<unsigned char>& block, const vector<unsigned char>& key);
vector<unsigned char> decryptBlock(const vector<unsigned char>& block, const vector<unsigned char>& key);
vector<vector<unsigned char>> generateKeys(const vector<unsigned char>& key,int& number_of_keys);
vector<unsigned char>fiestel(vector<unsigned char> key, vector<unsigned char> block_48);



int main() {

    cout << "Hello, World!" << endl;
//    vector<unsigned char> *send_vec = new vector<unsigned char> ();
    vector<unsigned char> send_v;
    unsigned char cmd3[] = {0x010 ,0x31 ,0x32 ,0x11 ,0x35 ,0x39 ,0x42 ,0x38};
//                            ,0x34 ,0x31 ,0x42 ,0x36 ,0x41 ,0x33 ,0x36 ,0x37
//                            ,0x35 ,0x39 ,0x33 ,0x37 ,0x41 ,0x41 ,0x35 ,0x31
//                            ,0x44 ,0x42 ,0x45 ,0x44 ,0x45 ,0x31 ,0x45 ,0x38
//                            ,0x42 ,0x31 ,0x33 ,0x33 ,0x35 ,0x34 ,0x45 ,0x45
//                            ,0x43 ,0x46 ,0x46 ,0x45 ,0x44 ,0x34 ,0x41 ,0x32
//                            ,0x45 ,0x32 ,0x31 ,0x43 ,0x36 ,0x42 ,0x45 ,0x46
//                            ,0x34 ,0x43 ,0x32 ,0x12 ,0x10 ,0x30 ,0x01 ,0x30
//
//    };
    for (unsigned char var:cmd3)
        send_v.push_back(var);

    vector<unsigned char> key_v;
    unsigned char key[] = {'a','b','c','d','e','f','g','h'};

    for (unsigned char var:key)
        key_v.push_back(var);



    vector<unsigned char> temp = encryptBlock(send_v,key_v);





    return 0;
}


vector<unsigned char> encryptBlock(const vector<unsigned char>& block, const vector<unsigned char>& key) {
    /*
 initialPermutation <- computeInitialPermutation(block) right <- initialPermutation.right
 left <- initialPermutation.left
 subKeys <- generateKeys(key)
     for each key in subKeys:
     res <- F(right, key) temp <- right
     right <- res XOR left left <- temp
     return computeReversedPermutation(right || left)
  */






    //straight pbox
    int per[32]={16 ,  7 , 20  ,21,
                 29 , 12 , 28 , 17,
                 1 , 15,  23,  26,
                 5 , 18 , 31  ,10,
                 2  , 8 , 24 , 14,
                 32  ,27,   3 ,  9,
                 19  ,13,  30,   6,
                 22 , 11  , 4 , 25};



    int IP[8][8] =  {{58,   50,   42,    34,    26,   18 ,   10  ,  2},
                    {60  ,  52 ,  44 ,   36  ,  28  , 20  ,  12 ,   4},
                    {62  ,  54  , 46 ,   38 ,   30  , 22    ,14,    6},
                    {64   , 56  , 48  ,  40  ,  32 ,  24  ,  16,    8},
                    {57   , 49 ,  41  ,  33   , 25 ,  17  ,   9   , 1},
                    {59 ,   51 ,  43   , 35   , 27   ,19    ,11,    3},
                    {61  ,  53  , 45  ,  37 ,   29 ,  21  ,  13 ,   5},
                    {63  ,  55,   47 ,   39 ,   31 ,  23  ,  15 ,   7}
    };




// Initial permutation
    int bit, newpos;
    unsigned char desiredbit;
    unsigned char permutation[8] = {0};

    for (bit = 0; bit < 64; bit++) {
        newpos = (IP[bit / 8][bit%8]) - 1;
        desiredbit = block[bit / 8] & Positions[bit % 8];
        if (desiredbit != 0) {
            desiredbit = Positions[newpos % 8];
            permutation[newpos / 8] = desiredbit ^ permutation[newpos / 8];
        }
    }

    vector<unsigned char> block_left;
    vector<unsigned char> block_right;
    for(int i = 0; i < 8;i++){
        if(i < 4)
            block_left.push_back(permutation[i]);
        else
            block_right.push_back(permutation[i]);
    }
    cout << "Left:" << endl;
    for (unsigned char i : block_left) {
        cout << "0x" << static_cast<unsigned>(i) << endl;
    }
    cout << "Right:" << endl;
    for (unsigned char i : block_right) {
        cout << "0x" << static_cast<unsigned>(i) << endl;
    }

    int number_of_keys = 10;
    vector<vector<unsigned char>> all_keys = generateKeys(key, number_of_keys);


    //print all keys
    for(vector<unsigned char> key1: all_keys) {
        cout << "Key" << endl;
        for (int j = 0; j < 48; ++j) {
            unsigned char temp = key1[j / 8];
            if (temp & (1 << (7 - ((j) % 8)))) {
                cout << "1";
            } else
                cout << "0";
        }
        cout << endl;
    }
    cout << endl;

    //-------------------------------fiestel--------------------------


    //split a block into two
    int half_block_to_48[48] = {32    , 1 ,   2 ,    3 ,    4 ,   5,
                                4  ,   5 ,   6 ,    7  ,   8 ,   9,
                                8   ,  9  , 10  ,  11  ,  12 ,  13,
                                12 ,   13  , 14  ,  15  ,  16  , 17,
                                16   , 17 ,  18 ,   19  ,  20  , 21,
                                20 ,   21 ,  22  ,  23  ,  24  , 25,
                                24 ,   25 ,  26  ,  27  ,  28  , 29,
                                28  ,  29  , 30,    31  ,  32  ,  1};

    unsigned char left_block_48[6] = {};
    cout << "Left block expanded" << endl;
    for (int j = 0; j < 48; ++j) {
        unsigned char temp_b = block_left[(half_block_to_48[j]-1) / 8];
        if (temp_b & (1 << (7-((half_block_to_48[j]-1)%8)))) {
            cout << "1";
            left_block_48[j/8] |= 1UL << 7-(j%8);
        }
        else
            cout << "0";
    }
    cout << endl;

    unsigned char right_block_48[6] = {};
    cout << "Right block expanded" << endl;
    for (int j = 0; j < 48; ++j) {
        unsigned char temp_b = block_right[(half_block_to_48[j]-1) / 8];
        if (temp_b & (1 << (7-((half_block_to_48[j]-1)%8)))) {
            cout << "1";
            right_block_48[j/8] |= 1UL << 7-(j%8);
        }
        else
            cout << "0";
    }
    cout << endl;

    unsigned char temp_xored_block[6] = {0};

//    for (int k = 0; k < 6; ++k) {
//        temp_xored_block[k] = right_block_48[k]^all_keys[0][k];
//    }

    //actutal xoring for the blocks
    for (int l = 0; l < 16; ++l) {
        if(l%2 == 0)                                    // even
        {
//            for (int k = 0; k < 6; ++k) {
//                temp_xored_block[k] = right_block_48[k]^all_keys[l][k];        //key number l
//            }

            vector<unsigned char> temp_block_vector;
            temp_block_vector.reserve(6);
            for (unsigned char i : right_block_48) {
                temp_block_vector.push_back(i);

            }
            vector<unsigned char> temp = all_keys[0];
            vector<unsigned char> temp_resoult_of_iteration = fiestel(all_keys[l],temp_block_vector);





        } else {                                        //odd
//            for (int k = 0; k < 6; ++k) {
//                temp_xored_block[k] = right_block_48[k]^all_keys[l][k];        //key number l
//            }

        }

    }






    cout << endl;

}


vector<unsigned char> fiestel(vector<unsigned char> key, vector<unsigned char> block_48){
    //sbox configuration 6 bit to 4 bit
    int s[8][4][16]=
            {{    //0
                     14,4,13,1,2,15,11,8,3,10,6,12,5,9,0,7, //[0][0][n]
                     0,15,7,4,14,2,13,1,10,6,12,11,9,5,3,8,
                     4,1,14,8,13,6,2,11,15,12,9,7,3,10,5,0,
                     15,12,8,2,4,9,1,7,5,11,3,14,10,0,6,13
             },
             {    //1
                     15,1,8,14,6,11,3,4,9,7,2,13,12,0,5,10,
                     3,13,4,7,15,2,8,14,12,0,1,10,6,9,11,5,
                     0,14,7,11,10,4,13,1,5,8,12,6,9,3,2,15,
                     13,8,10,1,3,15,4,2,11,6,7,12,0,5,14,9
             },
             {    //2
                     10,0,9,14,6,3,15,5,1,13,12,7,11,4,2,8,
                     13,7,0,9,3,4,6,10,2,8,5,14,12,11,15,1,
                     13,6,4,9,8,15,3,0,11,1,2,12,5,10,14,7,
                     1,10,13,0,6,9,8,7,4,15,14,3,11,5,2,12
             },
             {    //3
                     7,13,14,3,0,6,9,10,1,2,8,5,11,12,4,15,
                     13,8,11,5,6,15,0,3,4,7,2,12,1,10,14,9,
                     10,6,9,0,12,11,7,13,15,1,3,14,5,2,8,4,
                     3,15,0,6,10,1,13,8,9,4,5,11,12,7,2,14
             },
             {    //4
                     2,12,4,1,7,10,11,6,8,5,3,15,13,0,14,9,
                     14,11,2,12,4,7,13,1,5,0,15,10,3,9,8,6,
                     4,2,1,11,10,13,7,8,15,9,12,5,6,3,0,14,
                     11,8,12,7,1,14,2,13,6,15,0,9,10,4,5,3
             },
             {    //5
                     12,1,10,15,9,2,6,8,0,13,3,4,14,7,5,11,
                     10,15,4,2,7,12,9,5,6,1,13,14,0,11,3,8,
                     9,14,15,5,2,8,12,3,7,0,4,10,1,13,11,6,
                     4,3,2,12,9,5,15,10,11,14,1,7,6,0,8,13
             },
             {    //6
                     4,11,2,14,15,0,8,13,3,12,9,7,5,10,6,1,
                     13,0,11,7,4,9,1,10,14,3,5,12,2,15,8,6,
                     1,4,11,13,12,3,7,14,10,15,6,8,0,5,9,2,
                     6,11,13,8,1,4,10,7,9,5,0,15,14,2,3,12
             },
             {    //7
                     13,2,8,4,6,15,11,1,10,9,3,14,5,0,12,7,
                     1,15,13,8,10,3,7,4,12,5,6,11,0,14,9,2,
                     7,11,4,1,9,12,14,2,0,6,10,13,15,3,5,8,
                     2,1,14,7,4,10,8,13,15,12,9,0,3,5,6,11
             }};
    tSixBits block_of_6_bits[8];
    tFortyEightBitsBits all_block = tFortyEightBitsBits();
    for (int i = 0; i < 48; ++i) {
        unsigned char temp_b = block_48[ i / 8];
        if (temp_b & (1 << (7-(i%8)))) {
            cout << "1";
            all_block.FortyEightBits |= 1UL << 48-(i%8);
        }
        else
            cout << "0";
    }
    cout << endl;



//    for (int k = 0; k < 48; ++k) {
//
//        block_of_6_bits[0]
//    }

    cout << s[0][0][15]  << endl;
    tTwoBits ayu;

    unsigned char temp_b = block_48[0];
    if (temp_b & (1 << (0))) {
        cout << "1";
        ayu.TwoBits |= 1UL << 0;
    }
    if (temp_b & (1 << (5))) {
        cout << "1";
        ayu.TwoBits |= 1UL << 1;
    }

    auto i1 = int(ayu.TwoBits);
    cout << i1 << endl;
//    for (int j = 0; j < 48; ++j) {
//       int k_in_32_bit_block = 0;
//        if( j < 6 ) {
//            int row=(int(xorout[j+5])-48)+(int(xorout[j])-48)*2;
//            int col= (int(xorout[j+1])-48)*8+(int(xorout[j+2])-48)*4+(int(xorout[j+3])-48)*2+(int(xorout[j+4])-48);
//            int temp=s[j/6][row][col];
//            unsigned char temp_b = block_right[(half_block_to_48[j] - 1) / 8];
//            if (temp_b & (1 << (7 - ((half_block_to_48[j] - 1) % 8)))) {
//               cout << "1";
//               right_block_48[j / 8] |= 1UL << 7 - (j % 8);
//           } else
//               cout << "0";
//       }
//        if(6 <= j < 12){
//
//        }
//    }





    return key;
}














vector<vector<unsigned char>> generateKeys(const vector<unsigned char>& key,int& number_of_keys){
    int map[48]={14,17,11,24,1,5,3,28,15,6,21,10,23,19,12,4,26,8,16,7,27,20,13,2,41,52,31,37,47,55,30,40,51,45,33,48,44,49,39,56,34,53,46,42,50,36,29,32
    };
    vector<vector<unsigned char>> all_keys;
    int keyPermut[56] = {
            57,   49,    41,   33,    25,    17,    9,
            1,    58  ,  50  , 42  ,  34  ,  26 ,  18,
            10,    2  ,  59 ,  51  ,  43 ,   35 ,  27,
            19,   11  ,   3 ,  60 ,   52  ,  44  , 36,
            63,   55 ,   47 ,  39 ,   31  ,  23  , 15,
            7,    62  ,  54 ,  46 ,   38  ,  30 ,  22,
            14,    6 ,   61,   53  ,  45  ,  37 ,  29,
            21 ,  13 ,    5 ,  28  ,  20  ,  12  ,  4
    };


    //permute key from 64 ---> 56 bit

    unsigned char key_permutation[7] = {0};
    cout << "Key permutation:" << endl;
    for (int j = 0; j < 56; ++j) {
        unsigned char temp = key[(keyPermut[j]-1) / 8];
        if (temp & (1 << (7-((keyPermut[j]-1)%8)))) {
            cout << "1";
            key_permutation[j/8] |= 1UL << 7-(j%8);
        }
        else
            cout << "0";
    }

    cout << endl;

//    cout << "Key one:" << endl;
//    unsigned char key_one[4] = {0};
//    unsigned char key_two[4] = {0};
//    for (int j = 0; j < 56; ++j) {
//        if(j == 28)
//            cout << endl << "Key two:" << endl;
//        unsigned char temp = key_permutation[j/ 8];
//        if (temp & (1 << (7-((j)%8)))) {
//            cout << "1";
//            if(j < 28)
//                key_one[j/8] |= 1UL << 7-(j%8);
//            else
//                key_two[(j-28)/8] |= 1UL << 7-((j-28)%8);
//        }
//        else
//            cout << "0";
//    }
//    cout << endl;




    // shift keys
    int shifts_needed[16] = {1,1,2,2,2,2,2,2,1,2,2,2,2,2,2,1};
    for (int i : shifts_needed) {


//        cout << "Key one:" << endl;
//        unsigned char key_one[4] = {0};
//        unsigned char key_two[4] = {0};
//        for (int j = 0; j < 56; ++j) {
//            if(j == 28)
//                cout << endl << "Key two:" << endl;
//            unsigned char temp = key_permutation[j/ 8];
//            if (temp & (1 << (7-((j)%8)))) {
//                cout << "1";
//                if(j < 28)
//                    key_one[j/8] |= 1UL << 7-(j%8);
//                else
//                    key_two[(j-28)/8] |= 1UL << 7-((j-28)%8);
//            }
//            else
//                cout << "0";
//        }
//        cout << endl;

        vector<unsigned char> temp_key;
        for (int j = 0; j < i; ++j) {

            cout << "Key one:" << endl;
            unsigned char key_one[4] = {0};
            unsigned char key_two[4] = {0};
            for (int a = 0; a < 56; ++a) {
                if(a == 28)
                    cout << endl << "Key two:" << endl;
                unsigned char temp_a = key_permutation[a/ 8];
                if (temp_a & (1 << (7-((a)%8)))) {
                    cout << "1";
                    if(a < 28)
                        key_one[a/8] |= 1UL << 7-(a%8);
                    else
                        key_two[(a-28)/8] |= 1UL << 7-((a-28)%8);
                }
                else
                    cout << "0";
            }
            cout << endl;


            unsigned char for_shifting_one[4] = {0};
            for (int l = 0; l < 4; ++l) {
                for_shifting_one[l] = key_one[l];
            }
            unsigned char for_shifting_two[4] = {0};
            for (int l = 0; l < 4; ++l) {
                for_shifting_two[l] = key_two[l];
            }


            bool set = false;
            unsigned int num = (uint32_t)for_shifting_one[0] << 24 |
                               (uint32_t)for_shifting_one[1] << 16 |
                               (uint32_t)for_shifting_one[2] << 8  |
                               (uint32_t)for_shifting_one[3];
            set = (num & (1 << 31)) != 0;
            num = num << 1;
            if(set) {
                key_one[3] |= 1UL << (7);
                num |= 1UL << (4);
            }
            unsigned char shifted_one[4];
            shifted_one[3] = num & 0xFF;
            shifted_one[2] = (num >> 8) & 0xFF;
            shifted_one[1] = (num >> 16) & 0xFF;
            shifted_one[0] = (num >> 24) & 0xFF;


            num = (uint32_t)for_shifting_two[0] << 24 |
                               (uint32_t)for_shifting_two[1] << 16 |
                               (uint32_t)for_shifting_two[2] << 8  |
                               (uint32_t)for_shifting_two[3];
            set = (num & (1 << 31)) != 0;
            num = num << 1;
            if(set) {
                key_one[3] |= 1UL << (7);
                num |= 1UL << (4);
            }
            unsigned char shifted_two[4];
            shifted_two[3] = num & 0xFF;
            shifted_two[2] = (num >> 8) & 0xFF;
            shifted_two[1] = (num >> 16) & 0xFF;
            shifted_two[0] = (num >> 24) & 0xFF;

            unsigned char key_after_collection[7] = {0};

            for (int m = 0; m < 56; ++m) {
                unsigned char temp_char;
                if(m < 28){
                    temp_char = shifted_one[m/ 8];
                    if (temp_char & (1 << (7-((m)%8)))) {
                        key_after_collection[m / 8] |= 1UL << 7 - (m % 8);
                    }

                }
                else {
                    temp_char = shifted_two[(m-28) / 8];
                    if (temp_char & (1 << (7 - ((m-28) % 8)))) {
                        key_after_collection[m / 8] |= 1UL << 7 - (m % 8);
                    }
                }
            }


            //print and vector initiation
//            cout << "Merged key"  << endl;
//            for (int m = 0; m < 56; ++m) {
//                temp.push_back(key_after_collection[m]);
//
//                unsigned char temp_char;
//                temp_char = key_after_collection[m / 8];
//                if (temp_char & (1 << (7 - ((m) % 8)))) {
//                    cout << "1";
//                }
//                else
//                    cout << "0";
//            }
//            cout << endl;



            for (int k = 0; k < 56; ++k) {
                key_permutation[k] = key_after_collection[k];
            }
        }

        cout << "Merged key"  << endl;
        for (int m = 0; m < 56; ++m) {
//            temp.push_back(key_permutation[m]);

            unsigned char temp_char;
            temp_char = key_permutation[m / 8];
            if (temp_char & (1 << (7 - ((m) % 8)))) {
                cout << "1";
            }
            else
                cout << "0";
        }
        cout << endl;


        //now make 48 from 56 key
        cout << "Derived key 48 bits"  << endl;
        unsigned char bits[6] = {0};

        for (int j = 0; j < 48; ++j) {
            unsigned char temp_b = key_permutation[(map[j]-1) / 8];
            if (temp_b & (1 << (7-((map[j]-1)%8)))) {
                cout << "1";
                bits[j/8] |= 1UL << 7-(j%8);
            }
            else
                cout << "0";
        }
        cout << endl;

        for (unsigned char bit : bits) {
            temp_key.push_back(bit);
        }




        all_keys.push_back(temp_key);
    }












//    unsigned char bits[6] = {0};
//
//    for (int j = 0; j < 48; ++j) {
//        unsigned char temp = key[(map[j]-1) / 8];
//        if (temp & (1 << (7-((map[j]-1)%8)))) {
//            cout << "1";
//            bits[j/8] |= 1UL << 7-(j%8);
//        }
//        else
//            cout << "0";
//    }
//    cout << endl;


//    for(vector<unsigned char> key1: all_keys) {
//        cout << "Vector" << endl;
//        for (int j = 0; j < 48; ++j) {
//            unsigned char temp = key1[j / 8];
//            if (temp & (1 << (7 - ((j) % 8)))) {
//                cout << "1";
//            } else
//                cout << "0";
//        }
//        cout << endl;
//    }
//    cout << endl;





    return all_keys;
}




vector<unsigned char> decryptBlock(const vector<unsigned char>& block, const vector<unsigned char>& key)
{

}
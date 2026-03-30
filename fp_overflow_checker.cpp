#include <iostream>
#include <bitset>
#include <cmath>

using namespace std;

bitset<32> getBits(float value) {
    return bitset<32>(*reinterpret_cast<unsigned int*>(&value));
}

void printFloatBits(bitset<32> bits) {
    string s = bits.to_string();

    // Split into parts
    string sign = s.substr(0, 1);
    string exponent = s.substr(1, 8);
    string fraction = s.substr(9, 23);

    cout << sign << " " << exponent << " " << fraction;
}

// Extract exponent
int getExponent(bitset<32> bits) {
    unsigned int raw = bits.to_ulong();
    return ((raw >> 23) & 0xFF) - 127;
}

// Compute ULP
float computeULP(float value) {
    int e = getExponent(getBits(value));
    return pow(2.0f, e - 23);
}

// Find overflow threshold
float findThreshold(float increment) {
    float x = increment;

    while (true) {
        float ulp = computeULP(x);
        if (ulp > increment) break;
        x *= 2.0f;
    }
    return x;
}


int main(int argc, char *argv[]) {

    if (argc != 3) {
        cout << "usage:" << endl;
        cout << "./fp_overflow_checker loop_bound loop_counter" << endl;
        cout << endl;
        cout << "loop_bound is a positive floating-point value" << endl;
        cout << "loop_counter is a positive floating-point value" << endl;
        cout << endl;
        return 1;
    }

    float bound = atof(argv[1]);
    float increment = atof(argv[2]);

    bitset<32> boundBits = getBits(bound);
    bitset<32> incBits = getBits(increment);

    cout << endl << "Loop bound:   ";
    printFloatBits(boundBits);
    cout << endl;

    cout << "Loop counter: ";
    printFloatBits(incBits);
    cout << endl << endl;

    float ulp = computeULP(bound);

    if (increment < ulp) {

        float threshold = findThreshold(increment);

        cout << "Warning: Possible overflow!" << endl;
        cout << "Overflow threshold: " << endl;

        cout << "        " << threshold << endl << "        ";

        printFloatBits(getBits(threshold));
        cout << endl;
    } else {
        cout << "There is no overflow!" << endl;
    }

    return 0;
}
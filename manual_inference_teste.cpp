#include <iostream>
#include <cmath>

using namespace std;

void predict(int *input){
    int dense_1[4][10] = {{-19795, -14373, -19952, 4640, 2655, 27663, -20498, 1125, 7651, -10118},
                          {-8699, 3250, -12542, 28004, 9959, 27722, 3376, -11837, -24572, -19184},
                          {-13993, 32767, 8969, 11855, 1715, -23566, 21762, 19421, 22433, 11648},
                          {-5314, 11186, -11959, -3542, -27207, -25821, 25303, 16529, 29229, -20525}};
    
    int bias_1[10] = {0, -6043, 0, 7864, 7246, 16157, -7381, -9306, -8645, 0};
    int Q_1 = 15;
    
    
    int dense_2[10][10] = {{7063, 1485, 6155, 1627, -16790, -10793, -15479, -5787, -15510, 7666},
                          {-13089, -9515, 7404, 10963, -484, -2801, 27071, -15239, -321, -8363},
                          {-6608, 11179, -7746, -13298, 3485, 1918, 11388, -8925, -4735, 4927},
                          {-238, -8662, -8102, -5263, 4839, -16748, 8927, 9138, -2206, 27355},
                          {10415, -15844, -12594, -907, -2645, 12060, -16742, 1607, -2134, 14925},
                          {29497, -9404, 16067, -12229, -2436, 2177, -2774, 1981, 20464, 32767},
                          {-1816, 3675, 10932, -10785, -13626, -16766, 10484, -306, -1415, -19715},
                          {-10282, 3398, 15089, -5589, 18898, -11967, 17821, -12408, -12345, -21476},
                          {154, -12257, 12779, -15871, 21812, 228, 23260, 228, -18271, -28151},
                          {12769, -8197, -3118, 17216, -11596, -13220, 7844, 673, 13013, 17692}};

    
    int bias_2[10] = {4834, 0, -1394, 0, 2371, 0, -2693, 624, 2184, 12395};
    int Q_2 = 15;
    
    
    int dense_3[10][3] = {{19664, 9150, -23742},
                          {17410, 5808, 9579},
                          {-5316, -6239, -921},
                          {-11091, -17293, -12750},
                          {-18056, 12197, 8456},
                          {-3604, 12754, -11371},
                          {-25862, 10387, 32488},
                          {15457, 6670, -594},
                          {17926, -14600, 3124},
                          {30210, 24997, -32768}};

    
    int bias_3[10] = {171, 3732, -3904};
    int Q_3 = 15;
    
    
    
    
    int y[10] = {0};
    
    
    
    //============================================= CAMADA 1
    
    
    for (int i = 0; i < 10; i++) {

        long acc = 0;  // acumula o dot-product (use long para evitar overflow)

        for (int j = 0; j < 4; j++) {
            acc += (long)input[j] * (long)dense_1[j][i];
        }

        // soma o bias já quantizado
        acc += (long)bias_1[i];

        // volta para Q1.15 (shift)
        acc = acc >> Q_1;

        // ReLU
        if (acc < 0) acc = 0;

        y[i] = (int)acc;
    }
    
    
    //========================================= camada 2
    
    for (int i = 0; i < 10; i++) {

        long acc = 0;  // acumula o dot-product (use long para evitar overflow)

        for (int j = 0; j < 10; j++) {
            acc += (long)y[j] * (long)dense_2[j][i];
        }

        // soma o bias já quantizado
        acc += (long)bias_2[i];

        // volta para Q1.15 (shift)
        acc = acc >> Q_2;

        // ReLU
        if (acc < 0) acc = 0;

        y[i] = (int)acc;
    }
    
    // ==================================================== CAMADA 3
    
    for (int i = 0; i < 3; i++) {

        long acc = 0;  // acumula o dot-product (use long para evitar overflow)

        for (int j = 0; j < 10; j++) {
            acc += (long)y[j] * (long)dense_3[j][i];
        }

        // soma o bias já quantizado
        acc += (long)bias_3[i];

        // volta para Q1.15 (shift)
        acc = acc >> Q_3;

        // ReLU
        if (acc < 0) acc = 0;

        y[i] = (int)acc;
    }
    
    
    
    // imprime os valores
    for (int i = 0; i < 10; i++) {
        cout << "y[" << i << "] = " << y[i] << endl;
    }
    
    
    //EXTERNO A FPGA
    // --- DESQUANTIZAÇÃO ---
    float f0 = y[0] / 32768.0f;
    float f1 = y[1] / 32768.0f;
    float f2 = y[2] / 32768.0f;
    
    // --- SOFTMAX ESTÁVEL ---
    float m = max(f0, max(f1, f2));
    
    float e0 = exp(f0 - m);
    float e1 = exp(f1 - m);
    float e2 = exp(f2 - m);
    
    float sum = e0 + e1 + e2;
    
    float p0 = e0 / sum;
    float p1 = e1 / sum;
    float p2 = e2 / sum;
    
    cout << "Softmax: [" << p0 << ", " << p1 << ", " << p2 << "]\n";
    
}


int main()
{
    int input[4] = {167117, 114688, 45875, 6554};
    
    
    predict(input);
    

    return 0;
}

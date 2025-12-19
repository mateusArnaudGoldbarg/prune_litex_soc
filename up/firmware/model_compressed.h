#ifndef MODEL_COMPRESSED_H
#define MODEL_COMPRESSED_H

// -----------------------------------------
// Layer 1 (Dense 1)
// -----------------------------------------

// Pesos da camada 1: shape (4, 10)




#define NNZ_1_1 1
static const int dense_1_1[NNZ_1_1] = {-15761};
static const int idx_1_1[NNZ_1_1] = {1};

#define NNZ_1_2 2
static const int dense_1_2[NNZ_1_2] = {21376, 15945};
static const int idx_1_2[NNZ_1_2] = {1,2};

#define NNZ_1_3 0
static const int dense_1_3[NNZ_1_3] = {};
static const int idx_1_3[NNZ_1_3] = {};

#define NNZ_1_4 2
static const int dense_1_4[NNZ_1_4] = {32767, 32767};
static const int idx_1_4[NNZ_1_4] = {2,3};

#define NNZ_1_5 1
static const int dense_1_5[NNZ_1_5] = {-16361};
static const int idx_1_5[NNZ_1_5] = {2};

#define NNZ_1_6 2
static const int dense_1_6[NNZ_1_6] = {-15367, -17513};
static const int idx_1_6[NNZ_1_6] = {0,3};

#define NNZ_1_7 2
static const int dense_1_7[NNZ_1_7] = {-17752, 19326};
static const int idx_1_7[NNZ_1_7] = {0,3};

#define NNZ_1_8 2
static const int dense_1_8[NNZ_1_8] = {-31378, 25059};
static const int idx_1_8[NNZ_1_8] = {1,3};

#define NNZ_1_9 1
static const int dense_1_9[NNZ_1_9] = {32767};
static const int idx_1_9[NNZ_1_9] = {0};

#define NNZ_1_10 1
static const int dense_1_10[NNZ_1_10] = {21005};
static const int idx_1_10[NNZ_1_10] = {2};

//CAMADA 2

#define NNZ_2_1 4
static const int dense_2_1[NNZ_2_1] = {12546, -12292, 12148, -17552};
static const int idx_2_1[NNZ_2_1] = {0, 1, 2, 7};

#define NNZ_2_2 4
static const int dense_2_2[NNZ_2_2] = {-14749, -14233, 17236, 12555};
static const int idx_2_2[NNZ_2_2] = {0, 4, 6, 9};

#define NNZ_2_3 3
static const int dense_2_3[NNZ_2_3] = {-14339, -12838, -9849};
static const int idx_2_3[NNZ_2_3] = {0, 4, 7};

#define NNZ_2_4 4
static const int dense_2_4[NNZ_2_4] = {11326, -16323, 12337, 16324};
static const int idx_2_4[NNZ_2_4] = {0, 1, 2, 3};

#define NNZ_2_5 4
static const int dense_2_5[NNZ_2_5] = {-11637, 12923, 9766, -17303};
static const int idx_2_5[NNZ_2_5] = {1, 2, 5, 9};

#define NNZ_2_6 4
static const int dense_2_6[NNZ_2_6] = {12448, 12434, 14730, 32767};
static const int idx_2_6[NNZ_2_6] = {0, 4, 5, 8};

#define NNZ_2_7 2
static const int dense_2_7[NNZ_2_7] = {-13981, 14589};
static const int idx_2_7[NNZ_2_7] = {0,2};

#define NNZ_2_8 6
static const int dense_2_8[NNZ_2_8] = {-16902, -13024, 25884, -14117, -11769, 12748};
static const int idx_2_8[NNZ_2_8] = {1, 2, 3, 4, 7, 9};

#define NNZ_2_9 4
static const int dense_2_9[NNZ_2_9] = {10240, 16190, -15738, -12877};
static const int idx_2_9[NNZ_2_9] = {3, 5, 8, 9};

#define NNZ_2_10 4
static const int dense_2_10[NNZ_2_10] = {32767, -14896, 32767, -21698};
static const int idx_2_10[NNZ_2_10] = {3, 5, 7, 8};


//CAMADA 3

#define NNZ_3_1 5
static const int dense_3_1[NNZ_3_1] = {-21951, -19418, 32767, -23001, -32768};
static const int idx_3_1[NNZ_3_1] = {0, 4, 5, 7, 9};

#define NNZ_3_2 4
static const int dense_3_2[NNZ_3_2] = {21049, -20814, 17410, 16987};
static const int idx_3_2[NNZ_3_2] = {0, 4, 5, 6};

#define NNZ_3_3 5
static const int dense_3_3[NNZ_3_3] = {-20449, -32768, 18463, 21072, 32767};
static const int idx_3_3[NNZ_3_3] = {0, 5, 6, 8, 9};


static const int nnz_1[10] = {
				NNZ_1_1,
				NNZ_1_2,
				NNZ_1_3,
				NNZ_1_4,
				NNZ_1_5,
				NNZ_1_6,
				NNZ_1_7,
				NNZ_1_8,
				NNZ_1_9,
				NNZ_1_10
};

static const int nnz_2[10] = {
				NNZ_2_1,
				NNZ_2_2,
				NNZ_2_3,
				NNZ_2_4,
				NNZ_2_5,
				NNZ_2_6,
				NNZ_2_7,
				NNZ_2_8,
				NNZ_2_9,
				NNZ_2_10
};

static const int nnz_3[10] = {
				NNZ_3_1,
				NNZ_3_2,
				NNZ_3_3
};

// Bias da camada 1: shape (10)
static const int bias_1_c[10] = {
     0, 0, 0, -32768, 0, 0, 0, 0, 0, 0
};

// Bias da camada 2: shape (10)
static const int bias_2_c[10] = {
    0, 0, 0, 0, 0, 28778, 0, 0, 0, 0
};
// Bias da camada 3: shape (3)
static const int bias_3_c[3] = {
    9202, 0, -18791
};
#endif // MODEL_COMPRESSED_H


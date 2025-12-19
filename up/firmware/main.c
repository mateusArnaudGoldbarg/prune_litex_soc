#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "model.h"
#include "model_pruned.h"
#include "model_compressed.h"
#include "data.h"
#include <inttypes.h>
#include <stdint.h>
#include <unistd.h>
#include <math.h>
#include <time.h>


#include <irq.h>
#include <uart.h>
#include <console.h>
#include <generated/csr.h>

#ifndef MAX
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif

static const uint16_t exp_lut_q15[65] = {
    11,    13,    15,    18,    22,    27,    33,    40,
    49,    60,    73,    89,   109,   133,   163,   200,
   246,   302,   371,   456,   560,   687,   843,  1035,
  1270,  1558,  1915,  2353,  2892,  3558,  4377,  5378,
  6606,  8114,  9964, 12235, 15027, 18461, 22674, 27820,
 34171, 41905, 51391, 63189, 77894, 96139,118754,146855,
181292,223810,276070,340207,418121,513745,631615,776436,
  32768
};


static char *readstr(void)
{
	char c[2];
	static char s[64];
	static int ptr = 0;

	if(readchar_nonblock()) {
		c[0] = readchar();
		c[1] = 0;
		switch(c[0]) {
			case 0x7f:
			case 0x08:
				if(ptr > 0) {
					ptr--;
					putsnonl("\x08 \x08");
				}
				break;
			case 0x07:
				break;
			case '\r':
			case '\n':
				s[ptr] = 0x00;
				putsnonl("\n");
				ptr = 0;
				return s;
			default:
				if(ptr >= (sizeof(s) - 1))
					break;
				putsnonl(c);
				s[ptr] = c[0];
				ptr++;
				break;
		}
	}

	return NULL;
}

static char *get_token(char **str)
{
	char *c, *d;

	c = (char *)strchr(*str, ' ');
	if(c == NULL) {
		d = *str;
		*str = *str+strlen(*str);
		return d;
	}
	*c = 0;
	d = *str;
	*str = c+1;
	return d;
}

static void prompt(void)
{
	printf("RUNTIME>");
}

static void help(void)
{
	puts("Available commands:");
	puts("help                            - this command");
	puts("reboot                          - reboot CPU");
	puts("led                             - led test");
	puts("dot                             - Normal Inference");
	puts("prune                           - Pruned Inference");
}

static void reboot(void)
{
	ctrl_reset_write(1);
}

static void toggle_led(void)
{
	int i;
	printf("invertendo led...\n");
    i = leds_out_read();
    leds_out_write(!i);
}

void print_int64(int64_t val) {
    if (val < 0) {
        printf("-");
        val = (~val) +1;
    }
    uint32_t high = (uint32_t)(val >> 32);
    uint32_t low  = (uint32_t)val;
    
    printf("0. %ld\n", (signed long)high);
    
    if (high > 0)
        printf("1. %lu   -  %09lu\n", (unsigned long)high, (unsigned long)low);
    else
        printf("2. %lu\n", (unsigned long)low);
}

void print_float(const char *name, float v)
{
    int32_t i = (int32_t)(v * 10000.0f);
    if (i < 0) {
        printf("%s = -%ld.%04ld\n",
               name,
               (long)(-i / 10000),
               (long)(-i % 10000));
    } else {
        printf("%s = %ld.%04ld\n",
               name,
               (long)(i / 10000),
               (long)(i % 10000));
    }
}

static inline uint64_t rdcycle(void)
{
    uint64_t cycles;
    asm volatile ("rdcycle %0" : "=r"(cycles));
    return cycles;
}


static void calc_dot_product(void)
{
	int32_t y[10] = {0,0,0,0,0,0,0,0,0,0};
	uint64_t result;
	
	// ===== contadores de tempo =====
	uint32_t t_dense1 = 0;
	uint32_t t_dense2 = 0;
	uint32_t t_dense3 = 0;
	uint32_t cycles = 0;
	
	uint32_t times_dense1[150];
	uint32_t times_dense2[150];
	uint32_t times_dense3[150];
	
	//n-ésima amostra
	for(int n=0;n<150;n++){
	
		dotProduct_a0_write(input_data[n][0]); 
		dotProduct_a1_write(input_data[n][1]);
		dotProduct_a2_write(input_data[n][2]); 
		dotProduct_a3_write(input_data[n][3]);
		dotProduct_a4_write(0); 
		dotProduct_a5_write(0);
		dotProduct_a6_write(0); 
		dotProduct_a7_write(0);
		dotProduct_a8_write(0); 
		dotProduct_a9_write(0);
		
		for(int i=0;i<10;i++){
			// Espera o hardware terminar
			dotProduct_nnz_write(10);

			// dense_1
			dotProduct_b0_write(dense_1[0][i]); 
			dotProduct_b1_write(dense_1[1][i]);
			dotProduct_b2_write(dense_1[2][i]); 
			dotProduct_b3_write(dense_1[3][i]);
			dotProduct_b4_write(dense_1[4][i]); 
			dotProduct_b5_write(dense_1[5][i]);
			dotProduct_b6_write(dense_1[6][i]); 
			dotProduct_b7_write(dense_1[7][i]);
			dotProduct_b8_write(dense_1[8][i]); 
			dotProduct_b9_write(dense_1[9][i]);
			dotProduct_bias_write(bias_1[i]);
			
			dotProduct_start_write(1);
			
			while (dotProduct_busy_read());
			
			result = dotProduct_result_read();
		    	
			cycles = dotProduct_cycles_read();
			t_dense1 += cycles;
		    	
		    	y[i] = result >>15;
		}
				
		times_dense1[n] = t_dense1;
		
		t_dense1 = 0;
		
		//relu_1
		for(int j=0; j<10;j++){
			if (y[j] <= 0){
				y[j] = 0;
			}
		}
		
		//DENSE_2
		dotProduct_a0_write(y[0]); 
		dotProduct_a1_write(y[1]);
		dotProduct_a2_write(y[2]); 
		dotProduct_a3_write(y[3]);
		dotProduct_a4_write(y[4]); 
		dotProduct_a5_write(y[5]);
		dotProduct_a6_write(y[6]); 
		dotProduct_a7_write(y[7]);
		dotProduct_a8_write(y[8]); 
		dotProduct_a9_write(y[9]);
		
		
		for(int j=0; j<10;j++){
			y[j] = 0;
		}
		
		for(int i=0;i<10;i++){
		
			dotProduct_nnz_write(10);
			
			dotProduct_b0_write(dense_2[0][i]); 
			dotProduct_b1_write(dense_2[1][i]);
			dotProduct_b2_write(dense_2[2][i]); 
			dotProduct_b3_write(dense_2[3][i]);
			dotProduct_b4_write(dense_2[4][i]); 
			dotProduct_b5_write(dense_2[5][i]);
			dotProduct_b6_write(dense_2[6][i]); 
			dotProduct_b7_write(dense_2[7][i]);
			dotProduct_b8_write(dense_2[8][i]); 
			dotProduct_b9_write(dense_2[9][i]);
			
			dotProduct_bias_write(bias_2[i]);
			
			// Inicia cálculo no hardware
			dotProduct_start_write(1);
			
			// Espera o hardware terminar
			while (dotProduct_busy_read());
			
			result = dotProduct_result_read();
		    	
		    	cycles = dotProduct_cycles_read();
			
			t_dense2 += cycles;
		    	
		    	y[i] = result >> 15;
		}
		
		times_dense2[n] = t_dense2;
		t_dense2 = 0;
		
		//relu_2
		for(int j=0; j<10;j++){
			if (y[j] <= 0){
				y[j] = 0;
			}
		}
		
		//DENSE_3
		dotProduct_a0_write(y[0]); 
		dotProduct_a1_write(y[1]);
		dotProduct_a2_write(y[2]); 
		dotProduct_a3_write(y[3]);
		dotProduct_a4_write(y[4]); 
		dotProduct_a5_write(y[5]);
		dotProduct_a6_write(y[6]); 
		dotProduct_a7_write(y[7]);
		dotProduct_a8_write(y[8]); 
		dotProduct_a9_write(y[9]);
		
		for(int i=0;i<3;i++){
			dotProduct_nnz_write(10);
			dotProduct_b0_write(dense_3[0][i]); 
			dotProduct_b1_write(dense_3[1][i]);
			dotProduct_b2_write(dense_3[2][i]); 
			dotProduct_b3_write(dense_3[3][i]);
			dotProduct_b4_write(dense_3[4][i]); 
			dotProduct_b5_write(dense_3[5][i]);
			dotProduct_b6_write(dense_3[6][i]); 
			dotProduct_b7_write(dense_3[7][i]);
			dotProduct_b8_write(dense_3[8][i]); 
			dotProduct_b9_write(dense_3[9][i]);
			
			dotProduct_bias_write(bias_3[i]);
			
			// Inicia cálculo no hardware
			dotProduct_start_write(1);
			
			// Espera o hardware terminar
			while (dotProduct_busy_read());
			
			result = dotProduct_result_read();
		    	
			cycles = dotProduct_cycles_read();
			
			t_dense3 += cycles;    			
    					    	
		    	y[i] = result >> 15;
		    	
		}
		
		times_dense3[n] = t_dense3;
		t_dense3 = 0;
		
		int max_i = 0;
		for (int f=0; f<3;f++){
			if(y[f] > y[max_i]){
				max_i = f;	
			}
		}
	}
	
	printf("--------------------NORMAL RESULTS-----------------------\n");
	printf("------------QUANTIDADE DE CICLOS POR CAMADA--------------\n");
	printf("DENSE 1:\n");
	for(int o = 0; o < 1; o++){
		printf("%lu, ", times_dense1[o]);
	}
	printf("\n\n");
	
	printf("DENSE 2:\n");
	for(int o = 0; o < 1; o++){
		printf("%lu, ", times_dense2[o]);
	}
	printf("\n\n");

	printf("DENSE 3:\n");
	for(int o = 0; o < 1; o++){
		printf("%lu, ", times_dense3[o]);
	}
	printf("\n\n");

}


static void calc_prune(void)
{
	int32_t A[10]= {0,0,0,0,0,0,0,0,0,0};
	int32_t B[10]= {0,0,0,0,0,0,0,0,0,0};
	int32_t y[10] = {0,0,0,0,0,0,0,0,0,0};
	uint64_t result;
	
	// ===== contadores de tempo =====
	uint32_t t_dense1 = 0;
	uint32_t t_dense2 = 0;
	uint32_t t_dense3 = 0;
	uint32_t cycles = 0;
	
	uint32_t times_dense1[150];
	uint32_t times_dense2[150];
	uint32_t times_dense3[150];
	
	//n-ésima amostra
	for(int n=0;n<150;n++){
		// dense_1
			
		for(int i=0;i<10;i++){
			// Espera o hardware terminar
			dotProduct_nnz_write(nnz_1[i]);
			if (i == 0){
				for(int aaa = 0; aaa < nnz_1[i]; aaa++){
					A[idx_1_1[aaa]] = input_data[aaa];
					B[idx_1_1[aaa]] = dense_1_1[aaa];
				}
			}
			if (i == 1){
				for(int aaa = 0; aaa < nnz_1[i]; aaa++){
					A[idx_1_2[aaa]] = input_data[aaa];
					B[idx_1_2[aaa]] = dense_1_2[aaa];
				}
			}
			
			if (i == 2){
				for(int aaa = 0; aaa < nnz_1[i]; aaa++){
					A[idx_1_3[aaa]] = input_data[aaa];
					B[idx_1_3[aaa]] = dense_1_3[aaa];
				}
			}
			
			if (i == 3){
				for(int aaa = 0; aaa < nnz_1[i]; aaa++){
					A[idx_1_4[aaa]] = input_data[aaa];
					B[idx_1_4[aaa]] = dense_1_4[aaa];
				}
			}
			
			if (i == 4){
				for(int aaa = 0; aaa < nnz_1[i]; aaa++){
					A[idx_1_5[aaa]] = input_data[aaa];
					B[idx_1_5[aaa]] = dense_1_5[aaa];
				}
			}
			
			if (i == 5){
				for(int aaa = 0; aaa < nnz_1[i]; aaa++){
					A[idx_1_6[aaa]] = input_data[aaa];
					B[idx_1_6[aaa]] = dense_1_6[aaa];
				}
			}
			
			if (i == 6){
				for(int aaa = 0; aaa < nnz_1[i]; aaa++){
					A[idx_1_7[aaa]] = input_data[aaa];
					B[idx_1_7[aaa]] = dense_1_7[aaa];
				}
			}
			
			if (i == 7){
				for(int aaa = 0; aaa < nnz_1[i]; aaa++){
					A[idx_1_8[aaa]] = input_data[aaa];
					B[idx_1_8[aaa]] = dense_1_8[aaa];
				}
			}
			
			if (i == 8){
				for(int aaa = 0; aaa < nnz_1[i]; aaa++){
					A[idx_1_9[aaa]] = input_data[aaa];
					B[idx_1_9[aaa]] = dense_1_9[aaa];
				}
			}
			
			if (i == 9){
				for(int aaa = 0; aaa < nnz_1[i]; aaa++){
					A[idx_1_10[aaa]] = input_data[aaa];
					B[idx_1_10[aaa]] = dense_1_10[aaa];
				}
			}
			
			dotProduct_a0_write(A[0]); 
			dotProduct_a1_write(A[1]);
			dotProduct_a2_write(A[2]); 
			dotProduct_a3_write(A[3]);
			dotProduct_a4_write(A[4]); 
			dotProduct_a5_write(A[5]);
			dotProduct_a6_write(A[6]); 
			dotProduct_a7_write(A[7]);
			dotProduct_a8_write(A[8]); 
			dotProduct_a9_write(A[9]);
			
			dotProduct_b0_write(B[0]); 
			dotProduct_b1_write(B[1]);
			dotProduct_b2_write(B[2]); 
			dotProduct_b3_write(B[3]);
			dotProduct_b4_write(B[4]); 
			dotProduct_b5_write(B[5]);
			dotProduct_b6_write(B[6]); 
			dotProduct_b7_write(B[7]);
			dotProduct_b8_write(B[8]); 
			dotProduct_b9_write(B[9]);
			
			
			dotProduct_bias_write(bias_1_p[i]);
			
			// Inicia cálculo no hardware

			dotProduct_start_write(1);

			while (dotProduct_busy_read());
			
			result = dotProduct_result_read();
		    	
		    	cycles = dotProduct_cycles_read();
			t_dense1 += cycles;
				
		    	y[i] = result >>15;
		    	
		}
				
		times_dense1[n] = t_dense1;
		
		t_dense1 = 0;
		
		//relu_1
		for(int j=0; j<10;j++){
			if (y[j] <= 0){
				y[j] = 0;
			}
		}
		
		for(int j=0; j<10;j++){
			A[j] = 0;
			B[j] = 0;
		}
		
		for(int i=0;i<10;i++){
			dotProduct_nnz_write(nnz_2[i]);
			if (i == 0){
				for(int aaa = 0; aaa < nnz_2[i]; aaa++){
					A[idx_2_1[aaa]] = y[aaa];
					B[idx_2_1[aaa]] = dense_2_1[aaa];
				}
			}
			if (i == 1){
				for(int aaa = 0; aaa < nnz_2[i]; aaa++){
					A[idx_2_2[aaa]] = y[aaa];
					B[idx_2_2[aaa]] = dense_2_2[aaa];
				}
			}
			
			if (i == 2){
				for(int aaa = 0; aaa < nnz_2[i]; aaa++){
					A[idx_2_3[aaa]] = y[aaa];
					B[idx_2_3[aaa]] = dense_2_3[aaa];
				}
			}
			
			if (i == 3){
				for(int aaa = 0; aaa < nnz_2[i]; aaa++){
					A[idx_2_4[aaa]] = y[aaa];
					B[idx_2_4[aaa]] = dense_2_4[aaa];
				}
			}
			
			if (i == 4){
				for(int aaa = 0; aaa < nnz_2[i]; aaa++){
					A[idx_2_5[aaa]] = y[aaa];
					B[idx_2_5[aaa]] = dense_2_5[aaa];
				}
			}
			
			if (i == 5){
				for(int aaa = 0; aaa < nnz_2[i]; aaa++){
					A[idx_2_6[aaa]] = y[aaa];
					B[idx_2_6[aaa]] = dense_2_6[aaa];
				}
			}
			
			if (i == 6){
				for(int aaa = 0; aaa < nnz_2[i]; aaa++){
					A[idx_2_7[aaa]] = y[aaa];
					B[idx_2_7[aaa]] = dense_2_7[aaa];
				}
			}
			
			if (i == 7){
				for(int aaa = 0; aaa < nnz_2[i]; aaa++){
					A[idx_2_8[aaa]] = y[aaa];
					B[idx_2_8[aaa]] = dense_2_8[aaa];
				}
			}
			
			if (i == 8){
				for(int aaa = 0; aaa < nnz_2[i]; aaa++){
					A[idx_2_9[aaa]] = y[aaa];
					B[idx_2_9[aaa]] = dense_2_9[aaa];
				}
			}
			
			if (i == 9){
				for(int aaa = 0; aaa < nnz_2[i]; aaa++){
					A[idx_2_10[aaa]] = y[aaa];
					B[idx_2_10[aaa]] = dense_2_10[aaa];
				}
			}
			
			dotProduct_a0_write(A[0]); 
			dotProduct_a1_write(A[1]);
			dotProduct_a2_write(A[2]); 
			dotProduct_a3_write(A[3]);
			dotProduct_a4_write(A[4]); 
			dotProduct_a5_write(A[5]);
			dotProduct_a6_write(A[6]); 
			dotProduct_a7_write(A[7]);
			dotProduct_a8_write(A[8]); 
			dotProduct_a9_write(A[9]);
			
			dotProduct_b0_write(B[0]); 
			dotProduct_b1_write(B[1]);
			dotProduct_b2_write(B[2]); 
			dotProduct_b3_write(B[3]);
			dotProduct_b4_write(B[4]); 
			dotProduct_b5_write(B[5]);
			dotProduct_b6_write(B[6]); 
			dotProduct_b7_write(B[7]);
			dotProduct_b8_write(B[8]); 
			dotProduct_b9_write(B[9]);
			
			dotProduct_bias_write(bias_2_p[i]);
			
			// Inicia cálculo no hardware
			dotProduct_start_write(1);
			
			// Espera o hardware terminar
			while (dotProduct_busy_read());
			
			result = dotProduct_result_read();
		    	
			cycles = dotProduct_cycles_read();
			t_dense2 += cycles;
		    	
		    	y[i] = result >> 15;
		}
		
		times_dense2[n] = t_dense2;
		t_dense2 = 0;
		
		//relu_2
		for(int j=0; j<10;j++){
			if (y[j] <= 0){
				y[j] = 0;
			}
		}
		
		for(int j=0; j<10;j++){
			A[j] = 0;
			B[j] = 0;
		}		
		
		//DENSE_3
		
		for(int i=0;i<3;i++){
			
			dotProduct_nnz_write(nnz_3[i]);
			if (i == 0){
				for(int aaa = 0; aaa < nnz_3[i]; aaa++){
					A[idx_3_1[aaa]] = y[aaa];
					B[idx_3_1[aaa]] = dense_3_1[aaa];
				}
			}
			if (i == 1){
				for(int aaa = 0; aaa < nnz_3[i]; aaa++){
					A[idx_3_2[aaa]] = y[aaa];
					B[idx_3_2[aaa]] = dense_3_2[aaa];
				}
			}
			
			if (i == 2){
				for(int aaa = 0; aaa < nnz_3[i]; aaa++){
					A[idx_3_3[aaa]] = y[aaa];
					B[idx_3_3[aaa]] = dense_3_3[aaa];
				}
			}
			
			
			dotProduct_a0_write(A[0]); 
			dotProduct_a1_write(A[1]);
			dotProduct_a2_write(A[2]); 
			dotProduct_a3_write(A[3]);
			dotProduct_a4_write(A[4]); 
			dotProduct_a5_write(A[5]);
			dotProduct_a6_write(A[6]); 
			dotProduct_a7_write(A[7]);
			dotProduct_a8_write(A[8]); 
			dotProduct_a9_write(A[9]);
			
			dotProduct_b0_write(B[0]); 
			dotProduct_b1_write(B[1]);
			dotProduct_b2_write(B[2]); 
			dotProduct_b3_write(B[3]);
			dotProduct_b4_write(B[4]); 
			dotProduct_b5_write(B[5]);
			dotProduct_b6_write(B[6]); 
			dotProduct_b7_write(B[7]);
			dotProduct_b8_write(B[8]); 
			dotProduct_b9_write(B[9]);
			
			dotProduct_bias_write(bias_3_p[i]);
			
			// Inicia cálculo no hardware
			dotProduct_start_write(1);
			// Espera o hardware terminar
			while (dotProduct_busy_read());
			
			result = dotProduct_result_read();
		    	
			cycles = dotProduct_cycles_read();
			t_dense3 += cycles;
		    	
		    	y[i] = result >> 15;
		}
		
		times_dense3[n] = t_dense3;
		t_dense3 = 0;
		
		int max_i = 0;
		for (int f=0; f<3;f++){
			if(y[f] > y[max_i]){
				max_i = f;	
			}
		}
	}
	
	printf("--------------------PRUNED RESULTS-----------------------\n");
	printf("------------QUANTIDADE DE CICLOS POR CAMADA--------------\n");
	printf("DENSE 1:\n");
	
	for(int o = 0; o < 1; o++){
		printf("%lu, ", times_dense1[o]);
	}
	
	printf("\n\n");
	
	printf("DENSE 2:\n");
	for(int o = 0; o < 1; o++){
		printf("%lu, ", times_dense2[o]);
	}
	printf("\n\n");

	printf("DENSE 3:\n");
	for(int o = 0; o < 1; o++){
		printf("%lu, ", times_dense3[o]);
	}
	printf("\n\n");
}


static void console_service(void)
{
	char *str;
	char *token;

	str = readstr();
	if(str == NULL) return;
	token = get_token(&str);
	if(strcmp(token, "help") == 0)
		help();
	else if(strcmp(token, "reboot") == 0)
		reboot();
	else if(strcmp(token, "led") == 0)
		toggle_led();
	else if(strcmp(token, "dot") == 0)
        	calc_dot_product();
        else if(strcmp(token, "prune") == 0)
        	calc_prune();
       
	prompt();
	
}



int main(void)
{
	// Desliga o timer
	timer0_en_write(0);

	// Carrega valor máximo
	timer0_load_write(0xFFFFFFFF);

	// Desabilita reload
	timer0_reload_write(0);

	// Atualiza valor
	timer0_update_value_write(1);

	// Liga o timer
	timer0_en_write(1);
#ifdef CONFIG_CPU_HAS_INTERRUPT
	irq_setmask(0);
	irq_setie(1);
#endif
	uart_init();

	puts("\nLab004 - CPU testing software built "__DATE__" "__TIME__"\n");
    	printf("Hellorld!\n");
	help();
	prompt();

	while(1) {
		console_service();
	}

	return 0;
}

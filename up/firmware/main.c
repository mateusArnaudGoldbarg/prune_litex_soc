#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "model.h"
#include "data.h"
#include <inttypes.h>
#include <stdint.h>
#include <unistd.h>

#include <irq.h>
#include <uart.h>
#include <console.h>
#include <generated/csr.h>

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
	puts("dot                             - dot product");
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
/*
static long calc_dot_product_SW(int A[], int B[], int s){
	long result = 0;
	for(int i = 0; i < s; i++){
		result += A[i]*B[i];
	}
	return result;
} 
*/

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


static void calc_dot_product(void)
{
	int32_t y[10] = {0,0,0,0,0,0,0,0,0,0};
	uint64_t result;
	uint32_t result_invertido;
	
	
	// dense_1
	dotProduct_a0_write(input_data[0][0]); 
	dotProduct_a1_write(input_data[0][1]);
	dotProduct_a2_write(input_data[0][2]); 
	dotProduct_a3_write(input_data[0][3]);
	dotProduct_a4_write(0); 
	dotProduct_a5_write(0);
	dotProduct_a6_write(0); 
	dotProduct_a7_write(0);
	dotProduct_a8_write(0); 
	dotProduct_a9_write(0);
	
	for(int i=0;i<10;i++){
		dotProduct_b0_write(dense_1[0][i]); 
		dotProduct_b1_write(dense_1[1][i]);
		dotProduct_b2_write(dense_1[2][i]); 
		dotProduct_b3_write(dense_1[3][i]);
		dotProduct_b4_write(dense_1[3][i]); 
		dotProduct_b5_write(dense_1[3][i]);
		dotProduct_b6_write(dense_1[3][i]); 
		dotProduct_b7_write(dense_1[3][i]);
		dotProduct_b8_write(dense_1[3][i]); 
		dotProduct_b9_write(dense_1[3][i]);
		
		dotProduct_bias_write(bias_1[i]);
		
		//printf("Iniciando calculos...\n");
		// Inicia cálculo no hardware
		dotProduct_start_write(1);

		// Espera o hardware terminar
		while (dotProduct_busy_read());

		//printf("Finalizado\n");
		
		result = dotProduct_result_read();
		
		//uint32_t high = (uint32_t)(result >> 32);
	    	//uint32_t low  = (uint32_t)result;
	    	//uint32_t invertido_low = 0;
	    	//uint32_t invertido_high = 0;
	    	
	    	//invertido_high = ~high + 1;
	    	
	    	//if(invertido_high>0)
	    	//	invertido_low = ~low + 1;
	    	
	    	//invertido_low = invertido_low >> 15;
	    	
	    	result_invertido = (~result +1)>>15;
	    	
	    	
	    	y[i] = (signed long)(~(result_invertido));
	    	
	    	
	    	//printf("result em complemento de dois = %ld\n", y[i]);
	
	}
	//relu_1
	for(int j=0; j<10;j++){
		if (y[j] <= 0){
			y[j] = 0;
		}
	}
	
	for(int j=0; j<10;j++){
		printf("%ld ", y[j]);
	}
	printf("\n");
	
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
	prompt();
}



int main(void)
{
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

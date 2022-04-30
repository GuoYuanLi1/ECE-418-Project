#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>


const int primes[64] = {2, 3, 5, 7, 11, 13, 17, 19, 
						23, 29, 31, 37, 41, 43, 47, 53, 
						59, 61, 67, 71, 73, 79, 83, 89, 
						97, 101, 103, 107, 109, 113, 127, 131, 
						137, 139, 149, 151, 157, 163, 167, 173, 
						179, 181, 191, 193, 197, 199, 211, 223, 
						227, 229, 233, 239, 241, 251, 257, 263, 
						269, 271, 277, 281, 283, 293, 307, 311
					}; // first 64 prime numbers


int MAX_BLOCKS = 100; // maximum 521-bit blocks allowed
int blocks_need = 0; // num of 512-bit blocks needed
uint32_t words[100][64]; // maximum 64 32-bit words blocks allowed
uint32_t hash[8]; // 8 32-bit hash values
uint32_t k[64]; // 64 32-bit round constants
char digest[64]; // final digest


void step1(char * message);
void step2();
void step3();
void step4();
void step5();
void step6();
uint32_t rot(uint32_t word, uint8_t num);

int main() {
	// Input message
	char * message = "harry zhu";

	// Expected result for testbench
	char expected[64] = "756eb2f60fc4434e202326a4bbb1925d24c40b421028038bdccde8de1ff1ed44";
	
	// Set return value for output compaison
	uint8_t returnVal = 1;

	// Set retrive index for the first mismatched output
	uint8_t index = -1;

	// Execute the SHA_256 algorithm
	step1(message);
	step2();
	step3();
	step4();
	step5();
	step6();

	// Check the output digest array against expected array
	int i;
	for(i=0; i<64; i++) {
		if(expected[i] != digest[i]) {
			returnVal = 0;
			index = i;
			break;
		}
	}

	// Print Results
 	if(returnVal == 0){
 		printf("*** *** *** *** \n");
 		printf("Mismatch! Occurred at index: %d \n", index);
 		printf("*** *** *** *** \n");
 	} else {
 		printf("*** *** *** *** \n");
 		printf("Final digest is good!\n");
 		printf("*** *** *** *** \n");
 	}
 
 	// Return 0 if outputs are correct
 	return returnVal;
	   
}


void step1(char * message) {
	// Find size of message
	uint64_t size = strlen(message);
	
	// Calculate number of 512-bit blocks needed
	blocks_need = ceil(1.0 * size / 512);

	// Check if number of 512-bit blocks needed exceeds maximum blocks allowed
	if(blocks_need > MAX_BLOCKS) blocks_need = MAX_BLOCKS;

	// Initialize constants
	int i;
	uint8_t j, k;
	uint8_t append = 128; // Append a 1 at the end of the message bits
	int count = 0;
	uint64_t length = size * 8;
	
	

    for(i=0; count<=size && i<blocks_need; i++) {
    	for(j=0; count<=size && j<16; j++) {
    		for(k=0; k<4; k++) {
    			if(count == size) {
    				// Append the 1 after the end of message
    				words[i][j] += append << (8 * (3-k));

    				// Add length to the last 2 32-bit words
    				words[i][14] = length >> 32; // most significant 32 bits
    				words[i][15] = (uint32_t)length; // least significant 32 bits

    				count++;
    				break;
    			}else{
    				words[i][j] += (uint8_t)message[count] << (8 * (3-k));
    			}
    			count++;
    		}
    	}
    }

}


void step2() {
	double bFractional = 0.0; 
	double dFractional;
	double fraFactor = 0.1;
   	uint8_t temp = 0;
   	int i,j;

   	for(i=0; i<8; i++) {
   		dFractional = sqrt(primes[i]) - (int)sqrt(primes[i]);

   		for(j=0;j<32;j++) {
      		dFractional = dFractional * 2;
      		temp = dFractional;
      		bFractional = bFractional + fraFactor * temp;
      		if(temp ==1) {
      			dFractional = dFractional - temp;
      			hash[i] += pow(2,31-j);
      		}
     		fraFactor = fraFactor / 10;
   		}
   		
   	}  
	
}

void step3() {
	double bFractional = 0.0; 
	double dFractional;
	double fraFactor = 0.1;
   	uint8_t temp;
   	int i,j;

   	for(i=0; i<64; i++) {
  
   		dFractional = cbrt(primes[i]) - (int)cbrt(primes[i]);

   		for(j=0;j<32;j++) {
      		dFractional = dFractional * 2;
      		temp = dFractional;
      		bFractional = bFractional + fraFactor * temp;
      		if(temp ==1) {
      			dFractional = dFractional - temp;
      			k[i] += pow(2, 31-j);
      		}
     		fraFactor = fraFactor / 10;
   		}
   	} 


}


void step4() {
	uint32_t S0, S1;

	int i;
	uint8_t j;

	for(i=0; i<blocks_need; i++) {
		for(j=16; j<64; j++) {
			S0 = rot(words[i][j-15],7) ^ rot(words[i][j-15],18) ^ (words[i][j-15] >> 3);
			S1 = rot(words[i][j-2],17) ^ rot(words[i][j-2],19) ^ (words[i][j-2] >> 10);
			words[i][j] = words[i][j-16] + S0 + words[i][j-7] + S1;
		}
	}
	
}

void step5() {

	// Initialize a-h
	uint32_t a = hash[0];
	uint32_t b = hash[1];
	uint32_t c = hash[2];
	uint32_t d = hash[3];
	uint32_t e = hash[4];
	uint32_t f = hash[5];
	uint32_t g = hash[6];
	uint32_t h = hash[7];
	
	int i;
	uint8_t j;
	uint32_t S1, S0, temp1, temp2, ch, maj;

   	for(i=0; i<blocks_need; i++) {
		for(j=0; j<64; j++) {
			
			// Compression Loop
			S1 = rot(e,6) ^ rot(e,11) ^ rot(e,25);
			ch = (e & f) ^ ((~e) & g);
			temp1 = h + S1 + ch + k[j] + words[i][j]; 
			S0 = rot(a,2) ^ rot(a,13) ^ rot(a,22); 
			maj = (a & b) ^ (a & c) ^ (b & c) ;
			temp2 = (S0 + maj);
			h = g; 
			g = f;
			f = e;
			e = (d + temp1);
			d = c;
			c = b;
			b = a;
			a = (temp1 + temp2);
			
		}
	}

	// Update hash values
	hash[0] += a;
	hash[1] += b;
	hash[2] += c;
	hash[3] += d;
	hash[4] += e;
	hash[5] += f;
	hash[6] += g;
	hash[7] += h;

}


void step6() {
	
	// Initialize variables
	uint8_t k, i, j, temp;
	uint32_t quotient;

	
	for(i=0; i<8; i++) {
		quotient = hash[i];
		j = 7 + 8 * i;
		for(k=0; k<8; k++) {
			temp = quotient % 16;
			if( temp < 10) temp += 48; 
			else temp += 87;
			digest[j]= (char)temp;
			quotient = quotient / 16;
			j--;
		}

	}

}


uint32_t rot(uint32_t word, uint8_t num) {
	return (word >> num)|(word << (32 - num));
}









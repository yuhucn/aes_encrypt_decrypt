#include <iostream>
#include <vector>
#include <fstream>
#include <string.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <iomanip>
#include <bitset>
using namespace std;

//General Functions
string read_file(const char* file_name) {
	string str = "";
	string ret = "";
	ifstream the_file (file_name);

	if (the_file.is_open()) {
		while (getline(the_file,str)){
			ret = ret + str;
		}
	}
	the_file.close();
	return ret;
}

int get_int(string h){
	return (int)strtol(h.c_str(), 0, 16);
}

void copy_array(unsigned char to_copy[4][4], unsigned char copy_here[4][4]) {
	for(int i = 0; i < 4; i++) {
		for(int j = 0; j < 4; j++) {
			copy_here[i][j] = to_copy[i][j];
		}
	}
}

void xor_array(unsigned char first[4][4], unsigned char sec[4][4], unsigned char to_here[4][4]) {
	for(int i = 0; i < 4; i++) {
			for(int j = 0; j < 4; j++) {
				to_here[i][j] = first[i][j] ^ sec[i][j];
		}
	}
}

//Printing Functions
void print_box_base(unsigned char box[16][16], int base) {

	if(base == 10){ cout << "\n\nPrint Sbox in decimal\n"; }
	if(base == 16){ cout << "\n\nPrint Sbox in hexadecimal\n"; }
	if(base == 2){ cout << "\n\nPrint Sbox in binary\n"; }

	cout << "   ";
	for(int i = 0; i < 16; i++) {
		if(base == 10){ cout << setfill(' ') << setw(3) << hex << i << " "; }
		if(base == 16){ cout << setfill(' ') << setw(2) << hex << i << " "; }
		if(base == 2){ cout << setfill(' ') << setw(8) << hex << i << " "; }
	}
	cout << endl;
	for(int i = 0; i < 16; i++) {
		cout << hex << i << ": ";
		for(int j = 0; j < 16; j++){
			if(base == 10){ cout << setw(3) << dec << (int)box[i][j] << " "; }
			if(base == 16){ cout << setfill('0') << setw(2) << hex << (int)box[i][j] << " "; }
			if(base == 2){ cout << setfill('0') << setw(2) << (bitset<8>)(int)box[i][j] << " ";}
		}
	cout << endl;
	}
	cout << endl << endl;
}

void print_state_base(unsigned char box[4][4], int base) {
	cout << "   ";
	for(int i = 0; i < 4; i++) {
		if(base == 10){ cout << setfill(' ') << setw(3) << hex << i << " "; }
		if(base == 16){ cout << setfill(' ') << setw(2) << hex << i << " "; }
		if(base == 2){ cout << setfill(' ') << setw(8) << hex << i << " "; }
	}
	cout << endl;
	for(int i = 0; i < 4; i++) {
		cout << hex << i << ": ";
		for(int j = 0; j < 4; j++){
			if(base == 10){ cout << setw(3) << dec << (int)box[i][j] << " "; }
			if(base == 16){ cout << setfill('0') << setw(2) << hex << (int)box[i][j] << " "; }
			if(base == 2){ cout << setfill('0') << setw(2) << (bitset<8>)(int)box[i][j] << " ";}
		}
	cout << endl;
	}
	cout << endl << endl;
}

void print_round_keys(unsigned int key[44]) {
	for(int i = 0; i < 44; i++) {
		cout << setfill('0') << setw(8) << hex << key[i];
		if(i != 43){
			cout << ", ";
		}
		if((i+1) % 4 == 0 && i != 0) {
			cout << endl;
		}
	}
	cout << endl << endl;
}

void print_for_output(unsigned char to_out[4][4]){
	for(int i = 0; i < 4; i++) {
		for(int j = 0; j < 4; j++) {
			cout << setfill('0') << setw(2) << hex << (int)to_out[j][i] << "  ";
		}
		cout << "     ";
	}
	cout << endl << endl;
}


/////////////////////////////////////////////////////////////////
//	INITIALIZE SBOX - create the sbox using a text file
/////////////////////////////////////////////////////////////////
void init_sboxes(unsigned char box[16][16], const char* file_name) {
	string str = read_file(file_name);
	string hexa = "";
	char* arr;
	int sbox_el;
	int col = 0;
	int row = 0;
	arr = strtok(&str[0]," \n");

	while (arr != NULL) {
		hexa = "0x" + (string)arr;
		sbox_el = get_int(hexa);

		// for debugging - print hex and decimal
		//cout << hexa << ": " << setw(3) << sbox_el << "  ";
		box[row][col] = (unsigned char)sbox_el;
		if( col == 15 ){
			row += 1;
			col = -1;
			//cout << endl;
		}
		col += 1;
		arr = strtok(NULL," \n");
	}
}

/////////////////////////////////////////////////////////////////
//	INITIALIZE INPUT - create the plaintext using a text file
/////////////////////////////////////////////////////////////////
void init_state(unsigned char box[4][4], const char* file_name) {
	string str = read_file(file_name);
	string hexa = "";
	char* arr;
	int sbox_el;
	int col = 0;
	int row = 0;
	arr = strtok(&str[0]," \n");

	while (arr != NULL) {
		hexa = "0x" + (string)arr;
		sbox_el = get_int(hexa);

		// for debugging - print hex and decimal
		//cout << hexa << ": " << setw(3) << sbox_el << "  ";
		box[row][col] = (unsigned char)sbox_el;
		if( row == 3 ){
			col += 1;
			row = -1;
			//cout << endl;
		}
		row += 1;
		arr = strtok(NULL," \n");
	}
}


/////////////////////////////////////////////////////////////////
//	SUBSTITUTE BYTES - replaces byte elements with sbox element
/////////////////////////////////////////////////////////////////
void sub_bytes(unsigned char state[4][4], unsigned char sbox[16][16]) {
	int row;
	int col;

	for(int i = 0; i < 4; i++) {
		for(int j = 0; j < 4; j++) {
			//get row and column reference in the sbox
			row = (state[i][j] & 0xf0) >> 4;
			col = state[i][j] & 0x0f;
			//get sbox equivalent
			state[i][j] = sbox[row][col];
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//	SHIFT ROWS - shifts rows to the left (second row by 1, third rwo by 2, fourth row by 3)
/////////////////////////////////////////////////////////////////////////////////////////////////////
void shift_rows(unsigned char state[4][4]) {
	//first row = the same
	//second row = shift by 1 to left
	unsigned char one;
	unsigned char two;
	unsigned char three;

	one = state[1][0];
	for(int i = 0; i < 3; i++) {
		state[1][i] = state[1][i+1];
	}
	state[1][3] = one;

	one = state[2][0];
	two = state[2][1];
	for(int i = 0; i < 2; i++) {
		state[2][i] = state[2][i+2];
	}
	state[2][2] = one;
	state[2][3] = two;

	one = state[3][0];
	two = state[3][1];
	three = state[3][2];
	state[3][0] = state[3][3];
	state[3][1] = one;
	state[3][2] = two;
	state[3][3] = three;
}

/////////////////////////////////////////////////////////////////
//	GET WORDS - transform a key column to a 32 bit word
/////////////////////////////////////////////////////////////////
void get_words(unsigned char cipher[4][4], unsigned int words[4]) {
	unsigned int word = 0;

	for(int i = 0; i < 4; i++) {
		word = 0;
		for(int j = 0; j < 4; j++) {
			word = word + (unsigned int)cipher[j][i];
			if(j < 3) {
				word = word << 8;
			}
		}
		words[i] = word;
	}
}

//////////////////////////////////////////////////////////////////
//	ROTATE WORD - rotates a word 8 bits to the left
//////////////////////////////////////////////////////////////////
unsigned int rotate_word(unsigned int word) {
	unsigned int first = word >> 24;
	unsigned int rest = word << 8;
	unsigned int result = first + rest;
	return result;
}

///////////////////////////////////////////////////////////////////////////////
//	SUBSTITUTE WORD - substitute consecutive 8 bits with the sbox lookup table
///////////////////////////////////////////////////////////////////////////////
unsigned int sub_word(unsigned int word, unsigned char sbox[16][16]) {
	unsigned int ret_word = 0;	//substituted word to return
	unsigned int temp;
	unsigned char temp2;
	int row;					//row number
	int col;					//column number

	for(int i = 0; i < 4; i++) {
		//get consecutive 8 bits
		temp = word << (i*8);
		temp = temp >> 24;
		temp2 = (unsigned char)temp;
		//get row and column number in the sbox
		row = (temp2 & 0xf0) >> 4;
		col = temp2 & 0x0f;
		//get the sbox equivalent
		temp2 = sbox[row][col];
		//record the equivalent to return
		ret_word = ret_word + temp2;
		if(i != 3){
			ret_word = ret_word << 8;
		}
	}

	return ret_word;
}

////////////////////////////////////////////////////////////////////////
//	GENERATE ROUND KEY - generates all round key given a ciphertext
////////////////////////////////////////////////////////////////////////
void gen_round_key(unsigned char cipher[4][4], unsigned char sbox[16][16], unsigned int round_keys[43]) {
	unsigned int words[4];	//32 bit word in the key
	unsigned int temp;

	//round constant word array
	unsigned int rcon[10] = {0x01000000, 0x02000000, 0x04000000, 0x08000000, 0x10000000,
							 0x20000000, 0x40000000, 0x80000000, 0x1b000000, 0x36000000};
	//parse the key for the 32 bit words
	get_words(cipher,words);
	//copy words to roundkey array
	for(int i = 0; i < 4; i++) {
		round_keys[i] = words[i];
	}
	//the expansion routine as specified in the FIPS document
	for(int i = 4; i < 44; i++) {
		temp = round_keys[i-1];
		if((i % 4) == 0) {
			temp = sub_word(rotate_word(temp), sbox) ^ rcon[i/4-1];
		}
		round_keys[i] = (round_keys[i-4] ^ temp);
	}
}

////////////////////////////////////////////////////////////
//	ADD ROUND KEY - XOR the round key to the state
////////////////////////////////////////////////////////////
void add_round_key(unsigned char state[4][4], unsigned int round_keys[43], int round) {
	unsigned int r_key;		//the round key to XOR

	for(int i = 0; i < 4; i++) {
		for(int j = 0; j < 4; j++) {
			r_key = round_keys[(round*4) + i];
			r_key = r_key << (j*8);
			r_key = r_key >> 24;
			r_key = (unsigned char)r_key;
			state[j][i] = state[j][i] ^ r_key;
		}
	}
}

/////////////////////////////////////////////////////////////
//	MIX COLUMN (similar to NIST FIPS algorithm)
/////////////////////////////////////////////////////////////
void mix_columns(unsigned char state[4][4]) {
	unsigned int rows[4];	//row elements of the array column being worked on
	unsigned int temp;

	for(int i = 0; i < 4; i++) {
		rows[0] = (int)state[0][i];
		rows[1] = (int)state[1][i];
		rows[2] = (int)state[2][i];
		rows[3] = (int)state[3][i];
		for(int j = 0; j < 4; j++) {
			//x
			temp = rows[(0+j)%4] << 1;
			//x+1
			temp = temp ^ (rows[(1+j)%4] << 1);
			temp = temp ^ rows[(1+j)%4];
			//1
			temp = temp ^ rows[(2+j)%4];
			//1
			temp = temp ^ rows[(3+j)%4];
			if(temp >= 256){
				temp = temp ^ 0x1b;
			}
			state[j][i] = temp;
		}
	}
}

////////////////////////////////////////////////////////////////////////
//	AES Encryption Routine (similar to the NIST FIPS)
////////////////////////////////////////////////////////////////////////
void aes_encryption(unsigned char in[4][4], unsigned char state[4][4],
		unsigned int round_keys[44], unsigned char sbox[16][16]) {

	cout << "ENCRYPTION PROCESS" << endl << "------------------" << endl;

	copy_array(in,state);
	cout << "Plain Text:" << endl;
	print_for_output(state);

	cout << "Round 1" << endl << "---------" << endl;
	add_round_key(state, round_keys, 0);
	print_for_output(state);

	for(int i = 1; i < 10; i++) {
		sub_bytes(state, sbox);
		shift_rows(state);
		mix_columns(state);
		add_round_key(state, round_keys, i);
		if(i != 9) {
			cout << "Round " << i+1 << endl << "---------" << endl;
		} else {
			cout << "Last Round" << endl << "---------" << endl;
		}
		print_for_output(state);
	}

	sub_bytes(state, sbox);
	shift_rows(state);
	add_round_key(state, round_keys, 10);
	cout << "CipherText:" << endl << "---------" << endl;
	print_for_output(state);
	cout << endl;
}

////////////////////////////////////////////////////////////////////////////////////
//	INVERSE SUB BYTES - replaces all elements using the inverse sbox lookup table
////////////////////////////////////////////////////////////////////////////////////
void inv_sub_bytes(unsigned char state[4][4], unsigned char inv_sbox[16][16]) {
	sub_bytes(state, inv_sbox);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//	INVERSE SHIFT ROWS - shifts rows to the right (second row by 1, third rwo by 2, fourth row by 3)
/////////////////////////////////////////////////////////////////////////////////////////////////////
void inv_shift_rows(unsigned char state[4][4]) {
	//first row = the same
	//second row = shift by 1 to left
	unsigned char one;	//second row element
	unsigned char two;	//third row element
	unsigned char three;//fourth row element

	three = state[1][3];
	for(int i = 3; i > 0; i--) {
		state[1][i] = state[1][i-1];
	}
	state[1][0] = three;

	two = state[2][2];
	three = state[2][3];
	for(int i = 3; i > 1; i--) {
		state[2][i] = state[2][i-2];
	}
	state[2][0] = two;
	state[2][1] = three;

	one = state[3][1];
	two = state[3][2];
	three = state[3][3];
	state[3][3] = state[3][0];
	state[3][2] = three;
	state[3][1] = two;
	state[3][0] = one;
}

/////////////////////////////////////////////////////////////
//	INVERSE MIX COLUMN (similar to NIST FIPS algorithm)
/////////////////////////////////////////////////////////////
void inv_mix_columns(unsigned char state[4][4]) {
	unsigned int temp;
	unsigned int first, sec, third, last;	//= a^-1(x) X row elements
	unsigned int to_ret[4][4];				//= XOR of all elements

	for(int i = 0; i < 4; i++) {
		//cout  << endl << (int)rows[0] << ", " << (int)rows[1] << ", " << (int)rows[2] << ", " << (int)rows[3];
		for(int j = 0; j < 4; j++) {
			//0x0e
			first = (state[(0+j)%4][i] << 1) ^ (state[(0+j)%4][i] << 2) ^ (state[(0+j)%4][i] << 3);
			//0x0b
			sec = (state[(1+j)%4][i] << 0) ^ (state[(1+j)%4][i] << 1) ^ (state[(1+j)%4][i] << 3);
			//0x0d
			third = (state[(2+j)%4][i] << 0) ^ (state[(2+j)%4][i] << 2) ^ (state[(2+j)%4][i] << 3);
			//0x09
			last = (state[(3+j)%4][i] << 0) ^ (state[(3+j)%4][i] << 3);
			//XOR all rows
			temp = first ^ sec ^ third ^ last;
			//remove the 9th, 10th, and 11bits then add XOR with irreducible polynomial
			if(temp >= 1024){
				temp = temp & 0x3ff;
				temp = temp ^ 0x6c;
			}
			if(temp >= 512){
				temp = temp & 0x1ff;
				temp = temp ^ 0x36;
			}
			if(temp >= 256){
				temp = temp & 0xff;
				temp = temp ^ 0x1b;
			}
			//record the result
			to_ret[j][i] = temp;
		}
	}
	//copy mixed column result to variable state
	for(int i = 0; i < 4; i++) {
		for(int j = 0; j < 4; j++) {
			state[i][j] = to_ret[i][j];
		}
	}
}

////////////////////////////////////////////////////////////////////////
//	AES Decryption Routine (similar to the NIST FIPS)
////////////////////////////////////////////////////////////////////////
void aes_decryption(unsigned char cipher[4][4], unsigned char state[4][4],
		unsigned int round_keys[44], unsigned char inv_sbox[16][16]) {

	cout << "DECRYPTION PROCESS" << endl << "------------------" << endl;
	//state = in
	copy_array(cipher, state);
	cout << "CipherText:" << endl;
	print_for_output(state);

	//AddRoundkey(state, w[0,Nb-1])
	add_round_key(state, round_keys, 10);
	//print_for_output(state);

	for(int i = 9; i > 0; i--) {
		inv_shift_rows(state);
		//print_for_output(state);
		inv_sub_bytes(state, inv_sbox);
		//print_for_output(state);
		cout << "Round " << i << endl << "---------" << endl;
		print_for_output(state);
		add_round_key(state, round_keys, i);
		//print_for_output(state);
		inv_mix_columns(state);
		//print_for_output(state);
	}

	inv_shift_rows(state);
	inv_sub_bytes(state, inv_sbox);

	cout << "Round " << 0 << endl << "---------" << endl;
	print_for_output(state);

	add_round_key(state, round_keys, 0);
	cout << "CipherText:" << endl << "---------" << endl;
	print_for_output(state);
	cout << endl;
}

int main() {
	unsigned int round_keys[44];	//list of round key generated from the cipher
	unsigned char sbox[16][16];		//the encryption sbox
	unsigned char inv_sbox[16][16];	//the decryption sbox
	unsigned char in[4][4];			//the plaintext in an array
	unsigned char key[4][4];		//the key in an array
	const char* plaintext;			//file name of plaintext
	const char* keyfile;			//file name of key

	unsigned char e_state[4][4];
	unsigned char d_state[4][4];

	//initiate sbox and inverse sbox
	init_sboxes(sbox, "aes_sbox.txt");
	init_sboxes(inv_sbox, "aes_inv_sbox.txt");

	//test script block for Test 1
	plaintext = "test1plaintext.txt";
	keyfile = "test1key.txt";
	//get plaintext and cipherkey
	cout << "Plaintext Filename: " << plaintext << endl;
	init_state(in, plaintext);
	cout << "Key Filename: " << keyfile << endl << endl;
	init_state(key, keyfile);
	//generates the round keys
	gen_round_key(key, sbox, round_keys);
	cout << "Key Schedule:" << endl;
	print_round_keys(round_keys);
	//encryption and decryption
	aes_encryption(in, e_state, round_keys, sbox);
	aes_decryption(e_state, d_state, round_keys, inv_sbox);

	//test script block for Test 2
	plaintext = "test2plaintext.txt";
	keyfile = "test2key.txt";
	//get plaintext and cipherkey
	cout << "Plaintext Filename: " << plaintext << endl;
	init_state(in, plaintext);
	cout << "Key Filename: " << keyfile << endl << endl;
	init_state(key, keyfile);
	//generates the round keys
	gen_round_key(key, sbox, round_keys);
	cout << "Key Schedule:" << endl;
	print_round_keys(round_keys);
	//encryption and decryption
	aes_encryption(in, e_state, round_keys, sbox);
	aes_decryption(e_state, d_state, round_keys, inv_sbox);

	//test script block for Test 3
	plaintext = "test3plaintext.txt";
	keyfile = "test3key.txt";
	//get plaintext and cipherkey
	cout << "Plaintext Filename: " << plaintext << endl;
	init_state(in, plaintext);
	cout << "Key Filename: " << keyfile << endl << endl;
	init_state(key, keyfile);
	//generates the round keys
	gen_round_key(key, sbox, round_keys);
	cout << "Key Schedule:" << endl;
	print_round_keys(round_keys);
	//encryption and decryption
	aes_encryption(in, e_state, round_keys, sbox);
	aes_decryption(e_state, d_state, round_keys, inv_sbox);

	cout << "End of Processing";

	return 0;
}

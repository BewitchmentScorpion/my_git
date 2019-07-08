#include <bits/stdc++.h>
using namespace std;
typedef unsigned int ui;
const int LEN = 1e6 + 5;

char s[105];

struct flow_register {
	string type;
	int opt, npc, rs1, rs2, imm, rd, ALU, shamt;
	flow_register() {
		type = "";
		opt = npc = rs1 = rs2 = imm = rd = ALU = shamt = 0;	
	}
} FR[4];

int pw[55], pc, x[55], bin[55][55], dfn;
uint8_t mem[LEN];
map <char, int> HEX;
map <string, int> fmt;

void init() {
	HEX['0'] = 0;
	HEX['1'] = 1;
	HEX['2'] = 2;
	HEX['3'] = 3;
	HEX['4'] = 4;
	HEX['5'] = 5;
	HEX['6'] = 6;
	HEX['7'] = 7;
	HEX['8'] = 8;
	HEX['9'] = 9;
	HEX['A'] = 10;
	HEX['B'] = 11;
	HEX['C'] = 12;
	HEX['D'] = 13;
	HEX['E'] = 14;
	HEX['F'] = 15;
	pw[0] = 1;
	for (int i = 1; i < 8; i++) {
		pw[i] = pw[i - 1] * 16;
	}
	for (int i = 0; i < 32; i++) {
		for (int j = 0; j < 32; j++) {
			bin[i][j] = 0;
			for (int k = i; k <= j; k++) {
				bin[i][j] |= 1 << k;
			}
		}
	}
	int id = 0;
	fmt["LUI"] = id++;
	fmt["AUIPC"] = id++;
	fmt["JAL"] = id++;
	fmt["JALR"] = id++;
	fmt["BEQ"] = id++;
	fmt["BNE"] = id++;
	fmt["BLT"] = id++;
	fmt["BGE"] = id++;
	fmt["BLTU"] = id++;
	fmt["BGEU"] = id++;
	fmt["LB"] = id++;
	fmt["LH"] = id++;
	fmt["LW"] = id++;
	fmt["LBU"] = id++;
	fmt["LHU"] = id++;
	fmt["SB"] = id++;
	fmt["SH"] = id++;
	fmt["SW"] = id++;
	fmt["ADDI"] = id++;
	fmt["SLTI"] = id++;
	fmt["SLTIU"] = id++;
	fmt["XORI"] = id++;
	fmt["ORI"] = id++;
	fmt["ANDI"] = id++;
	fmt["SLLI"] = id++;
	fmt["SRLI"] = id++;
	fmt["SRAI"] = id++;
	fmt["ADD"] = id++;
	fmt["SUB"] = id++;
	fmt["SLL"] = id++;
	fmt["SLT"] = id++;
	fmt["SLTU"] = id++;
	fmt["XOR"] = id++;
	fmt["SRL"] = id++;
	fmt["SRA"] = id++;
	fmt["OR"] = id++;
	fmt["AND"] = id++;
}
int get_bit(int x, int l, int r) {
	return (x & bin[l][r]) >> l;
}
int get_imm(int opt, char c) {
	int imm = 0;
	if (c == 'I') {
		imm = get_bit(opt, 20, 30);
		if (opt & (1 << 31)) {
			imm |= bin[11][31];
		}
	} else if (c == 'S') {
		imm = get_bit(opt, 7, 7);
		imm |= get_bit(opt, 8, 11) << 1;
		imm |= get_bit(opt, 25, 30) << 5;
		if (opt & (1 << 31)) {
			imm |= bin[11][31];
		}
	} else if (c == 'B') {
		imm = get_bit(opt, 8, 11) << 1;
		imm |= get_bit(opt, 25, 30) << 5;
		imm |= get_bit(opt, 7, 7) << 11;
		if (opt & (1 << 31)) {
			imm |= bin[12][31];
		}
	} else if (c == 'U') {
		imm = get_bit(opt, 12, 31) << 12;
	} else if (c == 'J') {
		imm = get_bit(opt, 21, 24) << 1;
		imm |= get_bit(opt, 25, 30) << 5;
		imm |= get_bit(opt, 20, 20) << 11;
		imm |= get_bit(opt, 12, 19) << 12;
		if (opt & (1 << 31)) {
			imm |= bin[20][31];
		}
	}
	return imm;
}
void decode() {
	FR[1] = FR[0];
	int opt = FR[1].opt;
	string type;
	int opcode = 0, rs1 = 0, rs2 = 0, rd = -1, imm = 0, funct3 = 0, funct7 = 0;
	int shamt = get_bit(opt, 20, 24);
	opcode = get_bit(opt, 0, 6);
	if (opcode == 3) {
		funct3 = get_bit(opt, 12, 14);
		rd = get_bit(opt, 7, 11);
		rs1 = get_bit(opt, 15, 19);
		imm = get_imm(opt, 'I');
		switch (funct3) {
			case 0: //LB
				type = "LB";
				break;
			case 1: //LH
				type = "LH";
				break;
			case 2: //LW
				type = "LW";
				break;
			case 4: //LBU
				type = "LBU";
				break;
			case 5: //LHU
				type = "LHU";
				break;
				
		}
	} else if (opcode == 35) {
		rs1 = get_bit(opt, 15, 19);
		rs2 = get_bit(opt, 20, 24);
		imm = get_imm(opt, 'S');
		funct3 = get_bit(opt, 12, 14);
		switch (funct3) {
			case 0: //SB
				type = "SB";
				break;
			case 1: //SH
				type = "SH";
				break;
			case 2: //SW
				type = "SW";
				break;
		}
	} else if (opcode == 55) { //LUI
		rd = get_bit(opt, 7, 11);
		imm = get_imm(opt, 'U');
		type = "LUI";
	} else if (opcode == 23) { //AUIPC
		rd = get_bit(opt, 7, 11);
		imm = get_imm(opt, 'U');
		type = "AUIPC";
	} else if (opcode == 19) {
		funct3 = get_bit(opt, 12, 14);
		funct7 = get_bit(opt, 25, 31);
		rd = get_bit(opt, 7, 11);
		rs1 = get_bit(opt, 15, 19);
		imm = get_imm(opt, 'I');
		switch (funct3) {
			case 0: //ADDI
				type = "ADDI";
				break;
			case 2: //SLTI
				type = "SLTI";
				break;
			case 3: //SLTIU
				type = "SLTIU";
				break;
			case 4: //XORI
				type = "XORI";
				break;
			case 6: //ORI
				type = "ORI";
				break;
			case 7: //ANDI
				type = "ANDI";
				break;
			case 1: //SLLI
				type = "SLLI";
				break;
			case 5:
				if (funct7 == 0) { //SRLI
					type = "SRLI";
				} else { //SRAI
					type = "SRAI";
				}
				break;
		}
	} else if (opcode == 51) {
		rs1 = get_bit(opt, 15, 19);
		rs2 = get_bit(opt, 20, 24);
		rd = get_bit(opt, 7, 11);
		funct3 = get_bit(opt, 12, 14);
		funct7 = get_bit(opt, 25, 31);
		switch (funct3) {
			case 0: 
				if (funct7 == 0) { //ADD
					type = "ADD";
				} else { //SUB
					type = "SUB";
				}
				break;
			case 1: //SLL
				type = "SLL";
				break;
			case 2: //SLT
				type = "SLT";
				break;
			case 3: //SLTU
				type = "SLTU";
				break;
			case 4: //XOR
				type = "XOR";
				break;
			case 5: 
				if (funct7 == 0) { //SRL
					type = "SRL";
				} else { //SRA
					type = "SRA";
				}
				break;
			case 6: //OR
				type = "OR";
				break;
			case 7: //AND
				type = "AND";
				break;
		}
	} else if (opcode == 111) { //JAL
		rd = get_bit(opt, 7, 11);
		imm = get_imm(opt, 'J');
		FR[1].ALU = FR[0].npc;
		pc += imm - 4;
		type = "JAL";
	} else if (opcode == 103) { //JALR
		rd = get_bit(opt, 7, 11);
		rs1 = get_bit(opt, 15, 19);
		imm = get_imm(opt, 'I');
		FR[1].ALU = FR[0].npc;
		pc = (x[rs1] + imm) & bin[1][31];
		type = "JALR";
	} else {
		rs1 = get_bit(opt, 15, 19);
		rs2 = get_bit(opt, 20, 24);
		rd = get_bit(opt, 7, 11);
		funct3 = get_bit(opt, 12, 14);
		imm = get_imm(opt, 'B');
		switch (funct3) {
			case 0: //BEQ
				pc += x[rs1] == x[rs2] ? imm - 4 : 0;
				type = "BEQ";
				break;
			case 1: //BNE
				pc += x[rs1] != x[rs2] ? imm - 4 : 0;
				type = "BNE";
				break;
			case 4: //BLT
				pc += x[rs1] < x[rs2] ? imm - 4 : 0;
				type = "BLT";
				break;
			case 5: //BGE
				pc += x[rs1] >= x[rs2] ? imm - 4 : 0;
				type = "BGE";
				break;
			case 6: //BLTU
				pc += uint32_t(x[rs1]) < uint32_t(x[rs2]) ? imm - 4 : 0;
				type = "BLTU";
				break;
			case 7: //BGEU
				pc += uint32_t(x[rs1]) >= uint32_t(x[rs2]) ? imm - 4 : 0;
				type = "BGEU";
				break;
		}
	}
	FR[1].opt = opt;
	FR[1].rs1 = x[rs1];
	FR[1].rs2 = x[rs2];
	FR[1].shamt = shamt;
	FR[1].type = type;
	FR[1].imm = imm;
	FR[1].rd = rd;
}
void LUI() {
	FR[2].ALU = FR[1].imm;
}
void AUIPC() {
	FR[2].ALU = FR[1].npc + FR[1].imm - 4;
}
void JAL() {}
void JALR() {}
void BEQ() {}
void BNE() {}
void BLT() {}
void BGE() {}
void BLTU() {}
void BGEU() {}
void LB() {
	FR[2].ALU = FR[1].rs1 + FR[1].imm;
}
void LBU() {
    FR[2].ALU = FR[1].rs1 + FR[1].imm;
}
void LH() {
    FR[2].ALU = FR[1].rs1 + FR[1].imm;
}
void LHU() {
    FR[2].ALU = FR[1].rs1 + FR[1].imm;
}
void LW() {
    FR[2].ALU = FR[1].rs1 + FR[1].imm;
}
void SB() {
    FR[2].ALU = FR[1].rs1 + FR[1].imm;
}
void SH() {
    FR[2].ALU = FR[1].rs1 + FR[1].imm;
}
void SW() {
	FR[2].ALU = FR[1].rs1 + FR[1].imm;
}
void ADDI() {
    FR[2].ALU = FR[1].rs1 + FR[1].imm;
}
void SLTI() {
    FR[2].ALU = (FR[1].rs1 < FR[1].imm);
}
void SLTIU() {
    FR[2].ALU = (uint32_t(FR[1].rs1) < uint32_t(FR[1].imm));
}
void XORI() {
    FR[2].ALU = FR[1].rs1 ^ FR[1].imm;
}
void ORI() {
    FR[2].ALU = FR[1].rs1 | FR[1].imm;
}
void ANDI() {
	FR[2].ALU = FR[1].rs1 & FR[1].imm;
}
void SLLI() {
    FR[2].ALU = FR[1].rs1 << FR[1].imm;
}
void SRLI() {
    FR[2].ALU = uint32_t(FR[1].rs1) >> uint32_t(FR[1].imm);
}
void SRAI() {
    FR[2].ALU = FR[1].rs1 >> FR[1].imm;
}
void ADD() {
	FR[2].ALU = FR[1].rs1 + FR[2].rs2;
}
void SUB() {
    FR[2].ALU = FR[1].rs1 - FR[2].rs2;
}
void SLL() {
    FR[2].ALU = FR[1].rs1 << (FR[2].rs2 & 31);
}
void SLT() {
    FR[2].ALU = (FR[1].rs1 < FR[2].rs2);
}
void SLTU() {
    FR[2].ALU = (uint32_t(FR[1].rs1) < uint32_t(FR[2].rs2));
}
void XOR() {
    FR[2].ALU = FR[1].rs1 ^ FR[2].rs2;
}
void SRL() {
    FR[2].ALU = uint32_t(FR[1].rs1) >> uint32_t(FR[2].rs2 & 31);
}
void SRA() {
    FR[2].ALU = FR[1].rs1 >> (FR[2].rs2 & 31);
}
void OR() {
    FR[2].ALU = FR[1].rs1 | FR[2].rs2;
}
void AND() {
    FR[2].ALU = FR[1].rs1 & FR[2].rs2;
}
void execute() {
	FR[2] = FR[1];
	switch (fmt[FR[2].type]) {
		case 0 : LUI(); break;
		case 1 : AUIPC(); break;
		case 2 : JAL(); break; 
		case 3 : JALR(); break;
		case 4 : BEQ(); break;
		case 5 : BNE(); break;
		case 6 : BLT(); break;
		case 7 : BGE(); break;
		case 8 : BLTU(); break;
		case 9 : BGEU(); break;
		case 10 : LB(); break;
		case 11 : LH(); break;
		case 12 : LW(); break;
		case 13 : LBU(); break;
		case 14 : LHU(); break;
		case 15 : SB(); break;
		case 16 : SH(); break;
		case 17 : SW(); break;
		case 18 : ADDI(); break;
		case 19 : SLTI(); break;
		case 20 : SLTIU(); break;
		case 21 : XORI(); break;
		case 22 : ORI(); break;
		case 23 : ANDI(); break;
		case 24 : SLLI(); break;
		case 25 : SRLI(); break;
		case 26 : SRAI(); break;
		case 27 : ADD(); break;
		case 28 : SUB(); break;
		case 29 : SLL(); break;
		case 30 : SLT(); break;
		case 31 : SLTU(); break;
		case 32 : XOR(); break;
		case 33 : SRL(); break;
		case 34 : SRA(); break;
		case 35 : OR(); break;
		case 36 : AND(); break;
	}
}
void fetch() {
	memcpy(&FR[0].opt, mem + pc, 4);
	FR[0].npc = pc + 4;
	pc += 4;
}
void _LB() {
    int8_t t;
    memcpy(&t, mem + FR[2].ALU, 1);
    FR[3].ALU = t;
}
void _LBU() {
	uint8_t t;
	memcpy(&t, mem + FR[2].ALU, 1);
	FR[3].ALU = t;
}
void _LH() {
	int16_t t;
	memcpy(&t, mem + FR[2].ALU, 2);
	FR[3].ALU = t;
}
void _LHU() {
	int16_t t;
	memcpy(&t, mem + FR[2].ALU, 2);
	FR[3].ALU = t;
}
void _LW() {
	int t;
	memcpy(&t, mem + FR[2].ALU, 4);
	FR[3].ALU = t;
}
void _SB() {
	uint8_t t = FR[2].rs2;
	memcpy(mem + FR[2].ALU, &t, 1);
}
void _SH() {
	uint16_t t = FR[2].rs2;
	memcpy(mem + FR[2].ALU, &t, 2);
}
void _SW() {
	int t = FR[2].rs2;
	memcpy(mem + FR[2].ALU, &t, 4);
}
void mem_acc() {
	FR[3] = FR[2];
	switch (fmt[FR[3].type]) {
		case 10 : _LB(); break;
		case 11 : _LBU(); break;
		case 12 : _LH(); break;
		case 13 : _LHU(); break;
		case 14 : _LW(); break;
		case 15 : _SB(); break;
		case 16 : _SH(); break;
		case 17 : _SW(); break;
	}
}
void write_back() {
	if (FR[2].rd != -1) {
		x[FR[2].rd] = FR[3].ALU;
	}
}
void doit() {
	pc = 0;
	while (1) {
		fetch();
		if (FR[0].opt == 0x00c68223) break;
		decode();
		execute();
		mem_acc();
		write_back();
		x[0] = 0;
	}
	printf("%d\n", x[10] & 255);
}
bool get_instruction() {
	if (scanf("%s", s) == EOF) return false;
	if (s[0] == '@') {
		pc = 0;
		for (int i = 1; i <= 8; i++) {
			pc += HEX[s[i]] * pw[8 - i];
		}
		scanf("%s", s);
		mem[pc] = HEX[s[0]] * 16 + HEX[s[1]];
	} else {
		mem[++pc] = HEX[s[0]] * 16 + HEX[s[1]];
	}
	return true;
}
void read() {
	while (get_instruction()) {}
	for (int i = 0; i < pc; i++) {
	}
}
int main() {
	//freopen("1.txt", "r", stdin);
	init();
	read();
	doit();
	return 0;
}

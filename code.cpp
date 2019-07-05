#include <bits/stdc++.h>
using namespace std;
typedef unsigned int ui;
const int LEN = 1e6 + 5;

char s[105];
struct Inst {
	string type;
	int rs1, rs2, rd, imm, funct3, funct7;
	void out() {
		cout<<type<<' ';
		cout<<rs1<<' '<<rs2<<' '<<rd<<' '<<imm<<' '<<funct3<<' '<<funct7<<endl; 
	}
};

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
Inst decode(int opt) {
	string type;
	int opcode = 0, rs1 = 0, rs2 = 0, rd = 0, imm = 0, funct3 = 0, funct7 = 0; 
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
				imm = get_bit(opt, 20, 24);
				type = "SLLI";
				break;
			case 5:
				imm = get_bit(opt, 20, 24);
				if (funct7 == 0) { //SRLI
					type = "SRLI";
				} else { //SRAI
					type = "SRAI";
				}
				break;
		}
		switch (funct3) {
			
		}
	} else if (opcode == 19) {
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
		type = "JAL";
	} else if (opcode == 103) { //JALR
		rd = get_bit(opt, 7, 11);
		rs1 = get_bit(opt, 15, 19);
		imm = get_imm(opt, 'I');
		type = "JALR";
	} else {
		rs1 = get_bit(opt, 15, 19);
		rs2 = get_bit(opt, 20, 24);
		rd = get_bit(opt, 7, 11);
		funct3 = get_bit(opt, 12, 14);
		imm = get_imm(opt, 'B');
		switch (funct3) {
			case 0: //BEQ
				type = "BEQ";
				break;
			case 1: //BNE
				type = "BNE";
				break;
			case 4: //BLT
				type = "BLT";
				break;
			case 5: //BGE
				type = "BGE";
				break;
			case 6: //BLTU
				type = "BLTU";
				break;
			case 7: //BGEU
				type = "BGEU";
				break;
		}
	}
	return (Inst){type, rs1, rs2, rd, imm, funct3, funct7};
}
void LUI(const Inst &now) {
	x[now.rd] = now.imm;
	pc += 4;
}
void AUIPC(const Inst &now) {
	x[now.rd] = pc + now.imm;
	pc += 4;
}
void JAL(const Inst &now) {
	x[now.rd] = pc + 4;
	pc += now.imm;
}
void JALR(const Inst &now) {
    x[now.rd] = pc + 4;
    pc = (x[now.rs1] + now.imm) & (-2);
}
void BEQ(const Inst &now) {
    if (x[now.rs1] == x[now.rs2]) {
    	pc += now.imm;
    } else pc += 4;
}
void BNE(const Inst &now) { 
    if (x[now.rs1] != x[now.rs2]) {
    	pc += now.imm;
    } else pc += 4;
}
void BLT(const Inst &inst) {
    if (x[inst.rs1] < x[inst.rs2]) {
        pc += inst.imm;
    } else pc += 4;
}
void BGE(const Inst &now) {
    if (x[now.rs1] >= x[now.rs2]) {
        pc += now.imm;
    } else pc += 4;
}
void BLTU(const Inst &now) {
    if (uint32_t(x[now.rs1]) < uint32_t(x[now.rs2])) {
        pc += now.imm;
    } else pc += 4;
}
void BGEU(const Inst &now) {
    if (uint32_t(x[now.rs1]) >= uint32_t(x[now.rs2])) {
        pc += now.imm;
    } else pc += 4;
}
void LB(const Inst &now) {
    int8_t tmp;
    memcpy(&tmp, mem + x[now.rs1] + now.imm, 1);
    x[now.rd] = tmp;
    pc += 4;
}
void LBU(const Inst &now) {
    uint8_t tmp;
    memcpy(&tmp, mem + x[now.rs1] + now.imm, 1);
    x[now.rd] = tmp;
    pc += 4;
}
void LH(const Inst &now) {
    int16_t tmp;
    memcpy(&tmp, mem + x[now.rs1] + now.imm, 2);
    x[now.rd] = tmp;
    pc += 4;
}
void LHU(const Inst &now) {
    uint16_t tmp;
    memcpy(&tmp, mem + x[now.rs1] + now.imm, 2);
    x[now.rd] = tmp;
    pc += 4;
}
void LW(const Inst &now) {
    memcpy(&x[now.rd], mem + x[now.rs1] + now.imm, 4);
    pc += 4;
}
void SB(const Inst &now) {
    uint8_t tmp = x[now.rs2];
    memcpy(mem + x[now.rs1] + now.imm, &tmp, 1);
    pc += 4;
}
void SH(const Inst &now) {
    uint16_t tmp = x[now.rs2];
    memcpy(mem + x[now.rs1] + now.imm, &tmp, 2);
    pc += 4;
}
void SW(const Inst &now) {
	uint32_t tmp = x[now.rs2];
  	memcpy(mem + x[now.rs1] + now.imm, &tmp, 4);
    pc += 4;
}
void ADDI(const Inst &now) {
    x[now.rd] = x[now.rs1] + now.imm;
    pc += 4;
}
void SLTI(const Inst &now) {
    x[now.rd] = (x[now.rs1] < now.imm);
    pc += 4;
}
void SLTIU(const Inst &now) {
    x[now.rd] = (uint32_t(x[now.rs1]) < uint32_t(now.imm));
    pc += 4;
}
void XORI(const Inst &now) {
    x[now.rd] = x[now.rs1] ^ now.imm;
    pc += 4;
}
void ORI(const Inst &now) {
    x[now.rd] = x[now.rs1] | now.imm;
    pc += 4;
}
void ANDI(const Inst &now) {
    x[now.rd] = x[now.rs1] & now.imm;
    pc += 4;
}
void SLLI(const Inst &now) {
    x[now.rd] = x[now.rs1] << now.imm;
    pc += 4;
}
void SRLI(const Inst &now) {
    x[now.rd] = uint32_t(x[now.rs1]) >> uint32_t(now.imm);
    pc += 4;
}
void SRAI(const Inst &now) {
    x[now.rd] = x[now.rs1] >> now.imm;
    pc += 4;
}
void ADD(const Inst &now) {
    x[now.rd] = x[now.rs1] + x[now.rs2];
    pc += 4;
}
void SUB(const Inst &now) {
    x[now.rd] = x[now.rs1] - x[now.rs2];
    pc += 4;
}
void SLL(const Inst &now) {
    x[now.rd] = x[now.rs1] << (x[now.rs2] & 0x1f);
    pc += 4;
}
void SLT(const Inst &now) {
    x[now.rd] = (x[now.rs1] < x[now.rs2]);
    pc += 4;
}
void SLTU(const Inst &now) {
    x[now.rd] = (uint32_t(x[now.rs1]) < uint32_t(x[now.rs2]));
    pc += 4;
}
void XOR(const Inst &now) {
    x[now.rd] = x[now.rs1] ^ x[now.rs2];
    pc += 4;
}
void SRL(const Inst &now) {
    x[now.rd] = uint32_t(x[now.rs1]) >> uint32_t((x[now.rs2] & 0x1f));
    pc += 4;
}
void SRA(const Inst &now) {
    x[now.rd] = x[now.rs1] >> (x[now.rs2] & 0x1f);
    pc += 4;
}
void OR(const Inst &now) {
    x[now.rd] = x[now.rs1] | x[now.rs2];
    pc += 4;
}
void AND(const Inst &now) {
    x[now.rd] = x[now.rs1] & x[now.rs2];
    pc += 4;
}
void execute(const Inst &now) {
	switch (fmt[now.type]) {
		case 0 : LUI(now); break;
		case 1 : AUIPC(now); break;
		case 2 : JAL(now); break; 
		case 3 : JALR(now); break;
		case 4 : BEQ(now); break;
		case 5 : BNE(now); break;
		case 6 : BLT(now); break;
		case 7 : BGE(now); break;
		case 8 : BLTU(now); break;
		case 9 : BGEU(now); break;
		case 10 : LB(now); break;
		case 11 : LH(now); break;
		case 12 : LW(now); break;
		case 13 : LBU(now); break;
		case 14 : LHU(now); break;
		case 15 : SB(now); break;
		case 16 : SH(now); break;
		case 17 : SW(now); break;
		case 18 : ADDI(now); break;
		case 19 : SLTI(now); break;
		case 20 : SLTIU(now); break;
		case 21 : XORI(now); break;
		case 22 : ORI(now); break;
		case 23 : ANDI(now); break;
		case 24 : SLLI(now); break;
		case 25 : SRLI(now); break;
		case 26 : SRAI(now); break;
		case 27 : ADD(now); break;
		case 28 : SUB(now); break;
		case 29 : SLL(now); break;
		case 30 : SLT(now); break;
		case 31 : SLTU(now); break;
		case 32 : XOR(now); break;
		case 33 : SRL(now); break;
		case 34 : SRA(now); break;
		case 35 : OR(now); break;
		case 36 : AND(now); break;
	}
}
int get_opt(int &opt) {
	memcpy(&opt, mem + pc, 4);
	//opt = (mem[pc + 3] << 24) + (mem[pc + 2] << 16) + (mem[pc + 1] << 8) + mem[pc];
}
void doit() {
	pc = 0;
	while (1) {
	//	if (++dfn > 160) break;
		int opt;
		get_opt(opt);
		/*if (pc == 8) {
			cout<<(int)mem[8]<<' '<<(int)mem[8 + 1]<<' '<<(int)mem[pc + 2]<<' '<<(int)mem[pc + 3]<<endl;
		}*/
		//cout<<pc<<' '<<opt<<' ';
		if (opt == 0x00c68223) break;
		Inst now = decode(opt);
	//	cout<<pc<<' '<<(int)mem[8]<<' '<<(int)mem[8 + 1]<<' '<<(int)mem[8 + 2]<<' '<<(int)mem[8 + 3]<<"-----------------";
	//	now.out(); 
	//	cout<<pc<<' ';
	//	for (int i = 0; i < 32; i++) cout<<x[i]<<' ';
	//	cout<<endl;
		execute(now);
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
		//printf("%d ", (int)mem[i]);
	}
}
int main() {
//	freopen("1.txt", "r", stdin);
//	freopen("out.txt", "w", stdout);
	init();
	read();
	doit();
	return 0;
}

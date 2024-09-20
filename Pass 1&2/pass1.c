#include<stdio.h>
#include<string.h>
#include<stdlib.h>

int searchtable(char label[],int i) {
	char symbol[10];
	FILE *table;
    i == 0 ? table = fopen("symtab.txt","r") : fopen("optab.txt","r");
	int value;
	while( fscanf(table,"%s %d",symbol,&value) != EOF) {
		if(strcmp(label,symbol)==0) {
			fclose(table);
			return 1;
		}
	}
	fclose(table);
	return 0;
}

void writesymtab(char label[], int locctr) {
	FILE *symtab = fopen("symtab.txt", "a");
	fprintf(symtab, "%s %04X\n", label, locctr);
	fclose(symtab);
}

void main() {
    FILE *input,*output,*symtab,*length;
	int saddr,locctr;
	char label[10],opcode[10],operand[10];
	input = fopen("input.txt","r");
	output = fopen("intermediate.txt","w");
    symtab = fopen("symtab.txt", "w");
    fclose(symtab);
	
	if(!feof(input)) {
		fscanf(input,"%s %s %s",label,opcode,operand);
		if(!strcmp(opcode,"START")) {
			saddr = (int)strtol(operand,NULL,16);
			locctr = (int)strtol(operand,NULL,16);
			fprintf(output,"%04X %s %s %04s\n",locctr,label,opcode,operand);
		} else {
            rewind(input);
			locctr = 0;
			saddr = 0;
            fprintf(output,"%04X %s %s %04X\n",locctr,"PRGM","START",locctr);
		}
	}

	while(!feof(input)) {

		fscanf(input,"%s %s %s",label,opcode,operand);
        if(strcmp(opcode,"END")==0)
			break;

        fprintf(output,"%04X %s %s %s\n",locctr,label,opcode,operand);
        
        if(searchtable(label,0)==1) {
				printf("symbol error\n");
				exit(0);
		} else {
			if(searchtable(opcode,1)==1)
				locctr+=3;
			else if(strcmp(opcode,"WORD")==0) {
				writesymtab(label,locctr);
				locctr+=3;
			}
			else if(strcmp(opcode,"RESW")==0) {
				writesymtab(label,locctr);
				locctr += atoi(operand)*3;
			}
            else if(strcmp(opcode,"RESB")==0) {
				writesymtab(label,locctr);
				locctr += atoi(operand);
            }
			else if(strcmp(opcode,"BYTE")==0) {
				writesymtab(label,locctr);
				locctr+=(strlen(operand)-3);
			}
			else {
				printf("Opcode error\n");
				exit(0);
			}
		}
    }
    fprintf(output,"%04X %s %s %s\n",locctr,label,opcode,"**");
	printf("Program length = %04X\n",locctr-saddr);
	length = fopen("length.txt","w");
	fprintf(length,"%04X",locctr-saddr);

	fclose(input);
	fclose(output);
	fclose(length);
}
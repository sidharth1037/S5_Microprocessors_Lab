#include<stdio.h>
#include<stdlib.h>
#include<string.h>

int tcount=0,trecstart;
char tbuff[100]="";

int searchtable(char label[],char tablename[]) {
	char symbol[10];
    int value;
	FILE *table = fopen(tablename,"r");
	while( fscanf(table,"%s %X",symbol,&value) != EOF) {
		if(strcmp(label,symbol)==0) {
			fclose(table);
			return value;
		}
	}
	fclose(table);
	return -1;
}

void writeobjcode(char objcode[],int length,char loc[]) {
    if( (tcount+=length) <= 30 ) {
        sprintf(tbuff+strlen(tbuff),"^%s",objcode);
    } else {
        FILE *object = fopen("object.txt","a");
        fprintf(object,"T^%06X^%02X%s\n",trecstart,tcount-length,tbuff);
        sprintf(tbuff,"^%s",objcode);
        tcount = length;
        trecstart = (int)strtol(loc,NULL,16);
        fclose(object);
    }
}

void main() {
	int prgmlen,j,startaddr,opcode,labeladdr,i;
	char loc[10],label[10],opword[10],operand[10],bytes[10],objcode[15]="";
	FILE *intermediate, *object, *length,*listing;
	intermediate = fopen("intermediate.txt","r");
    length = fopen("length.txt","r");
	listing = fopen("listing.txt","w");
    object = fopen("object.txt","w");

    fscanf(intermediate,"%s%s%s%s",loc,label,opword,operand);
    fscanf(length,"%X",&prgmlen);
      	
    if(strcmp(opword,"START")==0) {
        startaddr = (int)strtol(operand,NULL,16);
    	fprintf(object,"H^%-6s^%06X^%06X\n",label,startaddr,prgmlen);
		fprintf(listing,"%s %s %s %s\n",loc,label,opword,operand);
        fclose(object);
    } else {
      	printf("Intermediate file error");
    	exit(0);
  	}
      	
    while( fscanf(intermediate,"%s%s%s%s",loc,label,opword,operand) != EOF) {
        if(!strcmp(opword,"END")) 
            break; 
        if(tcount==0)
            trecstart = (int)strtol(loc,NULL,16);

    	opcode = searchtable(opword,"optab.txt");
        if(opcode != -1) {
    		labeladdr = searchtable(operand,"symtab.txt");
    		if(labeladdr != -1) {
                sprintf(objcode,"%02X%04X",opcode,labeladdr);
                writeobjcode(objcode,3,loc);
                fprintf(listing,"%s %s %s %s\t\t%s\n",loc,label,opword,operand,objcode);
            }
    	}
        	
        else if (strcmp(opword,"WORD")==0) {
            sprintf(objcode,"%06X",atoi(operand));
            writeobjcode(objcode,3,loc);
            fprintf(listing,"%s %s %s %s\t\t%s\n",loc,label,opword,operand,objcode);
    	}

		else if (strcmp(opword,"BYTE")==0) {
            strncpy(bytes, &operand[2], strlen(operand)-3);
			for(i=0;bytes[i] != '\0';i++)
				sprintf( &objcode[i * 2], "%02X", (unsigned char)bytes[i] );
            objcode[i*2] = '\0';
            writeobjcode(objcode,i,loc);
            fprintf(listing,"%s %s %s %s\t\t%s\n",loc,label,opword,operand,objcode);
		}

		else if (strcmp(opword,"RESB")==0 || strcmp(opword,"RESW")==0) {
			fprintf(listing,"%s %s %s %s\n",loc,label,opword,operand);
		}
  	}

	fprintf(listing,"%s %s %s %s\n",loc,label,opword,operand);
    object = fopen("object.txt","a");
    fprintf(object,"T^%06X^%02X%s",trecstart,tcount,tbuff);
	fprintf(object,"\nE^%06X",startaddr);

    fclose(object);
    fclose(intermediate);
    fclose(listing);
    fclose(length);
}















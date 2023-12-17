#include <stdio.h>

//Joshua Sheputa 2023

void printString(char[]);
void printChar(char);
void readString(char[]);
void readSector(char*, int);
void writeSector(char*, int);
//void handleInterrupt21(int, int, int, int);
void deleteCommand(char*);
void writeFile(char*, char*, int);

//void makeTimerInterrupt();
void handleTimerInterrupt(int, int);
void returnFromTimer(int, int);

//interrupt(interNum, AX, BX, CX, DX)

int main() {

    makeInterrupt21();
    interrupt(0x21, 8, "this is a test message\n", "testmg", 3);
    makeTimerInterrupt();
    interrupt(0x21, 4, "shell", 0, 0);
    
    while(1);
}

void printString(char chars[]){
    char selectedChar = chars[0];
    int i=0;

    while(selectedChar != 0x0){
        char al = selectedChar;
        char ah = 0xe;
        int ax = ah * 256 + al;
        interrupt(0x10, ax, 0, 0, 0);
        i++;
        selectedChar = chars[i];

    }
}

void printChar(char c){
    char al = c;
    char ah = 0xe;
    int ax = ah * 256 + al;
    interrupt(0x10, ax, 0, 0, 0);

}

void readString(char input[]){
    char key;
    int i=0;
    do {
        key = interrupt(0x16, 0, 0, 0, 0);
        if(key != 0x8){
            input[i] = key;
            interrupt(0x10, 0xe*256+key, 0, 0, 0);
            i++;
        } else if (key == 0x8){
            if (i>0){
                i--;
                interrupt(0x10, 0xe * 256 + 0x8, 0, 0, 0); //backspace
                interrupt(0x10, 0xe * 256 + 0x20, 0, 0, 0); //space
                interrupt(0x10, 0xe * 256 + 0x8, 0, 0, 0); //backspace
            }
        }

    } while(key != 0xd);
    //interrupt(0x10, 0xe * 256 + 0xd, 0, 0, 0); //final carriage return
    interrupt(0x10, 0xe * 256 + 0xa, 0, 0, 0); //line feed
}

void readSector(char *buffer, int sector){
    int AH = 2;         //(this number tells the BIOS to read a sector as opposed to write)
    int AL = 1;         //number of sectors to read (use 1)
    char *BX = buffer;  //address where the data should be stored to (pass your char* array here)
    int CH = 0;         //track number
    int CL = sector + 1;//relative sector number (sector number plus one)
    int DH = 0;         //head number
    int DL = 0x80;      //device number (for the hard disk, use 0x80)

    int AX = AH*256+AL;
    int CX = CH*256+CL;
    int DX = DH*256+DL;

    interrupt(0x13, AX, BX, CX, DX);
}

void writeSector(char *buffer, int sector){
    int AH = 3;         //write mode
    int AL = 1;         //number of sectors to read (use 1)
    char *BX = buffer;  //address where the data should be stored to (pass your char* array here)
    int CH = 0;         //track number
    int CL = sector + 1;//relative sector number (sector number plus one)
    int DH = 0;         //head number
    int DL = 0x80;      //device number (for the hard disk, use 0x80)

    int AX = AH*256+AL;
    int CX = CH*256+CL;
    int DX = DH*256+DL;

    interrupt(0x13, AX, BX, CX, DX);
}

void printDirectory(char *directory) {
    int i;
    int j;
    for (i=0; i<512; i+=32) {
        if (directory[i] != 0) {
            //printString("File: ");
            for (j = 0; j < 6; j++) {
                printChar(directory[i + j]);
            }
            interrupt(0x10, 0xe * 256 + 0xd, 0, 0, 0);
            interrupt(0x10, 0xe * 256 + 0xa, 0, 0, 0);
        }
    }
}

void readFile(char *fileName, char *buffer, int *sectorsRead ){
    char directory[512];
    int fileEntry = 0; //current entry position
    int match;
    int j;
    
    readSector(directory, 2); //map at sector 1, directory at sector 2, kernel at sector 3
    //printDirectory(directory);

    for(fileEntry = 0; fileEntry<512; fileEntry+=32){
        match = 1;

        for (j=0; j<6; j++){ //loop through six characters of entry
            if (fileName[j] != directory[fileEntry + j]){ //filename char, entry offset plus current char
                //printChar('u');
                match = 0;
                break;
            }
        }

        if (match){
            int nameLength = 6;
            int sector = directory[fileEntry + nameLength];
            while(sector != 0){

                readSector(buffer, sector);
                buffer+=512;
                nameLength++;
                sector = directory[fileEntry + nameLength];
            }

            *sectorsRead += nameLength - 6;
            break;
        }
    }
    if(!match){
        *sectorsRead = 0;
        printChar('3');
        printString("Error: File not found.\r\n");
    }
}

void executeProgram(char *programName){
    char buffer[13312];
    int sectorsRead = 0;
    int i;
    readFile(programName, buffer, &sectorsRead);
    //printString("SectorsRead: ");
    //printChar(sectorsRead+48);
    //printString("\r\n");
    
    if (sectorsRead==0){
        printString("Error: Program not found.\r\n");
        printChar('x');
        return;
    } //else printString("Found file.\r\n");

    for (i=0; i<sectorsRead*512; i++) {
        putInMemory(0x2000, i, *(buffer+i));
    }
    //Call the assembly function void launchProgram(int segment), and pass segment 0x2000 as the parameter
    launchProgram(0x2000);

    printString("launchProgram returned! error!\r\n"); //does not reach this
}

void deleteFile(char *filename){
    char directory[512];
    char map[512];
    int dirIndex;
    int j;
    int match;
    int fileNameStart = 4;

    readSector(directory, 2);
    readSector(map, 1);

    for (dirIndex = 0; dirIndex < 512; dirIndex += 32) {
        match = 1;
        for (j = 0; j < 6; j++) {
            if (filename[j + fileNameStart] != directory[dirIndex + j]) {
                match = 0;
                printChar('x');
                break;
            }
        }

        if (match) {
            directory[dirIndex] = '\0'; 
        // Set the first byte of the file name to '\0'

            // Step through the sector numbers and update the Map
            for (j = 6; j < 32; j++) {
                int sector = directory[dirIndex + j];
                if (sector != 0x0) {
                    map[sector] = 0x0;
                }
            }
            // Write back the updated Directory and Map
            writeSector(directory, 2);
            writeSector(map, 1);
            return; // File found and deleted
        }
    }

    //syscall(0, "File not found\r\n");
    printChar('n');
    printChar('o');
    printChar('t');
    printChar(' ');
    printChar('f');
    printChar('\r');
    printChar('\n');
}

void writeFile(char* buffer, char* filename, int numberOfSectors) {
    char directory[512];
    char map[512];
    char tempbuffer[512];
    int i;
    int j;
    int k;
    int dirindex;
    int sectorcount;
    int bufferindex=0;

    readSector(directory, 2);
    readSector(map, 1);

    for (i=0; i < 512; i += 32) //find free entry in directory
		if (directory[i]==0)
			break;
	if (i!=512) {
        dirindex=i;
	}
    
    for (i=0; i<32; i++) //fill field with \0
		directory[dirindex + i]='\0';
	//copy the name over
	for (i=0; i<6; i++){
		if(filename[i]==0)
			break;
		directory[dirindex + i]=filename[i];
	}

    sectorcount = 0;
    for (j = 3; j < 256; j++) {//loop through map, start at entry 3
        //printChar('t');
        if (sectorcount >= numberOfSectors)
            break;
        if (map[j] == 0x00) {
            map[j] = 0xFF;
            sectorcount++;
            directory[dirindex+5+sectorcount] = j; //set sector number in directory
            
            for (k = 0; k < 512; k++)
                tempbuffer[k] = buffer[bufferindex+k];
            bufferindex += 512;
            writeSector(tempbuffer, j);
        }
    }
    if (j>255)
        return;
    //printChar('g');
    writeSector(directory,2);
    writeSector(map,1);
}

void terminate(){
    char shellname[6];
    shellname[0]='s';
    shellname[1]='h';
    shellname[2]='e';
    shellname[3]='l';
    shellname[4]='l';
    shellname[5]='\0';
    executeProgram(shellname);
}

void handleInterrupt21(int ax, int bx, int cx, int dx){
    if (ax==0){
        printString(bx);
    }
    if (ax==1){
        readString(bx);
    }
    if (ax==2){
        readSector(bx, cx);
    }
    if (ax==3){
        readFile(bx, cx, dx);
    }
    if (ax==4){
        executeProgram(bx);
    }
    if (ax==5){
        terminate();
    }
    if (ax==6){
        writeSector(bx, cx);
    }
    if (ax==7){
        deleteFile(bx);
    }
    if (ax==8){
        writeFile(bx, cx, dx);
    }
    if (ax > 8){
        printString("error");
        printChar('x');
    }
}

void handleTimerInterrupt(int segment, int sp){
    printChar('T');
    printChar('i');
    printChar('c');
    returnFromTimer(segment, sp);
}
//void returnFromTimer(int segment, int sp){}



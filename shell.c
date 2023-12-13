//Joshua Sheputa 2023

void typeCommand(char[]);
void execCommand(char[]);
void dirCommand();
void prntChar(char);
void copyCommand(char[]);

int main(){
    char input[80];
    while(1){
        syscall(0, "SH>");
        //input[80];
        syscall(1, input);
        //try to match
        //syscall(0, input);
        if (input[0]=='t' && input[1]=='y' && input[2]=='p' && input[3]=='e'){
            typeCommand(input);
        }
        else if (input[0]=='e' && input[1]=='x' && input[2]=='e' && input[3]=='c'){
            execCommand(input);
        } 
        else if (input[0]=='d' && input[1]=='i' && input[2]=='r'){
            dirCommand();
        }
        else if (input[0]=='d' && input[1]=='e' && input[2]=='l'){
            syscall(7, input);
        }
        else if (input[0]=='c' && input[1]=='o' && input[2]=='p' && input[3]=='y'){
            copyCommand(input);
        }
        else if (input[0]=='t' && input[1]=='e' && input[2]=='s' && input[3]=='t'){
            prntChar('b');
            prntChar('\r');
            prntChar('\n');
        }
        else syscall(0, "bad command\r\n");
    }
}

/*
0 printString
1 readString
2 readSector
3 readFile
4 executeProgram
5 terminate
*/

void prntChar(char letter){
    //copies character into string and prints string
    char charArray[2];
    charArray[0] = letter;
    charArray[1] = '\0';
    syscall(0, charArray);
}

void typeCommand(char command[]){
    char buffer[13312];
    int sectorsread = 0;
    char filename[6];
    int startname = 5;
    int i = 0;
    
    for(i=0; i<6; i++){
        filename[i] = '\0';
        filename[i] = command[i+startname];    
    }
    //filename[6] = '\0';

    syscall(3, filename, buffer, &sectorsread);
    if (sectorsread==0){
        syscall(0, "File not found\r\n");
    } else {
        syscall(0, buffer, 0, 0);
        syscall(0, "\r", 0, 0);
    }
}

void execCommand(char command[]){
    char filename[7];
    int startname = 5;
    int i = 0;
    
    for(i=0; i<6; i++){
        filename[i] = command[i+startname];    
    }
    filename[6] = "\0";
    syscall(4, filename);
}

void dirCommand(){
// first readsector
    char directory[512];
    char currentChar;
    int i;
    int j = 0;
    //readSector(directory, 2);
    syscall(2, directory, 2);
    
  
    for (i=0; i<512; i+=32) {
        if (directory[i] != 0) {
            //printString("File: ");
            for (j = 0; j < 6; j++) {
                currentChar = directory[i + j];
                if (currentChar == 0){
                    break;
                }
                prntChar(currentChar);
            }
            //convert to syscal or dummy printchar
                //copies character into string and prints string
            prntChar('\r');
            prntChar('\n');
//            interrupt(0x10, 0xe * 256 + 0xd, 0, 0, 0);
//            interrupt(0x10, 0xe * 256 + 0xa, 0, 0, 0);
        }
    }
}

void copyCommand(char input[]){ //not filling directory or map
    char filename1[6];
    char filename2[6];
    int i;
    int j;
    int k = 6;
    char buffer[512];
    int sectorsread = 0;

    for (i=0; i<6; i++){
        filename1[i] = '\0';
        filename1[i] = input[i+5];
        k++;
    }
    //filename1[i] = '\0';
    syscall(3, filename1, buffer, &sectorsread); //DID NOT WORK IF LOWER DOWN
    prntChar('-');
    syscall(0, filename1);
    prntChar('\n');
    prntChar('\r');

    for (j=0; input[j+k] != '\0'; j++){
        filename2[j] = '\0';
        filename2[j] = input[j+k];
        //prntChar(input[j+k]);
    }
    
    //filename2[j] = '\0';
    syscall(8, buffer, filename2, sectorsread);
    prntChar('-');
    syscall(0, filename2);
    prntChar('\n');
    prntChar('\r');

//    syscall(3, filename1, buffer, &sectorsread);
//    syscall(8, buffer, filename2, sectorsread);
}




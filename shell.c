//Joshua Sheputa 2023

void typeCommand(char[]);
void execCommand(char[]);

int main(){
    char input[80];
    while(1){
        syscall(0, "SH>");
        input[80];
        syscall(1, input);
        //try to match
        //syscall(0, input);
        if (input[0]=='t' && input[1]=='y' && input[2]=='p' && input[3]=='e'){
            typeCommand(input);
        }
        else if (input[0]=='e' && input[1]=='x' && input[2]=='e' && input[3]=='c'){
            execCommand(input);
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

void typeCommand(char command[]){
    char buffer[13312];
    int sectorsread = 0;
    char filename[7];
    int startname = 5;
    int i = 0;
    
    for(i=0; i<6; i++){
        filename[i] = command[i+startname];    
    }
    filename[6] = "\0";

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

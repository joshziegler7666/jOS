/*
 * the kernel. 
 * using the vga frame buffer passed in from the boot loader for now
 * also using ps/2 keyboard protocoll for now
*/
void kernel_main();

#include "badLookingAscii.h"

typedef struct {
    unsigned int                      *fb;
    unsigned int                      HorizontalResolution;
    unsigned int                      VerticalResolution;
} kernelParameters;

//load this first at 0x100000
void kernel_main(kernelParameters *kp);
//then load other functions
unsigned char getchar();
void rectangle(int x, int y, int width, int height, int color);
void printPos(int x, int y, char* text, int color);
int strLen(char *string);
void clearScreen();
char* intToStr(int integer);
unsigned long long hexToLongLong(char hex[]);
void checkCommand(char command[]);
void printConsole();
void consolePrint(char string[]);
int strComp(char a[], char b[]);
static unsigned char inb(unsigned short port);
static unsigned int inl(unsigned short port);

    

short screenWidth;
short screenHeight;
int *fb;
unsigned long long backgroundColor = 0x000000;

char text[999] = {0};
unsigned long long consoleColor = 0x0ffcf4;
int textLength = 0;
int lineLength = 0;


void kernel_main(kernelParameters *kp){
    screenWidth = kp->HorizontalResolution;
    screenHeight = kp->VerticalResolution;
    fb = kp->fb;  
    clearScreen();
    
    printPos(200, 200, "Hello From The Kernel \x01             Press Enter To Continue", 0x5DF542);
    int welcomeScreen = 1;
    char string[] = "This Is Not Verry Interesting Yet \x02";
    printPos(200, 300, string , 0x0ffcf4);

    volatile int *start = (volatile int*)0x8000;
    *start = 21;

    unsigned char keycodes[128] = {[0x1E] = 'a', [0x30] = 'b',[0x2E] = 'c',[0x20] = 'd',[0x12] = 'e',[0x21] = 'f',[0x22] = 'g',[0x23] = 'h',[0x17] = 'i',[0x24] = 'j',[0x25] = 'k',[0x26] = 'l',[0x32] = 'm',[0x31] = 'n',[0x18] = 'o',[0x19] = 'p',[0x10] = 'q',[0x13] = 'r',[0x1F] = 's',[0x14] = 't',[0x16] = 'u',[0x2F] = 'v',[0x11] = 'w',[0x2D] = 'x',[0x15] = 'y',[0x2C] = 'z',[0x39] = ' ',[0x0B] = '0',[0x02] = '1',[0x03] = '2',[0x04] = '3',[0x05] = '4',[0x06] = '5',[0x07] = '6',[0x08] = '7',[0x09] = '8',[0x0A] = '9',};    int shiftPressed = 0;
    unsigned char scanCode;

    while (1){
        scanCode = getchar();
        if (welcomeScreen && scanCode == 0x39){
            welcomeScreen=0;
            clearScreen();
        } else {
            if (scanCode & 0x80){                         //key up 
                if (scanCode == 0xAA){                    //shift up
                    shiftPressed = 0;
                }
            } else {                                      //key down
                if (keycodes[scanCode] > 0){
                    if (shiftPressed && scanCode != 0x39) text[textLength] = keycodes[scanCode]-32;
                    else text[textLength] = keycodes[scanCode];
                    lineLength++;
                    textLength++;
                    text[textLength] = '\0';
                    printConsole();
                }else if (scanCode == 0x0E){              //backspace
                    if (textLength >0 && lineLength > 0){
                        textLength--;
                        lineLength--;
                        text[textLength] = '\0';
                        clearScreen();
                        printConsole();                
                    }
                }else if (scanCode == 0x2A){              //shift down
                    shiftPressed = 1;
                }else if (scanCode == 0x1c){              //enter
                    text[textLength] = '\n';
                    textLength++;
                    text[textLength] = '\0';
                    checkCommand(&text[textLength-lineLength-1]);// -1 for \n
                    lineLength = 0;
                    printConsole();
                }
            }
        }
    }
    for(;;);
}

void clearScreen(){
    int totalSize = screenWidth*screenHeight;
    for (int i=0; i<totalSize; i++){
        fb[i] = backgroundColor;
    }
}

unsigned char getchar(){
    //wait until the bit 0 of status port is set
    while (!(inb(0x64) & 1));
    return inb(0x60);
}

void rectangle(int x, int y, int width, int height, int color){
    for (unsigned int xIndex=x; xIndex<x+width; xIndex++){
        for (unsigned int yIndex=y; yIndex<y+height; yIndex++){
            fb[yIndex*screenWidth+xIndex] = color;
        }
    } 
}

void printPos(int x, int y, char* text, int color){
    for (int i=0; i<strLen(text); i++){
        char character = text[i];
        for (char row=0; row<8; row++){
            for (char col=0; col<8; col++){
                if (letters[character][row] & (1 << (7-col))){
                    fb[((y+row)*screenWidth) + (x+col+(i*8))] = color;
                }
            }
        }
    }
}

void printConsole(){
    int ypos = 0;
    int lineIndex = 0;
    int cursorx = 0;
    clearScreen();
    for (int i=0; i<=strLen(text); i++){
        if (text[i] == '\n' || text[i] == '\0'){
            char current = text[i];
            text[i] = '\0';
            int lineLen = strLen(&text[lineIndex]);
            if (lineLen > 0 && (unsigned char)text[lineIndex+lineLen-1] == 0x80){ //error message
                printPos(300, 100+(ypos*10), &text[lineIndex], 0xFF0F0F);
            } else {
                printPos(300, 100+(ypos*10), &text[lineIndex], consoleColor);
            }
            cursorx = strLen(&text[lineIndex]);
            text[i] = current;
            ypos++;
            lineIndex = i+1;
        }
    }
    printPos(300+(cursorx*8), 100+((ypos-1)*10), (char[]){3,'\0'}, 0xFFFFFF); //cursor
}

void consolePrint(char string[]){
    for (int i=0; i<strLen(string); i++){
        text[textLength] = string[i];
        textLength++;
    }
    text[textLength] = '\0';
}

void checkCommand(char input[]){
    char params[10][50] = {{0}};
    int paramsCount = 0;
    int paramLen = 0;
    char command[50] = {0};
    int beforeCommand = 1;
    int commandLen = 0;
    //build the command and parameters
    for (int i=0; i<strLen(input); i++){
        if (beforeCommand && input[i] == '\0') break;
        if (input[i] == ' ' || input[i] =='\n'){
            if (beforeCommand){
                beforeCommand = 0;
                command[commandLen] = '\0';
            } else {
                params[paramsCount][paramLen] = '\0';
                paramsCount++;
                paramLen=0;
            }
        } else if (beforeCommand){ command[commandLen++] = input[i]; }
        else { params[paramsCount][paramLen++] = input[i]; }
    }
    // consolePrint("command I  ");
    // consolePrint(command);
    // consolePrint("\n");
    if (beforeCommand){ return; }
    if (strComp(command, "help")){
        consolePrint("\nwelcome to jOS by josh \x01\nnothing much going on yet\n    some commands to try\n\n"
            "help                 hopefully prints helpfull stuff\n"
            "backgroundColor      lets you change the color of the background with a hex color like 0x00FF00\n"
            "consoleColor         lets you change the color of the console text with a hex color like 0xFF0000\n"
            "i need to add more commands \x02\n\n");
    }
    else if (strComp(command, "backgroundColor")){
        if (paramsCount != 1){ consolePrint("you used the command wrong \x02\x80\ntype help\x80\n"); } 
        else { 
            int print = 1;
            for (int i=2; i<8; i++){
                if (!(params[0][i] >= 48 && params[0][i] <= 57  ||  params[0][i] >= 65 && params[0][i] <= 90)){
                    consolePrint("6 digit hex number needs to use capital letters and start with 0x\x80\n");
                    print = 0;
                    break;
                }
            }
            if (print){ 
                backgroundColor = hexToLongLong(params[0]); 
                consolePrint("sucsessfully changed the background color \x01\n");
            }
        }
        consolePrint("\n");
    }
    else if (strComp(command, "consoleColor")){
        if (paramsCount != 1){ consolePrint("you used the command wrong \x02\x80\ntype help\x80\n"); } 
        else { 
            int print = 1;
            for (int i=2; i<8; i++){
                if (!(params[0][i] >= 48 && params[0][i] <= 57  ||  params[0][i] >= 65 && params[0][i] <= 90)){
                    consolePrint("6 digit hex number needs to use capital letters and start with 0x\x80\n");
                    print = 0;
                    break;
                }
            }
            if (print){
                consoleColor = hexToLongLong(params[0]); 
                consolePrint("sucsessfully changed the console color \x01\n");
            }
        }
        consolePrint("\n");
    } else if (commandLen > 0){
        consolePrint(command);
        consolePrint(" is not a command \x02\n");
    }
}

int strLen(char *string){
    int result = 0;
    while (string[result] != '\0'){ result++; }
    return result;
}

int strComp(char a[], char b[]){
    int i = 0;
    while (a[i] != '\0' && b[i] != '\0'){
        if (a[i] != b[i]) return 0;
        i++;
    }
    return a[i] == b[i];
}

char* intToStr(int integer){
    static char result[20];
    int i = 0;
    if (integer==0){ return "0"; }

    while (integer > 0){
        result[i++] = (integer%10) + '0';
        integer /= 10;
    }
    result[i] = '\0';

    //reverse it
    int start = 0;
    int end = i - 1;
    while (start < end){
        char temp = result[start];
        result[start] = result[end];
        result[end] = temp;
        start++;
        end--;
    }
    return result;
}

unsigned long long hexToLongLong(char hex[]){
    for (int i=2; i<8; i++){
        if (hex[i] >= 48 && hex[i] <= 57){
            hex[i] = hex[i]-48;
        } else if (hex[i] >= 65 && hex[i] <= 90){
            hex[i] = hex[i]-55;
        } else {
            consolePrint("6 digit hex number needs to use capital letters and numbers and start with 0x\x80\n");
            return 0; //error
        }
    }
    return (unsigned long long)(hex[7]) + 
           (unsigned long long)(hex[6]*16) + 
           (unsigned long long)(hex[5]*16*16) + 
           (unsigned long long)(hex[4]*16*16*16) + 
           (unsigned long long)(hex[3]*16*16*16*16) + 
           (unsigned long long)(hex[2]*16*16*16*16*16);
}


static unsigned char inb(unsigned short port){
    unsigned char data;
    asm volatile("inb %1, %0"
                 :"=a"(data)
                 :"Nd"(port)
                 :
                 );
    return data;
}

static unsigned int inl(unsigned short port){
    unsigned int data;
    asm volatile("inl %1, %0"
                 :"=a"(data)
                 :"Nd"(port)
                 :
                 );
    return data;
}
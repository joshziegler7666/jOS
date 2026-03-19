#include "characters.h"

typedef struct {
    unsigned int                      *fb;
    unsigned int                      HorizontalResolution;
    unsigned int                      VerticalResolution;
} kernelParameters;


//load this first at 0x100000
void kernel_main(kernelParameters *kp);
//then load other functions
void rectangle(int x, int y, int width, int height, int color);
void print(int x, int y, char* text, int color);
int strlen(char *string);
    

short screenWidth;
short screenHeight;
int *fb;


void kernel_main(kernelParameters *kp){
	screenWidth = kp->HorizontalResolution;
	screenHeight = kp->VerticalResolution;
	fb = kp->fb;

    rectangle(0, 0, screenWidth, screenHeight, 0x28369E);
    
    print(200, 200, "ABCD", 0x5DF542);

    for(;;);
}

void rectangle(int x, int y, int width, int height, int color){
    for (unsigned int xIndex=x; xIndex<x+width; xIndex++){
        for (unsigned int yIndex=y; yIndex<y+height; yIndex++){
            fb[yIndex*screenWidth+xIndex] = color;
        }
    }
}

void print(int x, int y, char* text, int color){
    for (int i=0; i<strlen(text); i++){
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

int strlen(char *string){
    int result = 0;
    while (string[result] != '\0'){
        result++;
    }
    return result;
}



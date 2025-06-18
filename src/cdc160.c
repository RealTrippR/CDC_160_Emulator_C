#include "CDC_160.h"
#include <stdio.h>
#include <locale.h>
#include <fcntl.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

/*
| Char | Description         | Hex (CP437) | Unicode |
| ---- | ------------------- | ----------- | ------- |
| ╔    | Top-left corner     | 0xC9        | U+2554  |
| ╗    | Top-right corner    | 0xBB        | U+2557  |
| ╝    | Bottom-right corner | 0xBC        | U+255D  |
| ╚    | Bottom-left corner  | 0xC8        | U+255A  |
| ═    | Horizontal line     | 0xCD        | U+2550  |
| ║    | Vertical line       | 0xBA        | U+2551  |
| ╠    | Left T junction     | 0xCC        | U+2560  |
| ╣    | Right T junction    | 0xB9        | U+2563  |
| ╦    | Top T junction      | 0xCB        | U+2566  |
| ╩    | Bottom T junction   | 0xCA        | U+2569  |
| ╬    | Cross junction      | 0xCE        | U+256C  |
*/

void setDigitOfReg(uint8_t option, uint8_t val, struct CDC_160* cdc) {
    Word12* regToFlip = NULL;
    uint8_t digitToSet = 0;
    if (option >= 0 && option < 4) {
        digitToSet = option;
        regToFlip = &cdc->proc.regP;
    }
    else if (option > 3 && option < 8) {
        digitToSet = option - 4;
        regToFlip = &cdc->proc.regA;
    }
    else if (option > 7 && option < 12) {
        digitToSet = option - 8;
        regToFlip = &cdc->proc.regZ;
    }

    digitToSet = 3 - digitToSet;
    if (regToFlip) {
        if (val < 8) {
            Word12 bitmask = 07777;
            bitmask ^= 07 << digitToSet * 3;

            *regToFlip = *regToFlip & bitmask;
            *regToFlip |= val << digitToSet * 3;
        }
    }
}

void runOption(int16_t option, struct CDC_160* cdc) 
{
    switch (option) 
    {
    case 0 + 12:
        CDC_160_TurnOn(cdc);
        break;
    case 1 + 12:
        CDC_160_TurnOff(cdc);
        break;
    case 2 + 12:
        CDC_160_RunMode(cdc);
        break;
    case 3 + 12:
        CDC_160_PauseMode(cdc);
        break;
    case 4 + 12:
        CDC_160_StepMode(cdc);
        break;
    case 5 + 12:
        CDC_160_Load(cdc);
        break;
    case 6 + 12:
        CDC_160_Clear(cdc);
        break;
    }
}

void printReg(char* buff8, uint8_t highlightedDigit, bool isRed, bool __blink) 
{
    static size_t msWaitCounter = 0x0;
    static unsigned long timeLastMS = 0x0;
    static bool showBar = true;
    static bool sleeping = false;
   
    if (__blink == false) {
        sleeping = false;
        showBar = true;
        msWaitCounter = 0x0;
    }
    else {
        if (!sleeping) {
            showBar = !showBar;
        }
        sleepmsNonBlocking(400lu, &msWaitCounter, &timeLastMS, &sleeping);
        
    }
    timeLastMS = getTickCount32();




    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    printf(ANSI_COLOR_RESET);
    for (uint8_t i = 0; i < 4; ++i) 
    {
        if (highlightedDigit == i) {
            if (isRed) {
                if (showBar) {
                    SetConsoleTextAttribute(hConsole,
                        BACKGROUND_RED);
                } else{
                    SetConsoleTextAttribute(hConsole,
                        FOREGROUND_RED);
                }
            }
            else {
                if (showBar) {
                    SetConsoleTextAttribute(hConsole,
                        BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE);
                }
                else {
                    SetConsoleTextAttribute(hConsole,
                        FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
                }
            }
        }
        else {
            if (isRed) {
                SetConsoleTextAttribute(hConsole,
                    FOREGROUND_RED);
            }
            else {
                SetConsoleTextAttribute(hConsole,
                    FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
            }
        }

        printf("%c", buff8[i]);
    }
    SetConsoleTextAttribute(hConsole,
        FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}

char lines[100][101];
uint32_t curline;

void CDC_160_InitCLI() 
{
    // set DOS mode
    SetConsoleOutputCP(437);
    SetConsoleCP(437);
    memset(lines, 0, sizeof(lines));

    curline = 0;
    snprintf(lines[curline], 100, "\xC9\xCD\xCD\xCD\xCB\xCD\xCD\xCD\xCD\xCB\xCD\xCD"
        "\xCD\xCD\xCB\xCD\xCD\xCD\xCD\xBB");

    curline = 2;
    snprintf(lines[curline], 100, "\xCC\xCD\xCD\xCD\xCA\xCD\xCD\xCD\xCD\xCA\xCD\xCD"
        "\xCD\xCD\xCA\xCD\xCD\xCD\xCD\xB9");
    curline = 3;
    snprintf(lines[curline], 100, "\xBASTT  P    A    Z  \xBA");

    curline = 4;
    snprintf(lines[curline], 100, "\xC8\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD"
        "\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xBC");

    curline = 5;
    snprintf(lines[curline], 100, "ON");
    curline = 6;
    snprintf(lines[curline], 100, "OFF");
    curline = 7;
    snprintf(lines[curline], 100, "RUN");
    curline = 8;
    snprintf(lines[curline], 100, "PAUSE");
    curline = 9;
    snprintf(lines[curline], 100, "STEP");
    curline = 10;
    snprintf(lines[curline], 100, "LOAD");
    curline = 11;
    snprintf(lines[curline], 100, "CLEAR");
    curline = 12;
    snprintf(lines[curline], 100, "QUIT");
    curline = 13;
}

bool CDC_160_PrintCLI(struct CDC_160* cdc)
{
    bool retval = true;
    curline = 0u;

    struct Processor* proc = &cdc->proc;

    char p[8];
    char a[8];
    char z[8];

    if (cdc->on) {
        Word12toStrOctalFillZeros(proc->regP, p);
        Word12toStrOctalFillZeros(proc->regA, a);
        Word12toStrOctalFillZeros(proc->regZ, z);
    }
    else {
        memcpy_s(p, 5, "    ", 5);
        memcpy_s(a, 5, "    ", 5);
        memcpy_s(z, 5, "    ", 5);
    }


    char statusColor[10] = { 0 };
    char errcode[4] = "\0\0\0\0";
    if (cdc->on) {
        memcpy_s(errcode, 4, "\xB2\xB2\xB2", 4);
    }
    else {
        memcpy_s(errcode, 4, "   ", 4);
    }
    
    if (cdc->on && (cdc->selectFailure == true || cdc->stepMode !=0)){
        strcpy_s(statusColor, 10, ANSI_COLOR_RED);
        if (cdc->selectFailure) {
            memcpy(errcode, "SEL", 3);
        }
    }
    else {
        strcpy_s(statusColor, 10, ANSI_COLOR_GREEN);
    }
    

    static bool waitingForNumberInput = false;
    static int16_t option = 0;
    if (_kbhit()) {
        static int16_t dig = -1;
        int ch = _getch(); 
        if (waitingForNumberInput && ch != 13 && ch < '8') {
            dig = ch - '0';
            setDigitOfReg(option, dig, cdc);
            waitingForNumberInput = false;
            dig = -1;
        }

        if (ch == 13) {
            if (option < 12) {
                waitingForNumberInput = true;
            }
            else {
                runOption(option, cdc);
                if (option == 19) {
                    retval = false;
                }
            }
        }

        if (ch == 224) {
            int ext = _getch(); // key ext. key code
            if (ext == 72) {

                option--; // up arrow
            }
            else if (ext == 80) {
                option++; // down arrow
            }
        }

        if (option < 0) {
            option = 0;
        }
        if (option > 19) {
            option = 19;
        }
    }    






    if (!cdc->on) {
        waitingForNumberInput = false;
    }

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hConsole, &cursorInfo);
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(hConsole, &cursorInfo);



    for (size_t i = 0; i < 100; ++i) {
        if (lines[i][0] != '\0' || i==1) {
            SetConsoleTextAttribute(hConsole,
                FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
            if (option >= 0) {
                if (option >= 12 && i + 7 == option) {
                    SetConsoleTextAttribute(hConsole,
                        BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE);
                }
            }


            if (i == 1) {
               /* snprintf(lines[curline], 100,
                    ANSI_COLOR_RESET "\xBA%s%s"
                    ANSI_COLOR_RESET "\xBA%s%04s"
                    ANSI_COLOR_RESET "\xBA%s%04s"
                    ANSI_COLOR_RESET "\xBA%04s\xBA", statusColor, errcode, isred, p, isred, a, z);*/
                printf("\xBA%s%s" ANSI_COLOR_RESET "\xBA", statusColor, errcode);

                printReg(p, option,!cdc->readyToOperate && cdc->on, waitingForNumberInput);
                printf(ANSI_COLOR_RESET"\xBA");
                printReg(a, option - 4, !cdc->readyToOperate && cdc->on, waitingForNumberInput);
                printf(ANSI_COLOR_RESET"\xBA");
                printReg(z, option - 8, false, waitingForNumberInput);
                printf(ANSI_COLOR_RESET"\xBA");

                printf("\n");
            }
            else {
                printf("%s\n", lines[i]);
            }

            SetConsoleTextAttribute(hConsole,
                FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        }
    }

    printf("\033[15A\r");


    return retval;

}
void CDC_160_SaveStateToDisk(struct CDC_160* cdc, const char* stateFilePath)
{
    FILE* fptr;
    fopen_s(&fptr, stateFilePath, "wb");


    if (fptr) {
        uint8_t data[sizeof(cdc->proc) + 4096];
        memset(data, 0, sizeof(data));

        memcpy(data, &cdc->proc, sizeof(cdc->proc));
        memcpy(data + sizeof(cdc->proc), cdc->mem.data, 4096);

        fwrite(data, sizeof(data), 1, fptr);

        fclose(fptr);
    }
}

void CDC_160_LoadStateFromDisk(struct CDC_160* cdc, const char* stateFilePath)
{
    FILE* fptr;
    fopen_s(&fptr, stateFilePath, "r");


    if (fptr) {
        uint8_t data[sizeof(cdc->proc) + 4096];
        fread_s(data, sizeof(data), 4096, 1, fptr);
        
        memcpy(&cdc->proc, data, sizeof(cdc->proc));
        memcpy(cdc->mem.data, data + sizeof(cdc->proc), 4096);

        fclose(fptr);
    }
}
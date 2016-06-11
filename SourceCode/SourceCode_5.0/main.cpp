
#include <conio.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#include "sqlite3.h"

#define   WINDOW_SIZE_WIDTH     0x00000050
#define   WINDOW_SIZE_HEIGHT    0x00000019

#define   VAR_START_SYMBOL      0x0000007B
#define   VAR_END_SYMBOL        0x0000007D
#define   VAR_VALUE_SYMBOL      0x0000007C


//-- Global Variable ---------------------------------------------------------------------------------------

char   gInterfaceVarFilePathFormat[]  = ".\\Interface\\%s.var";
char   gDataBaseFilePath[]            = ".\\DataBase\\DataBase.db3";
char   gDataBaseJournalFilePath[]     = ".\\DataBase\\DataBase.db3-journal";
char   gWgetFilePath[]                = ".\\Command\\wget.exe";
char   gTempFilePath[]                = ".\\TempFile\\TempFile.var";

char   g8ComicBaseUrl[]               = "http://www.8comic.com/";
char   gAllComicCodeFilePath[]        = ".\\JavaScrip\\AllComicCode.js";
char   gComicviewFileUrl[]            = "http://www.8comic.com/js/comicview.js";
char   gHtmlFormatUrl[]               = "http://www.8comic.com/html/%s.html";
char   gShowKeyStartKeyFormat[]       = "cview('%s-1.html',";
char   gShowKeyEndKeyFormat[]         = ");";
char   gCodesStartKey[]               = "codes=\"";
char   gCodesEndKey[]                 = "\".split('|')";

char   gBrowserIndex[]                = ".\\index.html";

int    gKeyCodeLength                 = 3;
char   gPicNameFormat[]               = "%03d_%s.jpg";
char   gPicUrlFormat[]                = "http://img%s.8comic.com/%s/%s/%s/%s";
char   gPicDownloadFolderPathFormat[] = ".\\Download\\%s\\第%s章節\\";

char   gDownloadFolderPath[]          = ".\\Download\\";

char   gOAComicBookAnnouncementUrl[]  = "https://dl.dropbox.com/u/5842721/My%20Works/OA_ComicBook/Announcement.var";
char   gThisVersion[]                 = "ComicBook_5.0_F";

char   gOAComicBookExecutableName[]   = "OA_ComicBook.exe";


//-- Set Code Type -----------------------------------------------------------------------------------------

char* w2m (const wchar_t* wcs) {
  /*width -> many Unicode - ANSI*/
  int len;
  char* buf;
  len =wcstombs(NULL,wcs,0);
  if (len == 0)
    return NULL;
  buf = (char *)malloc(sizeof(char)*(len+1));
  memset(buf, 0, sizeof(char) *(len+1));
  len =wcstombs(buf,wcs,len+1);
  return buf;
}

wchar_t* m2w (const char* mbs) {
  /*many -> width ANSI - Unicode*/
  int len;
  wchar_t* buf;
  len =mbstowcs(NULL,mbs,0);
  if (len == 0)
    return NULL;
  buf = (wchar_t *)malloc(sizeof(wchar_t)*(len+1));
  memset(buf, 0, sizeof(wchar_t) *(len+1));
  len =mbstowcs(buf,mbs,len+1);
  return buf;
}

wchar_t * ANSIToUnicode (const char* str ) {
  int textlen ;
  wchar_t * result;
  textlen = MultiByteToWideChar( CP_ACP, 0, str,-1, NULL,0 );
  result = (wchar_t *)malloc((textlen+1)*sizeof(wchar_t));
  memset(result,0,(textlen+1)*sizeof(wchar_t));
  MultiByteToWideChar(CP_ACP, 0,str,-1,(LPWSTR)result,textlen );
  return result;
}

char * UnicodeToANSI (const wchar_t* str ) {
  char* result;
  int textlen;
  textlen = WideCharToMultiByte( CP_ACP, 0, str, -1, NULL, 0, NULL, NULL );
  result =(char *)malloc((textlen+1)*sizeof(char));
  memset( result, 0, sizeof(char) * ( textlen + 1 ) );
  WideCharToMultiByte( CP_ACP, 0, str, -1, result, textlen, NULL, NULL );
  return result;
}

wchar_t * UTF8ToUnicode (const char* str ) {
  int textlen ;
  wchar_t * result;
  textlen = MultiByteToWideChar( CP_UTF8, 0, str,-1, NULL,0 );
  result = (wchar_t *)malloc((textlen+1)*sizeof(wchar_t));
  memset(result,0,(textlen+1)*sizeof(wchar_t));
  MultiByteToWideChar(CP_UTF8, 0,str,-1,(LPWSTR)result,textlen );
  return result;
}

char * UnicodeToUTF8 (const wchar_t* str ) {
  char* result;
  int textlen;
  textlen = WideCharToMultiByte( CP_UTF8, 0, str, -1, NULL, 0, NULL, NULL );
  result =(char *)malloc((textlen+1)*sizeof(char));
  memset(result, 0, sizeof(char) * ( textlen + 1 ) );
  WideCharToMultiByte( CP_UTF8, 0, str, -1, result, textlen, NULL, NULL );
  return result;
}

char* ANSIToUTF8 (const char* str) {
  return UnicodeToUTF8(ANSIToUnicode(str));
}

char* UTF8ToANSI (const char* str) {
  return UnicodeToANSI(UTF8ToUnicode(str));
}


//-- Set Screen Window Cursor ------------------------------------------------------------------------------

void GetWindowSize (int* x, int* y) {
  CONSOLE_SCREEN_BUFFER_INFO    CSBufferInfo;
  GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &CSBufferInfo);

  *x = CSBufferInfo.srWindow.Right - CSBufferInfo.srWindow.Left + 1;
  *y = CSBufferInfo.srWindow.Bottom - CSBufferInfo.srWindow.Top + 1;
}

bool SetBufferSize (int x, int y) {
  COORD size;
  size.X = x;
  size.Y = y;
  return SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), size);
}

bool SetWindowPosition (int L, int T, int R, int B) {
  _SMALL_RECT rect = {L, T, R, B};
  return SetConsoleWindowInfo (GetStdHandle(STD_OUTPUT_HANDLE),true ,&rect);
}

void GetBufferSize (int* x, int* y) {
  CONSOLE_SCREEN_BUFFER_INFO    CSBufferInfo;
  GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &CSBufferInfo);

  *x = CSBufferInfo.dwSize.X;
  *y = CSBufferInfo.dwSize.Y;
}

void WindowMaxSize (int* x, int* y) {
  int bx=0,by=0;
  GetBufferSize (&bx, &by);
  int wx=0,wy=0;
  GetWindowSize (&wx, &wy);
  SetBufferSize (9999, 9999);

  for ( *x=3000;*x>0 && !SetWindowPosition(0,0,*x,0);(*x)--);
  SetWindowPosition (0,0,wx-1,wy-1);

  for ( *y=2000;*y>0 && !SetWindowPosition(0,0,0,*y);(*y)--);
  SetWindowPosition (0,0,wx-1,wy-1);

  SetBufferSize (bx, by);
  system ("CLS");
  (*x)++;
  (*y)++;
}

void BufferMinSize (int* x, int* y) {
  int bx=0,by=0;
  GetBufferSize (&bx, &by);
  int wx=0,wy=0;
  GetWindowSize (&wx, &wy);
  SetWindowPosition(0,0,0,0);

  for ( *x=1;*x<bx && !SetBufferSize(*x,by);(*x)++);
  for ( *y=1;*y<by && !SetBufferSize(bx,*y);(*y)++);

  SetBufferSize(bx,by);
  SetWindowPosition(0,0,wx-1,wy-1);
}

void SetWindowBufferSize (int x, int y) {

  int MaxWinX=0,MaxWinY=0;
  int MinBufX=0,MinBufY=0;
  WindowMaxSize(&MaxWinX,&MaxWinY);
  BufferMinSize(&MinBufX,&MinBufY);

  x = x > MaxWinX ? MaxWinX : x;
  y = y > MaxWinY ? MaxWinY : y;

  x = x > 14 ? x : 14;
  y = y > 1  ? y : 1;

  SetBufferSize (x,y);
  SetWindowPosition (0,0,x-1,y-1);
  SetBufferSize (x,y);
  SetWindowPosition (0,0,x-1,y-1);
}

void SetCursorPosition (int x, int y) {
  COORD coord = {x, y};
  SetConsoleCursorPosition ( GetStdHandle ( STD_OUTPUT_HANDLE ), coord );
}

void SetCursorVisible (bool IsShow) {
  CONSOLE_CURSOR_INFO ConCurInf;
  ConCurInf.dwSize = 10;
  ConCurInf.bVisible = IsShow;
  SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &ConCurInf);
}

void GetCursorPosition (int* x, int* y) {
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE),&csbi);
  *x = csbi.dwCursorPosition.X;
  *y = csbi.dwCursorPosition.Y;
}

void SetBufferWindowSameSize (int x, int y) {
  SetBufferSize (x,y);
  SetWindowPosition (0,0,x-1,y-1);
  SetBufferSize (x,y);
  SetWindowPosition (0,0,x-1,y-1);
}

char GetPositionchar (int x, int y) {
  DWORD nChr;
  char Char;
  COORD size = {x, y};
  ReadConsoleOutputCharacter(GetStdHandle(STD_OUTPUT_HANDLE), &Char, 1, size, &nChr);
  return Char;
}

void ClearRangeScreen (int x1, int y1, int x2, int y2, bool Isfull) {
  int bx=0,by=0;
  GetBufferSize (&bx,&by);

  int nx=0,ny=0;
  GetCursorPosition (&nx,&ny);


  for (int i = y2; i >= y1; i--) {
    for (int j = bx - 1; j >= 0; j--) {
      if ( ((i < y2) || (j <= x2 - 1)) && ((i > y1) || (j >= x1))) {
        SetCursorPosition (j,i);
        printf (" \b");
      }
    }
  }

  if (Isfull) {
    int nnx=0,nny=0;
    GetCursorPosition (&nnx,&nny);
    if (nnx != 0)
      printf ("\n");

    for (int i = y2; (i < ny && nx == 0) || (i <= ny && nx != 0); i++) {
      for (int j = 0; j < bx; j++) {
          printf ("%c", GetPositionchar (j,i));
      }
    }

    nnx=0,nny=0;
    GetCursorPosition (&nnx,&nny);
    SetCursorPosition (nx,ny);

    for (int i = ny; i >= nny; i--) {
      for (int j = bx - 1; j >= 0; j--) {
        if ( ((i < ny) || (j <= nx - 1)) && ((i > nny) || (j >= nnx))) {
          SetCursorPosition (j,i);
          printf (" \b");
        }
      }
    }
  }
  else {
    SetCursorPosition (nx,ny);
  }
}

void ClearScreenToKey (char* Key) {
  int bx=0,by=0;
  GetBufferSize (&bx,&by);
  int nx=0,ny=0;
  GetCursorPosition (&nx,&ny);

  int kx=-1,ky=-1;
  int count = strlen (Key) - 1;

  for (int i = ny; i >= 0; i--) {
    for (int j = bx - 1; j >= 0; j--) {
      char c = GetPositionchar (j,i);
      if (c == Key[count]) {
        count--;
      }
      else {
        count = strlen (Key) - 1;
      }
      if (count < 0) {
        kx=j;
        ky=i;
        j = 0;
        i = 0;
      }
    }
  }

  if (kx!=-1 && ky!=-1) {
    for (int i = ny; i >= ky; i--) {
      for (int j = bx - 1; j >= 0; j--) {
        SetCursorPosition (j,i);
        if (!(j == bx - 1 && i == ny))
          printf (" ");
        printf ("\b");
      }
    }
  }
}


//-- Public Function --------------------------------------------------------------------------------------

bool Free (void** Object) {
  if ((*Object) != NULL)
    free (*Object);
  (*Object) = NULL;
}

int GetIntLength (int Num) {
  int Index = 1;
  for (Index = 1; (Num = Num / 10) > 0 ; Index++);
  return Index;
}

int GetFileSize (char* FileName) {
  int Size = 0;
  FILE *FilePoint = NULL;
  if (FileName != NULL) {
    if ((FilePoint = fopen (FileName, "r")) != NULL) {
      fseek(FilePoint, 0, SEEK_END);
      Size = ftell(FilePoint);
      fseek(FilePoint, 0, SEEK_SET);
      fclose(FilePoint);
    }
  }
  return Size;
}

char* MergeString (char* Format, ...) {
  char* OutString = NULL;
  if (Format != NULL) {
    int      Length = 0;
    char*    FormatPoint = Format;
    va_list  args;
    char*    TempString;

    char*    ptr = NULL;
    int l = 0;
    int ShowNum = 0;
    char* ShowString = NULL;
    bool IsZero = false;

    va_start (args, *Format);
    for (; *FormatPoint != '\0'; *FormatPoint++)
      if (*FormatPoint == '%') {
        switch (*++FormatPoint) {
          case 's':
            TempString = va_arg(args, char *);
            if (TempString != NULL)
              Length = Length + strlen (TempString);
            break;
          case 'd':
            Length = Length + GetIntLength (va_arg(args, int));
            break;
          case '%':
            Length++;
            break;
          case '0': case '1': case '2': case '3': case '4':
          case '5': case '6': case '7': case '8': case '9':
            ptr = FormatPoint;
            for (l = 0; *ptr < 0x3A && *ptr > 0x2F; *++ptr)
              l = l * 10 + (*ptr - 0x30);

            if (*ptr == 'd') {
              l = (l > GetIntLength(va_arg(args, int))? l:0);
            }
            else if (*ptr == 's') {
              l = (l > strlen(va_arg(args, char*))? l:0);
            }

            Length = Length + l;

            FormatPoint = ptr;
            break;
        }
      }
      else {
        Length++;
      }
    va_end(args);


    OutString = (char*) malloc (sizeof (char) * (Length + 1));
    strcpy(OutString, "");

    va_start(args, *Format);
    for (int NowLength = 0; *Format != '\0'; *Format++)
      if (*Format == '%') {
        switch (*++Format) {
          case 's':
            TempString = va_arg(args, char *);
            if (TempString != NULL)
              sprintf (OutString, "%s%s", OutString, TempString);
            break;
          case 'd':
            sprintf (OutString, "%s%d", OutString, va_arg(args, int));
            break;
          case '%':
            sprintf (OutString, "%s%%", OutString);
            break;

          case '0': case '1': case '2': case '3': case '4':
          case '5': case '6': case '7': case '8': case '9':
            if (*Format == '0')
              IsZero = true;

            for (l = 0; *Format < 0x3A && *Format > 0x2F; *++Format)
              l = l * 10 + (*Format - 0x30);

            if (*Format == 'd') {
              ShowNum = va_arg(args, int);
              for (int i=0;(i < (l - GetIntLength(ShowNum))) && (l > GetIntLength(ShowNum));i++) {
                sprintf (OutString, "%s%s", OutString,IsZero?"0":" ");
              }
              sprintf (OutString, "%s%d", OutString, ShowNum);
            }
            else if (*Format == 's') {
              ShowString = va_arg(args, char*);

              for (int i=0;(i < (l - strlen(ShowString))) && (l > strlen(ShowString));i++) {
                sprintf (OutString, "%s%s", OutString,IsZero?"0":" ");
              }
              sprintf (OutString, "%s%s", OutString, ShowString);
            }

            break;
        }
      }
      else {
        NowLength = strlen (OutString);
        OutString[NowLength++] = *Format;
        OutString[NowLength++] = '\0';
      }
    return OutString;
    va_end(args);
  }
}

void Print (char* StringFormat, ...) {
  char *ptr = StringFormat;
  va_list args;

  va_start(args, StringFormat);
  int l = 0;
  int j = 0;
  int ShowNum = 0;
  char* ShowString = NULL;
  char *nptr;

  for (; *ptr != '\0'; *ptr++) {
    switch (*ptr) {
      case '\\':
        switch (*++ptr) {
          case 'n':
            Print ("\n");
            break;
        }
        break;

      case '%':
        switch (*++ptr) {
          case 's':
            Print (va_arg(args, char *));
            break;

          case 'd':
            printf ("%d", va_arg(args, int));
            break;

          case '0': case '1': case '2': case '3': case '4':
          case '5': case '6': case '7': case '8': case '9':
            nptr = ptr;

            for (l = 0; *ptr < 0x3A && *ptr > 0x2F; *++ptr)
              l = l * 10 + (*ptr - 0x30);

            if (*ptr == 'd') {
              ShowNum = va_arg(args, int);

              if (*(ptr+1) == '<') {
                Print ("%d", ShowNum);
                for (j = 0; (l > GetIntLength(ShowNum)) && (j < l - GetIntLength(ShowNum)); j++)
                  if (*nptr == '0') printf ("0");
                  else printf (" ");
                *ptr++;
              }
              else if (*(ptr+1) == '|') {
                for (j = 0; (l > GetIntLength(ShowNum)) && (j < ((l - GetIntLength(ShowNum)) / 2)); j++)
                  if (*nptr == '0') printf ("0");
                  else printf (" ");
                Print ("%d", ShowNum);
                for (j = 0; (l > GetIntLength(ShowNum)) && (j < ((l - GetIntLength(ShowNum)) / 2)); j++)
                  if (*nptr == '0') printf ("0");
                  else printf (" ");
                if ((l > GetIntLength(ShowNum)) && (GetIntLength(ShowNum)% 2) == 1)
                  printf (" ");
                *ptr++;
              }
              else {
                for (j = 0; (l > GetIntLength(ShowNum)) && (j < l - GetIntLength(ShowNum)); j++)
                  if (*nptr == '0') printf ("0");
                  else printf (" ");
                Print ("%d", ShowNum);
              }
            }
            else if (*ptr == 's'){
              ShowString = va_arg(args, char *);

              if (*(ptr+1) == '<') {
                Print ("%s", ShowString);
                for (j = 0; (l > strlen(ShowString)) && (j < l - strlen(ShowString)); j++)
                  if (*nptr == '0') printf ("0");
                  else printf (" ");
                *ptr++;
              }
              else if (*(ptr+1) == '|') {
                for (j = 0; (l > strlen(ShowString)) && (j < ((l - strlen(ShowString)) / 2)); j++)
                  if (*nptr == '0') printf ("0");
                  else printf (" ");
                Print ("%s", (ShowString));
                for (j = 0; (l > strlen(ShowString)) && (j < ((l - strlen(ShowString)) / 2)); j++)
                  if (*nptr == '0') printf ("0");
                  else printf (" ");
                if ((l > strlen(ShowString)) && (strlen(ShowString)% 2) == 1)
                  printf (" ");
                *ptr++;
              }
              else {
                for (j = 0; (l > strlen(ShowString)) && (j < l - strlen(ShowString)); j++)
                  if (*nptr == '0') printf ("0");
                  else printf (" ");
                Print ("%s", ShowString);
              }
            }
            else{
              ptr = nptr;
              putchar(*ptr);
            }
            break;
          case '%':
            printf ("%%");
            break;
          case '=':
            char KeyChar[] = "nbgcrpyw";
            int Color[2] = {0};

            for (int Index = 0, Light = 0; Index < 2; Index++) {
              Color[Index] = (int)*++ptr;

              if ((Color[Index] < 0x61) && (Color[Index] != 'N')) Light = 0x08;
              else Light = 0x00;

              Color[Index] |= 0x20;

              for (int KeyIndex = 0; KeyIndex < strlen(KeyChar); KeyIndex++)
                if (Color[Index] == KeyChar[KeyIndex])
                  Color[Index] = Light | KeyIndex ;
            }
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (Color[0] << 4) | Color[1]);
            break;
        }
        break;

      default :
        putchar(*ptr);
        break;
    }
  }
  va_end(args);
}

char* ReadVarFile (char* FilePath, char* Key, int ValueIndex) {
  char* Value = NULL;
  if (ValueIndex > -1) {
    int    FileSize = 0;
    FileSize = GetFileSize (FilePath);

    if (FileSize > 0) {
      FILE   *FilePoint;
      if ((FilePoint = fopen (FilePath, "r")) != NULL) {
        char* Article = NULL;
        Article = (char*) malloc(sizeof (char) * (FileSize + 1));

        int Index = 0;
        for (Index = 0; (Article[Index]=getc(FilePoint)) != EOF; Index++);
        Article[Index] = '\0';
        fclose (FilePoint);


        for (Index = 0; Index < strlen (Article); Index++) {
          int StartSymbolPoint = 0;
          int EndSymbolPoint = 0;

          if (Article[Index] == VAR_START_SYMBOL){
            StartSymbolPoint = Index;
            for (Index++; Index < strlen (Article); Index++) {
              if (Article[Index] == VAR_END_SYMBOL){
                EndSymbolPoint = Index;

                for (StartSymbolPoint++; Article[StartSymbolPoint] == ' '; StartSymbolPoint++);
                for (EndSymbolPoint--; Article[EndSymbolPoint] == ' '; EndSymbolPoint--);

                if (EndSymbolPoint - StartSymbolPoint > -1) {
                  int TempStrLength = EndSymbolPoint - StartSymbolPoint + 1;
                  char *TempString = NULL;
                  TempString = (char*) malloc (sizeof (char) * (TempStrLength + 1));

                  for (int TempIndex = StartSymbolPoint, TempStrLength = 0; TempIndex <= EndSymbolPoint; TempIndex++)
                    TempString[TempStrLength++] = Article[TempIndex];
                  TempString[TempStrLength++] = '\0';

                  if (strcmp (TempString, Key) == 0) {

                    for (EndSymbolPoint++; Article[EndSymbolPoint] == ' '; EndSymbolPoint++);

                    for (Index++; Index < strlen (Article); Index++) {
                      StartSymbolPoint = Index;
                      if (Article[Index] == VAR_START_SYMBOL) {
                        break;
                      }
                    }

                    int StartPoint = 0;
                    int EndPoint   = 0;
                    int ValueCount = 0;
                    for (int TempIndex = EndSymbolPoint; TempIndex <= StartSymbolPoint; TempIndex++) {
                      StartPoint = TempIndex;

                      for (TempIndex++; TempIndex <= StartSymbolPoint; TempIndex++) {
                        if (Article[TempIndex] == VAR_VALUE_SYMBOL) {

                          if ((ValueCount++) == ValueIndex) {
                            EndPoint = TempIndex;

                            for (StartPoint++; Article[StartPoint] == ' '; StartPoint++);
                            for (EndPoint--; Article[EndPoint] == ' '; EndPoint--);

                            int ValueLength = EndPoint - StartPoint + 1;

                            Value = (char*) malloc (sizeof (char) * (ValueLength + 1));

                            for (int Index = StartPoint, ValueLength = 0; Index <= EndPoint; Index++)
                              Value[ValueLength++] = Article[Index];
                            Value[ValueLength++] = '\0';

                            Index = strlen (Article);
                            TempIndex = StartSymbolPoint;
                          }
                          TempIndex--;
                          break;
                        }
                      }
                    }
                  }
                  free (TempString);
                }
                break;
              }
            }
          }
        }
        free (Article);
      }
    }
  }
  return Value;
}

bool FileIsExists (char* FileName) {
  FILE   *FilePoint;
  if ((FilePoint = fopen(FileName, "r")) != NULL) {
    fclose(FilePoint);
    return true;
  }
  return false;
}

bool IsConnectedToInternet () {
  system ("CLS");
  Print (" ==> 測試網路中...\n");

  bool IsConnected = false;
  if (system("ping -n 1 www.gogle.com") == 0 || system("ping -n 1 www.yahoo.com.tw") == 0)
    IsConnected = true;
  else
    IsConnected = false;

  Print (" ==> 測試結束!\n");
  system ("CLS");

  return IsConnected;
}

void ClearFile (char* FileName){
  FILE* FilePoint = NULL;
  if ((FilePoint = fopen (FileName, "w")) != NULL) {
    fprintf (FilePoint, "");
    fclose (FilePoint);
  }
}

bool RunWgetCommand (char* WgetFile, char* Url, char* FileName, int MaxRunTime, bool IsSsl){
  Print ("%=Nr  ======> Start Run Command To Get Data..%=Nw\n");
  Print ("%=NN<ClearPoint>%=Nw\n");

  ClearFile(FileName);

  char* Command = NULL;
  if (IsSsl)
    Command = MergeString ("%s %s -O %s --no-check-certificate", WgetFile, Url, FileName);
  else
    Command = MergeString ("%s %s -O %s", WgetFile, Url, FileName);

  bool                  Issuccess            = false;

  if (MaxRunTime > 0) {
    STARTUPINFO           t_StartupInfo        = {0};
    PROCESS_INFORMATION   t_ProcessInformation = {0};

    t_StartupInfo.cb = sizeof(t_StartupInfo);
    bool bSuccess = CreateProcess (NULL, Command, NULL, NULL, TRUE, NORMAL_PRIORITY_CLASS, NULL, NULL, &t_StartupInfo, &t_ProcessInformation);

    if (bSuccess){
      DWORD dwRect = WaitForSingleObject(t_ProcessInformation.hProcess, MaxRunTime * 1000);
      switch (dwRect){
        case WAIT_OBJECT_0:
          Issuccess = true;
          break;

        case WAIT_TIMEOUT:
          Issuccess = false;
          TerminateProcess(t_ProcessInformation.hProcess, 0);
          break;
      }
    }
  }
  else {
    Issuccess = true;
    system (Command);
  }

  free (Command);
  ClearScreenToKey("<ClearPoint>");

  if (Issuccess)
    Print ("%=Nr  ======> Run Command And Get Data Success!%=Nw\n");
  else
    Print ("%=Ny  ======> Run Command And Get Data Failure!%=Nw\n");

  return Issuccess;
}

void OpenFolder (char* FolderPath) {
  char* OpenDownloadFolderCommand = MergeString ("start %s", FolderPath);
  system(OpenDownloadFolderCommand);
  Free ((void**) &OpenDownloadFolderCommand);
}

void ClearFolder (char* Path) {
  Print ("%=NN<ClearPoint>%=Nw");

  char* DelCommand = MergeString ("DEL /Q %s", Path);
  system(DelCommand);
  Free ((void**) &DelCommand);

  char* RDCommand = MergeString ("RD /S /Q %s", Path);
  system(RDCommand);
  Free ((void**) &RDCommand);

  char* MDCommand = MergeString ("MD %s", Path);
  system(MDCommand);
  Free ((void**) &MDCommand);

  ClearScreenToKey("<ClearPoint>");
}

void ProgressBarTop (int Num, char Title[]){
  int  BarWidth = 58;
  int  Percent  = ((Num>100?100:Num) * 100) / 100;
  int  Barsite  = (Percent * BarWidth) /100;
  int  Index    = 0;
  char *Bar     = NULL;

  if (Barsite > BarWidth)
    Barsite = BarWidth;

  Bar = (char* ) malloc (sizeof (char) * (BarWidth + 1));

  for (Index = 0 ;Index < BarWidth; Index++)
    if (Index + 1 < Barsite )
      Bar[Index] = '=';
    else
      if (Index + 1 == Barsite)
        Bar[Index] = '>';
      else
        Bar[Index] = ' ';
  Bar[Index] = '\0';

  int nx = 0,ny = 0;
  GetCursorPosition(&nx, &ny);
  SetCursorPosition(0,0);

  Print ("%=NP  %10s [%s] %3d%% %=Nw", Title, Bar, Percent);
  Print ("\n --===--------------------------------------------------------------------===--");

  SetCursorPosition(nx,ny);

  for (int i = 0; i < 2 - ny; i++)
    Print ("\n");

  Free ((void** ) &Bar);
}

//-- Interface ---------------------------------------------------------------------------------------------

typedef struct   _INTERFACE_                 INTERFACE;
typedef struct   _INTERFACE_MESSAGE_         INTERFACE_MESSAGE;
typedef struct   _ITEM_                      ITEM;

typedef struct _INTERFACE_ {
  char*                 Key;
  char*                 MessageTitle;
  char*                 ItemTitle;
  char*                 Explain_1;
  char*                 Explain_2;
  char*                 Explain_3;
  char*                 Explain_4;
  char*                 Symbol;

  INTERFACE_MESSAGE*    MessageList;
  ITEM*                 Item;

  INTERFACE*            Next;
} INTERFACE;

typedef struct _INTERFACE_MESSAGE_ {
  char*                 Message;
  INTERFACE_MESSAGE*    Next;
} INTERFACE_MESSAGE;

typedef struct _ITEM_ {
  char*                 Name;
  ITEM*                 Next;
} ITEM;

void AddMessageStarLine (INTERFACE_MESSAGE** MessageStart) {
  if ((*MessageStart) != NULL) {
    INTERFACE_MESSAGE* StartStar = (INTERFACE_MESSAGE*) malloc (sizeof (INTERFACE_MESSAGE) * (1));
    INTERFACE_MESSAGE* EndStar = (INTERFACE_MESSAGE*) malloc (sizeof (INTERFACE_MESSAGE) * (1));

    int StarLength = strlen ((*MessageStart)->Message) + 4;
    StarLength = ((StarLength > 78) ? 78:StarLength);

    StartStar->Message = (char*) malloc (sizeof (char) * (StarLength + 1));
    EndStar->Message = (char*) malloc (sizeof (char) * (StarLength + 1));

    for (int Index = 0; ((Index < StarLength) && ((StartStar->Message[Index] = '*') == '*') && ((EndStar->Message[Index] = '*') == '*')) || (((StartStar->Message[Index] = '\0') != '\0') || ((EndStar->Message[Index] = '\0') != '\0')); Index++);

    StartStar->Next = (*MessageStart);
    EndStar->Next = NULL;
    for (INTERFACE_MESSAGE* MessagePoint = (*MessageStart); (MessagePoint->Next != NULL) || ((MessagePoint->Next = EndStar) == NULL); MessagePoint = MessagePoint->Next);

    (*MessageStart) = StartStar;
  }
}

void SetMessageSameLength (INTERFACE_MESSAGE* MessageStart) {
  int MaxLength = 0;
  for (INTERFACE_MESSAGE* MessagePoint = MessageStart; MessagePoint != NULL; MessagePoint = MessagePoint->Next)
    if (strlen (MessagePoint->Message) > MaxLength)
      MaxLength = strlen (MessagePoint->Message);


  for (INTERFACE_MESSAGE* MessagePoint = MessageStart; MessagePoint != NULL; MessagePoint = MessagePoint->Next) {
    if (strlen (MessagePoint->Message) < MaxLength) {
      char* OldString = MessagePoint->Message;
      MessagePoint->Message = (char*) malloc (sizeof (char) * (MaxLength + 1));


      for (int Index = 0; Index < MaxLength; Index++) {
        if (Index < strlen (OldString))
          MessagePoint->Message[Index] = OldString[Index];
        else
          MessagePoint->Message[Index] = ' ';
      }
      Free ((void**) &OldString);
      MessagePoint->Message[MaxLength] = '\0';
    }
  }
}

void AddInterface (INTERFACE** InterfaceStart, char* Key) {
  char* InterfaceVarFilePath = MergeString (gInterfaceVarFilePathFormat, Key);


  INTERFACE* NewInterface = NULL;
  NewInterface = (INTERFACE*) malloc (sizeof (INTERFACE) * (1));

  NewInterface->Key = MergeString ("%s", Key);

  NewInterface->MessageTitle = ReadVarFile (InterfaceVarFilePath,"MessageTitle",1);

  NewInterface->ItemTitle = ReadVarFile (InterfaceVarFilePath,"ItemTitle",1);

  NewInterface->Explain_1 = ReadVarFile (InterfaceVarFilePath,"Explain_1",1);

  NewInterface->Explain_2 = ReadVarFile (InterfaceVarFilePath,"Explain_2",1);

  NewInterface->Explain_3 = ReadVarFile (InterfaceVarFilePath,"Explain_3",1);

  NewInterface->Explain_4 = ReadVarFile (InterfaceVarFilePath,"Explain_4",1);

  NewInterface->Symbol = ReadVarFile (InterfaceVarFilePath,"Symbol",1);

  NewInterface->MessageList = NULL;
  for (int Index = 0; ; Index++) {
    char* TempString = MergeString ("Message_%d",Index);
    char* MessageString = ReadVarFile (InterfaceVarFilePath,TempString,1);
    Free ((void**) &TempString);

    if (MessageString == NULL) {
      break;
    }
    else {
      INTERFACE_MESSAGE* Message = NULL;
      Message = (INTERFACE_MESSAGE*) malloc (sizeof (INTERFACE_MESSAGE) * (1));
      Message->Message = MessageString;

      Message->Next = NULL;
      if (NewInterface->MessageList == NULL) {
        NewInterface->MessageList = Message;
      }
      else {
        INTERFACE_MESSAGE* MessagePoint = NULL;
        for (MessagePoint = NewInterface->MessageList; MessagePoint->Next != NULL; MessagePoint = MessagePoint->Next);
        MessagePoint->Next = Message;
      }
    }
  }
  SetMessageSameLength (NewInterface->MessageList);
  AddMessageStarLine (&(NewInterface->MessageList));

  NewInterface->Item = NULL;
  for (int Index = 0; ; Index++) {
    char* TempString = MergeString ("Item_%d",Index);
    char* ItemString = ReadVarFile (InterfaceVarFilePath,TempString,1);
    Free ((void**) &TempString);

    if (ItemString == NULL) {
      break;
    }
    else {
      ITEM* Item = NULL;
      Item = (ITEM*) malloc (sizeof (ITEM) * (1));
      Item->Name = ItemString;

      Item->Next = NULL;
      if (NewInterface->Item == NULL) {
        NewInterface->Item = Item;
      }
      else {
        ITEM* ItemPoint = NULL;
        for (ItemPoint = NewInterface->Item; ItemPoint->Next != NULL; ItemPoint = ItemPoint->Next);
        ItemPoint->Next = Item;
      }
    }
  }

  NewInterface->Next = NULL;

  if ((*InterfaceStart) == NULL) {
    (*InterfaceStart) = NewInterface;
  }
  else {
    INTERFACE* InterfacePoint = NULL;
    for (InterfacePoint = (*InterfaceStart); InterfacePoint->Next != NULL; InterfacePoint = InterfacePoint->Next);
    InterfacePoint->Next = NewInterface;
  }

  Free ((void**) &InterfaceVarFilePath);
}

void FreeMessage (INTERFACE_MESSAGE** Message) {
  if ((*Message) != NULL) {
    Free((void**) &((*Message)->Message));
    if ((*Message)->Next != NULL)
      FreeMessage (&((*Message)->Next));
    Free ((void**) &(*Message));
  }
}

void FreeItem (ITEM** Item) {
  if ((*Item) != NULL) {
    Free((void**) &((*Item)->Name));
    if ((*Item)->Next != NULL)
      FreeItem (&((*Item)->Next));
    Free ((void**) &(*Item));
  }
}

void FreeInterface (INTERFACE** Interface) {
  if ((*Interface) != NULL) {

    Free ((void**) &((*Interface)->Key));
    Free ((void**) &((*Interface)->MessageTitle));
    Free ((void**) &((*Interface)->ItemTitle));
    Free ((void**) &((*Interface)->Explain_1));
    Free ((void**) &((*Interface)->Explain_2));
    Free ((void**) &((*Interface)->Explain_3));
    Free ((void**) &((*Interface)->Explain_4));
    Free ((void**) &((*Interface)->Symbol));

    if ((*Interface)->MessageList != NULL)
      FreeMessage (&((*Interface)->MessageList));

    if ((*Interface)->Item != NULL)
      FreeItem (&((*Interface)->Item));

    if ((*Interface)->Next != NULL)
      FreeInterface (&((*Interface)->Next));


    Free ((void**) &(*Interface));
  }
}

void LocateInterface (INTERFACE** Interface) {
  system ("CLS");
  Print (" ==> 初始化介面中，請稍後...\n");

  AddInterface (Interface, "MainMenu");
  AddInterface (Interface, "ChoiceComicBook");
  AddInterface (Interface, "UpdateContinu");
  AddInterface (Interface, "UpdateSuccessOpenBrowser");
  AddInterface (Interface, "DownloadContinu");
  AddInterface (Interface, "DownloadComicBook");
  AddInterface (Interface, "DownloadChapter");
  AddInterface (Interface, "DownloadSuccessOpenFolder");
  AddInterface (Interface, "ManagementDownload");
  AddInterface (Interface, "WarningClearDownload");
  AddInterface (Interface, "ClearDownloadSuccess");
  AddInterface (Interface, "NoAnyOAComicBookUpdate");
  AddInterface (Interface, "HasOAComicBookUpdate");
  AddInterface (Interface, "OAComicBookUpdateSuccess");
  AddInterface (Interface, "CanNotConnectedToInternet");
  AddInterface (Interface, "NoChoiceComicBook");
  AddInterface (Interface, "NoChapterComicBook");
  AddInterface (Interface, "CanNotConnectedTo8Comic");
  AddInterface (Interface, "DownloadFileMaxRunTime");
  AddInterface (Interface, "UpdateOpenAllComicCodeFailure");
  AddInterface (Interface, "ResetOAComicBookContinu");
  AddInterface (Interface, "ResetOAComicBookSuccess");

  Print (" ==> 初始化介面完成!\n\n");
  system ("CLS");
}

INTERFACE* GetInterface (INTERFACE* Interface, char* Key) {
  INTERFACE* Return = NULL;
  if ((Interface != NULL) && (Key != NULL)) {
    for (Interface;Interface != NULL;Interface = Interface->Next) {
      if (strcmp (Interface->Key,Key) == 0) {
        Return = Interface;
        break;
      }
    }
  }
  return Return;
}

ITEM* GetItemStart (ITEM* Item, int Num) {
  for (int Index = 0; (Index < Num) && (Item != NULL); Item = Item->Next, Index++);
  return Item;
}

int GetItemCount (ITEM* Item) {
  int Index = 0;
  for (Index = 0; Item != NULL; Item = Item->Next, Index++);
  return Index;
}

void HelpInterface () {
  system ("CLS");

  Print ("\n");
  Print (" %=pW %76s| %=Nw\n"," ");
  Print (" %=pW %76s| %=Nw\n","【 選單介面輔助說明 】");
  Print (" %=pW %76s| %=Nw\n"," ");

  Print ("\n");

  Print ("  %76s|\n\n"         ,"%=NG【基本命令】         【移動瀏覽】         【求助說明】   %=Nw");
  Print ("  %76s|\n\n"         ,"       選擇項目   %=NC→%=Nw        往上一篇     %=NCPgUp%=Nw    寫說操作  %=NCh/H%=Nw ");
  Print ("  %76s|\n\n"         ,"       離開選單   %=NC←%=Nw        往下一篇     %=NCPgDn%=Nw                  ");
  Print ("  %76s|\n\n"         ,"       上一個選單 %=NC↑%=Nw        移動到第一篇 %=NCHome%=Nw                  ");
  Print ("  %76s|\n\n"         ,"       下一個選單 %=NC↓%=Nw        一動到最後篇 %=NCEnd%=Nw                   ");

  Print ("\n\n\n\n\n\n\n\n");

  Print (" %=bW %76s| %=Nw"         ,"請按 空白鍵 繼續，本程式沒有小天使可以協助。");

  getch ();
  system ("CLS");
}

bool ShowFeatureExplainInterface (int KeyBoardCode) {
  bool IsFeatureExplainBack = false;

  switch (KeyBoardCode) {
    case 72: case 104:
      HelpInterface ();
      IsFeatureExplainBack = true;
      break;
  }

  return IsFeatureExplainBack;
}

int InterfaceCursor (int Left, int Top, int* NowItem, char* Symbol, int Height, int Page, int ItemMaxCount, bool* IsFeatureExplainBack) {
  int Return = -1;

  SetCursorPosition (Left, Top + (*NowItem % Height)  );
  Print ("%s\b", Symbol);

  int firstchar = getch ();

  switch (firstchar) {
    case 27: // esc
      system ("CLS");
      exit (0);
      break;

    case 13: // Enter
      Return = (*NowItem);
      break;

    case 224:
      switch (getch ()) {
        case 0x48: // up
          (*NowItem)--;
          break;
        case 0x50: // down
          (*NowItem)++;
          break;
        case 0x4B: //left
          Return = -2;
          break;
        case 0x4D: // right
          Return = (*NowItem);
          break;
        case 0x49: // page up
          (*NowItem) = Height * (Page - 1);
          if ((*NowItem) <= 0)
            (*NowItem) = 0;
          break;
        case 0x51: // page down
          (*NowItem) = Height * (Page + 1);
          if ((*NowItem) >= ItemMaxCount)
            (*NowItem) = abs(ItemMaxCount - 1);
          break;
        case 0x47: // home
          (*NowItem) = 0;
          break;
        case 0x4F: //end
          (*NowItem) = abs(ItemMaxCount - 1);
          break;
      }
      break;


    default:

      *IsFeatureExplainBack = ShowFeatureExplainInterface (firstchar);
      break;
  }
  Print (" \b");
  return Return;
}

void ShowInterface (INTERFACE* Interface, char* KeyWord, int* Choice) {

  if ((Interface != NULL) && (KeyWord != NULL) && ((Interface = GetInterface (Interface, KeyWord)) != NULL)) {
    system ("CLS");
    int Top        =  0;
    int Left       =  0;
    int PageHeight =  WINDOW_SIZE_HEIGHT;
    int Height     =  0;
    int NowItem    =  0;
    int Page       =  0;


    if (((*Choice) < 0) || ((*Choice) >= GetItemCount (Interface->Item)))
      NowItem = abs (GetItemCount (Interface->Item) - 1);
    else
      NowItem = (*Choice);
    *Choice = -1;

    while ((*Choice) == -1) {
      Height = PageHeight - 6;

      system ("CLS");
      Print ("\n");
      if ((Interface->MessageList !=NULL) || (Interface->MessageTitle != NULL)) {
        Print (" %=RW %76s| %=Nw\n", Interface->MessageTitle);Height--;
        for (INTERFACE_MESSAGE* Message = Interface->MessageList; Message != NULL; Message = Message->Next) {
          Print (" %=rW%78s|%=Nw\n", Message->Message);Height--;
        }
        Print ("\n");Height--;
      }

      Page = NowItem / Height;
      Print (" %=bW %76s| %=Nw\n", Interface->ItemTitle);
      Print (" %=Nw %76s %=Nw\n", Interface->Explain_1);
      Print (" %=wN %76s %=Nw\n", Interface->Explain_2);

      GetCursorPosition (&Left, &Top);

      ITEM* Item = GetItemStart (Interface->Item, (NowItem / Height) * Height);

      for (int Index = 0; Index < Height; Index++) {
        if (Item != NULL) {
          Print (" %=Nw  %4d.  %68s %=Nw\n",(Index + 1) + (Page * Height), Item->Name);
          Item = Item->Next;
        }
        else
          Print ("\n");
      }

      Print (" %=cN %24s %=wN %50s %=Nw\n", Interface->Explain_3, Interface->Explain_4);

      while ((*Choice) == -1) {
        bool IsFeatureExplainBack = false;
        (*Choice) = InterfaceCursor (2, Top, &NowItem, Interface->Symbol, Height, Page, GetItemCount (Interface->Item), &IsFeatureExplainBack);

        if (IsFeatureExplainBack)
          break;
        if (NowItem == -1)
          NowItem = abs (GetItemCount (Interface->Item) - 1);
        if (NowItem >= GetItemCount (Interface->Item))
          NowItem = 0;
        if ((NowItem >= (Height * (Page + 1))) || (NowItem < (Height * (Page))))
          break;
      }
    }
    system ("CLS");
  }
}


//-- DataBase SQL ComicBook Chapter ------------------------------------------------------------------------

typedef struct   _COMIC_BOOK_                COMIC_BOOK;
typedef struct   _CHAPTER_                   CHAPTER;
typedef struct   _SQL_COMMAND_               SQL_COMMAND;

typedef struct _COMIC_BOOK_ {
  char*                 Sequence;
  char*                 ComicIndex;
  char*                 Name;
  char*                 IsEnable;
  char*                 Codes;

  COMIC_BOOK*           Next;
} COMIC_BOOK;

typedef struct _CHAPTER_ {
  char*                 ChapterIndex;
  char*                 Sid;
  char*                 Did;
  char*                 PicRange;
  char*                 Code;

  CHAPTER*              Next;
} CHAPTER;

typedef struct _SQL_COMMAND_ {
  char*                 Sql;
  SQL_COMMAND*          Next;
} SQL_COMMAND;

void AddSqlCommand (SQL_COMMAND** SqlCommandStart, char* Sql) {
  SQL_COMMAND* SqlCommand = (SQL_COMMAND*) malloc (sizeof (SQL_COMMAND) * (1));

  SqlCommand->Sql = MergeString (Sql);
  SqlCommand->Next = NULL;

  if ((*SqlCommandStart) == NULL) {
    (*SqlCommandStart) = SqlCommand;
  }
  else {
    SQL_COMMAND* SqlCommandPoint = NULL;
    for (SqlCommandPoint = (*SqlCommandStart); SqlCommandPoint->Next != NULL; SqlCommandPoint = SqlCommandPoint->Next);
    SqlCommandPoint->Next = SqlCommand;
  }
}

void FreeSqlCommand (SQL_COMMAND** SqlCommandStart) {
  if ((*SqlCommandStart) != NULL) {
    Free ((void**) &((*SqlCommandStart)->Sql));
    if ((*SqlCommandStart)->Next != NULL)
      FreeSqlCommand (&((*SqlCommandStart)->Next));
    Free ((void**) &(*SqlCommandStart));
  }
}

bool CheckDataBase () {
  int      Result = 0;
  bool     IsSuccess = true;
  char*    ErroeMessage = NULL;
  sqlite3* DataBaseLink;

  if (FileIsExists (gDataBaseJournalFilePath)) {
    remove (gDataBaseJournalFilePath);
    if (FileIsExists (gDataBaseFilePath))
      remove (gDataBaseFilePath);
  }

  Free ((void**) &gDataBaseJournalFilePath);

  if (GetFileSize(gDataBaseFilePath) < 5)
    remove (gDataBaseFilePath);

  if (!FileIsExists (gDataBaseFilePath)) {

    system ("CLS");
    Print (" ==> 起始資料庫系統中，請稍後...\n");

    Result = sqlite3_open_v2 (gDataBaseFilePath, &DataBaseLink, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);

    if (Result == SQLITE_OK){
      SQL_COMMAND* SqlCommand = NULL;

      AddSqlCommand (&SqlCommand, "CREATE TABLE ComicBook (Sequence INTEGER PRIMARY KEY, ComicIndex TEXT, Name TEXT, IsEnable TEXT, Codes TEXT);");
      AddSqlCommand (&SqlCommand, "CREATE TABLE ShowBaseUrl (Sequence INTEGER PRIMARY KEY, Key TEXT, Value TEXT);");
      AddSqlCommand (&SqlCommand, "CREATE TABLE Variable (Sequence INTEGER PRIMARY KEY, Key TEXT, Value TEXT);");

      AddSqlCommand (&SqlCommand, "INSERT INTO ComicBook (ComicIndex, Name, IsEnable, Codes) VALUES ('103',   '海賊王',                'True',  '');");
      AddSqlCommand (&SqlCommand, "INSERT INTO ComicBook (ComicIndex, Name, IsEnable, Codes) VALUES ('102',   '火影忍者',              'True',  '');");
      AddSqlCommand (&SqlCommand, "INSERT INTO ComicBook (ComicIndex, Name, IsEnable, Codes) VALUES ('1049',  '棋靈王',                'True',  '');");
      AddSqlCommand (&SqlCommand, "INSERT INTO ComicBook (ComicIndex, Name, IsEnable, Codes) VALUES ('8977',  'SPEC-天',               'True',  '');");
      AddSqlCommand (&SqlCommand, "INSERT INTO ComicBook (ComicIndex, Name, IsEnable, Codes) VALUES ('483',   '醫龍',                  'False', '');");
      AddSqlCommand (&SqlCommand, "INSERT INTO ComicBook (ComicIndex, Name, IsEnable, Codes) VALUES ('1635',  '蠟筆小新',              'True',  '');");
      AddSqlCommand (&SqlCommand, "INSERT INTO ComicBook (ComicIndex, Name, IsEnable, Codes) VALUES ('635',   '死亡筆記',              'True',  '');");
      AddSqlCommand (&SqlCommand, "INSERT INTO ComicBook (ComicIndex, Name, IsEnable, Codes) VALUES ('653',   '名偵探柯南',            'True',  '');");
      AddSqlCommand (&SqlCommand, "INSERT INTO ComicBook (ComicIndex, Name, IsEnable, Codes) VALUES ('197',   '金田一少年事件簿',      'False', '');");
      AddSqlCommand (&SqlCommand, "INSERT INTO ComicBook (ComicIndex, Name, IsEnable, Codes) VALUES ('1475',  '犬夜叉',                'False', '');");
      AddSqlCommand (&SqlCommand, "INSERT INTO ComicBook (ComicIndex, Name, IsEnable, Codes) VALUES ('272',   '魔法少年賈修',          'True',  '');");
      AddSqlCommand (&SqlCommand, "INSERT INTO ComicBook (ComicIndex, Name, IsEnable, Codes) VALUES ('3263',  '家庭教師Reborn',        'False', '');");
      AddSqlCommand (&SqlCommand, "INSERT INTO ComicBook (ComicIndex, Name, IsEnable, Codes) VALUES ('1356',  '光速蒙面俠21',          'False', '');");
      AddSqlCommand (&SqlCommand, "INSERT INTO ComicBook (ComicIndex, Name, IsEnable, Codes) VALUES ('4404',  '怪醫黑杰克',            'False', '');");
      AddSqlCommand (&SqlCommand, "INSERT INTO ComicBook (ComicIndex, Name, IsEnable, Codes) VALUES ('1966',  'GTO麻辣教師',           'True',  '');");
      AddSqlCommand (&SqlCommand, "INSERT INTO ComicBook (ComicIndex, Name, IsEnable, Codes) VALUES ('1910',  'KERORO軍曹',            'True',  '');");
      AddSqlCommand (&SqlCommand, "INSERT INTO ComicBook (ComicIndex, Name, IsEnable, Codes) VALUES ('3626',  '烏龍派出所',            'True',  '');");
      AddSqlCommand (&SqlCommand, "INSERT INTO ComicBook (ComicIndex, Name, IsEnable, Codes) VALUES ('126',   '網球王子',              'True',  '');");
      AddSqlCommand (&SqlCommand, "INSERT INTO ComicBook (ComicIndex, Name, IsEnable, Codes) VALUES ('104',   '死神',                  'False', '');");
      AddSqlCommand (&SqlCommand, "INSERT INTO ComicBook (ComicIndex, Name, IsEnable, Codes) VALUES ('752',   '北斗神拳',              'False', '');");
      AddSqlCommand (&SqlCommand, "INSERT INTO ComicBook (ComicIndex, Name, IsEnable, Codes) VALUES ('1551',  '銀魂',                  'False', '');");
      AddSqlCommand (&SqlCommand, "INSERT INTO ComicBook (ComicIndex, Name, IsEnable, Codes) VALUES ('723',   '七龍珠-舊版',           'False', '');");
      AddSqlCommand (&SqlCommand, "INSERT INTO ComicBook (ComicIndex, Name, IsEnable, Codes) VALUES ('3927',  '七龍珠',                'True',  '');");
      AddSqlCommand (&SqlCommand, "INSERT INTO ComicBook (ComicIndex, Name, IsEnable, Codes) VALUES ('639',   '頭文字D',               'False', '');");
      AddSqlCommand (&SqlCommand, "INSERT INTO ComicBook (ComicIndex, Name, IsEnable, Codes) VALUES ('105',   '獵人',                  'True',  '');");
      AddSqlCommand (&SqlCommand, "INSERT INTO ComicBook (ComicIndex, Name, IsEnable, Codes) VALUES ('833',   '通靈王',                'False', '');");
      AddSqlCommand (&SqlCommand, "INSERT INTO ComicBook (ComicIndex, Name, IsEnable, Codes) VALUES ('919',   '烘焙王',                'False', '');");
      AddSqlCommand (&SqlCommand, "INSERT INTO ComicBook (ComicIndex, Name, IsEnable, Codes) VALUES ('1050',  '遊戲王',                'True',  '');");
      AddSqlCommand (&SqlCommand, "INSERT INTO ComicBook (ComicIndex, Name, IsEnable, Codes) VALUES ('2092',  '游戲王GX',              'False', '');");
      AddSqlCommand (&SqlCommand, "INSERT INTO ComicBook (ComicIndex, Name, IsEnable, Codes) VALUES ('378',   '元祖!抓狂一族',         'True',  '');");
      AddSqlCommand (&SqlCommand, "INSERT INTO ComicBook (ComicIndex, Name, IsEnable, Codes) VALUES ('4579',  '抓狂一族',              'False', '');");
      AddSqlCommand (&SqlCommand, "INSERT INTO ComicBook (ComicIndex, Name, IsEnable, Codes) VALUES ('2432',  '中華一番-小當家',       'True',  '');");
      AddSqlCommand (&SqlCommand, "INSERT INTO ComicBook (ComicIndex, Name, IsEnable, Codes) VALUES ('3058',  '將太的壽司全國大會篇',  'False', '');");
      AddSqlCommand (&SqlCommand, "INSERT INTO ComicBook (ComicIndex, Name, IsEnable, Codes) VALUES ('1940',  '極速方程式',            'False', '');");
      AddSqlCommand (&SqlCommand, "INSERT INTO ComicBook (ComicIndex, Name, IsEnable, Codes) VALUES ('2310',  '東大特訓班',            'False', '');");
      AddSqlCommand (&SqlCommand, "INSERT INTO ComicBook (ComicIndex, Name, IsEnable, Codes) VALUES ('743',   '城市獵人',              'False', '');");
      AddSqlCommand (&SqlCommand, "INSERT INTO ComicBook (ComicIndex, Name, IsEnable, Codes) VALUES ('1351',  '鬼眼狂刀',              'False', '');");
      AddSqlCommand (&SqlCommand, "INSERT INTO ComicBook (ComicIndex, Name, IsEnable, Codes) VALUES ('856',   '新世紀福音戰士',        'True',  '');");
      AddSqlCommand (&SqlCommand, "INSERT INTO ComicBook (ComicIndex, Name, IsEnable, Codes) VALUES ('3088',  '碇真嗣育成計劃',        'False', '');");
      AddSqlCommand (&SqlCommand, "INSERT INTO ComicBook (ComicIndex, Name, IsEnable, Codes) VALUES ('423',   '風之谷',                'False', '');");
      AddSqlCommand (&SqlCommand, "INSERT INTO ComicBook (ComicIndex, Name, IsEnable, Codes) VALUES ('175',   '流星花園',              'False', '');");
      AddSqlCommand (&SqlCommand, "INSERT INTO ComicBook (ComicIndex, Name, IsEnable, Codes) VALUES ('1054',  'NANA',                  'True',  '');");
      AddSqlCommand (&SqlCommand, "INSERT INTO ComicBook (ComicIndex, Name, IsEnable, Codes) VALUES ('876',   '亂碼1/2',               'False', '');");
      AddSqlCommand (&SqlCommand, "INSERT INTO ComicBook (ComicIndex, Name, IsEnable, Codes) VALUES ('133',   '草莓百分百',            'False', '');");
      AddSqlCommand (&SqlCommand, "INSERT INTO ComicBook (ComicIndex, Name, IsEnable, Codes) VALUES ('609',   '草莓棉花糖',            'False', '');");
      AddSqlCommand (&SqlCommand, "INSERT INTO ComicBook (ComicIndex, Name, IsEnable, Codes) VALUES ('1131',  '戀愛季節',              'False', '');");
      AddSqlCommand (&SqlCommand, "INSERT INTO ComicBook (ComicIndex, Name, IsEnable, Codes) VALUES ('113',   '純情房東俏房客',        'False', '');");
      AddSqlCommand (&SqlCommand, "INSERT INTO ComicBook (ComicIndex, Name, IsEnable, Codes) VALUES ('2152',  '涼宮春日的憂郁',        'False', '');");
      AddSqlCommand (&SqlCommand, "INSERT INTO ComicBook (ComicIndex, Name, IsEnable, Codes) VALUES ('1920',  '地獄少女',              'False', '');");
      AddSqlCommand (&SqlCommand, "INSERT INTO ComicBook (ComicIndex, Name, IsEnable, Codes) VALUES ('2133',  '聖斗士星矢',            'False', '');");
      AddSqlCommand (&SqlCommand, "INSERT INTO ComicBook (ComicIndex, Name, IsEnable, Codes) VALUES ('3690',  '聖斗士星矢冥王神話',    'False', '');");
      AddSqlCommand (&SqlCommand, "INSERT INTO ComicBook (ComicIndex, Name, IsEnable, Codes) VALUES ('2324',  '一休和尚',              'False', '');");
      AddSqlCommand (&SqlCommand, "INSERT INTO ComicBook (ComicIndex, Name, IsEnable, Codes) VALUES ('239',   '加菲貓',                'False', '');");
      AddSqlCommand (&SqlCommand, "INSERT INTO ComicBook (ComicIndex, Name, IsEnable, Codes) VALUES ('607',   '足球小將',              'False', '');");
      AddSqlCommand (&SqlCommand, "INSERT INTO ComicBook (ComicIndex, Name, IsEnable, Codes) VALUES ('3924',  '足球小將Road To 2002',  'False', '');");
      AddSqlCommand (&SqlCommand, "INSERT INTO ComicBook (ComicIndex, Name, IsEnable, Codes) VALUES ('2396',  '足球小將世青篇',        'False', '');");
      AddSqlCommand (&SqlCommand, "INSERT INTO ComicBook (ComicIndex, Name, IsEnable, Codes) VALUES ('233',   '棒球大聯盟',            'False', '');");

      AddSqlCommand (&SqlCommand, "INSERT INTO Variable (Key, Value) VALUES ('MaxWgetRunTime', '10');");

      for (SQL_COMMAND* SqlCommandPoint = SqlCommand; SqlCommandPoint != NULL; SqlCommandPoint = SqlCommandPoint->Next) {
        Result = sqlite3_exec(DataBaseLink, SqlCommandPoint->Sql, 0, 0, &ErroeMessage);

        if (Result != SQLITE_OK) {
          Print ("\n");
          Print (" %=NR 資料庫初始錯誤!! %=Nw\n");
          Print ("============================\n");
          Print (" %=NW SQL 語法%=Nw : %=rW%s%=Nw\n", SqlCommandPoint->Sql);
          Print ("----------------------------\n");
          Print (" %=NW 錯誤訊息%=Nw : %=NR%s%=Nw\n", ErroeMessage);
          Print ("============================\n\n");
          IsSuccess = false;
          getch ();
          break;
        }
      }
      FreeSqlCommand (&SqlCommand);
    }
    sqlite3_close(DataBaseLink);

    if (IsSuccess)
      Print (" ==> 資料庫系統設定完成!\n");
    else
      Print (" ==> 資料庫系統設定失敗!\n");

    system ("CLS");
  }
  return IsSuccess;
}

COMIC_BOOK* GetComicBookList (bool IsGetEnable) {
  COMIC_BOOK*     ComicBookStart = NULL;

  if (CheckDataBase() == true) {
    int           Result = 0;
    sqlite3*      DataBaseLink;
    sqlite3_stmt  *stmt;

    Result = sqlite3_open_v2 (gDataBaseFilePath, &DataBaseLink, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);

    if (Result == SQLITE_OK) {
      char* Select = NULL;

      if (IsGetEnable)
        Select = MergeString ("SELECT Sequence, ComicIndex, Name, IsEnable, Codes FROM ComicBook WHERE IsEnable = 'True'");
      else
        Select = MergeString ("SELECT Sequence, ComicIndex, Name, IsEnable, Codes FROM ComicBook");

      Result = sqlite3_prepare(DataBaseLink, Select, -1, &stmt, NULL);

      if (Result == SQLITE_OK) {

        for (COMIC_BOOK* ComicBook = NULL; sqlite3_step(stmt) == SQLITE_ROW; ComicBook = NULL) {
          ComicBook = (COMIC_BOOK*) malloc (sizeof (COMIC_BOOK) * 1);

          ComicBook->Sequence   = MergeString("%s", sqlite3_column_text(stmt, 0));
          ComicBook->ComicIndex = MergeString("%s", sqlite3_column_text(stmt, 1));
          ComicBook->Name       = MergeString("%s", sqlite3_column_text(stmt, 2));
          ComicBook->IsEnable   = MergeString("%s", sqlite3_column_text(stmt, 3));
          ComicBook->Codes      = MergeString("%s", sqlite3_column_text(stmt, 4));

          ComicBook->Next       = NULL;

          if (ComicBookStart == NULL) {
            ComicBookStart = ComicBook;
          }
          else {
            COMIC_BOOK* ComicBookPoint = NULL;
            for (ComicBookPoint = ComicBookStart; ComicBookPoint->Next != NULL; ComicBookPoint = ComicBookPoint->Next);
            ComicBookPoint->Next = ComicBook;
          }
        }
      }
      sqlite3_finalize(stmt);
      Free ((void**) &Select);
    }
    sqlite3_close(DataBaseLink);
  }
  return ComicBookStart;
}

void FreeComicBook (COMIC_BOOK** ComicBook) {
  if ((*ComicBook) != NULL) {
    Free ((void**) &((*ComicBook)->Sequence));
    Free ((void**) &((*ComicBook)->ComicIndex));
    Free ((void**) &((*ComicBook)->Name));
    Free ((void**) &((*ComicBook)->IsEnable));
    Free ((void**) &((*ComicBook)->Codes));

    if ((*ComicBook)->Next != NULL)
      FreeComicBook (&((*ComicBook)->Next));
    Free ((void**) &(*ComicBook));
  }
}

void FreeChapter (CHAPTER** Chapter) {
  if ((*Chapter) != NULL) {
    Free ((void**) &((*Chapter)->ChapterIndex));
    Free ((void**) &((*Chapter)->Sid));
    Free ((void**) &((*Chapter)->Did));
    Free ((void**) &((*Chapter)->PicRange));
    Free ((void**) &((*Chapter)->Code));
    if ((*Chapter)->Next != NULL)
      FreeChapter (&((*Chapter)->Next));
    Free ((void**) &(*Chapter));
  }
}

COMIC_BOOK* GetComicBookStart (COMIC_BOOK* ComicBook, int Num) {
  for (int Index = 0; (Index < Num) && (ComicBook != NULL); ComicBook = ComicBook->Next, Index++);
  return ComicBook;
}

int GetComicBookCount (COMIC_BOOK* ComicBook) {
  int Index = 0;
  for (Index = 0; ComicBook != NULL; ComicBook = ComicBook->Next, Index++);
  return Index;
}

bool SetShowBaseUrl (char* Key, char* Value) {
  bool IsSuccess = false;

  if (CheckDataBase() && (Key != NULL) && (Value != NULL)) {
    sqlite3*      DataBaseLink;
    int           Result = 0;
    sqlite3_stmt  *stmt;
    char*         ErrorMessage = NULL;

    Result = sqlite3_open_v2(
               gDataBaseFilePath,
               &DataBaseLink,
               SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
               NULL
               );

    if (Result == SQLITE_OK) {
      char* Sql = MergeString ("SELECT COUNT (Sequence) FROM ShowBaseUrl WHERE Key = '%s'", Key);
      Result = sqlite3_prepare(DataBaseLink, Sql, -1, &stmt, NULL);
      Free ((void**) &Sql);

      if ((Result == SQLITE_OK) && (sqlite3_step(stmt) == SQLITE_ROW)) {

        if (sqlite3_column_int (stmt, 0) > 0)
          Sql = MergeString ("UPDATE ShowBaseUrl SET Value = '%s' WHERE Key = '%s'", Value, Key);
        else
          Sql = MergeString ("INSERT INTO ShowBaseUrl (Key, Value) VALUES ('%s', '%s')", Key, Value);

        Result = sqlite3_exec(DataBaseLink, Sql, 0, 0, &ErrorMessage);
        Free ((void**) &Sql);

        if (Result == SQLITE_OK)
          IsSuccess = true;
      }
      sqlite3_finalize(stmt);
    }
    sqlite3_close(DataBaseLink);
  }
  return IsSuccess;
}

char* GetShowBaseUrl (char* Key) {
  char* Value = NULL;
  if (CheckDataBase() && (Key != NULL)) {
    int           Result = 0;
    sqlite3*      DataBaseLink;
    sqlite3_stmt  *stmt;

    Result = sqlite3_open_v2(
               gDataBaseFilePath,
               &DataBaseLink,
               SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
               NULL
               );

    if (Result == SQLITE_OK) {
      char* Select = MergeString ("SELECT Value FROM ShowBaseUrl WHERE Key = '%s'", Key);
      Result = sqlite3_prepare(DataBaseLink, Select, -1, &stmt, NULL);
      Free ((void**) &Select);

      if ((Result == SQLITE_OK) && (sqlite3_step(stmt) == SQLITE_ROW))
        Value = MergeString("%s", sqlite3_column_text(stmt, 0));
      sqlite3_finalize(stmt);
    }
    sqlite3_close(DataBaseLink);
  }
  return Value;
}

bool SetComicBookCodes (char* Sequence, char* Codes) {
  bool IsSuccess = false;

  if (CheckDataBase() && (Sequence != NULL) && (Codes != NULL)) {
    sqlite3*      DataBaseLink;
    int           Result = 0;
    char*         ErrorMessage = NULL;

    Result = sqlite3_open_v2(
               gDataBaseFilePath,
               &DataBaseLink,
               SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
               NULL
               );

    if (Result == SQLITE_OK) {
      char* Sql = MergeString ("UPDATE ComicBook SET Codes = '%s' WHERE Sequence = '%s'", Codes, Sequence);
      Result = sqlite3_exec(DataBaseLink, Sql, 0, 0, &ErrorMessage);
      Free ((void**) &Sql);
      if (Result == SQLITE_OK)
        IsSuccess = true;
    }

    sqlite3_close(DataBaseLink);
  }
  return IsSuccess;
}

int GetChapterCount (CHAPTER* Chapter) {
  int Index = 0;
  for (Index = 0; Chapter != NULL; Chapter = Chapter->Next, Index++);
  return Index;
}

CHAPTER* GetChapterStart (CHAPTER* Chapter, int Num) {
  for (int Index = 0; (Index < Num) && (Chapter != NULL); Chapter = Chapter->Next, Index++);
  return Chapter;
}

CHAPTER* GetChapterList (char* Codes) {
  CHAPTER* ChapterStart = NULL;

  if (Codes != NULL) {
    int Index  = 0;
    int KeyNum = 0;

    int IndexStart    = -1, IndexEnd    = -1;
    int SidStart      = -1, SidEnd      = -1;
    int DidStart      = -1, DidEnd      = -1;
    int PicRangeStart = -1, PicRangeEnd = -1;
    int CodeStart     = -1, CodeEnd     = -1;

    for (Index = 0,KeyNum = 0; Index < strlen (Codes); Index++) {
      if (KeyNum == 0){
        for (IndexStart = Index; Index < strlen (Codes) && Codes[Index] != ' '; Index++) {
          IndexEnd = Index;
        }
        KeyNum++;
      }
      else if (KeyNum == 1 ) {
        for (SidStart = Index; Index < strlen (Codes) && Codes[Index] != ' '; Index++) {
          SidEnd = Index;
        }
        KeyNum++;
      }
      else if (KeyNum == 2 ) {
        for (DidStart = Index; Index < strlen (Codes) && Codes[Index] != ' '; Index++) {
          DidEnd = Index;
        }
        KeyNum++;
      }
      else if (KeyNum == 3 ) {
        for (PicRangeStart = Index; Index < strlen (Codes) && Codes[Index] != ' '; Index++) {
          PicRangeEnd = Index;
        }
        KeyNum++;
      }
      else {
        for (CodeStart = Index; Index < strlen (Codes) && Codes[Index] != '|'; Index++) {
          CodeEnd = Index;
        }
        KeyNum = 0;
      }

      if ((IndexStart != -1) && (IndexEnd != -1) && (SidStart != -1) && (SidEnd != -1) && (DidStart != -1) && (DidEnd != -1) && (PicRangeStart != -1) && (PicRangeEnd != -1) && (CodeStart != -1) && (CodeEnd != -1)) {
        int IndexLength    = IndexEnd    - IndexStart    + 1;
        int SidLength      = SidEnd      - SidStart      + 1;
        int DidLength      = DidEnd      - DidStart      + 1;
        int PicRangeLength = PicRangeEnd - PicRangeStart + 1;
        int CodeLength     = CodeEnd     - CodeStart     + 1;

        char* ChapterIndex = (char*) malloc (sizeof (char) * (IndexLength    + 1));
        char* Sid          = (char*) malloc (sizeof (char) * (SidLength      + 1));
        char* Did          = (char*) malloc (sizeof (char) * (DidLength      + 1));
        char* PicRange     = (char*) malloc (sizeof (char) * (PicRangeLength + 1));
        char* Code         = (char*) malloc (sizeof (char) * (CodeLength     + 1));

        IndexLength    = 0;
        SidLength      = 0;
        DidLength      = 0;
        PicRangeLength = 0;
        CodeLength     = 0;

        for (int TempIndex = IndexStart;    ((TempIndex < strlen (Codes) && (TempIndex < IndexEnd + 1)    && ((ChapterIndex[IndexLength++] = Codes[TempIndex]) != '\0')) || ((ChapterIndex[IndexLength++] = '\0') != '\0')); TempIndex++);
        for (int TempIndex = SidStart;      ((TempIndex < strlen (Codes) && (TempIndex < SidEnd + 1)      && ((Sid[SidLength++] = Codes[TempIndex]) != '\0'))            || ((Sid[SidLength++] = '\0') != '\0'));            TempIndex++);
        for (int TempIndex = DidStart;      ((TempIndex < strlen (Codes) && (TempIndex < DidEnd + 1)      && ((Did[DidLength++] = Codes[TempIndex]) != '\0'))            || ((Did[DidLength++] = '\0') != '\0'));            TempIndex++);
        for (int TempIndex = PicRangeStart; ((TempIndex < strlen (Codes) && (TempIndex < PicRangeEnd + 1) && ((PicRange[PicRangeLength++] = Codes[TempIndex]) != '\0'))  || ((PicRange[PicRangeLength++] = '\0') != '\0'));  TempIndex++);
        for (int TempIndex = CodeStart;     ((TempIndex < strlen (Codes) && (TempIndex < CodeEnd + 1)     && ((Code[CodeLength++] = Codes[TempIndex]) != '\0'))          || ((Code[CodeLength++] = '\0') != '\0'));          TempIndex++);

        IndexStart    = -1; IndexEnd    = -1;
        SidStart      = -1; SidEnd      = -1;
        DidStart      = -1; DidEnd      = -1;
        PicRangeStart = -1; PicRangeEnd = -1;
        CodeStart     = -1; CodeEnd     = -1;

        if (atoi (ChapterIndex) < 8000) {

          CHAPTER* Chapter = (CHAPTER*) malloc (sizeof (CHAPTER) * (1));

          Chapter->ChapterIndex = ChapterIndex;
          Chapter->Sid          = Sid;
          Chapter->Did          = Did;
          Chapter->PicRange     = PicRange;
          Chapter->Code         = Code;

          Chapter->Next = NULL;

          if (ChapterStart == NULL)
            ChapterStart = Chapter;
          else
            for (CHAPTER* ChapterPoint = ChapterStart;(ChapterPoint->Next != NULL) || ((ChapterPoint->Next = Chapter) == NULL); ChapterPoint = ChapterPoint->Next);
        }
      }
    }
  }
  return ChapterStart;
}

bool SetVariable (char* Key, char* Value) {
  Print (" ==> 設定中，請稍後...\n");
  bool IsSuccess = false;

  if (CheckDataBase() && (Key != NULL) && (Value != NULL)) {


    sqlite3*      DataBaseLink;
    int           Result = 0;
    sqlite3_stmt  *stmt;
    char*         ErrorMessage = NULL;

    Result = sqlite3_open_v2(
               gDataBaseFilePath,
               &DataBaseLink,
               SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
               NULL
               );

    if (Result == SQLITE_OK) {
      char* Sql = MergeString ("SELECT COUNT (Sequence) FROM Variable WHERE Key = '%s'", Key);
      Result = sqlite3_prepare(DataBaseLink, Sql, -1, &stmt, NULL);
      Free ((void**) &Sql);

      if ((Result == SQLITE_OK) && (sqlite3_step(stmt) == SQLITE_ROW)) {

        if (sqlite3_column_int (stmt, 0) > 0)
          Sql = MergeString ("UPDATE Variable SET Value = '%s' WHERE Key = '%s'", Value, Key);
        else
          Sql = MergeString ("INSERT INTO Variable (Key, Value) VALUES ('%s', '%s')", Key, Value);

        Result = sqlite3_exec(DataBaseLink, Sql, 0, 0, &ErrorMessage);
        Free ((void**) &Sql);

        if (Result == SQLITE_OK)
          IsSuccess = true;
      }
      sqlite3_finalize(stmt);
    }
    sqlite3_close(DataBaseLink);
  }

  if (IsSuccess)
    Print (" ==> 設定完成!\n");
  else
    Print (" ==> 設定失敗!\n");

  return IsSuccess;
}

char* GetVariable (char* Key) {
  char* Value = NULL;
  if (CheckDataBase() && (Key != NULL)) {
    int           Result = 0;
    sqlite3*      DataBaseLink;
    sqlite3_stmt  *stmt;

    Result = sqlite3_open_v2(
               gDataBaseFilePath,
               &DataBaseLink,
               SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
               NULL
               );

    if (Result == SQLITE_OK) {
      char* Select = MergeString ("SELECT Value FROM Variable WHERE Key = '%s'", Key);
      Result = sqlite3_prepare(DataBaseLink, Select, -1, &stmt, NULL);
      Free ((void**) &Select);

      if ((Result == SQLITE_OK) && (sqlite3_step(stmt) == SQLITE_ROW))
        Value = MergeString("%s", sqlite3_column_text(stmt, 0));
      sqlite3_finalize(stmt);
    }
    sqlite3_close(DataBaseLink);
  }
  return Value;
}

bool SetComicBookIsEnable (char* Sequence) {
  bool IsSuccess = false;
  Print (" ==> 設定中，請稍後...\n");

  if (CheckDataBase() && (Sequence != NULL) && (strcmp (Sequence, "") != 0) && (strlen (Sequence) > 0)) {


    int           Result = 0;
    char*         ErrorMessage = NULL;
    sqlite3*      DataBaseLink;

    Result = sqlite3_open_v2(
               gDataBaseFilePath,
               &DataBaseLink,
               SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
               NULL
               );

    if (Result == SQLITE_OK) {
      char* Sql = MergeString ("UPDATE ComicBook SET IsEnable = (CASE WHEN IsEnable = 'True' THEN 'False' ELSE 'True' END) WHERE Sequence = '%s'", Sequence);
      Result = sqlite3_exec(DataBaseLink, Sql, 0, 0, &ErrorMessage);
      Free ((void**) &Sql);

      if (Result == SQLITE_OK)
        IsSuccess = true;
    }
    sqlite3_close(DataBaseLink);

  }

  if (IsSuccess)
    Print (" ==> 設定完成!\n");
  else
    Print (" ==> 設定失敗!\n");

  return IsSuccess;
}


//-- Function 2. -------------------------------------------------------------------------------------------

void OpenBrowser () {
  Print (" ==> 請稍後，正在開啟瀏覽器..\n");
  system (gBrowserIndex);
  Print (" ==> 開啟瀏覽器完成!\n");
}


//-- Function 0. -------------------------------------------------------------------------------------------

void ShowChoiceComicBook (INTERFACE* Interface, char* KeyWord, int* Choice) {
  COMIC_BOOK* ComicBookList = GetComicBookList (false);

  if ((Interface != NULL) && (KeyWord != NULL) && ((Interface = GetInterface (Interface, KeyWord)) != NULL) && (ComicBookList != NULL)) {
    system ("CLS");
    int  Top              =  0;
    int  Left             =  0;
    int  PageHeight       =  WINDOW_SIZE_HEIGHT;
    int  Height           =  0;
    int  NowItem          =  0;
    int  Page             =  0;
    int  ItemIndex        =  0;

    char* FollowSymbol    = NULL;
    char* FollowExplain   = NULL;

    if (((*Choice) < 0) || ((*Choice) >= GetComicBookCount (ComicBookList)))
      NowItem = abs (GetComicBookCount (ComicBookList) - 1);
    else
      NowItem = (*Choice);
    *Choice = -1;

    while ((*Choice) == -1) {
      Height = PageHeight - 6;

      system ("CLS");
      Print ("\n");
      if ((Interface->MessageList !=NULL) || (Interface->MessageTitle != NULL)) {
        Print (" %=RW %76s| %=Nw\n", Interface->MessageTitle);Height--;
        for (INTERFACE_MESSAGE* Message = Interface->MessageList; Message != NULL; Message = Message->Next) {
          Print (" %=rW%78s|%=Nw\n", Message->Message);Height--;
        }
        Print ("\n");Height--;
      }

      Page = NowItem / Height;
      Print (" %=bW %76s| %=Nw\n", Interface->ItemTitle);
      Print (" %=Nw %76s %=Nw\n", Interface->Explain_1);
      Print (" %=wN %76s %=Nw\n", Interface->Explain_2);

      GetCursorPosition (&Left, &Top);

      COMIC_BOOK* ComicBook = GetComicBookStart (ComicBookList, (NowItem / Height) * Height);

      for (int Index = 0; Index < Height; Index++) {
        if (ComicBook != NULL) {
          ItemIndex     = 0;
          FollowSymbol  = NULL;
          FollowExplain = NULL;

          if (strcmp (ComicBook->IsEnable,"True") == 0) {
            ItemIndex = (Index + 1) + (Page * Height);
            FollowSymbol = MergeString ("■");
            FollowExplain = MergeString ("已選取, 您將 Follow 此本漫畫");
          }
          else {
            ItemIndex = (Index + 1) + (Page * Height);
            FollowSymbol = MergeString ("□");
            FollowExplain = MergeString ("未選取, 尚未 Follow 此本漫畫");
          }

          Print (" %=Nw  %4d.  %6s   %28s< %30s %=Nw\n",
            ItemIndex,
            FollowSymbol,
            ComicBook->Name,
            FollowExplain
            );

          Free ((void**) &FollowSymbol);
          Free ((void**) &FollowExplain);

          ComicBook = ComicBook->Next;
        }
        else
          Print ("\n");
      }

      Print (" %=cN %24s %=wN %50s %=Nw\n", Interface->Explain_3, Interface->Explain_4);

      while ((*Choice) == -1) {
        bool IsFeatureExplainBack = false;
        (*Choice) = InterfaceCursor (2, Top, &NowItem, Interface->Symbol, Height, Page, GetComicBookCount (ComicBookList), &IsFeatureExplainBack);

        if (IsFeatureExplainBack)
          break;
        if (NowItem == -1)
          NowItem = abs (GetComicBookCount (ComicBookList) - 1);
        if (NowItem >= GetComicBookCount (ComicBookList))
          NowItem = 0;
        if ((NowItem >= (Height * (Page + 1))) || (NowItem < (Height * (Page))))
          break;
      }
    }
    FreeComicBook (&ComicBookList);
    system ("CLS");
  }
}

void ChoiceComicBook (INTERFACE* Interface) {
  int Choice = 0;

  while (true) {
    ShowChoiceComicBook (Interface, "ChoiceComicBook", &Choice);
    system ("CLS");
    if (Choice < 0) {
      break;
    }
    else {
      COMIC_BOOK* ComicBookList = GetComicBookList (false);
      if (ComicBookList != NULL) {
        COMIC_BOOK* ComicBook = GetComicBookStart (ComicBookList, Choice);

        SetComicBookIsEnable (ComicBook->Sequence);

        FreeComicBook (&ComicBookList);
      }
    }
  }
}


//-- Function 1. -------------------------------------------------------------------------------------------

void TransformJs (char* FileName){

  if (FileName != NULL) {
    FILE   *FilePoint;
    int    FileSize = 0;
    char   *Article = NULL;
    int    Index = 0;
    int    KeyIndex = 0;
    int    Start = -1;
    int    End = -1;
    int    Middle = -1;
    int    UrlIndex = 0;

    int    ConditionIndex = 0;
    int    ConditionCount = 23;

    char   *StartKey = NULL;
    char   StartKeyFormat[] = "catid==%d";

    char   MiddleKey[] = "baseurl=\"";
    char   EndKey[] = "\";";

    int    UrlLength = 0;
    char   *BaseUrl = NULL;


    FileSize = GetFileSize (FileName);

    if (FileSize > 0) {
      if ((FilePoint = fopen (FileName, "r")) != NULL) {
        Article = (char*) malloc(sizeof (char) * (FileSize + 1));

        for (Index = 0; (Article[Index]=getc(FilePoint)) != EOF; Index++)
          if (Article[Index] == ' ')
            Index--;

        Article[Index] = '\0';
        fclose(FilePoint);

        UrlIndex = 0;

        for (ConditionIndex = 1,UrlLength = 0; ConditionIndex < ConditionCount; ConditionIndex++){
          StartKey = (char *) malloc (sizeof (char) * (strlen (StartKeyFormat) + GetIntLength (ConditionIndex) + 1));
          sprintf (StartKey, StartKeyFormat, ConditionIndex);

          for (Index = 0, Start = -1, End = -1; Index < strlen (Article); Index++){
            for (KeyIndex = 0; (Start == -1) && (KeyIndex < strlen(StartKey)) && (((Index + KeyIndex) < strlen (Article) ) && (Article[Index + KeyIndex] == StartKey[KeyIndex])); KeyIndex++);
            if((KeyIndex == strlen (StartKey))){
              Start = Index + strlen(StartKey);
              if((Article[Start]<58) && (Article[Start] > 47))
                Start = -1;
            }
            for (KeyIndex = 0; (Start > 0) && (KeyIndex < strlen(MiddleKey)) && (((Index + KeyIndex) < strlen (Article) ) && (Article[Index + KeyIndex] == MiddleKey[KeyIndex])); KeyIndex++);
            if(KeyIndex == strlen (MiddleKey)){
              Middle = Index + strlen(MiddleKey);
            }
            for (KeyIndex = 0; (Start > 0) && (Middle > 0) && (KeyIndex < strlen(EndKey)) && (((Index + KeyIndex) < strlen (Article) ) && (Article[Index + KeyIndex] == EndKey[KeyIndex])); KeyIndex++);
            if(KeyIndex == strlen (EndKey)){
              End = Index;
              break;
            }
          }
          if ((Start > 0) && (Middle > 0) && (End > 0) && (End > Start) && (Middle > Start)) {
            UrlLength = End - Middle + 1;
            BaseUrl = (char *) malloc (sizeof (char) * (UrlLength));

            for (Index = Middle, UrlIndex = 0; Index < End; Index++) {
                BaseUrl[UrlIndex++] = Article[Index];
            }
            BaseUrl[UrlIndex++] = '\0';

            char* Key = MergeString ("%d", ConditionIndex);
            SetShowBaseUrl (Key, BaseUrl);
            Free((void**) &Key);
            Free ((void**) &BaseUrl);
          }
          Free ((void**) &StartKey);
        }
        Free ((void**) &Article);
      }
    }
  }
}

char* GetKeySearchValue (char* StartKey, char* EndKey, char* FileName, bool IsIgnoreSpace){
  char* Value = NULL;
  if ((FileName != NULL) && (StartKey != NULL) && (EndKey != NULL)) {
    FILE   *FilePoint;
    int    FileSize = 0;
    char   *Article = NULL;
    int    Index = 0;
    int    KeyIndex = 0;
    int    Start = -1;
    int    End = -1;

    FileSize = GetFileSize (FileName);

    if (FileSize > 0) {
      if ((FilePoint = fopen (FileName, "r")) != NULL) {
        Article = (char*) malloc(sizeof (char) * (FileSize + 1));
        for (Index = 0; (Article[Index]=getc(FilePoint)) != EOF; Index++)
          if ((IsIgnoreSpace) && (Article[Index] == ' '))
            Index--;
        Article[Index] = '\0';
        fclose(FilePoint);

        for (Index = 0, Start = -1, End = -1; Index < strlen (Article); Index++){

          for (KeyIndex = 0; (Start == -1) && (KeyIndex < strlen(StartKey)) && (((Index + KeyIndex) < strlen (Article) ) && (Article[Index + KeyIndex] == StartKey[KeyIndex])); KeyIndex++);
          if((KeyIndex == strlen (StartKey))){
            Start = Index + strlen(StartKey);
          }
          for (KeyIndex = 0; (Start > 0) && (End == -1) && (KeyIndex < strlen(EndKey)) && (((Index + KeyIndex) < strlen (Article) ) && (Article[Index + KeyIndex] == EndKey[KeyIndex])); KeyIndex++);
          if(KeyIndex == strlen (EndKey)){
            End = Index;
            break;
          }
        }
        if ((Start > 0) && (End > 0) && (End > Start) && (strlen (Article) > End)) {
          int Length = End - Start;
          Value = (char*) malloc (sizeof (char) * (Length + 1));

          for (Index = Start,KeyIndex = 0; Index < End; Index++)
            Value[KeyIndex++] = Article[Index];
          Value[KeyIndex] = '\0';
        }
        free (Article);
      }
    }
  }
  return Value;
}

int GetAddProgress (int Denominator) {
  int Hundred = 100;
  int Index   = 0;
  int Value   = 0;

  for (Index = 0; (Denominator * Index) < Hundred; Index++);

  if (abs (Hundred - (Denominator * Index)) > abs (Hundred - (Denominator * (Index - 1))))
    Value = Index - 1;
  else
    Value = Index;

  return Value;
}

void UpdateComicBook (INTERFACE* Interface) {
  int Choice = 0;
  ShowInterface (Interface, "UpdateContinu", &Choice);

  if  (Choice == 0) {

    COMIC_BOOK* ComicBookList = GetComicBookList (true);

    if (ComicBookList != NULL) {
      int MaxWgetRunTime = atoi (GetVariable ("MaxWgetRunTime"));
      int ComicBookCount = GetComicBookCount (ComicBookList);
      int Progress = 0;
      int AddProgress = GetAddProgress(5 + (ComicBookCount * 3));

      ProgressBarTop (Progress, "Update");

      Print (" ==> 連接網站測試...\n");
      RunWgetCommand (gWgetFilePath, g8ComicBaseUrl, gTempFilePath, MaxWgetRunTime, false);
      Print (" ==> 測試完成!\n");
      ProgressBarTop (Progress += AddProgress, "Update");


      if (GetFileSize (gTempFilePath) > 0) {

        Print ("\n");
        Print (" ==> 正在取得 Comicview.js 檔案...\n");
        RunWgetCommand (gWgetFilePath, gComicviewFileUrl, gTempFilePath, MaxWgetRunTime, false);
        Print (" ==> 取得 Comicview.js 檔案完成!\n");
        ProgressBarTop (Progress += AddProgress, "Update");


        Print ("\n");
        Print (" ==> 正在轉換 Comicview.js 檔案...\n");
        TransformJs (gTempFilePath);
        Print (" ==> 轉換 Comicview.js 檔案完成!\n");
        ProgressBarTop (Progress += AddProgress, "Update");

        FILE *FilePoint = NULL;
        ClearFile(gAllComicCodeFilePath);

        if ((FilePoint = fopen (gAllComicCodeFilePath, "a")) != NULL) {

          Print ("\n");
          Print (" ==> 正在寫入 AllComicCode.js 檔頭...\n");
          putc (0xEF, FilePoint);
          putc (0xBB, FilePoint);
          putc (0xBF, FilePoint);

          fprintf (FilePoint, "%s\n", ANSIToUTF8 ("//"));
          fprintf (FilePoint, "%s\n", ANSIToUTF8 ("// 漫畫代碼,名稱,章節代碼"));
          fprintf (FilePoint, "%s\n", ANSIToUTF8 ("//"));
          fprintf (FilePoint, "%s\n", ANSIToUTF8 ("var gComicList = new Array ("));
          Print (" ==> 寫入 AllComicCode.js 檔頭完成!\n");
          ProgressBarTop (Progress += AddProgress, "Update");


          for (COMIC_BOOK* ComicBook = ComicBookList; ComicBook != NULL; ComicBook = ComicBook->Next) {
            system ("CLS");
            ProgressBarTop (Progress, "Update");


            Print (" ==> 正在取得 %s 的 Html 檔案...\n", ComicBook->Name);
            char* HtmlUrl = MergeString (gHtmlFormatUrl, ComicBook->ComicIndex);
            RunWgetCommand (gWgetFilePath, HtmlUrl, gTempFilePath, MaxWgetRunTime, false);
            Free ((void**) &HtmlUrl);
            Print (" ==> 取得 Html 檔案完成!\n");
            ProgressBarTop (Progress += AddProgress, "Update");


            Print ("\n");
            Print (" ==> 正在取得 %s 的 Show Page 檔案...\n", ComicBook->Name);
            char* ShowKeySatrtKey = MergeString (gShowKeyStartKeyFormat, ComicBook->ComicIndex);
            char* ShowKeyEndKey = MergeString (gShowKeyEndKeyFormat);
            char* ShowKey = GetKeySearchValue (ShowKeySatrtKey, ShowKeyEndKey, gTempFilePath, true);

            Free ((void**) &ShowKeySatrtKey);
            Free ((void**) &ShowKeyEndKey);

            char* ShowBaseUrl = GetShowBaseUrl (ShowKey);
            Free ((void**) &ShowKey);

            char* ShowUrl = MergeString ("%s%s.html",ShowBaseUrl, ComicBook->ComicIndex);
            Free ((void**) &ShowBaseUrl);

            RunWgetCommand (gWgetFilePath, ShowUrl, gTempFilePath, MaxWgetRunTime, false);
            Free ((void**) &ShowUrl);
            Print (" ==> 取得 Show Page 檔案完成!\n");
            ProgressBarTop (Progress += AddProgress, "Update");


            Print ("\n");
            Print (" ==> 正在取得 %s 的 Code 並且寫入 AllComicCode.js 檔案...\n", ComicBook->Name);
            char* Codes = GetKeySearchValue (gCodesStartKey,gCodesEndKey,gTempFilePath,false);
            SetComicBookCodes (ComicBook->Sequence, Codes);

            fprintf (FilePoint, "  new Array (%s, \"%s\", \"%s\")", ComicBook->ComicIndex, ANSIToUTF8 (ComicBook->Name), Codes);
            if (ComicBook->Next != NULL)
              fprintf (FilePoint, ",");
            fprintf (FilePoint, "\n");

            Free ((void**) &Codes);
            Print (" ==> 取得 Code 以及寫入檔案完成!\n");
            ProgressBarTop (Progress += AddProgress, "Update");
          }
          system ("CLS");
          ProgressBarTop (Progress, "Update");

          Print ("\n");
          Print (" ==> 正在寫入 AllComicCode.js 檔尾，完成更新...\n");
          fprintf (FilePoint, "%s\n", ANSIToUTF8 (");"));
          fclose (FilePoint);
          Print (" ==> 寫入完成，更新完成!\n");
          ProgressBarTop ( 100, "Update");

          Choice = 0;
          ShowInterface (Interface, "UpdateSuccessOpenBrowser", &Choice);
          if (Choice == 0)
            OpenBrowser ();
        }
        else {
          Choice = 0;
          ShowInterface (Interface, "UpdateOpenAllComicCodeFailure", &Choice);
          if (Choice == 0) {}
          else if (Choice == 1) {
            FreeInterface (&Interface);
            exit (0);
          }
        }
      }
      else {
        Choice = 0;
        ShowInterface (Interface, "CanNotConnectedTo8Comic", &Choice);
      }
      FreeComicBook (&ComicBookList);
    }
    else {
      ShowInterface (Interface, "NoChoiceComicBook", &Choice);
      if (Choice == 0)
        ChoiceComicBook (Interface);
    }
  }
  else if (Choice == 1) {
    ChoiceComicBook (Interface);
  }
}


//-- Function 3. -------------------------------------------------------------------------------------------

int GetChapterCountByCodes (char* Codes) {
  int Count = 0;
  int Num = 0;
  for (int Index = 0; Index < strlen (Codes); Index++)
    if (Codes[Index] == '|') {
      Num = 0;
      for (int NumIndex = Index + 1; Codes[NumIndex] != ' '; NumIndex++)
        Num = Num*10 + (Codes[NumIndex] - '0');
      if (Num < 8000)
        Count++;
    }
  return ((Count > 0) ? (Count + 1) : 0);
}

void ShowDownloadComicBook (INTERFACE* Interface, char* KeyWord, int* Choice) {
  COMIC_BOOK* ComicBookList = GetComicBookList (true);

  if ((Interface != NULL) && (KeyWord != NULL) && ((Interface = GetInterface (Interface, KeyWord)) != NULL) && (ComicBookList != NULL)) {
    system ("CLS");
    int  Top              =  0;
    int  Left             =  0;
    int  PageHeight       =  WINDOW_SIZE_HEIGHT;
    int  Height           =  0;
    int  NowItem          =  0;
    int  Page             =  0;

    if (((*Choice) < 0) || ((*Choice) >= GetComicBookCount (ComicBookList)))
      NowItem = abs (GetComicBookCount (ComicBookList) - 1);
    else
      NowItem = (*Choice);
    *Choice = -1;

    while ((*Choice) == -1) {
      Height = PageHeight - 6;

      system ("CLS");
      Print ("\n");
      if ((Interface->MessageList !=NULL) || (Interface->MessageTitle != NULL)) {
        Print (" %=RW %76s| %=Nw\n", Interface->MessageTitle);Height--;
        for (INTERFACE_MESSAGE* Message = Interface->MessageList; Message != NULL; Message = Message->Next) {
          Print (" %=rW%78s|%=Nw\n", Message->Message);Height--;
        }
        Print ("\n");Height--;
      }

      Page = NowItem / Height;
      Print (" %=bW %76s| %=Nw\n", Interface->ItemTitle);
      Print (" %=Nw %76s %=Nw\n", Interface->Explain_1);
      Print (" %=wN %76s %=Nw\n", Interface->Explain_2);

      GetCursorPosition (&Left, &Top);

      COMIC_BOOK* ComicBook = GetComicBookStart (ComicBookList, (NowItem / Height) * Height);

      for (int Index = 0; Index < Height; Index++) {
        if (ComicBook != NULL) {
          char* ChapterCount = MergeString ("共有 %3d 個章節", GetChapterCountByCodes (ComicBook->Codes));

          Print (" %=Nw  %4d.  %30s %37s %=Nw\n",
            (Index + 1) + (Page * Height),
            ComicBook->Name,
            ChapterCount
            );

          Free ((void**) &ChapterCount);
          ComicBook = ComicBook->Next;
        }
        else
          Print ("\n");
      }

      Print (" %=cN %24s %=wN %50s %=Nw\n", Interface->Explain_3, Interface->Explain_4);

      while ((*Choice) == -1) {
        bool IsFeatureExplainBack = false;
        (*Choice) = InterfaceCursor (2, Top, &NowItem, Interface->Symbol, Height, Page, GetComicBookCount (ComicBookList), &IsFeatureExplainBack);

        if (IsFeatureExplainBack)
          break;
        if (NowItem == -1)
          NowItem = abs (GetComicBookCount (ComicBookList) - 1);
        if (NowItem >= GetComicBookCount (ComicBookList))
          NowItem = 0;
        if ((NowItem >= (Height * (Page + 1))) || (NowItem < (Height * (Page))))
          break;
      }
    }
    FreeComicBook (&ComicBookList);
    system ("CLS");
  }
}

void ShowDownloadChapter (INTERFACE* Interface, char* KeyWord, int* Choice, CHAPTER* ChapterList) {

  if ((Interface != NULL) && (KeyWord != NULL) && ((Interface = GetInterface (Interface, KeyWord)) != NULL) && (ChapterList != NULL)) {
    system ("CLS");
    int  Top              =  0;
    int  Left             =  0;
    int  PageHeight       =  WINDOW_SIZE_HEIGHT;
    int  Height           =  0;
    int  NowItem          =  0;
    int  Page             =  0;

    if (((*Choice) < 0) || ((*Choice) >= GetChapterCount (ChapterList)))
      NowItem = abs (GetChapterCount (ChapterList) - 1);
    else
      NowItem = (*Choice);
    *Choice = -1;

    while ((*Choice) == -1) {
      Height = PageHeight - 6;

      system ("CLS");
      Print ("\n");
      if ((Interface->MessageList !=NULL) || (Interface->MessageTitle != NULL)) {
        Print (" %=RW %76s| %=Nw\n", Interface->MessageTitle);Height--;
        for (INTERFACE_MESSAGE* Message = Interface->MessageList; Message != NULL; Message = Message->Next) {
          Print (" %=rW%78s|%=Nw\n", Message->Message);Height--;
        }
        Print ("\n");Height--;
      }

      Page = NowItem / Height;
      Print (" %=bW %76s| %=Nw\n", Interface->ItemTitle);
      Print (" %=Nw %76s %=Nw\n", Interface->Explain_1);
      Print (" %=wN %76s %=Nw\n", Interface->Explain_2);

      GetCursorPosition (&Left, &Top);

      CHAPTER* Chapter = GetChapterStart (ChapterList, (NowItem / Height) * Height);

      for (int Index = 0; Index < Height; Index++) {
        if (Chapter != NULL) {
          char* ChapterIndex = MergeString ("第 %3s 章節", Chapter->ChapterIndex);
          char* PicRange     = MergeString ("共有 %3s 張漫畫圖片", Chapter->PicRange);

          Print (" %=Nw  %4d.  %24s %43s %=Nw\n",
            (Index + 1) + (Page * Height),
            ChapterIndex,
            PicRange
            );

          Free ((void**) &ChapterIndex);
          Free ((void**) &PicRange);
          Chapter = Chapter->Next;
        }
        else
          Print ("\n");
      }

      Print (" %=cN %24s %=wN %50s %=Nw\n", Interface->Explain_3, Interface->Explain_4);

      while ((*Choice) == -1) {
        bool IsFeatureExplainBack = false;
        (*Choice) = InterfaceCursor (2, Top, &NowItem, Interface->Symbol, Height, Page, GetChapterCount (ChapterList), &IsFeatureExplainBack);

        if (IsFeatureExplainBack)
          break;
        if (NowItem == -1)
          NowItem = abs (GetChapterCount (ChapterList) - 1);
        if (NowItem >= GetChapterCount (ChapterList))
          NowItem = 0;
        if ((NowItem >= (Height * (Page + 1))) || (NowItem < (Height * (Page))))
          break;
      }
    }
    system ("CLS");
  }
}

void DownloadComicBook (INTERFACE* Interface) {
  int Choice = 0;
  int MaxWgetRunTime = atoi (GetVariable ("MaxWgetRunTime"));

  Print (" ==> 連接網站測試...\n");
  RunWgetCommand (gWgetFilePath, g8ComicBaseUrl, gTempFilePath, MaxWgetRunTime, false);
  Print (" ==> 測試完成...\n");

  if (GetFileSize (gTempFilePath) > 0) {
    Choice = 0;
    ShowInterface (Interface, "DownloadContinu", &Choice);

    if (Choice == 0) {
      int ComicBookChoice = 0;

      while (ComicBookChoice >= 0) {
        ShowDownloadComicBook (Interface, "DownloadComicBook", &ComicBookChoice);

        if (ComicBookChoice < 0){
          break;
        }
        else{
          COMIC_BOOK* ComicBookList = GetComicBookList (true);

          if (ComicBookList != NULL) {
            int ChapterChoice = -1;
            while (ChapterChoice >= -1) {
              COMIC_BOOK* ComicBook = GetComicBookStart (ComicBookList, ComicBookChoice);

              CHAPTER* ChapterList = GetChapterList (ComicBook->Codes);
              ShowDownloadChapter (Interface, "DownloadChapter", &ChapterChoice, ChapterList);

              if (ChapterList != NULL) {
                if (ChapterChoice >= 0) {
                  CHAPTER* Chapter = GetChapterStart (ChapterList, ChapterChoice);
                  system ("CLS");

                  char* PicDownloadFolderPath = MergeString (gPicDownloadFolderPathFormat, ComicBook->Name, Chapter->ChapterIndex);

                  ClearFolder(PicDownloadFolderPath);

                  int ChapterCount = atoi (Chapter->PicRange);
                  int Progress = 0;
                  int AddProgress = GetAddProgress(ChapterCount);
                  int PageDownliadCount = 2;

                  for (int Index = 1; Index <= atoi (Chapter->PicRange); Index++) {
                    if ((Index % PageDownliadCount) == 1) {
                      system ("CLS");
                      ProgressBarTop (Progress, "Download");
                    }

                    int KeyPoint = (((Index - 1) / 10) % 10) + (((Index - 1) % 10) * 3);

                    char* KeyCode = (char*) malloc (sizeof (char) * (gKeyCodeLength + 1));
                    for (int KeyIndex = KeyPoint, KeyCount = 0; ((KeyIndex < (KeyPoint + gKeyCodeLength)) && ((KeyCode[KeyCount++] = Chapter->Code[KeyIndex]) != '\0')) ||((KeyCode[KeyCount++] = '\0') != '\0'); KeyIndex++);

                    char* PicName = MergeString (gPicNameFormat, Index, KeyCode);
                    Free ((void**) &KeyCode);

                    char* PicUrl = MergeString (gPicUrlFormat, Chapter->Sid, Chapter->Did, ComicBook->ComicIndex, Chapter->ChapterIndex, PicName);
                    char* PicFilePath = MergeString ("%s%s", PicDownloadFolderPath, PicName);
                    Free ((void**) &PicName);

                    if ((Index % PageDownliadCount) != 1)
                      Print ("\n");

                    Print (" ==> 正在下載第 %d 張圖片...\n", Index);
                    RunWgetCommand (gWgetFilePath, PicUrl, PicFilePath, MaxWgetRunTime, false);
                    ProgressBarTop (Progress += AddProgress, "Download");
                    Print (" ==> 下載圖片完成!\n");

                    Free ((void**) &PicFilePath);
                    Free ((void**) &PicUrl);
                  }

                  Choice = 0;
                  ShowInterface (Interface, "DownloadSuccessOpenFolder", &Choice);
                  if (Choice == 0) {
                    OpenFolder (PicDownloadFolderPath);
                  }
                  else if (Choice == 1) {
                  }
                  else if (Choice == 2) {
                    ChapterChoice = -2;
                  }
                  else if (Choice == 3) {
                    ComicBookChoice = -2;
                    ChapterChoice   = -2;
                  }
                  Free ((void**) &PicDownloadFolderPath);
                }
                else {
                  break;
                }
                FreeChapter (&ChapterList);
              }
              else {
                ShowInterface (Interface, "NoChapterComicBook", &Choice);
                if (Choice == -2) {
                  ChapterChoice = -2;
                }
                else if (Choice == 0) {
                  UpdateComicBook (Interface);
                }
                else if (Choice == 1){
                  ChapterChoice = -2;
                }
                else if (Choice == 2){
                  ComicBookChoice = -2;
                  ChapterChoice = -2;
                }
              }
            }
            FreeComicBook (&ComicBookList);
          }
          else {
            ShowInterface (Interface, "NoChoiceComicBook", &Choice);
            if (Choice == 0) {
              ChoiceComicBook (Interface);
            }
            else {
              ComicBookChoice = -2;
            }
          }
        }
      }
    }
    else if (Choice == 1) {
      OpenFolder(gDownloadFolderPath);
    }
  }
  else {
    Choice = 0;
    ShowInterface (Interface, "CanNotConnectedTo8Comic", &Choice);
  }
}


//-- Function 4. -------------------------------------------------------------------------------------------

void ManagementDownloadFolder (INTERFACE* Interface) {
  int Choice = 0;
  while (Choice >= 0) {
    ShowInterface (Interface, "ManagementDownload", &Choice);
    if (Choice == 0) {
      OpenFolder (gDownloadFolderPath);
    }
    else if (Choice == 1) {
      int WarningChoice = 0;
      ShowInterface (Interface, "WarningClearDownload", &WarningChoice);

      if (WarningChoice == 0) {
        ClearFolder (gDownloadFolderPath);
        int SuccessChoice = 0;
        ShowInterface (Interface, "ClearDownloadSuccess", &SuccessChoice);

        if (SuccessChoice == 0) {}
        else if (SuccessChoice == 1) {
          Choice = -2;
        }
        else if (SuccessChoice == 2) {
          OpenFolder (gDownloadFolderPath);
        }
      }
      else if (WarningChoice == 1) {}
      else if (WarningChoice == 2) {
        Choice = -2;
      }
    }
  }
}


//-- Function 7. -------------------------------------------------------------------------------------------

void ShowAppupdateItemContent (char* Content) {
  Print ("\n");
  Print (" %=rW %76s| %=Nw\n", "以下是目前更新的內容");
  Print ("%s", Content);
  getch ();
}

void CheakOAComicBookUpdate (INTERFACE* Interface) {
  Print (" ==> 檢查更新中...\n");
  int Choice = 0;
  int MaxWgetRunTime = atoi (GetVariable ("MaxWgetRunTime"));

  RunWgetCommand (gWgetFilePath, gOAComicBookAnnouncementUrl, gTempFilePath, MaxWgetRunTime, true);

  if (GetFileSize (gTempFilePath) > 0) {
    char* NewVersion = ReadVarFile (gTempFilePath, "ComicBook_New_Version", 1);

    if (strcmp (NewVersion, gThisVersion) != 0) {
      int HasUpdateChoice = 0;
      while (HasUpdateChoice >= 0) {
        ShowInterface (Interface, "HasOAComicBookUpdate", &HasUpdateChoice);

        if (HasUpdateChoice == 0) {
          char* NewVersionDownloadUrl = ReadVarFile (gTempFilePath, "ComicBook_New_Version_Download_Url", 1);
          char* NewDownloadAppName = ReadVarFile (gTempFilePath, "ComicBook_New_Download_App_Name", 1);

          RunWgetCommand (gWgetFilePath, NewVersionDownloadUrl, NewDownloadAppName, MaxWgetRunTime, true);

          int SuccessChoice = 0;
          ShowInterface (Interface, "OAComicBookUpdateSuccess", &SuccessChoice);

          if (SuccessChoice == 0) {
            char* OpenNewApp = MergeString ("Start %s", NewDownloadAppName);
            system (OpenNewApp);
            Free ((void**) &OpenNewApp);
          }
          HasUpdateChoice = -2;

          Free ((void**) &NewDownloadAppName);
          Free ((void**) &NewVersionDownloadUrl);
        }
        else if (HasUpdateChoice == 1) {
          char* NewVersionUpdateItemContent = ReadVarFile (gTempFilePath, "ComicBook_New_Version_Update_Item_Content", 1);
          ShowAppupdateItemContent (NewVersionUpdateItemContent);

          Free ((void**) &NewVersionUpdateItemContent);
        }
        else if (HasUpdateChoice == 2) {
          HasUpdateChoice = -2;
        }
      }
    }
    else {
      ShowInterface (Interface, "NoAnyOAComicBookUpdate", &Choice);
    }
    Free ((void**) &NewVersion);
  }
  else {
    ShowInterface (Interface, "NoAnyOAComicBookUpdate", &Choice);
  }
  ClearFile (gTempFilePath);
}


//-- Function 8. -------------------------------------------------------------------------------------------

void AboutOAComicBook () {
  Print ("\n");
  Print (" %=rW %76s| %=Nw\n", "");
  Print (" %=rW %76s| %=Nw\n", gThisVersion);
  Print (" %=rW %76s| %=Nw\n", "");

  Print ("\n");
  Print ("  版權說明 : 本程式僅提供學術研究，私自作為營利與原創者無關!\n\n");
  Print ("             此程式為個人創作的作品，你如果要拿去改成你的，儘管拿去吧!\n\n");
  Print ("             出問題別來問我 XDD !\n\n");
  Print ("  引用資源 : 本程式引用線上漫畫網站 http://www.8comic.com，還是請支持原創!\n\n");
  Print ("  友情提示 : 由於本程式還在開發中，並非最完美階段，故請勿任意改變資料夾結構，\n\n");
  Print ("             以避免造成一些奇怪的錯誤!\n\n");
  Print ("  使用流程 :\n\n");
  Print ("   解壓縮 >> 執行 OA_ComicBook.exe >> 1. 選擇漫畫 >> 2. 更新漫畫 >> 3.瀏覽漫畫\n\n");
  Print ("  發布日期 : 2012/11/17 05:00\n\n");

  Print (" %=bW %76s| %=Nw","請按任意鍵回到 主要選單。");
  getch ();
}


//-- Function 6. -------------------------------------------------------------------------------------------

void ResetOAComicBook (INTERFACE* Interface) {
  int Choice = 0;

  while (Choice >=0) {
    ShowInterface (Interface, "ResetOAComicBookContinu", &Choice);
    if (Choice == 0) {

      ClearFolder (gDownloadFolderPath);
      ClearFile (gTempFilePath);
      ClearFile (gAllComicCodeFilePath);
      ClearFile (gDataBaseFilePath);

      CheckDataBase ();

      Choice = 0;
      ShowInterface (Interface, "ResetOAComicBookSuccess", &Choice);
      if (Choice == 0) {
        char* ResetSuccessReOpenOAComicBook = MergeString ("Start %s", gOAComicBookExecutableName);
        system (ResetSuccessReOpenOAComicBook);
        Free ((void**) &ResetSuccessReOpenOAComicBook);
        FreeInterface (&Interface);
        exit (0);
      }
      Choice = -2;
    }
    if (Choice == 1) {
      OpenFolder (gDownloadFolderPath);
    }
    if (Choice == 2) {
      Choice = -2;
    }
  }
}


//-- Function 5. -------------------------------------------------------------------------------------------

int DownloadFileMaxRunTimeItemValue (int Index) {
  int Value = 0;
  switch (Index) {
    case 0:
      Value = 5 * 1;
      break;

    case 1:
      Value = 10 * 1;
      break;

    case 2:
      Value = 30 * 1;
      break;

    case 3:
      Value = 1 * 60;
      break;

    case 4:
      Value = 5 * 60;
      break;

    case 5:
      Value = 10 * 60;
      break;

    case 6:
      Value = 30 * 60;
      break;

    case 7:
      Value = 1 * 60 * 60;
      break;

    case 8:
      Value = 2 * 60 * 60;
      break;

    case 9:
      Value = 0;
      break;
  }
  return Value;
}

void ShowChoiceeMaxRunTime (INTERFACE* Interface, char* KeyWord, int NowMaxWgetRunTime, int* Choice) {

  if ((Interface != NULL) && (KeyWord != NULL) && ((Interface = GetInterface (Interface, KeyWord)) != NULL)) {
    system ("CLS");
    int Top        =  0;
    int Left       =  0;
    int PageHeight =  WINDOW_SIZE_HEIGHT;
    int Height     =  0;
    int NowItem    =  0;
    int Page       =  0;


    if (((*Choice) < 0) || ((*Choice) >= GetItemCount (Interface->Item)))
      NowItem = abs (GetItemCount (Interface->Item) - 1);
    else
      NowItem = (*Choice);
    *Choice = -1;

    while ((*Choice) == -1) {
      Height = PageHeight - 6;

      system ("CLS");
      Print ("\n");
      if ((Interface->MessageList !=NULL) || (Interface->MessageTitle != NULL)) {
        Print (" %=RW %76s| %=Nw\n", Interface->MessageTitle);Height--;
        for (INTERFACE_MESSAGE* Message = Interface->MessageList; Message != NULL; Message = Message->Next) {
          Print (" %=rW%78s|%=Nw\n", Message->Message);Height--;
        }
        Print ("\n");Height--;
      }

      Page = NowItem / Height;
      Print (" %=bW %76s| %=Nw\n", Interface->ItemTitle);
      Print (" %=Nw %76s %=Nw\n", Interface->Explain_1);
      Print (" %=wN %76s %=Nw\n", Interface->Explain_2);

      GetCursorPosition (&Left, &Top);

      ITEM* Item = GetItemStart (Interface->Item, (NowItem / Height) * Height);

      for (int Index = 0; Index < Height; Index++) {
        if (Item != NULL) {
          char* FollowSymbol = NULL;

          if (NowMaxWgetRunTime == DownloadFileMaxRunTimeItemValue (Index))
            FollowSymbol = MergeString ("■");
          else
            FollowSymbol = MergeString ("□");

          Print (" %=Nw  %4d.   %4s %62s %=Nw\n"
                 ,(Index + 1) + (Page * Height)
                 , FollowSymbol
                 , Item->Name);

          Free ((void**) &FollowSymbol);

          Item = Item->Next;
        }
        else
          Print ("\n");
      }

      Print (" %=cN %24s %=wN %50s %=Nw\n", Interface->Explain_3, Interface->Explain_4);

      while ((*Choice) == -1) {
        bool IsFeatureExplainBack = false;
        (*Choice) = InterfaceCursor (2, Top, &NowItem, Interface->Symbol, Height, Page, GetItemCount (Interface->Item), &IsFeatureExplainBack);

        if (IsFeatureExplainBack)
          break;
        if (NowItem == -1)
          NowItem = abs (GetItemCount (Interface->Item) - 1);
        if (NowItem >= GetItemCount (Interface->Item))
          NowItem = 0;
        if ((NowItem >= (Height * (Page + 1))) || (NowItem < (Height * (Page))))
          break;
      }
    }
    system ("CLS");
  }
}

void SetDownloadFileMaxRunTime (INTERFACE* Interface) {
  int Choice = 0;

  while (true) {
    int MaxWgetRunTime = atoi (GetVariable ("MaxWgetRunTime"));

    ShowChoiceeMaxRunTime (Interface, "DownloadFileMaxRunTime", MaxWgetRunTime, &Choice);
    system ("CLS");
    if (Choice < 0) {
      break;
    }
    else {
      char* Value = MergeString ("%d", DownloadFileMaxRunTimeItemValue (Choice));
      SetVariable ("MaxWgetRunTime", Value);
      Free ((void**) &Value);
    }
  }
}


//-- Main Function The Program Entry Point -----------------------------------------------------------------

int main(int argc, char* argv[]) {

  SetBufferWindowSameSize (WINDOW_SIZE_WIDTH, WINDOW_SIZE_HEIGHT);
  SetCursorVisible (false);

  INTERFACE* Interface = NULL;
  LocateInterface (&Interface);

  if (Interface != NULL) {
    int Choice = 0;

    if (IsConnectedToInternet ()) {
      CheckDataBase ();

      while ((Choice >= -2) && (Interface != NULL)) {

        ShowInterface (Interface, "MainMenu", &Choice);

        switch (Choice) {
          case 0:
            ChoiceComicBook (Interface);
            break;

          case 1:
            UpdateComicBook (Interface);
            break;

          case 2:
            OpenBrowser ();
            break;

          case 3:
            DownloadComicBook (Interface);
            break;

          case 4:
            ManagementDownloadFolder (Interface);
            break;

          case 5:
            SetDownloadFileMaxRunTime (Interface);
            break;

          case 6:
            ResetOAComicBook (Interface);
            break;

          case 7:
            CheakOAComicBookUpdate (Interface);
            break;

          case 8:
            AboutOAComicBook ();
            break;

          case 9:
            Choice = -3;
            break;
        }
        system ("CLS");
      }
    }
    else {
      ShowInterface (Interface, "CanNotConnectedToInternet", &Choice);
    }
    FreeInterface (&Interface);
  }
  else {
    Print ("\n\n錯誤!!! 初始化介面失敗!!! 請洽詢程式設計者!!! \n\n");
  }

/**/
    return 0;
}

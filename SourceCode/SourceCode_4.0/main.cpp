#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>
#include <time.h>
#include "sqlite3.h"


#define   WINDOW_SIZE_WIDTH     0x00000050
#define   WINDOW_SIZE_HEIGHT    0x00000019

#define   VAR_START_SYMBOL      0x0000005B
#define   VAR_END_SYMBOL        0x0000005D
#define   VAR_VALUE_SYMBOL      0x0000007C



//--------------------------------------------------------------UTF-8
/*width -> many Unicode - ANSI*/
char* w2m(const wchar_t* wcs) {
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
/*many -> width ANSI - Unicode*/
wchar_t* m2w(const char* mbs) {
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
wchar_t * ANSIToUnicode( const char* str ) {
  int textlen ;
  wchar_t * result;
  textlen = MultiByteToWideChar( CP_ACP, 0, str,-1, NULL,0 );
  result = (wchar_t *)malloc((textlen+1)*sizeof(wchar_t));
  memset(result,0,(textlen+1)*sizeof(wchar_t));
  MultiByteToWideChar(CP_ACP, 0,str,-1,(LPWSTR)result,textlen );
  return result;
}

char * UnicodeToANSI( const wchar_t* str ) {
  char* result;
  int textlen;
  textlen = WideCharToMultiByte( CP_ACP, 0, str, -1, NULL, 0, NULL, NULL );
  result =(char *)malloc((textlen+1)*sizeof(char));
  memset( result, 0, sizeof(char) * ( textlen + 1 ) );
  WideCharToMultiByte( CP_ACP, 0, str, -1, result, textlen, NULL, NULL );
  return result;
}

wchar_t * UTF8ToUnicode( const char* str ) {
  int textlen ;
  wchar_t * result;
  textlen = MultiByteToWideChar( CP_UTF8, 0, str,-1, NULL,0 );
  result = (wchar_t *)malloc((textlen+1)*sizeof(wchar_t));
  memset(result,0,(textlen+1)*sizeof(wchar_t));
  MultiByteToWideChar(CP_UTF8, 0,str,-1,(LPWSTR)result,textlen );
  return result;
}

char * UnicodeToUTF8( const wchar_t* str ) {
  char* result;
  int textlen;
  textlen = WideCharToMultiByte( CP_UTF8, 0, str, -1, NULL, 0, NULL, NULL );
  result =(char *)malloc((textlen+1)*sizeof(char));
  memset(result, 0, sizeof(char) * ( textlen + 1 ) );
  WideCharToMultiByte( CP_UTF8, 0, str, -1, result, textlen, NULL, NULL );
  return result;
}
char* ANSIToUTF8(const char* str) {
  return UnicodeToUTF8(ANSIToUnicode(str));
}

char* UTF8ToANSI(const char* str) {
  return UnicodeToANSI(UTF8ToUnicode(str));
}
//-----------------------------------------------------------SetScreen



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

void WindowMaxSize (int *x,int*y) {
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
  system ("cls");
  (*x)++;
  (*y)++;
}
void BufferMinSize (int *x,int*y) {
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
void SetWindowBufferSize (int InputX,int InputY){

  int MaxWinX=0,MaxWinY=0;
  int MinBufX=0,MinBufY=0;
  WindowMaxSize(&MaxWinX,&MaxWinY);
  BufferMinSize(&MinBufX,&MinBufY);

  InputX = InputX > MaxWinX ? MaxWinX : InputX;
  InputY = InputY > MaxWinY ? MaxWinY : InputY;

  InputX = InputX > 14 ? InputX : 14;
  InputY = InputY > 1  ? InputY : 1;

  SetBufferSize (InputX,InputY);
  SetWindowPosition (0,0,InputX-1,InputY-1);
  SetBufferSize (InputX,InputY);
  SetWindowPosition (0,0,InputX-1,InputY-1);
}




void SetCursorPosition (int x, int y) {
  COORD coord = {x, y};
  SetConsoleCursorPosition ( GetStdHandle ( STD_OUTPUT_HANDLE ), coord );
}
void SetCursorVisible (bool IsShow){
  CONSOLE_CURSOR_INFO ConCurInf;
  ConCurInf.dwSize = 10;
  ConCurInf.bVisible = IsShow;
  SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &ConCurInf);
}
void GetCursorPosition (int *x, int *y){
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE),&csbi);
  *x = csbi.dwCursorPosition.X;
  *y = csbi.dwCursorPosition.Y;
}
void SetBufferWindowSameSize (int wx,int wy) {
  SetBufferSize (wx,wy);
  SetWindowPosition (0,0,wx-1,wy-1);
  SetBufferSize (wx,wy);
  SetWindowPosition (0,0,wx-1,wy-1);
}

char GetPositionchar (int x, int y) {
  DWORD nChr;
  char Char;
  COORD size = {x, y};
  ReadConsoleOutputCharacter(GetStdHandle(STD_OUTPUT_HANDLE), &Char, 1, size, &nChr);
  return Char;
}
void ClearRangeScreen (int x1, int y1, int x2, int y2,bool Isfull) {
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
  SetCursorVisible (false);
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
  SetCursorVisible (true);
}

//-----------------------------------------------------------public

void SetSqlString (char** String) {
  if (String != NULL) {
    char* OldString = (*String);
    int KeyCount = 0;

    for (int Index = 0; Index < strlen (OldString); Index++)
      if (OldString[Index] == '\'')
        KeyCount++;

    char* NewString = NULL;
    NewString = (char*) malloc (sizeof (char) * (strlen (OldString) + KeyCount + 1));
    int NewStringCount = 0;

    for (int Index = 0; Index < strlen (OldString); Index++) {
      if (OldString[Index] == '\'')
        NewString[NewStringCount++] = '\'';
        NewString[NewStringCount++] = OldString[Index];
    }
    NewString[NewStringCount++] = '\0';

    (*String) = NewString;
    free (OldString);
  }
}
void Free (void* Object) {
  if (Object != NULL)
    free (Object);
}
bool FileIsExists(const char* FileName){
    if (FILE* File = fopen(FileName, "r")){
        fclose(File);
        return true;
    }
    return false;
}
int GetIntLength (int Num)
{
  int Index = 1;
  for (Index = 1; (Num = Num / 10) > 0 ; Index++);
  return Index;
}
char* MergeString (char* Format, ...)
{
  char* OutString = NULL;
  if (Format != NULL)
  {
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
      if (*FormatPoint == '%')
      {
        switch (*++FormatPoint)
        {
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
      else
      {
        Length++;
      }
    va_end(args);


    OutString = (char*) malloc (sizeof (char) * (Length + 1));
    strcpy(OutString, "");

    va_start(args, *Format);
    for (int NowLength = 0; *Format != '\0'; *Format++)
      if (*Format == '%') {
        switch (*++Format)
        {
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
      else
      {
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

bool IsConnectedToInternet(){
  Print ("測試網路....%=NN<ClearPoint>%=Nw\n");
  bool IsConnected = false;
  if (system("ping -n 1 www.gogle.com") == 0 || system("ping -n 1 www.yahoo.com.tw") == 0)
    IsConnected = true;
  else
    IsConnected = false;
  ClearScreenToKey("<ClearPoint>");
  return IsConnected;
}
void ClearFile (char* FileName){
  FILE* FilePoint = NULL;
  if ((FilePoint = fopen (FileName, "w")) != NULL) {
    fprintf (FilePoint, "");
    fclose (FilePoint);
  }
}
void RunCommand (char* WgetFile, char* Url, char* FileName, bool IsSsl){
  Print ("%=NR  ======> Start Run Command To Get Data..%=Nw\n");
  Print ("%=NN<ClearPoint>%=Nw\n");

  int x1=0,y1=0;
  int x2=0,y2=0;
  ClearFile(FileName);

  char* Command = NULL;
  if (IsSsl)
    Command = MergeString ("%s %s -O %s --no-check-certificate", WgetFile, Url, FileName);
  else
    Command = MergeString ("%s %s -O %s", WgetFile, Url, FileName);

  system(Command);
  free (Command);

  ClearScreenToKey("<ClearPoint>");
  Print ("%=NR  ======> Run Command And Get Data Success!%=Nw\n");

}
int GetFileSize (char* FileName) {
  FILE *FilePoint;
  int Size;
  if (FileName != NULL) {
    if ((FilePoint = fopen (FileName, "r")) == NULL) {
      Size = 0;
    }
    else {
      fseek(FilePoint, 0, SEEK_END);
      Size = ftell(FilePoint);
      fseek(FilePoint, 0, SEEK_SET);
      fclose(FilePoint);
    }
  }
  return Size;
}

void ProgressBarTop (int Num, char Title[]){
  int BarWidth = 58;
  int Percent = ((Num>100?100:Num) * 100) / 100;
  int Barsite = (Percent * BarWidth) /100;
  int Index = 0;
  char *Bar = NULL;

  if (Barsite > BarWidth)
    Barsite = BarWidth;

  Bar = (char *) malloc (sizeof (char) * (BarWidth + 1));

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
  Free (Bar);
}
//---------------------------------------------------------

typedef struct   _DATA_BASE_    DATABASE;
typedef struct   _INTERFACE_    INTERFACE;
typedef struct   _ITEM_         ITEM;
typedef struct   _COMIC_        COMIC;
typedef struct   _CHAPTER_      CHAPTER;


typedef void       (*FUNC_MODULE_CHECK_DB_EXISTS)        (DATABASE* DataBase);
typedef char*      (*FUNC_MODULE_GET_VARIABLE)           (DATABASE* DataBase, char* Key);
typedef INTERFACE* (*FUNC_MODULE_GET_INTERFACE)          (DATABASE* DataBase, char* Key);
typedef char**     (*FUNC_MODULE_GET_MESSAGE_LIST)       (INTERFACE* Interface, int* MessageLiseCount);
typedef void       (*FUNC_MODULE_FREE_INTERFACE)         (INTERFACE* Interface);
typedef ITEM*      (*FUNC_MODULE_GET_ITEM_LIST)          (DATABASE* DataBase, char* InterfaceSeq);

typedef void       (*FUNC_MODULE_FREE_ITEM)              (ITEM* Item);
typedef int        (*FUNC_MODULE_GET_ITEM_COUNT)         (ITEM* Item);
typedef ITEM*      (*FUNC_MODULE_GET_Start_ITEM)         (ITEM* ItemList, int Index);
typedef COMIC*     (*FUNC_MODULE_GET_COMIC)              (DATABASE* DataBase, int Sequence);
typedef void       (*FUNC_MODULE_FREE_COMIC)             (COMIC* Comic);
typedef int        (*FUNC_MODULE_GET_COMIC_COUNT)        (COMIC* Coimc);
typedef COMIC*     (*FUNC_MODULE_GET_Start_COMIC)        (COMIC* ComicList, int Index);
typedef int        (*FUNC_MODULE_GET_THIS_CHAPTER_COUNT) (DATABASE *DataBase, COMIC* Comic);
typedef CHAPTER*   (*FUNC_MODULE_GET_CHAPTER)            (DATABASE* DataBase, char* ChapterSeq);
typedef void       (*FUNC_MODULE_FREE_CHAPTER)           (CHAPTER* Chapter);
typedef int        (*FUNC_MODULE_GET_CHAPTER_COUNT)      (CHAPTER* Chapter);
typedef CHAPTER*   (*FUNC_MODULE_GET_Start_CHAPTER)      (CHAPTER* ChapterList, int Index);

typedef COMIC*     (*FUNC_MODULE_GET_COMIC_LIST)         (DATABASE* DataBase);
typedef CHAPTER*   (*FUNC_MODULE_GET_CHAPTER_LIST)       (DATABASE* DataBase, char* ComicSeq);
typedef char*      (*FUNC_MODULE_GET_SHOW_BASE_URL)      (DATABASE* DataBase, char* Key);
typedef bool       (*FUNC_MODULE_SET_VARIABLE)           (DATABASE* DataBase, char* Key, char* Value);

typedef bool       (*FUNC_MODULE_SET_CHAPTER)            (DATABASE* DataBase, char* ComicSeq, char* ChapterIndex, char* Sid, char* Did, char* PicRange, char* Code);
typedef bool       (*FUNC_MODULE_SET_BASE_URL)           (DATABASE* DataBase, char* Key, char* Value);
typedef void       (*FUNC_MODULE_FREE_DATABASE)          (DATABASE* DataBase);


typedef struct _DATA_BASE_ {
  char*                         DataBasePath;
  FUNC_MODULE_CHECK_DB_EXISTS   CheckDataBase;
  FUNC_MODULE_GET_VARIABLE      GetVariable;
  FUNC_MODULE_GET_INTERFACE     GetInterface;
  FUNC_MODULE_GET_ITEM_LIST     GetItemList;
  FUNC_MODULE_GET_COMIC         GetComic;
  FUNC_MODULE_GET_CHAPTER       GetChapter;
  FUNC_MODULE_GET_COMIC_LIST    GetComicList;
  FUNC_MODULE_GET_CHAPTER_LIST  GetChapterList;
  FUNC_MODULE_GET_SHOW_BASE_URL GetShowBaseUrl;
  FUNC_MODULE_SET_VARIABLE      SetVariable;
  FUNC_MODULE_SET_CHAPTER       SetChapter;
  FUNC_MODULE_SET_BASE_URL      SetShowBaseUrl;
  FUNC_MODULE_FREE_DATABASE     FreeDataBase;

} DATABASE;

typedef struct _INTERFACE_ {
  char*                         Sequence;
  char*                         Key;
  char*                         Title;
  char*                         Name;
  char*                         ItemNameTitle;
  char*                         Content;
  char*                         Explain;
  char*                         Message;
  char*                         Symbol;
  FUNC_MODULE_GET_MESSAGE_LIST  GetMessageLise;
  FUNC_MODULE_FREE_INTERFACE    FreeInterface;
} INTERFACE;

typedef struct _ITEM_ {
  char*                         InterfaceSeq;
  char*                         Name;
  char*                         EnglishName;
  char*                         Explain;
  FUNC_MODULE_FREE_ITEM         FreeItem;
  FUNC_MODULE_GET_ITEM_COUNT    GetItemCount;
  FUNC_MODULE_GET_Start_ITEM    GetStartItem;
  ITEM*                         Next;
} ITEM;

typedef struct _COMIC_ {
  char*                              Sequence;
  char*                              ComicIndex;
  char*                              Name;
  FUNC_MODULE_FREE_COMIC             FreeComic;
  FUNC_MODULE_GET_COMIC_COUNT        GetComicCount;
  FUNC_MODULE_GET_Start_COMIC        GetStartComic;

  FUNC_MODULE_GET_THIS_CHAPTER_COUNT GetThisComicChapterCount;

  COMIC*                             Next;
} COMIC;

typedef struct _CHAPTER_ {
  char*                         Sequence;
  char*                         ComicSeq;
  char*                         ChapterIndex;
  char*                         Sid;
  char*                         Did;
  char*                         PicRange;
  char*                         Code;
  FUNC_MODULE_FREE_CHAPTER      FreeChapter;
  FUNC_MODULE_GET_CHAPTER_COUNT GetChapterCount;
  FUNC_MODULE_GET_Start_CHAPTER GetStartChapter;
  CHAPTER*                      Next;
} CHAPTER;


void CheckDataBase (DATABASE* DataBase) {
  sqlite3* DataBaseLink;
  int      Result = 0;
  char*    ErroeMessage = NULL;
  char*    DataBaseJournal = MergeString ("%s-journal",DataBase->DataBasePath);

  if (FileIsExists (DataBaseJournal)) {
    remove (DataBaseJournal);
    if (FileIsExists (DataBase->DataBasePath))
      remove (DataBase->DataBasePath);
  }
  Free (DataBaseJournal);

  if (GetFileSize(DataBase->DataBasePath) < 10)
    remove (DataBase->DataBasePath);

  if (!FileIsExists (DataBase->DataBasePath)) {

    SetCursorVisible (false);
    Print (" ==> 起始資料庫系統中，請稍後...\n");
    Result = sqlite3_open_v2(
               DataBase->DataBasePath,
               &DataBaseLink,
               SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
               NULL
               );

    if (Result == SQLITE_OK){
      char* Create[] = {
         "CREATE TABLE Variable    (Sequence INTEGER PRIMARY KEY, Key TEXT,             Value TEXT);"
        ,"CREATE TABLE Interface   (Sequence INTEGER PRIMARY KEY, Key TEXT,             Title TEXT, Name TEXT,        ItemNameTitle TEXT,    Explain TEXT,   Content TEXT,Message TEXT,     Symbol TEXT);"
        ,"CREATE TABLE Item        (Sequence INTEGER PRIMARY KEY, InterfaceSeq TEXT,    Name TEXT,         EnglishName TEXT,      Explain TEXT);"
        ,"CREATE TABLE Comic       (Sequence INTEGER PRIMARY KEY, ComicIndex TEXT,      Name TEXT);"
        ,"CREATE TABLE Chapter     (Sequence INTEGER PRIMARY KEY, ComicSeq TEXT,        ChapterIndex TEXT, Sid TEXT,              Did TEXT,       PicRange TEXT,    Code TEXT);"
        ,"CREATE TABLE ShowBaseUrl (Sequence INTEGER PRIMARY KEY, Key TEXT,             Value TEXT);"
      };

      for (int Index = 0; Index < 6; Index++) {
        Result = sqlite3_exec(DataBaseLink, Create[Index], 0, 0, &ErroeMessage);
        if (Result != SQLITE_OK) {
          printf ("Create Table Error! Index : %d, Message : %s\n", Index, ErroeMessage);
          break;
        }
      }
      char* InsertVariable[] = {
         "INSERT INTO Variable (Sequence, KEY, Value) VALUES ( 1, 'File_Wget',            '.\\Command\\wget.exe');"
        ,"INSERT INTO Variable (Sequence, KEY, Value) VALUES ( 2, 'File_AllComicCode',    '.\\JavaScrip\\AllComicCode.js');"
        ,"INSERT INTO Variable (Sequence, KEY, Value) VALUES ( 3, 'Folder_TempFile',      '.\\TempFile\\TempFile.var');"
        ,"INSERT INTO Variable (Sequence, KEY, Value) VALUES ( 4, 'File_BrowserIndex',    '.\\index.html');"
        ,"INSERT INTO Variable (Sequence, KEY, Value) VALUES ( 5, 'Folder_Download',      '.\\Download\\');"
        ,"INSERT INTO Variable (Sequence, KEY, Value) VALUES ( 6, 'Url_WebBaseUrl',       'http://www.8comic.com/');"
        ,"INSERT INTO Variable (Sequence, KEY, Value) VALUES ( 7, 'Url_ComicView',        'http://www.8comic.com/js/comicview.js');"
        ,"INSERT INTO Variable (Sequence, KEY, Value) VALUES ( 8, 'Url_HtmlBaseUrl',      'http://www.8comic.com/html/');"
        ,"INSERT INTO Variable (Sequence, KEY, Value) VALUES ( 9, 'Line_Start',            '*******************************************');"
        ,"INSERT INTO Variable (Sequence, KEY, Value) VALUES (10, 'Format_PicName',       '%03d_%s.jpg');"
        ,"INSERT INTO Variable (Sequence, KEY, Value) VALUES (11, 'Format_PicUrl',        'http://img%s.8comic.com/%s/%s/%s/%s');"
        ,"INSERT INTO Variable (Sequence, KEY, Value) VALUES (12, 'PicNameKeyCodeLength', '3');"
        ,"INSERT INTO Variable (Sequence, KEY, Value) VALUES (13, 'Url_Announcement',     'https://dl.dropbox.com/u/5842721/My%20Works/OA_ComicBook/Announcement.var');"
        ,"INSERT INTO Variable (Sequence, KEY, Value) VALUES (14, 'Version',              'ComicBook_4.0_F');"

      };
      for (int Index = 0; Index < 14; Index++) {
        Result = sqlite3_exec(DataBaseLink, InsertVariable[Index], 0, 0, &ErroeMessage);
        if (Result != SQLITE_OK) {
          printf ("INSERT Variable Error! Index : %d, Message : %s\n", Index, ErroeMessage);
          break;
        }
      }

      char* InsertInterface[] = {
         "INSERT INTO Interface (Sequence, KEY, Title, Name, ItemNameTitle, Explain, Content, Message, Symbol) VALUES ( 1, 'MainMenu',                      '【主要 選單】',      '[ 主選單 ]'       , '   編號            名稱 -- Function Name                        選項內容說明', '[←]離開 [→]進入 [↑][↓]選擇 [PgUp][PgDn]翻頁 [Home][End]至首尾 [h]求助', '請選擇項目', '','>');"
        ,"INSERT INTO Interface (Sequence, KEY, Title, Name, ItemNameTitle, Explain, Content, Message, Symbol) VALUES ( 2, 'IsContinueUpDate',              '【友情 提示】',      '[ 更新漫畫 ]'     , ' 編號                 名稱 -- Function Name                     選項內容說明', '[←]離開 [→]進入 [↑][↓]選擇 [PgUp][PgDn]翻頁 [Home][End]至首尾 [h]求助', '請選擇項目', '警告! 更新當中，請勿任意的中斷結束此程式!\n若更新途中中斷，可能會造成資料庫錯誤!\n資料庫錯誤，可能會造成其他選項出現錯誤!\n所以請問，是否繼續執行更新!?\n','>');"
        ,"INSERT INTO Interface (Sequence, KEY, Title, Name, ItemNameTitle, Explain, Content, Message, Symbol) VALUES ( 3, 'IsUpDateEndOpenBrowser',        '【友情 提示】',      '[ 開啟瀏覽器 ]'   , ' 編號                 名稱 -- Function Name                     選項內容說明', '[←]離開 [→]進入 [↑][↓]選擇 [PgUp][PgDn]翻頁 [Home][End]至首尾 [h]求助', '請選擇項目', '恭喜!! 所有漫畫黨已經全部更新完畢!\n您可以回到主要選單並選擇 選項二 開始瀏覽漫畫!\n或者直接點擊 index.html 檔案瀏覽!\n還是您要直接啟動瀏覽器瀏覽?\n','>');"
        ,"INSERT INTO Interface (Sequence, KEY, Title, Name, ItemNameTitle, Explain, Content, Message, Symbol) VALUES ( 4, 'NoConnected',                   '【友情 提示】',      '[ 連線訊息 ]'     , ' 編號                 名稱 -- Function Name                     選項內容說明', '[←]離開 [→]進入 [↑][↓]選擇 [PgUp][PgDn]翻頁 [Home][End]至首尾 [h]求助', '請確認網路', '警告!! 無法連結至漫畫網站更新!!\n請確認您的網路是否暢通，若是網路確認沒有問題\n那可能漫畫網站的問題，請過段時間再試一次!\n','>');"
        ,"INSERT INTO Interface (Sequence, KEY, Title, Name, ItemNameTitle, Explain, Content, Message, Symbol) VALUES ( 5, 'ContinueDownload',              '【友情 提示】',      '[ 下載漫畫 ]'     , ' 編號                 名稱 -- Function Name                     選項內容說明', '[←]離開 [→]進入 [↑][↓]選擇 [PgUp][PgDn]翻頁 [Home][End]至首尾 [h]求助', '請選擇項目', '提醒! 因為此選項需要有漫畫資訊才可以下載\n所以此在開始下載前，請務必確認是否已經有更新!\n有過更新後，才會有最新的漫畫、章節的下載選項\n請確認後選擇項目，下載過程中請勿任意中斷程式!\n','>');"
        ,"INSERT INTO Interface (Sequence, KEY, Title, Name, ItemNameTitle, Explain, Content, Message, Symbol) VALUES ( 6, 'ComicListMenu',                 '【漫畫 選單】',      '[ 漫畫選單 ]'     , '   編號            名稱 -- Comic Name                           全部畫集數量', '[←]離開 [→]進入 [↑][↓]選擇 [PgUp][PgDn]翻頁 [Home][End]至首尾 [h]求助', '請選擇項目', '','>');"
        ,"INSERT INTO Interface (Sequence, KEY, Title, Name, ItemNameTitle, Explain, Content, Message, Symbol) VALUES ( 7, 'ChapterListMenu',               '【章節 選單】',      '[ 章節選單 ]'     , '   編號              名稱 -- Comic Name                       全部圖片頁面數', '[←]離開 [→]進入 [↑][↓]選擇 [PgUp][PgDn]翻頁 [Home][End]至首尾 [h]求助', '請選擇項目', '','>');"
        ,"INSERT INTO Interface (Sequence, KEY, Title, Name, ItemNameTitle, Explain, Content, Message, Symbol) VALUES ( 8, 'IsDownloadEndOpenFolder',       '【友情 提示】',      '[ 開啟下載 ]'     , ' 編號                 名稱 -- Function Name                     選項內容說明', '[←]離開 [→]進入 [↑][↓]選擇 [PgUp][PgDn]翻頁 [Home][End]至首尾 [h]求助', '請選擇項目', '恭喜!! 您所選擇的漫畫章節已經全數下載完畢\n請問您想直接開啟 下載資料夾 或回 章節選單\n還是回到漫畫選單或者回到主要目錄，請選擇!\n','>');"
        ,"INSERT INTO Interface (Sequence, KEY, Title, Name, ItemNameTitle, Explain, Content, Message, Symbol) VALUES ( 9, 'ManagementFolderMenu',          '【管理 選單】',      '[ 管理選單 ]'     , '   編號            名稱 -- Function Name                        選項內容說明', '[←]離開 [→]進入 [↑][↓]選擇 [PgUp][PgDn]翻頁 [Home][End]至首尾 [h]求助', '請選擇項目', '','>');"
        ,"INSERT INTO Interface (Sequence, KEY, Title, Name, ItemNameTitle, Explain, Content, Message, Symbol) VALUES (10, 'IsContinueClearDownloadFolder', '【友情 提示】',      '[ 清除資料夾 ]'   , ' 編號                 名稱 -- Function Name                     選項內容說明', '[←]離開 [→]進入 [↑][↓]選擇 [PgUp][PgDn]翻頁 [Home][End]至首尾 [h]求助', '請選擇項目', '警告!! 本選項會將所有下載資料夾內的所有漫畫\n皆全部刪除，如果您想保留又尚未備份，請勿繼續\n若您已經確定要刪除就請繼續進行刪除\n刪除過程中請勿任意中斷程式以免造成資料庫錯誤!!\n','>');"
        ,"INSERT INTO Interface (Sequence, KEY, Title, Name, ItemNameTitle, Explain, Content, Message, Symbol) VALUES (11, 'IsContinueResetAll',            '【友情 提示】',      '[ 還原 ]'         , ' 編號                 名稱 -- Function Name                     選項內容說明', '[←]離開 [→]進入 [↑][↓]選擇 [PgUp][PgDn]翻頁 [Home][End]至首尾 [h]求助', '請選擇項目', '警告!! 進行還原中請勿任意中斷程式以免造成錯誤\n本選項會將下載資料夾內所下載的漫畫刪除\n以及所有漫畫的更新碼歸零，還原若想瀏覽漫畫\n請務必再次更新漫畫資訊，請問是否繼續?\n','>');"
        ,"INSERT INTO Interface (Sequence, KEY, Title, Name, ItemNameTitle, Explain, Content, Message, Symbol) VALUES (12, 'ContinueUpdateApp',             '【友情 提示】',      '[ 更新 ]'         , ' 編號                 名稱 -- Function Name                     選項內容說明', '[←]離開 [→]進入 [↑][↓]選擇 [PgUp][PgDn]翻頁 [Home][End]至首尾 [h]求助', '請選擇項目', '提醒!! 關於 OA Comic Book 已經有更新版本了!!\n請問您要下載嘛!? 若要下載途中請勿任意中斷程式\n','>');"
        ,"INSERT INTO Interface (Sequence, KEY, Title, Name, ItemNameTitle, Explain, Content, Message, Symbol) VALUES (13, 'UpdateAppSuccess',              '【下載 完成】',      '[ 下載新版完成 ]' , ' 編號                 名稱 -- Function Name                     選項內容說明', '[←]離開 [→]進入 [↑][↓]選擇 [PgUp][PgDn]翻頁 [Home][End]至首尾 [h]求助', '請選擇項目', '恭喜!! 您已經下載最新版本的 OA Comic Book 了!\n請問要開啟它，並且安裝執行嘛!?\n','>');"
        ,"INSERT INTO Interface (Sequence, KEY, Title, Name, ItemNameTitle, Explain, Content, Message, Symbol) VALUES (14, 'IsNewVersion',                  '【友情 提示】',      '[ 沒有更新 ]'     , ' 編號                 名稱 -- Function Name                     選項內容說明', '[←]離開 [→]進入 [↑][↓]選擇 [PgUp][PgDn]翻頁 [Home][End]至首尾 [h]求助', '請選擇項目', '提醒!! 您的 OA Comic Book 已經是最新版本了!\n目前沒有任何更新檔可提供您更新! 謝謝!!\n','>');"
        };
      for (int Index = 0; Index < 14; Index++) {
        Result = sqlite3_exec(DataBaseLink, (InsertInterface[Index]), 0, 0, &ErroeMessage);
        if (Result != SQLITE_OK) {
          printf ("INSERT Interface Error! Index : %d, Message : %s\n", Index, ErroeMessage);
          break;
        }
      }

      char* InsertItem[] = {
          "INSERT INTO Item (Sequence, InterfaceSeq, Name, EnglishName, Explain) VALUES ( 1, '1', '更新漫畫檔',   'Update',           '將所有漫畫集數全部更新');"
         ,"INSERT INTO Item (Sequence, InterfaceSeq, Name, EnglishName, Explain) VALUES ( 2, '1', '瀏覽漫畫',     'Browser',          '開啟瀏覽器並且瀏覽漫畫');"
         ,"INSERT INTO Item (Sequence, InterfaceSeq, Name, EnglishName, Explain) VALUES ( 3, '1', '下載漫畫',     'Download',         '將選擇的漫畫下載至硬碟');"
         ,"INSERT INTO Item (Sequence, InterfaceSeq, Name, EnglishName, Explain) VALUES ( 4, '1', '還原本程式',   'Reset All',        '還原本應用程式所有設定');"
         ,"INSERT INTO Item (Sequence, InterfaceSeq, Name, EnglishName, Explain) VALUES ( 5, '1', '資料夾管理',   'Folder',           '管理儲存漫畫的資料夾');"
         ,"INSERT INTO Item (Sequence, InterfaceSeq, Name, EnglishName, Explain) VALUES ( 6, '1', '檢查更新',     'UpdateApp',        '檢查本程式是否有更新檔');"
         ,"INSERT INTO Item (Sequence, InterfaceSeq, Name, EnglishName, Explain) VALUES ( 7, '1', '程式說明',     'Explain',          '本應用程式使用相關說明');"
         ,"INSERT INTO Item (Sequence, InterfaceSeq, Name, EnglishName, Explain) VALUES ( 8, '1', '離開本程式',   'Exit',             '離開並且結束本應用程式');"

         ,"INSERT INTO Item (Sequence, InterfaceSeq, Name, EnglishName, Explain) VALUES ( 9, '2', '是的, 繼續更新','Yes, Go To Update Now!', '確定進入更新');"
         ,"INSERT INTO Item (Sequence, InterfaceSeq, Name, EnglishName, Explain) VALUES (10, '2', '不要, 不想更新','No, Don''t Update now!',  '回到主要選單');"
         ,"INSERT INTO Item (Sequence, InterfaceSeq, Name, EnglishName, Explain) VALUES (11, '3', '是的, 我要直接瀏覽','Yes, Open Browser Now!', '開啟瀏覽器瀏覽漫畫');"
         ,"INSERT INTO Item (Sequence, InterfaceSeq, Name, EnglishName, Explain) VALUES (12, '3', '不要, 先不瀏覽漫畫','No, Don''t Open now!',  '回到主要選單');"
         ,"INSERT INTO Item (Sequence, InterfaceSeq, Name, EnglishName, Explain) VALUES (13, '4', '了解, 回主要選單','OK, Back To Main menu!',  '回到主要選單');"

         ,"INSERT INTO Item (Sequence, InterfaceSeq, Name, EnglishName, Explain) VALUES (14, '5', '是的, 確定進入下載','Yes, Go To Download Now!',  '確定進入下載');"
         ,"INSERT INTO Item (Sequence, InterfaceSeq, Name, EnglishName, Explain) VALUES (15, '5', '了解, 先行更新漫畫','OK, Go To Update Now!',  '直接更新漫畫');"
         ,"INSERT INTO Item (Sequence, InterfaceSeq, Name, EnglishName, Explain) VALUES (16, '5', '不要, 回到主要選單',  'No, Back To Main menu!',  '回到主要選單');"

         ,"INSERT INTO Item (Sequence, InterfaceSeq, Name, EnglishName, Explain) VALUES (17, '8', '是的, 開啟資料夾',  'Yes, Open Folder Now!',  '開啟下載資料夾瀏覽');"
         ,"INSERT INTO Item (Sequence, InterfaceSeq, Name, EnglishName, Explain) VALUES (18, '8', '不要, 回章節選單',  'OK, Back To Chapter Menu!',  '回到章節選單');"
         ,"INSERT INTO Item (Sequence, InterfaceSeq, Name, EnglishName, Explain) VALUES (19, '8', '不要, 回漫畫選單',  'OK, Back To Comic Menu!',  '回到漫畫選單');"
         ,"INSERT INTO Item (Sequence, InterfaceSeq, Name, EnglishName, Explain) VALUES (20, '8', '不要, 回主要選單',  'OK, Back To Main Menu!',  '回到主要選單');"

         ,"INSERT INTO Item (Sequence, InterfaceSeq, Name, EnglishName, Explain) VALUES (21, '9', '開啟下載資料夾',  'Open Download Folder!',  '開啟下載檔案放置的資料夾');"
         ,"INSERT INTO Item (Sequence, InterfaceSeq, Name, EnglishName, Explain) VALUES (22, '9', '清除下載檔案','Clear Download Folder!',  '清除所有下載的漫畫');"

         ,"INSERT INTO Item (Sequence, InterfaceSeq, Name, EnglishName, Explain) VALUES (23, '10', '是的, 進行刪除','Yes, Clear All Now!',  '清除所有下載的漫畫');"
         ,"INSERT INTO Item (Sequence, InterfaceSeq, Name, EnglishName, Explain) VALUES (24, '10', '不要, 先不刪除','No, Don''t Clear All!',  '回到資料夾管理選單');"

         ,"INSERT INTO Item (Sequence, InterfaceSeq, Name, EnglishName, Explain) VALUES (25, '11', '是的, 進行還原','Yes, Reset All Now!',  '清除並且還原程式設定');"
         ,"INSERT INTO Item (Sequence, InterfaceSeq, Name, EnglishName, Explain) VALUES (26, '11', '不要, 先不還原','No, Don''t Reset All!',  '回到主要選單');"

         ,"INSERT INTO Item (Sequence, InterfaceSeq, Name, EnglishName, Explain) VALUES (27, '12', '是的, 下載更新','Yes, Update App Now!',  '下載最新版的程式');"
         ,"INSERT INTO Item (Sequence, InterfaceSeq, Name, EnglishName, Explain) VALUES (28, '12', '不要, 先不更新','No, Don''t Update App!',  '回到主要選單');"

         ,"INSERT INTO Item (Sequence, InterfaceSeq, Name, EnglishName, Explain) VALUES (29, '13', '是的, 執行程式','Yes, Installation Now!',  '執行安裝新版的程式');"
         ,"INSERT INTO Item (Sequence, InterfaceSeq, Name, EnglishName, Explain) VALUES (30, '13', '不要, 先不執行','No, Don''t Installation!',  '回到主要選單');"

         ,"INSERT INTO Item (Sequence, InterfaceSeq, Name, EnglishName, Explain) VALUES (31, '14', '了解, 回主選單','Ok, Back To Main Menu!',  '回到主要選單');"

      };


      for (int Index = 0; Index < 31; Index++) {
        Result = sqlite3_exec(DataBaseLink, InsertItem[Index], 0, 0, &ErroeMessage);
        if (Result != SQLITE_OK) {
          printf ("INSERT Item Error! Index : %d, Message : %s\n", Index, ErroeMessage);
          break;
        }
      }

      char* InsertComic[] = {
           "INSERT INTO Comic (Sequence, ComicIndex, Name) VALUES ( 1, '102', '火影忍者');"
          ,"INSERT INTO Comic (Sequence, ComicIndex, Name) VALUES ( 2, '103', '海賊王');"
          ,"INSERT INTO Comic (Sequence, ComicIndex, Name) VALUES ( 3, '104', '死神');"
          ,"INSERT INTO Comic (Sequence, ComicIndex, Name) VALUES ( 4, '105', '全職獵人');"
          ,"INSERT INTO Comic (Sequence, ComicIndex, Name) VALUES ( 5, '126', '網球王子');"
      };
      for (int Index = 0; Index < 5; Index++) {
        Result = sqlite3_exec(DataBaseLink, InsertComic[Index], 0, 0, &ErroeMessage);
        if (Result != SQLITE_OK) {
          printf ("INSERT Comic Error! Index : %d, Message : %s\n", Index, ErroeMessage);
          break;
        }
      }
      sqlite3_close(DataBaseLink);
    }
    Print (" ==> 資料庫系統設定完成!\n");
    SetCursorVisible (true);
  }
}
void FreeDataBase (DATABASE* DataBase) {
  if (DataBase != NULL) {
    Free (DataBase->DataBasePath);
    Free (DataBase);
  }
}

char* GetVariable(DATABASE* DataBase, char* Key) {
  char* Value = NULL;

  if (Key != NULL) {
    int           Result = 0;
    sqlite3*      DataBaseLink;
    sqlite3_stmt  *stmt;

    Result = sqlite3_open_v2(
               DataBase->DataBasePath,
               &DataBaseLink,
               SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
               NULL
               );

    if (Result == SQLITE_OK) {
      char* Select = MergeString ("SELECT Value FROM Variable WHERE Key = '%s'", Key);

      Result = sqlite3_prepare(DataBaseLink, Select, -1, &stmt, NULL);

      if (Result == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
          Key = MergeString("%s", sqlite3_column_text(stmt, 0));
        }
      }
      sqlite3_finalize(stmt);
      Free (Select);
    }
    sqlite3_close(DataBaseLink);
  }
  return Key;
}
void FreeInterface (INTERFACE* Interface) {
  if (Interface != NULL) {
    Free (Interface->Sequence);
    Free (Interface->Key);
    Free (Interface->Title);
    Free (Interface->Name);
    Free (Interface->ItemNameTitle);
    Free (Interface->Explain);
    Free (Interface->Content);
    Free (Interface->Message);
    Free (Interface->Symbol);
    Free (Interface);
  }
}
char** GetMessageLise (INTERFACE* Interface, int* MessageLiseCount){
  char** MessageLise = NULL;
  (*MessageLiseCount) = 0;
  if (Interface != NULL) {
    char* Message= Interface->Message;
    if (Message != NULL) {
      (*MessageLiseCount) = 0;
      for (int Index = 0; Index < strlen (Message); Index++)
        if (Message [Index] == '\n')
          (*MessageLiseCount)++;

      MessageLise = (char**) malloc (sizeof (char*) * (*MessageLiseCount));

      int Count = 0;
      int Start = 0;
      int MessageIndex = 0;
      int MaxLength = 0;
      for (int Index = 0; Index < strlen (Message); Index++)
        if (Message [Index] == '\n') {
          if (Index - Start > MaxLength)
            MaxLength = Index - Start;
          Start = Index + 1;
        }

      Count = 0;
      MessageIndex = 0;
      for (int Index = 0; Index < strlen (Message); Index++) {
        if (Count == 0)
          MessageLise[MessageIndex] = (char*) malloc (sizeof (char) * (MaxLength + 1));
        if (Message [Index] == '\n') {
          for (Count;Count < MaxLength;Count++)
            MessageLise[MessageIndex][Count] = ' ';
          MessageLise[MessageIndex][Count++] = '\0';
          MessageIndex++;
          Count = 0;
        }
        else {
          MessageLise[MessageIndex][Count++] = Message[Index];
        }
      }
    }
  }
  return MessageLise;
}
INTERFACE* GetInterface (DATABASE* DataBase, char* Key) {
  INTERFACE*    Interface = NULL;

  if (Key != NULL) {
    int           Result = 0;
    sqlite3*      DataBaseLink;
    sqlite3_stmt  *stmt;

    Result = sqlite3_open_v2(
               DataBase->DataBasePath,
               &DataBaseLink,
               SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
               NULL
               );

    if (Result == SQLITE_OK) {
      char* Select = MergeString ("SELECT Sequence,Key,Title,Name,ItemNameTitle,Explain,Content,Message,Symbol FROM Interface WHERE Key = '%s'", Key);
      Result = sqlite3_prepare(DataBaseLink, Select, -1, &stmt, NULL);

      if (Result == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
          Interface = (INTERFACE*) malloc (sizeof (INTERFACE) * 1);
          Interface->Sequence      = MergeString("%s", sqlite3_column_text(stmt, 0));
          Interface->Key           = MergeString("%s", sqlite3_column_text(stmt, 1));
          Interface->Title         = MergeString("%s", sqlite3_column_text(stmt, 2));
          Interface->Name          = MergeString("%s", sqlite3_column_text(stmt, 3));
          Interface->ItemNameTitle = MergeString("%s", sqlite3_column_text(stmt, 4));
          Interface->Explain       = MergeString("%s", sqlite3_column_text(stmt, 5));
          Interface->Content       = MergeString("%s", sqlite3_column_text(stmt, 6));
          Interface->Message       = MergeString("%s", sqlite3_column_text(stmt, 7));
          Interface->Symbol        = MergeString("%s", sqlite3_column_text(stmt, 8));
          Interface->GetMessageLise= GetMessageLise;
          Interface->FreeInterface = FreeInterface;
        }
      }
      sqlite3_finalize(stmt);
      Free (Select);
    }
    sqlite3_close(DataBaseLink);
  }
  return Interface;
}

void FreeItem (ITEM* Item) {
  if (Item != NULL) {
    Free (Item->InterfaceSeq);
    Free (Item->Name);
    Free (Item->EnglishName);
    Free (Item->Explain);
    if (Item->Next != NULL)
      FreeItem (Item->Next);
    Free (Item);
  }
}

int GetItemCount (ITEM* Item) {
  int Index = 0;
  for (Item; (Item != NULL); Item = Item->Next, Index++);
  return Index;
}
ITEM* GetStartItem (ITEM* ItemList, int Index) {
  for (int Count = 0; Count < Index && ItemList != NULL; Count++)
    ItemList = ItemList->Next;
  return ItemList;
}
ITEM* GetItemList (DATABASE* DataBase, char* InterfaceSeq) {
  ITEM*         ItemStart = NULL;

  if (InterfaceSeq != NULL) {
    int           Result = 0;
    sqlite3*      DataBaseLink;
    sqlite3_stmt  *stmt;

    Result = sqlite3_open_v2(
               DataBase->DataBasePath,
               &DataBaseLink,
               SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
               NULL
               );

    if (Result == SQLITE_OK) {
      char* Select = MergeString ("SELECT Name, EnglishName, Explain FROM Item WHERE InterfaceSeq = '%s'", InterfaceSeq);
      Result = sqlite3_prepare(DataBaseLink, Select, -1, &stmt, NULL);

      if (Result == SQLITE_OK) {
        for (ITEM* Item = NULL; sqlite3_step(stmt) == SQLITE_ROW; Item = NULL) {
          Item = (ITEM*) malloc (sizeof (ITEM) * 1);

          Item->InterfaceSeq   = MergeString (InterfaceSeq);
          Item->Name           = MergeString("%s", sqlite3_column_text(stmt, 0));
          Item->EnglishName    = MergeString("%s", sqlite3_column_text(stmt, 1));
          Item->Explain        = MergeString("%s", sqlite3_column_text(stmt, 2));
          Item->FreeItem       = FreeItem;
          Item->GetItemCount   = GetItemCount;
          Item->GetStartItem   = GetStartItem;
          Item->Next           = NULL;

          if (ItemStart == NULL) {
            ItemStart = Item;
          }
          else {
            ITEM* ItemPoint = NULL;
            for (ItemPoint = ItemStart; ItemPoint->Next != NULL; ItemPoint = ItemPoint->Next);
            ItemPoint->Next = Item;
          }
        }
      }
      sqlite3_finalize(stmt);
      Free (Select);
    }
    sqlite3_close(DataBaseLink);
  }
  return ItemStart;
}


void FreeComic (COMIC* Comic) {
  if (Comic != NULL) {
    Free (Comic->Sequence);
    Free (Comic->ComicIndex);
    Free (Comic->Name);
    if (Comic->Next != NULL)
      FreeComic (Comic->Next);
    Free (Comic);
  }
}
int GetComicCount (COMIC* Comic) {
  int Index = 0;
  for (Comic; (Comic != NULL); Comic = Comic->Next, Index++);
  return Index;
}
COMIC* GetStartComic (COMIC* ComicList, int Index) {
  for (int Count = 0; Count < Index && ComicList != NULL; Count++)
    ComicList = ComicList->Next;
  return ComicList;
}
int GetThisComicChapterCount (DATABASE *DataBase, COMIC* Comic) {
  int Count = 0;
  if (Comic != NULL) {

    int           Result = 0;
    sqlite3*      DataBaseLink;
    sqlite3_stmt  *stmt;

    Result = sqlite3_open_v2(
               DataBase->DataBasePath,
               &DataBaseLink,
               SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
               NULL
               );
    if (Result == SQLITE_OK) {
      char* Sql = MergeString ("SELECT COUNT (Sequence) FROM Chapter WHERE ComicSeq = '%s'", Comic->Sequence);
      Result = sqlite3_prepare(DataBaseLink, Sql, -1, &stmt, NULL);
      Free (Sql);
      if (Result == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
          Count = sqlite3_column_int (stmt, 0);
        }
      }
      sqlite3_finalize(stmt);
    }
    sqlite3_close(DataBaseLink);
  }
  return Count;
}
COMIC* GetComic (DATABASE* DataBase, int Sequence) {
  COMIC*    Comic = NULL;

  if (Sequence != 0) {
    int           Result = 0;
    sqlite3*      DataBaseLink;
    sqlite3_stmt  *stmt;

    Result = sqlite3_open_v2(
               DataBase->DataBasePath,
               &DataBaseLink,
               SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
               NULL
               );

    if (Result == SQLITE_OK) {
      char* Select = MergeString ("SELECT Sequence,ComicIndex,Name FROM Comic WHERE Sequence = '%d'", Sequence);
      Result = sqlite3_prepare(DataBaseLink, Select, -1, &stmt, NULL);
      if (Result == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
          Comic = (COMIC*) malloc (sizeof (COMIC) * 1);

          Comic->Sequence       = MergeString("%s", sqlite3_column_text(stmt, 0));
          Comic->ComicIndex     = MergeString("%s", sqlite3_column_text(stmt, 1));
          Comic->Name           = MergeString("%s", sqlite3_column_text(stmt, 2));
          Comic->FreeComic      = FreeComic;
          Comic->GetComicCount  = GetComicCount;
          Comic->GetStartComic  = GetStartComic;
          Comic->GetThisComicChapterCount  = GetThisComicChapterCount;
          Comic->Next           = NULL;
        }
      }
      sqlite3_finalize(stmt);
      Free (Select);
    }
    sqlite3_close(DataBaseLink);
  }
  return Comic;
}

int GetChapterCount (CHAPTER* Chapter) {
  int Index = 0;
  for (Chapter; (Chapter != NULL); Chapter = Chapter->Next, Index++);
  return Index;
}

void FreeChapter (CHAPTER* Chapter) {
  if (Chapter != NULL) {
    Free (Chapter->Sequence);
    Free (Chapter->ComicSeq);
    Free (Chapter->ChapterIndex);
    Free (Chapter->Sid);
    Free (Chapter->Did);
    Free (Chapter->PicRange);
    Free (Chapter->Code);
    if (Chapter->Next != NULL)
      FreeChapter (Chapter->Next);
    Free (Chapter);
  }
}

CHAPTER* GetStartChapter (CHAPTER* ChapterList, int Index) {
  for (int Count = 0; Count < Index && ChapterList != NULL; Count++)
    ChapterList = ChapterList->Next;
  return ChapterList;
}
CHAPTER* GetChapter (DATABASE* DataBase, char* ChapterSeq) {
  CHAPTER*    Chapter = NULL;

  if ((ChapterSeq != NULL)) {
    int           Result = 0;
    sqlite3*      DataBaseLink;
    sqlite3_stmt  *stmt;

    Result = sqlite3_open_v2(
               DataBase->DataBasePath,
               &DataBaseLink,
               SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
               NULL
               );

    if (Result == SQLITE_OK) {
      char* Select = MergeString ("SELECT Sequence,ComicSeq,ChapterIndex,Sid,Did,PicRange,Code FROM Chapter WHERE Sequence = '%s'", ChapterSeq);
      Result = sqlite3_prepare(DataBaseLink, Select, -1, &stmt, NULL);
      if (Result == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
          Chapter = (CHAPTER*) malloc (sizeof (CHAPTER) * 1);

          Chapter->Sequence      = MergeString("%s", sqlite3_column_text(stmt, 0));
          Chapter->ComicSeq      = MergeString("%s", sqlite3_column_text(stmt, 1));
          Chapter->ChapterIndex  = MergeString("%s", sqlite3_column_text(stmt, 2));
          Chapter->Sid           = MergeString("%s", sqlite3_column_text(stmt, 3));
          Chapter->Did           = MergeString("%s", sqlite3_column_text(stmt, 4));
          Chapter->PicRange      = MergeString("%s", sqlite3_column_text(stmt, 5));
          Chapter->Code          = MergeString("%s", sqlite3_column_text(stmt, 6));

          Chapter->FreeChapter     = FreeChapter;
          Chapter->GetChapterCount = GetChapterCount;
          Chapter->GetStartChapter = GetStartChapter;

          Chapter->Next = NULL;
        }
      }
      sqlite3_finalize(stmt);
      Free (Select);
    }
    sqlite3_close(DataBaseLink);
  }
  return Chapter;
}

COMIC* GetComicList(DATABASE* DataBase) {
  COMIC*    ComicStart = NULL;

  sqlite3_stmt  *stmt;
  int           Result = 0;
  sqlite3*      DataBaseLink;

  Result = sqlite3_open_v2(
             DataBase->DataBasePath,
             &DataBaseLink,
             SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
             NULL
             );

  if (Result == SQLITE_OK) {
    char* Select = MergeString ("SELECT Sequence,ComicIndex,Name FROM Comic");
    Result = sqlite3_prepare(DataBaseLink, Select, -1, &stmt, NULL);
    if (Result == SQLITE_OK) {
      for (COMIC* Comic = NULL; sqlite3_step(stmt) == SQLITE_ROW; Comic = NULL) {
        Comic = (COMIC*) malloc (sizeof (COMIC) * 1);

        Comic->Sequence   = MergeString("%s", sqlite3_column_text(stmt, 0));
        Comic->ComicIndex = MergeString("%s", sqlite3_column_text(stmt, 1));
        Comic->Name       = MergeString("%s", sqlite3_column_text(stmt, 2));
        Comic->FreeComic  = FreeComic;
        Comic->GetComicCount  = GetComicCount;
          Comic->GetStartComic  = GetStartComic;
          Comic->GetThisComicChapterCount  = GetThisComicChapterCount;
        Comic->Next       = NULL;

        if (ComicStart == NULL) {
          ComicStart = Comic;
        }
        else {
          COMIC* ComicPoint = NULL;
          for (ComicPoint = ComicStart; ComicPoint->Next != NULL; ComicPoint = ComicPoint->Next);
          ComicPoint->Next = Comic;
        }
      }
    }
    sqlite3_finalize(stmt);
    Free (Select);
  }
  sqlite3_close(DataBaseLink);

  return ComicStart;
}

CHAPTER* GetChapterList (DATABASE* DataBase, char* ComicSeq) {
  CHAPTER*    ChapterStart = NULL;

  if (ComicSeq != NULL) {
    int           Result = 0;
    sqlite3*      DataBaseLink;
    sqlite3_stmt  *stmt;

    Result = sqlite3_open_v2(
               DataBase->DataBasePath,
               &DataBaseLink,
               SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
               NULL
               );

    if (Result == SQLITE_OK) {
      char* Select = MergeString ("SELECT Sequence, ComicSeq, ChapterIndex, Sid, Did, PicRange, Code FROM Chapter WHERE ComicSeq = '%s'", ComicSeq);
      Result = sqlite3_prepare(DataBaseLink, Select, -1, &stmt, NULL);
      if (Result == SQLITE_OK) {
        for (CHAPTER* Chapter = NULL; sqlite3_step(stmt) == SQLITE_ROW; Chapter = NULL) {
          Chapter = (CHAPTER*) malloc (sizeof (CHAPTER) * 1);

          Chapter->Sequence         = MergeString("%s", sqlite3_column_text(stmt, 0));
          Chapter->ComicSeq         = MergeString("%s", sqlite3_column_text(stmt, 1));
          Chapter->ChapterIndex     = MergeString("%s", sqlite3_column_text(stmt, 2));
          Chapter->Sid              = MergeString("%s", sqlite3_column_text(stmt, 3));
          Chapter->Did              = MergeString("%s", sqlite3_column_text(stmt, 4));
          Chapter->PicRange         = MergeString("%s", sqlite3_column_text(stmt, 5));
          Chapter->Code             = MergeString("%s", sqlite3_column_text(stmt, 6));

          Chapter->FreeChapter     = FreeChapter;
          Chapter->GetChapterCount = GetChapterCount;
          Chapter->GetStartChapter = GetStartChapter;

          Chapter->Next = NULL;

          if (ChapterStart == NULL) {
            ChapterStart = Chapter;
          }
          else {
            CHAPTER* ChapterPoint = NULL;
            for (ChapterPoint = ChapterStart; ChapterPoint->Next != NULL; ChapterPoint = ChapterPoint->Next);
            ChapterPoint->Next = Chapter;
          }
        }
      }
      sqlite3_finalize(stmt);
      Free (Select);
    }
    sqlite3_close(DataBaseLink);
  }
  return ChapterStart;
}

char* GetShowBaseUrl(DATABASE* DataBase, char* Key) {
  char* Value = NULL;
  if (Key != NULL) {
    int           Result = 0;
    sqlite3*      DataBaseLink;
    sqlite3_stmt  *stmt;

    Result = sqlite3_open_v2(
               DataBase->DataBasePath,
               &DataBaseLink,
               SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
               NULL
               );

    if (Result == SQLITE_OK) {
      char* Select = MergeString ("SELECT Value FROM ShowBaseUrl WHERE Key = '%s'", Key);
      Result = sqlite3_prepare(DataBaseLink, Select, -1, &stmt, NULL);
      if (Result == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
          Value = MergeString("%s", sqlite3_column_text(stmt, 0));
        }
      }
      sqlite3_finalize(stmt);
      Free (Select);
    }
    sqlite3_close(DataBaseLink);
  }
  return Value;
}

bool SetVariable (DATABASE* DataBase, char* Key, char* Value) {
  bool IsSuccess = false;
  if ((Key != NULL) && (Value != NULL)) {
    char*         ErrorMessage = NULL;
    int           Result = 0;
    sqlite3*      DataBaseLink;
    sqlite3_stmt  *stmt;

    Result = sqlite3_open_v2(
               DataBase->DataBasePath,
               &DataBaseLink,
               SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
               NULL
               );
    if (Result == SQLITE_OK) {
      char* Sql = MergeString ("SELECT COUNT (Sequence) FROM Variable WHERE Key = '%s'", Key);
      Result = sqlite3_prepare(DataBaseLink, Sql, -1, &stmt, NULL);
      Free (Sql);

      if (Result == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
          int RowCount = sqlite3_column_int (stmt, 0);

          if (RowCount > 0) {
            Sql = MergeString ("UPDATE Variable SET Value = '%s' WHERE Key = '%s'", Value, Key);
            Result = sqlite3_exec(DataBaseLink, Sql, 0, 0, &ErrorMessage);
            Free (Sql);
            if (Result == SQLITE_OK) {
              IsSuccess = true;
            }
          }
        }
      }
      sqlite3_finalize(stmt);
    }
    sqlite3_close(DataBaseLink);
  }
  return IsSuccess;
}

bool SetChapter (DATABASE* DataBase, char* ComicSeq, char* ChapterIndex, char* Sid, char* Did, char* PicRange, char* Code) {
  bool IsSuccess = false;
  if ((ComicSeq != NULL) && (ChapterIndex != NULL) && (Sid != NULL) && (Did != NULL) && (PicRange != NULL) && (Code != NULL)) {
    char*         ErrorMessage = NULL;
    int           Result = 0;
    sqlite3*      DataBaseLink;
    sqlite3_stmt  *stmt;

    Result = sqlite3_open_v2(
               DataBase->DataBasePath,
               &DataBaseLink,
               SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
               NULL
               );
    if (Result == SQLITE_OK) {
      char* Sql = MergeString ("SELECT COUNT (Sequence) FROM Chapter WHERE ComicSeq = '%s' AND ChapterIndex = '%s'", ComicSeq, ChapterIndex);
      Result = sqlite3_prepare(DataBaseLink, Sql, -1, &stmt, NULL);
      Free (Sql);

      if (Result == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
          int RowCount = sqlite3_column_int (stmt, 0);

          if (RowCount > 0) {
            Sql = MergeString ("UPDATE Chapter SET Sid = '%s', Did = '%s', PicRange = '%s', Code = '%s'  WHERE ComicSeq = '%s' AND ChapterIndex = '%s'", Sid, Did, PicRange, Code, ComicSeq, ChapterIndex);
            Result = sqlite3_exec(DataBaseLink, Sql, 0, 0, &ErrorMessage);
            Free (Sql);
            if (Result == SQLITE_OK) {
              IsSuccess = true;
            }
          }
          else {
            Sql = MergeString ("INSERT INTO Chapter (ComicSeq, ChapterIndex, Sid, Did, PicRange, Code) VALUES ('%s', '%s', '%s', '%s', '%s', '%s')", ComicSeq, ChapterIndex, Sid, Did, PicRange, Code);
            Result = sqlite3_exec(DataBaseLink, Sql, 0, 0, &ErrorMessage);
            Free (Sql);
            if (Result == SQLITE_OK) {
              IsSuccess = true;
            }
          }
        }
      }
      sqlite3_finalize(stmt);
    }
    sqlite3_close(DataBaseLink);
  }
  return IsSuccess;
}

bool SetShowBaseUrl (DATABASE* DataBase, char* Key, char* Value) {
  bool IsSuccess = false;
  if ((Key != NULL) && (Value != NULL)) {
    char*         ErrorMessage = NULL;
    int           Result = 0;
    sqlite3*      DataBaseLink;
    sqlite3_stmt  *stmt;

    Result = sqlite3_open_v2(
               DataBase->DataBasePath,
               &DataBaseLink,
               SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
               NULL
               );
    if (Result == SQLITE_OK) {
      char* Sql = MergeString ("SELECT COUNT (Sequence) FROM ShowBaseUrl WHERE Key = '%s'", Key);
      Result = sqlite3_prepare(DataBaseLink, Sql, -1, &stmt, NULL);
      Free (Sql);

      if (Result == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
          int RowCount = sqlite3_column_int (stmt, 0);

          if (RowCount > 0) {
            Sql = MergeString ("UPDATE ShowBaseUrl SET Value = '%s' WHERE Key = '%s'", Value, Key);
            Result = sqlite3_exec(DataBaseLink, Sql, 0, 0, &ErrorMessage);
            Free (Sql);
            if (Result == SQLITE_OK) {
              IsSuccess = true;
            }
          }
          else {
            Sql = MergeString ("INSERT INTO ShowBaseUrl (Key, Value) VALUES ('%s', '%s')", Key, Value);
            Result = sqlite3_exec(DataBaseLink, Sql, 0, 0, &ErrorMessage);
            Free (Sql);
            if (Result == SQLITE_OK) {
              IsSuccess = true;
            }
          }
        }
      }
      sqlite3_finalize(stmt);
    }
    sqlite3_close(DataBaseLink);
  }
  return IsSuccess;
}
void LocateDataBaseStruct (DATABASE** DataBase, char* DataBasePath) {

  (*DataBase) = (DATABASE*) malloc (sizeof (DATABASE) * 1);
  (*DataBase)->DataBasePath = MergeString (DataBasePath);

  (*DataBase)->CheckDataBase   = CheckDataBase;
  (*DataBase)->GetVariable     = GetVariable;
  (*DataBase)->GetInterface    = GetInterface;
  (*DataBase)->GetItemList     = GetItemList;
  (*DataBase)->GetComic        = GetComic;
  (*DataBase)->GetComicList    = GetComicList;
  (*DataBase)->GetChapter      = GetChapter;
  (*DataBase)->GetChapterList  = GetChapterList;
  (*DataBase)->GetShowBaseUrl  = GetShowBaseUrl;

  (*DataBase)->SetVariable     = SetVariable;
  (*DataBase)->SetChapter      = SetChapter;
  (*DataBase)->SetShowBaseUrl  = SetShowBaseUrl;

  (*DataBase)->FreeDataBase    = FreeDataBase;

  (*DataBase)->CheckDataBase (*DataBase);
}
//-------------------------------------------------------------------function

void ShowHelpInterface (){
  system ("cls");

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

  Print (" %=BW %76s| %=Nw"         ,"請按 空白鍵 繼續，本程式沒有小天使可以協助。");

  SetCursorVisible (false);
  getch ();
  system ("cls");
  SetCursorVisible (true);
}
int InterfaceCursor (int Left, int Top, char *Key, int PageItemCount, int *NowIndex, int Height, int page, int ItemCount,bool* IsHelp) {
  int ReturnValue = 0;

  SetCursorPosition (Left, Top + (*NowIndex % Height) );
  printf ("%s\b", Key);
  //72 104

  int firstchar = getch ();

  if (firstchar == 72 || firstchar == 104) {
    ShowHelpInterface ();
    *IsHelp = true;
  }

  if (firstchar == 224) {
    switch (getch ()) {
      case 0x48: // up
        *NowIndex = *NowIndex - 1;
        break;
      case 0x50: // down
        *NowIndex = *NowIndex + 1;
        break;
      case 0x4B: //left
        ReturnValue = -1;
        break;
      case 0x4D: // right
        ReturnValue = *NowIndex + 1;
        break;
      case 0x49: // page up
        *NowIndex = (page * Height) - 1;
        break;
      case 0x51: // page down
        *NowIndex = (page) * Height + PageItemCount;
        break;
      case 0x47: // home
        *NowIndex = 0;
        break;
      case 0x4F: //end
        *NowIndex = ItemCount;
        break;
    }
  }

  printf (" \b");
  return ReturnValue;
}
void  ShowMainMain(DATABASE* DataBase,int *Choice) {

  INTERFACE* Interface  = DataBase->GetInterface (DataBase, "MainMenu");

  if (Interface != NULL) {
    ITEM*      ItemList   = DataBase->GetItemList (DataBase, Interface->Sequence);
    if (ItemList!=NULL) {
      int   Top = 0;
      int   Left = 0;
      int   page = 0;
      int   AllHeight = 19;
      int   ItemCount = ItemList->GetItemCount (ItemList);
      int   InPageItemCount  = 0;
      int   NowIndex = 0;

      SetCursorVisible (false);

      system ("CLS");

      *Choice = *Choice - 1;
      if (*Choice >= 0)
        if (*Choice >= ItemCount)
          NowIndex = ItemCount - 1;
        else
          NowIndex = *Choice;
      else
        NowIndex = ItemCount - 1;
      page = NowIndex / AllHeight;

      while (ItemList != NULL) {
        int Height = AllHeight;
        InPageItemCount = 0;

        Print ("\n");
        Print (" %=bW %76s| %=Nw\n", Interface->Title);
        Print (" %=Nw %76s| %=Nw\n", Interface->Explain);

        Print (" %=wN %76s| %=Nw\n", Interface->ItemNameTitle);
        GetCursorPosition (&Left, &Top);

        ITEM* Item = ItemList->GetStartItem (ItemList, NowIndex / Height * Height);
        for (int Index = 0; Index < Height; Index++) {
          if (Item != NULL) {
            Print ("   %5d. %15s -- %20s< %28s\n",(Index + 1) + (page * Height), Item->Name, Item->EnglishName, Item->Explain);
            InPageItemCount++;
            Item = Item->Next;
          }
          else {
            printf (" \n");
          }
        }

        Print (" %=cW %20s %=wN %62s %=Nw", Interface->Content,"%=wr(h)%=wN Help");

        while (true) {
          bool IsHelp = false;
          if (NowIndex == -1)
            NowIndex = 0;
          if (NowIndex >= InPageItemCount + page * Height)
            NowIndex = InPageItemCount + page * Height - 1;

          *Choice = InterfaceCursor ( 2, Top, Interface->Symbol, InPageItemCount, &NowIndex, Height, page, ItemCount, &IsHelp);
          if (IsHelp)
            break;
          if (((NowIndex) > InPageItemCount + page * Height - 1 && Item != NULL) || (NowIndex < (page) * Height && NowIndex != -1)) {
            page = NowIndex / Height;
            break;
          }
          if (*Choice != 0)
            break;

        }
        system ("CLS");
        if (*Choice != 0)
          break;

      }
      ItemList->FreeItem (ItemList);
    }
    Interface->FreeInterface (Interface);
  }
  SetCursorVisible (true);
}

void ShowContinueUpDateMenu (DATABASE* DataBase, int *Choice){

  INTERFACE* Interface  = DataBase->GetInterface (DataBase, "IsContinueUpDate");
  if (Interface != NULL) {
    ITEM*      ItemList   = DataBase->GetItemList (DataBase, Interface->Sequence);
    if (ItemList!=NULL) {
      int   Top = 0;
      int   Left = 0;
      int   page = 0;
      int   AllHeight = 15;
      int   ItemCount = ItemList->GetItemCount (ItemList);
      int   InPageItemCount  = 0;
      int   NowIndex = 0;

      char* StartLine            = DataBase->GetVariable (DataBase, "Line_Start");

      SetCursorVisible (false);

      system ("CLS");

      *Choice = *Choice - 1;
      if (*Choice >= 0)
        if (*Choice >= ItemCount)
          NowIndex = ItemCount - 1;
        else
          NowIndex = *Choice;
      else
        NowIndex = ItemCount - 1;
      page = NowIndex / AllHeight;

      while (ItemList != NULL) {
        int Height = AllHeight;
        InPageItemCount = 0;

        Print ("\n");
        Print (" %=RW %76s| %=Nw\n", Interface->Title);
        Print (" %=rW %76s| %=Nw\n", StartLine);

        int MessageLiseCount = 0;
        char** MessageLise = Interface->GetMessageLise (Interface, &MessageLiseCount);
        for (int Index = 0; Index < MessageLiseCount; Index++, Height--) {
          Print (" %=rW %76s| %=Nw\n", MessageLise[Index]);
          Free (MessageLise[Index]);
        }
        Free (MessageLise);

        Print (" %=rW %76s| %=Nw\n", StartLine);
        Print ("\n");
        Print (" %=bW %76s| %=Nw\n", Interface->Name);
        Print (" %=Nw %76s| %=Nw\n", Interface->Explain);
        Print (" %=wN %76s| %=Nw\n", Interface->ItemNameTitle);
        GetCursorPosition (&Left, &Top);

        ITEM* Item = ItemList->GetStartItem (ItemList, NowIndex / Height * Height);
        for (int Index = 0; Index < Height; Index++) {
          if (Item != NULL) {
            Print ("   %3d. %20s -- %23s< %22s\n",(Index + 1) + (page * Height), Item->Name, Item->EnglishName, Item->Explain);
            InPageItemCount++;
            Item = Item->Next;
          }
          else {
            printf (" \n");
          }
        }

        Print (" %=cW %20s %=wN %62s %=Nw", Interface->Content,"%=wr(h)%=wN Help");

        while (true) {
          bool IsHelp = false;

          if (NowIndex == -1)
            NowIndex = 0;
          if (NowIndex >= InPageItemCount + page * Height)
            NowIndex = InPageItemCount + page * Height - 1;

          *Choice = InterfaceCursor ( 2, Top, Interface->Symbol, InPageItemCount, &NowIndex, Height, page, ItemCount, &IsHelp);
          if (IsHelp)
            break;
          if (((NowIndex) > InPageItemCount + page * Height - 1 && Item != NULL) || (NowIndex < (page) * Height && NowIndex != -1)) {
            page = NowIndex / Height;
            break;
          }
          if (*Choice != 0)
            break;
        }
        system ("CLS");
        if (*Choice != 0)
          break;

      }
      Free (StartLine);
      ItemList->FreeItem (ItemList);
    }
    Interface->FreeInterface (Interface);
  }
  SetCursorVisible (true);
}

void TransformJs (DATABASE* DataBase, char* FileName){

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
    char   *Url = NULL;


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
            Url = (char *) malloc (sizeof (char) * (UrlLength));

            for (Index = Middle, UrlIndex = 0; Index < End; Index++) {
                Url[UrlIndex++] = Article[Index];
            }
            Url[UrlIndex++] = '\0';

            char* Key = MergeString ("%d", ConditionIndex);
            SetShowBaseUrl (DataBase, Key, Url);
            Free(Key);
            Free (Url);
          }
          Free (StartKey);
        }
        Free (Article);
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

void DeCodes(DATABASE* DataBase, char* Codes, char* ComicSeq){
  if ((DataBase != NULL) && (Codes != NULL) && (ComicSeq != NULL)) {
    int Index = 0;
    int KeyNum = 0;

    int IndexStart = -1, IndexEnd = -1;
    int SidStart = -1, SidEnd = -1;
    int DidStart = -1, DidEnd = -1;
    int PicRangeStart = -1, PicRangeEnd = -1;
    int CodeStart = -1, CodeEnd = -1;

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
        int IndexLength    = IndexEnd    - IndexStart + 1;
        int SidLength      = SidEnd      - SidStart + 1;
        int DidLength      = DidEnd      - DidStart + 1;
        int PicRangeLength = PicRangeEnd - PicRangeStart + 1;
        int CodeLength     = CodeEnd     - CodeStart + 1;

        char* ChapterIndex = (char*) malloc (sizeof (char) * (IndexLength + 1));
        char* Sid          = (char*) malloc (sizeof (char) * (SidLength + 1));
        char* Did          = (char*) malloc (sizeof (char) * (DidLength + 1));
        char* PicRange     = (char*) malloc (sizeof (char) * (PicRangeLength + 1));
        char* Code         = (char*) malloc (sizeof (char) * (CodeLength + 1));

        IndexLength = 0;
        SidLength = 0;
        DidLength = 0;
        PicRangeLength = 0;
        CodeLength = 0;

        for (int TempIndex = IndexStart; ((TempIndex < strlen (Codes) && (TempIndex < IndexEnd + 1) && ((ChapterIndex[IndexLength++] = Codes[TempIndex]) != '\0')) || ((ChapterIndex[IndexLength++] = '\0') != '\0')); TempIndex++);
        for (int TempIndex = SidStart; ((TempIndex < strlen (Codes) && (TempIndex < SidEnd + 1) && ((Sid[SidLength++] = Codes[TempIndex]) != '\0')) || ((Sid[SidLength++] = '\0') != '\0')); TempIndex++);
        for (int TempIndex = DidStart; ((TempIndex < strlen (Codes) && (TempIndex < DidEnd + 1) && ((Did[DidLength++] = Codes[TempIndex]) != '\0')) || ((Did[DidLength++] = '\0') != '\0')); TempIndex++);
        for (int TempIndex = PicRangeStart; ((TempIndex < strlen (Codes) && (TempIndex < PicRangeEnd + 1) && ((PicRange[PicRangeLength++] = Codes[TempIndex]) != '\0')) || ((PicRange[PicRangeLength++] = '\0') != '\0')); TempIndex++);
        for (int TempIndex = CodeStart; ((TempIndex < strlen (Codes) && (TempIndex < CodeEnd + 1) && ((Code[CodeLength++] = Codes[TempIndex]) != '\0')) || ((Code[CodeLength++] = '\0') != '\0')); TempIndex++);

        IndexStart    = -1; IndexEnd    = -1;
        SidStart      = -1; SidEnd      = -1;
        DidStart      = -1; DidEnd      = -1;
        PicRangeStart = -1; PicRangeEnd = -1;
        CodeStart     = -1; CodeEnd     = -1;

        Print ("%=NR  ======> 將第 %4s 章節的 Code 存入資料庫!%=NN<ClearPoint>%=Nw\n", ChapterIndex);
        DataBase->SetChapter (DataBase, ComicSeq, ChapterIndex, Sid, Did, PicRange, Code);
        Print ("%=NR  ======> 第 %4s 章節的 Code 存入資料庫完成!%=Nw\n", ChapterIndex);
        ClearScreenToKey("<ClearPoint>");
      }
    }
  }
}

void ShowUpdateEndOpenBrowser (DATABASE* DataBase, int *Choice){

  INTERFACE* Interface  = DataBase->GetInterface (DataBase, "IsUpDateEndOpenBrowser");
  if (Interface != NULL) {
    ITEM*      ItemList   = DataBase->GetItemList (DataBase, Interface->Sequence);
    if (ItemList!=NULL) {
      int   Top = 0;
      int   Left = 0;
      int   page = 0;
      int   AllHeight = 15;
      int   ItemCount = ItemList->GetItemCount (ItemList);
      int   InPageItemCount  = 0;
      int   NowIndex = 0;

      char* StartLine            = DataBase->GetVariable (DataBase, "Line_Start");

      SetCursorVisible (false);

      system ("CLS");

      *Choice = *Choice - 1;
      if (*Choice >= 0)
        if (*Choice >= ItemCount)
          NowIndex = ItemCount - 1;
        else
          NowIndex = *Choice;
      else
        NowIndex = ItemCount - 1;
      page = NowIndex / AllHeight;

      while (ItemList != NULL) {
        int Height = AllHeight;
        InPageItemCount = 0;

        Print ("\n");
        Print (" %=RW %76s| %=Nw\n", Interface->Title);
        Print (" %=rW %76s| %=Nw\n", StartLine);

        int MessageLiseCount = 0;
        char** MessageLise = Interface->GetMessageLise (Interface, &MessageLiseCount);
        for (int Index = 0; Index < MessageLiseCount; Index++, Height--) {
          Print (" %=rW %76s| %=Nw\n", MessageLise[Index]);
          Free (MessageLise[Index]);
        }
        Free (MessageLise);

        Print (" %=rW %76s| %=Nw\n", StartLine);
        Print ("\n");
        Print (" %=bW %76s| %=Nw\n", Interface->Name);
        Print (" %=Nw %76s| %=Nw\n", Interface->Explain);
        Print (" %=wN %76s| %=Nw\n", Interface->ItemNameTitle);
        GetCursorPosition (&Left, &Top);

        ITEM* Item = ItemList->GetStartItem (ItemList, NowIndex / Height * Height);
        for (int Index = 0; Index < Height; Index++) {
          if (Item != NULL) {
            Print ("   %3d. %20s -- %23s< %22s\n",(Index + 1) + (page * Height), Item->Name, Item->EnglishName, Item->Explain);
            InPageItemCount++;
            Item = Item->Next;
          }
          else {
            printf (" \n");
          }
        }

        Print (" %=cW %20s %=wN %62s %=Nw", Interface->Content,"%=wr(h)%=wN Help");

        while (true) {
          bool IsHelp = false;

          if (NowIndex == -1)
            NowIndex = 0;
          if (NowIndex >= InPageItemCount + page * Height)
            NowIndex = InPageItemCount + page * Height - 1;

          *Choice = InterfaceCursor ( 2, Top, Interface->Symbol, InPageItemCount, &NowIndex, Height, page, ItemCount, &IsHelp);

          if (IsHelp)
            break;
          if (((NowIndex) > InPageItemCount + page * Height - 1 && Item != NULL) || (NowIndex < (page) * Height && NowIndex != -1)) {
            page = NowIndex / Height;
            break;
          }
          if (*Choice != 0)
            break;
        }
        system ("CLS");
        if (*Choice != 0)
          break;

      }
      Free (StartLine);
      ItemList->FreeItem (ItemList);
    }
    Interface->FreeInterface (Interface);
  }
  SetCursorVisible (true);
}
void ShowNoConnected (DATABASE* DataBase, int *Choice){

  INTERFACE* Interface  = DataBase->GetInterface (DataBase, "NoConnected");
  if (Interface != NULL) {
    ITEM*      ItemList   = DataBase->GetItemList (DataBase, Interface->Sequence);
    if (ItemList!=NULL) {
      int   Top = 0;
      int   Left = 0;
      int   page = 0;
      int   AllHeight = 15;
      int   ItemCount = ItemList->GetItemCount (ItemList);
      int   InPageItemCount  = 0;
      int   NowIndex = 0;

      char* StartLine            = DataBase->GetVariable (DataBase, "Line_Start");

      SetCursorVisible (false);

      system ("CLS");

      *Choice = *Choice - 1;
      if (*Choice >= 0)
        if (*Choice >= ItemCount)
          NowIndex = ItemCount - 1;
        else
          NowIndex = *Choice;
      else
        NowIndex = ItemCount - 1;
      page = NowIndex / AllHeight;

      while (ItemList != NULL) {
        int Height = AllHeight;
        InPageItemCount = 0;

        Print ("\n");
        Print (" %=RW %76s| %=Nw\n", Interface->Title);
        Print (" %=rW %76s| %=Nw\n", StartLine);

        int MessageLiseCount = 0;
        char** MessageLise = Interface->GetMessageLise (Interface, &MessageLiseCount);
        for (int Index = 0; Index < MessageLiseCount; Index++, Height--) {
          Print (" %=rW %76s| %=Nw\n", MessageLise[Index]);
          Free (MessageLise[Index]);
        }
        Free (MessageLise);

        Print (" %=rW %76s| %=Nw\n", StartLine);
        Print ("\n");
        Print (" %=bW %76s| %=Nw\n", Interface->Name);
        Print (" %=Nw %76s| %=Nw\n", Interface->Explain);
        Print (" %=wN %76s| %=Nw\n", Interface->ItemNameTitle);
        GetCursorPosition (&Left, &Top);

        ITEM* Item = ItemList->GetStartItem (ItemList, NowIndex / Height * Height);
        for (int Index = 0; Index < Height; Index++) {
          if (Item != NULL) {
            Print ("   %3d. %20s -- %23s< %22s\n",(Index + 1) + (page * Height), Item->Name, Item->EnglishName, Item->Explain);
            InPageItemCount++;
            Item = Item->Next;
          }
          else {
            printf (" \n");
          }
        }

        Print (" %=cW %20s %=wN %62s %=Nw", Interface->Content,"%=wr(h)%=wN Help");

        while (true) {
          bool IsHelp = false;

          if (NowIndex == -1)
            NowIndex = 0;
          if (NowIndex >= InPageItemCount + page * Height)
            NowIndex = InPageItemCount + page * Height - 1;

          *Choice = InterfaceCursor ( 2, Top, Interface->Symbol, InPageItemCount, &NowIndex, Height, page, ItemCount, &IsHelp);
          if (IsHelp)
            break;
          if (((NowIndex) > InPageItemCount + page * Height - 1 && Item != NULL) || (NowIndex < (page) * Height && NowIndex != -1)) {
            page = NowIndex / Height;
            break;
          }
          if (*Choice != 0)
            break;
        }
        system ("CLS");
        if (*Choice != 0)
          break;

      }
      Free (StartLine);
      ItemList->FreeItem (ItemList);
    }
    Interface->FreeInterface (Interface);
  }
  SetCursorVisible (true);
}
void OpenBrowser (DATABASE* DataBase) {
  Print (" ==> 請稍後，正在開啟瀏覽器..\n");
  char* BrowserIndex = DataBase->GetVariable (DataBase,"File_BrowserIndex");
  system (BrowserIndex);
  Free (BrowserIndex);
  Print (" ==> 開啟瀏覽器完成!\n");
}
void Update (DATABASE* DataBase) {
  int Choice = 1;
  ShowContinueUpDateMenu (DataBase, &Choice);
  if (Choice == 1) {
    COMIC* ComicList = DataBase->GetComicList (DataBase);
    if (ComicList != NULL) {
      int   AddProgress = 100/((ComicList->GetComicCount(ComicList) * 5) + 5);
      int   Progress = 0;
      char* WgetFile        = DataBase->GetVariable (DataBase, "File_Wget");
      char* WebBaseUrl      = DataBase->GetVariable (DataBase, "Url_WebBaseUrl");
      char* TempFile        = DataBase->GetVariable (DataBase, "Folder_TempFile");

      ProgressBarTop (Progress,"Update");

      Print (" ==> 測試網路與連接網站..\n");

      RunCommand (WgetFile, WebBaseUrl, TempFile, false);
      Free (WebBaseUrl);

      if (GetFileSize(TempFile) > 0 && IsConnectedToInternet()) {
        ProgressBarTop (Progress+=AddProgress,"Update");
        Print (" ==> 測試網路完成!\n");

        char* ComicViewUrl = DataBase->GetVariable (DataBase, "Url_ComicView");

        Print ("\n");
        Print (" ==> 抓取 ComicView 檔案!\n");
        RunCommand (WgetFile, ComicViewUrl, TempFile, false);
        Free (ComicViewUrl);
        ProgressBarTop (Progress+=AddProgress,"Update");
        Print (" ==> 抓取 ComicView 檔案完成!\n");

        Print ("\n");
        Print (" ==> 翻譯轉換 ComicView 檔案!\n");
        TransformJs (DataBase, TempFile);
        ProgressBarTop (Progress+=AddProgress,"Update");
        Print (" ==> 翻譯轉換 ComicView 檔案完成!\n");

        char*  HtmlBaseUrl = DataBase->GetVariable (DataBase, "Url_HtmlBaseUrl");

        FILE *FilePoint = NULL;
        char* AllComicCodeFile = DataBase->GetVariable (DataBase, "File_AllComicCode");
        ClearFile(AllComicCodeFile);

        if ((FilePoint = fopen (AllComicCodeFile, "a")) != NULL) {

          Print ("\n");
          Print (" ==> 寫 AllComicCodeFile 檔頭!\n");
          putc (0xEF, FilePoint);
          putc (0xBB, FilePoint);
          putc (0xBF, FilePoint);

          fprintf (FilePoint, "%s\n", ANSIToUTF8 ("//"));
          fprintf (FilePoint, "%s\n", ANSIToUTF8 ("// 漫畫代碼,名稱,章節代碼"));
          fprintf (FilePoint, "%s\n", ANSIToUTF8 ("//"));
          fprintf (FilePoint, "%s\n", ANSIToUTF8 ("var gComicList = new Array ("));
          ProgressBarTop (Progress+=AddProgress,"Update");
          Print (" ==> 寫檔頭完成!\n");

          for (COMIC* Comic = ComicList; Comic != NULL; Comic = Comic->Next) {
            system ("cls");
            ProgressBarTop (Progress,"Update");

            Print (" ==> 取得 %s 的 Html 檔案!\n", Comic->Name);
            char* HtmlUrl = MergeString ("%s%s.html",HtmlBaseUrl, Comic->ComicIndex);
            RunCommand (WgetFile, HtmlUrl, TempFile, false);
            Free (HtmlUrl);
            ProgressBarTop (Progress+=AddProgress,"Update");
            Print (" ==> 取得 %s 的 Html 完成!\n", Comic->Name);


            Print ("\n");
            Print (" ==> 取得 %s 的 Show 檔案!\n", Comic->Name);

            char* ShowKeySatrt = MergeString ("cview('%s-1.html',", Comic->ComicIndex);
            char* ShowKey = NULL;
            ShowKey = GetKeySearchValue (ShowKeySatrt,");",TempFile,true);
            Free (ShowKeySatrt);

            char* ShowBaseUrl = DataBase->GetShowBaseUrl (DataBase, ShowKey);
            Free (ShowKey);

            char* ShowUrl = MergeString ("%s%s.html",ShowBaseUrl, Comic->ComicIndex);
            Free (ShowBaseUrl);
            RunCommand (WgetFile, ShowUrl, TempFile, false);
            Free (ShowUrl);
            ProgressBarTop (Progress+=AddProgress,"Update");
            Print (" ==> 取得 %s 的 Show 完成!\n", Comic->Name);


            Print ("\n");
            Print (" ==> 取得 %s Codes!\n", Comic->Name);
            char* Codes = GetKeySearchValue ("codes=\"","\".split('|')",TempFile,false);
            ProgressBarTop (Progress+=AddProgress,"Update");
            Print (" ==> 取得 %s Codes 完成!\n", Comic->Name);

            Print ("\n");
            Print (" ==> 解碼 DeCodes!\n");
            DeCodes (DataBase, Codes, Comic->Sequence);
            ProgressBarTop (Progress+=AddProgress,"Update");
            Print (" ==> 解碼 DeCodes 完成!\n");


            Print ("\n");
            Print (" ==> 將 Code 寫入 AllComicCodeFile 檔案!\n");
            fprintf (FilePoint, "  new Array (%s, \"%s\", \"%s\")", Comic->ComicIndex, ANSIToUTF8 (Comic->Name), Codes);
            if (Comic->Next != NULL)
              fprintf (FilePoint, ",");
            fprintf (FilePoint, "\n");
            ProgressBarTop (Progress+=AddProgress,"Update");
            Print (" ==> 寫入 Code 檔案 完成!\n");

            Free (Codes);
            ClearFile (TempFile);
            system ("cls");
          }
          Print ("\n");
          Print (" ==> 寫 AllComicCodeFile 檔尾!\n");
          fprintf (FilePoint, "%s\n", ANSIToUTF8 (");"));
          fclose (FilePoint);
          ProgressBarTop (Progress+=AddProgress,"Update");
          Print (" ==> 寫檔尾完成!\n");
        }
        Free (AllComicCodeFile);
        Free (HtmlBaseUrl);

        Choice = 1;
        ShowUpdateEndOpenBrowser (DataBase,&Choice);
        if (Choice == 1) {
          OpenBrowser (DataBase);
        }
      }
      else {
        ShowNoConnected (DataBase,&Choice);
      }
      Free (TempFile);
      Free (WgetFile);
      ComicList->FreeComic (ComicList);
    }
    else {
      Print (" ==> 資料庫有誤，找不到漫畫資料檔!\n");
    }
  }
}


void ShowContinueDownloadMenu (DATABASE* DataBase, int *Choice){
  INTERFACE* Interface  = DataBase->GetInterface (DataBase, "ContinueDownload");
  if (Interface != NULL) {
    ITEM*      ItemList   = DataBase->GetItemList (DataBase, Interface->Sequence);
    if (ItemList!=NULL) {
      int   Top = 0;
      int   Left = 0;
      int   page = 0;
      int   AllHeight = 15;
      int   ItemCount = ItemList->GetItemCount (ItemList);
      int   InPageItemCount  = 0;
      int   NowIndex = 0;

      char* StartLine            = DataBase->GetVariable (DataBase, "Line_Start");

      SetCursorVisible (false);

      system ("CLS");

      *Choice = *Choice - 1;
      if (*Choice >= 0)
        if (*Choice >= ItemCount)
          NowIndex = ItemCount - 1;
        else
          NowIndex = *Choice;
      else
        NowIndex = ItemCount - 1;
      page = NowIndex / AllHeight;

      while (ItemList != NULL) {
        int Height = AllHeight;
        InPageItemCount = 0;

        Print ("\n");
        Print (" %=RW %76s| %=Nw\n", Interface->Title);
        Print (" %=rW %76s| %=Nw\n", StartLine);

        int MessageLiseCount = 0;
        char** MessageLise = Interface->GetMessageLise (Interface, &MessageLiseCount);
        for (int Index = 0; Index < MessageLiseCount; Index++, Height--) {
          Print (" %=rW %76s| %=Nw\n", MessageLise[Index]);
          Free (MessageLise[Index]);
        }
        Free (MessageLise);

        Print (" %=rW %76s| %=Nw\n", StartLine);
        Print ("\n");
        Print (" %=bW %76s| %=Nw\n", Interface->Name);
        Print (" %=Nw %76s| %=Nw\n", Interface->Explain);
        Print (" %=wN %76s| %=Nw\n", Interface->ItemNameTitle);
        GetCursorPosition (&Left, &Top);

        ITEM* Item = ItemList->GetStartItem (ItemList, NowIndex / Height * Height);
        for (int Index = 0; Index < Height; Index++) {
          if (Item != NULL) {
            Print ("   %3d. %20s -- %24s< %21s\n",(Index + 1) + (page * Height), Item->Name, Item->EnglishName, Item->Explain);
            InPageItemCount++;
            Item = Item->Next;
          }
          else {
            printf (" \n");
          }
        }

        Print (" %=cW %20s %=wN %62s %=Nw", Interface->Content,"%=wr(h)%=wN Help");

        while (true) {
          bool IsHelp = false;

          if (NowIndex == -1)
            NowIndex = 0;
          if (NowIndex >= InPageItemCount + page * Height)
            NowIndex = InPageItemCount + page * Height - 1;

          *Choice = InterfaceCursor ( 2, Top, Interface->Symbol, InPageItemCount, &NowIndex, Height, page, ItemCount, &IsHelp);
          if (IsHelp)
            break;
          if (((NowIndex) > InPageItemCount + page * Height - 1 && Item != NULL) || (NowIndex < (page) * Height && NowIndex != -1)) {
            page = NowIndex / Height;
            break;
          }
          if (*Choice != 0)
            break;
        }
        system ("CLS");
        if (*Choice != 0)
          break;

      }
      Free (StartLine);
      ItemList->FreeItem (ItemList);
    }
    Interface->FreeInterface (Interface);
  }
  SetCursorVisible (true);
}

void  ShowComicList(DATABASE* DataBase,int *Choice) {

  INTERFACE* Interface  = DataBase->GetInterface (DataBase, "ComicListMenu");

  if (Interface != NULL) {
    COMIC*      ComicList   = DataBase->GetComicList (DataBase);
    if (ComicList!=NULL) {
      int   Top = 0;
      int   Left = 0;
      int   page = 0;
      int   AllHeight = 19;
      int   ItemCount = ComicList->GetComicCount (ComicList);
      int   InPageItemCount  = 0;
      int   NowIndex = 0;

      SetCursorVisible (false);

      system ("CLS");

      *Choice = *Choice - 1;
      if (*Choice >= 0)
        if (*Choice >= ItemCount)
          NowIndex = ItemCount - 1;
        else
          NowIndex = *Choice;
      else
        NowIndex = ItemCount - 1;
      page = NowIndex / AllHeight;

      while (ComicList != NULL) {
        int Height = AllHeight;
        InPageItemCount = 0;

        Print ("\n");
        Print (" %=bW %76s| %=Nw\n", Interface->Title);
        Print (" %=Nw %76s| %=Nw\n", Interface->Explain);

        Print (" %=wN %76s| %=Nw\n", Interface->ItemNameTitle);
        GetCursorPosition (&Left, &Top);

        COMIC* Comic = ComicList->GetStartComic (ComicList, NowIndex / Height * Height);
        for (int Index = 0; Index < Height; Index++) {
          if (Comic != NULL) {
            char* ComicChapterCount = MergeString ("共有 %3d 集(話)",Comic->GetThisComicChapterCount (DataBase, Comic));
            Print ("   %5d. %15s -- %20s< %28s\n",(Index + 1) + (page * Height), Comic->Name, Comic->Name, ComicChapterCount);
            Free (ComicChapterCount);
            InPageItemCount++;
            Comic = Comic->Next;
          }
          else {
            printf (" \n");
          }
        }

        Print (" %=cW %20s %=wN %62s %=Nw", Interface->Content,"%=wr(h)%=wN Help");

        while (true) {
          bool IsHelp = false;

          if (NowIndex == -1)
            NowIndex = 0;
          if (NowIndex >= InPageItemCount + page * Height)
            NowIndex = InPageItemCount + page * Height - 1;

          *Choice = InterfaceCursor ( 2, Top, Interface->Symbol, InPageItemCount, &NowIndex, Height, page, ItemCount, &IsHelp);
          if (IsHelp)
            break;
          if (((NowIndex) > InPageItemCount + page * Height - 1 && Comic != NULL) || (NowIndex < (page) * Height && NowIndex != -1)) {
            page = NowIndex / Height;
            break;
          }
          if (*Choice != 0)
            break;
        }
        system ("CLS");
        if (*Choice != 0)
          break;
      }
      if (*Choice>0) {
        COMIC* Comic = ComicList->GetStartComic (ComicList, *Choice - 1);
        *Choice = atoi (Comic->Sequence);
      }
      ComicList->FreeComic (ComicList);
    }
  }
  SetCursorVisible (true);
  Interface->FreeInterface (Interface);
}


void  ShowChapterList(DATABASE* DataBase,int *Choice, int ComicSeq) {

  INTERFACE* Interface  = DataBase->GetInterface (DataBase, "ChapterListMenu");

  if (Interface != NULL) {
    char* ComicSeqStringType = MergeString ("%d", ComicSeq);
    CHAPTER*      ChapterList   = DataBase->GetChapterList (DataBase, ComicSeqStringType);
    Free (ComicSeqStringType);

    if (ChapterList!=NULL) {
      int   Top = 0;
      int   Left = 0;
      int   page = 0;
      int   AllHeight = 19;
      int   ItemCount = ChapterList->GetChapterCount (ChapterList);
      int   InPageItemCount  = 0;
      int   NowIndex = 0;



      SetCursorVisible (false);

      system ("CLS");

      *Choice = *Choice - 1;
      if (*Choice >= 0)
        if (*Choice >= ItemCount)
          NowIndex = ItemCount - 1;
        else
          NowIndex = *Choice;
      else
        NowIndex = ItemCount - 1;
      page = NowIndex / AllHeight;

      while (ChapterList != NULL) {
        int Height = AllHeight;
        InPageItemCount = 0;

        Print ("\n");
        Print (" %=bW %76s| %=Nw\n", Interface->Title);
        Print (" %=Nw %76s| %=Nw\n", Interface->Explain);

        Print (" %=wN %76s| %=Nw\n", Interface->ItemNameTitle);
        GetCursorPosition (&Left, &Top);

        CHAPTER* Chapter = ChapterList->GetStartChapter (ChapterList, NowIndex / Height * Height);
        for (int Index = 0; Index < Height; Index++) {
          if (Chapter != NULL) {
            Print ("   %5d.      第 %4s 章節 -- Chapter. %10s<         共有 %3s 張漫畫圖片\n",(Index + 1) + (page * Height), Chapter->ChapterIndex, Chapter->ChapterIndex, Chapter->PicRange);
            InPageItemCount++;
            Chapter = Chapter->Next;
          }
          else {
            printf (" \n");
          }
        }

        Print (" %=cW %20s %=wN %62s %=Nw", Interface->Content,"%=wr(h)%=wN Help");

        while (true) {
          bool IsHelp = false;

          if (NowIndex == -1)
            NowIndex = 0;
          if (NowIndex >= InPageItemCount + page * Height)
            NowIndex = InPageItemCount + page * Height - 1;

          *Choice = InterfaceCursor ( 2, Top, Interface->Symbol, InPageItemCount, &NowIndex, Height, page, ItemCount, &IsHelp);
          if (IsHelp)
            break;
          if (((NowIndex) > InPageItemCount + page * Height - 1 && Chapter != NULL) || (NowIndex < (page) * Height && NowIndex != -1)) {
            page = NowIndex / Height;
            break;
          }
          if (*Choice != 0)
            break;
        }
        system ("CLS");
        if (*Choice != 0)
          break;
      }
      if (*Choice>0) {
        CHAPTER* Chapter = ChapterList->GetStartChapter (ChapterList, *Choice - 1);
        *Choice = atoi (Chapter->Sequence);
      }
      ChapterList->FreeChapter (ChapterList);
    }
    Interface->FreeInterface (Interface);
  }
  SetCursorVisible (true);
}
void CheckPath (char* Path) {

  Print ("%=NW<ClearPoint>%=Nw");
  char* CcCommand = MergeString ("CD %s", Path);
  bool IsHasPath = (system(CcCommand) == 0 ? true:false);
  Free (CcCommand);
  ClearScreenToKey("<ClearPoint>");


  if (IsHasPath) {
    char* DelCommand = MergeString ("DEL %s /Q", Path);
    system(DelCommand);
    Free (DelCommand);
  }
  else {
    char* MdCommand = MergeString ("MD %s", Path);
    system(MdCommand);
    Free (MdCommand);
  }
}

void ShowDownloadEndOpenFolder (DATABASE* DataBase, int *Choice){

  INTERFACE* Interface  = DataBase->GetInterface (DataBase, "IsDownloadEndOpenFolder");
  if (Interface != NULL) {
    ITEM*      ItemList   = DataBase->GetItemList (DataBase, Interface->Sequence);
    if (ItemList!=NULL) {
      int   Top = 0;
      int   Left = 0;
      int   page = 0;
      int   AllHeight = 15;
      int   ItemCount = ItemList->GetItemCount (ItemList);
      int   InPageItemCount  = 0;
      int   NowIndex = 0;

      char* StartLine            = DataBase->GetVariable (DataBase, "Line_Start");

      SetCursorVisible (false);

      system ("CLS");

      *Choice = *Choice - 1;
      if (*Choice >= 0)
        if (*Choice >= ItemCount)
          NowIndex = ItemCount - 1;
        else
          NowIndex = *Choice;
      else
        NowIndex = ItemCount - 1;
      page = NowIndex / AllHeight;

      while (ItemList != NULL) {
        int Height = AllHeight;
        InPageItemCount = 0;

        Print ("\n");
        Print (" %=RW %76s| %=Nw\n", Interface->Title);
        Print (" %=rW %76s| %=Nw\n", StartLine);

        int MessageLiseCount = 0;
        char** MessageLise = Interface->GetMessageLise (Interface, &MessageLiseCount);
        for (int Index = 0; Index < MessageLiseCount; Index++, Height--) {
          Print (" %=rW %76s| %=Nw\n", MessageLise[Index]);
          Free (MessageLise[Index]);
        }
        Free (MessageLise);

        Print (" %=rW %76s| %=Nw\n", StartLine);
        Print ("\n");
        Print (" %=bW %76s| %=Nw\n", Interface->Name);
        Print (" %=Nw %76s| %=Nw\n", Interface->Explain);
        Print (" %=wN %76s| %=Nw\n", Interface->ItemNameTitle);
        GetCursorPosition (&Left, &Top);

        ITEM* Item = ItemList->GetStartItem (ItemList, NowIndex / Height * Height);
        for (int Index = 0; Index < Height; Index++) {
          if (Item != NULL) {
            Print ("   %3d. %20s -- %26s< %19s\n",(Index + 1) + (page * Height), Item->Name, Item->EnglishName, Item->Explain);
            InPageItemCount++;
            Item = Item->Next;
          }
          else {
            printf (" \n");
          }
        }

        Print (" %=cW %20s %=wN %62s %=Nw", Interface->Content,"%=wr(h)%=wN Help");

        while (true) {
          bool IsHelp = false;

          if (NowIndex == -1)
            NowIndex = 0;
          if (NowIndex >= InPageItemCount + page * Height)
            NowIndex = InPageItemCount + page * Height - 1;

          *Choice = InterfaceCursor ( 2, Top, Interface->Symbol, InPageItemCount, &NowIndex, Height, page, ItemCount, &IsHelp);
          if (IsHelp)
            break;
          if (((NowIndex) > InPageItemCount + page * Height - 1 && Item != NULL) || (NowIndex < (page) * Height && NowIndex != -1)) {
            page = NowIndex / Height;
            break;
          }
          if (*Choice != 0)
            break;
        }
        system ("CLS");
        if (*Choice != 0)
          break;

      }
      Free (StartLine);
      ItemList->FreeItem (ItemList);
    }
    Interface->FreeInterface (Interface);
  }
  SetCursorVisible (true);
}

void Download (DATABASE* DataBase) {
  int Choice = 1;
  ShowContinueDownloadMenu (DataBase, &Choice);
  if (Choice == 1) {
    int ComicSeq = 1;
    while (ComicSeq > 0) {
      ShowComicList (DataBase, &ComicSeq);

      if (ComicSeq > 0) {
        int ChapterSeq = 0;

        while (ChapterSeq >= 0) {
          ChapterSeq = 0;
          ShowChapterList (DataBase, &ChapterSeq, ComicSeq);
          if (ChapterSeq > 0) {
            char*  WgetFile        = DataBase->GetVariable (DataBase, "File_Wget");
            char*  WebBaseUrl      = DataBase->GetVariable (DataBase, "Url_WebBaseUrl");
            char* TempFile        = DataBase->GetVariable (DataBase, "Folder_TempFile");

            ProgressBarTop (0,"Download");
            Print (" ==> 測試網路與連接網站..\n");
            RunCommand (WgetFile, WebBaseUrl, TempFile, false);
            Free (WebBaseUrl);

            if (GetFileSize(TempFile) > 0 && IsConnectedToInternet()) {
              Free (TempFile);
              Print (" ==> 測試網路完成!\n");

              char* ChapterSeqStringType = MergeString ("%d",ChapterSeq);
              CHAPTER* Chapter = DataBase->GetChapter (DataBase, ChapterSeqStringType);
              Free (ChapterSeqStringType);


              if (Chapter != NULL) {
                char*  ChapterIndex        = Chapter->ChapterIndex;
                char*  Sid                 = Chapter->Sid;
                char*  Did                 = Chapter->Did;
                char*  PicRange            = Chapter->PicRange;
                char*  Code                = Chapter->Code;
                char*  KeyCodeLength       = DataBase->GetVariable (DataBase,"PicNameKeyCodeLength");
                char*  PicNameFormat       = DataBase->GetVariable (DataBase, "Format_PicName");
                char*  PicUrlFormat        = DataBase->GetVariable (DataBase, "Format_PicUrl");
                COMIC* Comic               = DataBase->GetComic (DataBase,ComicSeq);
                char*  DownloadFolderPath  = DataBase->GetVariable (DataBase, "Folder_Download");
                char*  DownloadPath        = MergeString ("%s\\%s\\第%03s章節\\", DownloadFolderPath, Comic->Name,Chapter->ChapterIndex);
                Free   (DownloadFolderPath);
                int    AddProgress          = 100/(atoi (PicRange) + 1);
                int    Progress             = 0;

                ProgressBarTop (Progress,"Download");
                Print ("\n");
                Print (" ==> 下載位置初始!\n");
                CheckPath(DownloadPath);
                Print (" ==> 初始位置完成!\n");
                ProgressBarTop (Progress+=AddProgress,"Download");

                for (int Index = 1; Index <= atoi (PicRange); Index++) {
                  system ("CLS");
                  ProgressBarTop (Progress,"Download");

                  int KeyPoint = (((Index - 1) / 10) % 10) + (((Index - 1) % 10) * 3);
                  char* KeyCode = (char*) malloc (sizeof (char)* (atoi (KeyCodeLength) + 1));

                  for (int KeyIndex = KeyPoint, KeyCount = 0; ((KeyIndex < (KeyPoint + atoi (KeyCodeLength))) && ((KeyCode[KeyCount++] = Code[KeyIndex]) != '\0')) ||((KeyCode[KeyCount++] = '\0') != '\0'); KeyIndex++);

                  char* PicName = MergeString (PicNameFormat, Index, KeyCode);
                  char* PicUrl = MergeString (PicUrlFormat, Sid, Did, Comic->ComicIndex, ChapterIndex,PicName);
                  char* FilePath = MergeString ("%s%s",DownloadPath,PicName);

                  Print (" ==> 下載第 %3d 張漫畫!\n", Index);
                  RunCommand (WgetFile, PicUrl, FilePath, false);
                  Print (" ==> 下載漫畫完成!\n");
                  ProgressBarTop (Progress+=AddProgress,"Download");

                  Free (PicName);
                  Free (FilePath);
                  Free (PicUrl);
                  Free (KeyCode);
                  system ("CLS");
                }
                Free (Comic);
                Free (PicUrlFormat);
                Free (PicNameFormat);
                Free (KeyCodeLength);

                ProgressBarTop (100,"Download");

                int Choice = 1;
                ShowDownloadEndOpenFolder (DataBase, &Choice);

                if (Choice == 1) {
                  char* OpenFolderCommand = MergeString ("start %s", DownloadPath);
                  system(OpenFolderCommand);
                  Free (OpenFolderCommand);
                }
                else if (Choice == 2){

                }
                else if (Choice == 3){
                  ChapterSeq = -1;
                }
                else if (Choice == 4){
                  ChapterSeq = -1;
                  ComicSeq = -1;
                }
                Free (DownloadPath);
              }
              Free (Chapter);
            }
            else {
              ShowNoConnected (DataBase,&Choice);
              ChapterSeq = -1;
              ComicSeq = -1;
            }
            Free (WgetFile);
          }
          else {
            break;
          }
        }
      }
      else {
        break;
      }
    }
  }
  else if (Choice == 2) {
    Update (DataBase);
  }
}

void ShowManagementFolderMenu (DATABASE* DataBase, int* Choice) {
  INTERFACE* Interface  = DataBase->GetInterface (DataBase, "ManagementFolderMenu");

  if (Interface != NULL) {
    ITEM*      ItemList   = DataBase->GetItemList (DataBase, Interface->Sequence);
    if (ItemList!=NULL) {
      int   Top = 0;
      int   Left = 0;
      int   page = 0;
      int   AllHeight = 19;
      int   ItemCount = ItemList->GetItemCount (ItemList);
      int   InPageItemCount  = 0;
      int   NowIndex = 0;

      SetCursorVisible (false);

      system ("CLS");

      *Choice = *Choice - 1;
      if (*Choice >= 0)
        if (*Choice >= ItemCount)
          NowIndex = ItemCount - 1;
        else
          NowIndex = *Choice;
      else
        NowIndex = ItemCount - 1;
      page = NowIndex / AllHeight;

      while (ItemList != NULL) {
        int Height = AllHeight;
        InPageItemCount = 0;

        Print ("\n");
        Print (" %=bW %76s| %=Nw\n", Interface->Title);
        Print (" %=Nw %76s| %=Nw\n", Interface->Explain);

        Print (" %=wN %76s| %=Nw\n", Interface->ItemNameTitle);
        GetCursorPosition (&Left, &Top);

        ITEM* Item = ItemList->GetStartItem (ItemList, NowIndex / Height * Height);
        for (int Index = 0; Index < Height; Index++) {
          if (Item != NULL) {
            Print ("   %5d. %15s -- %22s< %26s\n",(Index + 1) + (page * Height), Item->Name, Item->EnglishName, Item->Explain);
            InPageItemCount++;
            Item = Item->Next;
          }
          else {
            printf (" \n");
          }
        }

        Print (" %=cW %20s %=wN %62s %=Nw", Interface->Content,"%=wr(h)%=wN Help");

        while (true) {
          bool IsHelp = false;

          if (NowIndex == -1)
            NowIndex = 0;
          if (NowIndex >= InPageItemCount + page * Height)
            NowIndex = InPageItemCount + page * Height - 1;

          *Choice = InterfaceCursor ( 2, Top, Interface->Symbol, InPageItemCount, &NowIndex, Height, page, ItemCount, &IsHelp);
          if (IsHelp)
            break;
          if (((NowIndex) > InPageItemCount + page * Height - 1 && Item != NULL) || (NowIndex < (page) * Height && NowIndex != -1)) {
            page = NowIndex / Height;
            break;
          }
          if (*Choice != 0)
            break;
        }
        system ("CLS");
        if (*Choice != 0)
          break;

      }
      ItemList->FreeItem (ItemList);
    }
    Interface->FreeInterface (Interface);
  }
  SetCursorVisible (true);
}

void ShowContinueClearDownloadFolderMenu (DATABASE* DataBase, int *Choice){

  INTERFACE* Interface  = DataBase->GetInterface (DataBase, "IsContinueClearDownloadFolder");
  if (Interface != NULL) {
    ITEM*      ItemList   = DataBase->GetItemList (DataBase, Interface->Sequence);
    if (ItemList!=NULL) {
      int   Top = 0;
      int   Left = 0;
      int   page = 0;
      int   AllHeight = 15;
      int   ItemCount = ItemList->GetItemCount (ItemList);
      int   InPageItemCount  = 0;
      int   NowIndex = 0;

      char* StartLine            = DataBase->GetVariable (DataBase, "Line_Start");

      SetCursorVisible (false);

      system ("CLS");

      *Choice = *Choice - 1;
      if (*Choice >= 0)
        if (*Choice >= ItemCount)
          NowIndex = ItemCount - 1;
        else
          NowIndex = *Choice;
      else
        NowIndex = ItemCount - 1;
      page = NowIndex / AllHeight;

      while (ItemList != NULL) {
        int Height = AllHeight;
        InPageItemCount = 0;

        Print ("\n");
        Print (" %=RW %76s| %=Nw\n", Interface->Title);
        Print (" %=rW %76s| %=Nw\n", StartLine);

        int MessageLiseCount = 0;
        char** MessageLise = Interface->GetMessageLise (Interface, &MessageLiseCount);
        for (int Index = 0; Index < MessageLiseCount; Index++, Height--) {
          Print (" %=rW %76s| %=Nw\n", MessageLise[Index]);
          Free (MessageLise[Index]);
        }
        Free (MessageLise);

        Print (" %=rW %76s| %=Nw\n", StartLine);
        Print ("\n");
        Print (" %=bW %76s| %=Nw\n", Interface->Name);
        Print (" %=Nw %76s| %=Nw\n", Interface->Explain);
        Print (" %=wN %76s| %=Nw\n", Interface->ItemNameTitle);
        GetCursorPosition (&Left, &Top);

        ITEM* Item = ItemList->GetStartItem (ItemList, NowIndex / Height * Height);
        for (int Index = 0; Index < Height; Index++) {
          if (Item != NULL) {
            Print ("   %3d. %20s -- %20s< %25s\n",(Index + 1) + (page * Height), Item->Name, Item->EnglishName, Item->Explain);
            InPageItemCount++;
            Item = Item->Next;
          }
          else {
            printf (" \n");
          }
        }

        Print (" %=cW %20s %=wN %62s %=Nw", Interface->Content,"%=wr(h)%=wN Help");

        while (true) {
          bool IsHelp = false;

          if (NowIndex == -1)
            NowIndex = 0;
          if (NowIndex >= InPageItemCount + page * Height)
            NowIndex = InPageItemCount + page * Height - 1;

          *Choice = InterfaceCursor ( 2, Top, Interface->Symbol, InPageItemCount, &NowIndex, Height, page, ItemCount, &IsHelp);

          if (IsHelp)
            break;

          if (((NowIndex) > InPageItemCount + page * Height - 1 && Item != NULL) || (NowIndex < (page) * Height && NowIndex != -1)) {
            page = NowIndex / Height;
            break;
          }
          if (*Choice != 0)
            break;
        }
        system ("CLS");
        if (*Choice != 0)
          break;

      }
      Free (StartLine);
      ItemList->FreeItem (ItemList);
    }
    Interface->FreeInterface (Interface);
  }
  SetCursorVisible (true);
}
void ClearDownloadFolder (DATABASE* DataBase) {
  Print ("%=NN<ClearPoint>%=Nw");
  char*  DownloadFolderPath  = DataBase->GetVariable (DataBase, "Folder_Download");

  char* DelCommand = MergeString ("DEL /Q %s", DownloadFolderPath);
  system(DelCommand);
  Free (DelCommand);

  char* RDCommand = MergeString ("RD /S /Q %s", DownloadFolderPath);
  system(RDCommand);
  Free (RDCommand);

  char* MDCommand = MergeString ("MD %s", DownloadFolderPath);
  system(MDCommand);
  Free (MDCommand);

  Free   (DownloadFolderPath);
  ClearScreenToKey("<ClearPoint>");
}
void Folder (DATABASE* DataBase) {
  int Choice = 1;

  while (Choice > 0) {
    Choice = 1;
    ShowManagementFolderMenu (DataBase, &Choice);

    if (Choice == 1) {
      char*  DownloadFolderPath  = DataBase->GetVariable (DataBase, "Folder_Download");
      char*  OpenFolderCommand = MergeString ("start %s", DownloadFolderPath);
      system (OpenFolderCommand);
      Free (OpenFolderCommand);
      Free   (DownloadFolderPath);
    }
    else if (Choice == 2) {
      Choice = 1;
      ShowContinueClearDownloadFolderMenu (DataBase, &Choice);
      if (Choice == 1) {
        ClearDownloadFolder (DataBase);
      }
      else if (Choice == -1) {
        Choice = 2;
      }
    }
  }
}

void ShowContinueResetAllMenu (DATABASE* DataBase, int *Choice){

  INTERFACE* Interface  = DataBase->GetInterface (DataBase, "IsContinueResetAll");
  if (Interface != NULL) {
    ITEM*      ItemList   = DataBase->GetItemList (DataBase, Interface->Sequence);
    if (ItemList!=NULL) {
      int   Top = 0;
      int   Left = 0;
      int   page = 0;
      int   AllHeight = 15;
      int   ItemCount = ItemList->GetItemCount (ItemList);
      int   InPageItemCount  = 0;
      int   NowIndex = 0;

      char* StartLine            = DataBase->GetVariable (DataBase, "Line_Start");

      SetCursorVisible (false);

      system ("CLS");

      *Choice = *Choice - 1;
      if (*Choice >= 0)
        if (*Choice >= ItemCount)
          NowIndex = ItemCount - 1;
        else
          NowIndex = *Choice;
      else
        NowIndex = ItemCount - 1;
      page = NowIndex / AllHeight;

      while (ItemList != NULL) {
        int Height = AllHeight;
        InPageItemCount = 0;

        Print ("\n");
        Print (" %=RW %76s| %=Nw\n", Interface->Title);
        Print (" %=rW %76s| %=Nw\n", StartLine);

        int MessageLiseCount = 0;
        char** MessageLise = Interface->GetMessageLise (Interface, &MessageLiseCount);
        for (int Index = 0; Index < MessageLiseCount; Index++, Height--) {
          Print (" %=rW %76s| %=Nw\n", MessageLise[Index]);
          Free (MessageLise[Index]);
        }
        Free (MessageLise);

        Print (" %=rW %76s| %=Nw\n", StartLine);
        Print ("\n");
        Print (" %=bW %76s| %=Nw\n", Interface->Name);
        Print (" %=Nw %76s| %=Nw\n", Interface->Explain);
        Print (" %=wN %76s| %=Nw\n", Interface->ItemNameTitle);
        GetCursorPosition (&Left, &Top);

        ITEM* Item = ItemList->GetStartItem (ItemList, NowIndex / Height * Height);
        for (int Index = 0; Index < Height; Index++) {
          if (Item != NULL) {
            Print ("   %3d. %20s -- %20s< %25s\n",(Index + 1) + (page * Height), Item->Name, Item->EnglishName, Item->Explain);
            InPageItemCount++;
            Item = Item->Next;
          }
          else {
            printf (" \n");
          }
        }

        Print (" %=cW %20s %=wN %62s %=Nw", Interface->Content,"%=wr(h)%=wN Help");

        while (true) {
          bool IsHelp = false;

          if (NowIndex == -1)
            NowIndex = 0;
          if (NowIndex >= InPageItemCount + page * Height)
            NowIndex = InPageItemCount + page * Height - 1;

          *Choice = InterfaceCursor ( 2, Top, Interface->Symbol, InPageItemCount, &NowIndex, Height, page, ItemCount, &IsHelp);
          if (IsHelp)
            break;
          if (((NowIndex) > InPageItemCount + page * Height - 1 && Item != NULL) || (NowIndex < (page) * Height && NowIndex != -1)) {
            page = NowIndex / Height;
            break;
          }
          if (*Choice != 0)
            break;
        }
        system ("CLS");
        if (*Choice != 0)
          break;

      }
      Free (StartLine);
      ItemList->FreeItem (ItemList);
    }
    Interface->FreeInterface (Interface);
  }
  SetCursorVisible (true);
}
void ResetAll (DATABASE* DataBase) {
  int Choice = 1;
  ShowContinueResetAllMenu (DataBase, &Choice);
  if (Choice == 1) {
    Print (" ==> 正在還原，請稍後...\n");
    ClearDownloadFolder (DataBase);
    SetCursorVisible (false);

    char* TempFile = DataBase->GetVariable (DataBase, "Folder_TempFile");
    ClearFile (TempFile);
    Free (TempFile);

    char* AllComicCodeFile = DataBase->GetVariable (DataBase, "File_AllComicCode");
    remove (AllComicCodeFile);
    Free (AllComicCodeFile);

    remove (DataBase->DataBasePath);

    DataBase->CheckDataBase(DataBase);
    Print (" ==> 還原完成!!\n");
    SetCursorVisible (true);
  }
}

void ReadVarFile (char* FilePath, char* Key, int ValueIndex, char** Value){
  (*Value) = NULL;
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

                            (*Value) = (char*) malloc (sizeof (char) * (ValueLength + 1));

                            for (int Index = StartPoint, ValueLength = 0; Index <= EndPoint; Index++)
                              (*Value)[ValueLength++] = Article[Index];
                            (*Value)[ValueLength++] = '\0';

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
}

void ShowContinueUpdateAppMenu (DATABASE* DataBase, int *Choice){

  INTERFACE* Interface  = DataBase->GetInterface (DataBase, "ContinueUpdateApp");
  if (Interface != NULL) {
    ITEM*      ItemList   = DataBase->GetItemList (DataBase, Interface->Sequence);
    if (ItemList!=NULL) {
      int   Top = 0;
      int   Left = 0;
      int   page = 0;
      int   AllHeight = 15;
      int   ItemCount = ItemList->GetItemCount (ItemList);
      int   InPageItemCount  = 0;
      int   NowIndex = 0;

      char* StartLine            = DataBase->GetVariable (DataBase, "Line_Start");

      SetCursorVisible (false);

      system ("CLS");

      *Choice = *Choice - 1;
      if (*Choice >= 0)
        if (*Choice >= ItemCount)
          NowIndex = ItemCount - 1;
        else
          NowIndex = *Choice;
      else
        NowIndex = ItemCount - 1;
      page = NowIndex / AllHeight;

      while (ItemList != NULL) {
        int Height = AllHeight;
        InPageItemCount = 0;

        Print ("\n");
        Print (" %=RW %76s| %=Nw\n", Interface->Title);
        Print (" %=rW %76s| %=Nw\n", StartLine);

        int MessageLiseCount = 0;
        char** MessageLise = Interface->GetMessageLise (Interface, &MessageLiseCount);
        for (int Index = 0; Index < MessageLiseCount; Index++, Height--) {
          Print (" %=rW %76s| %=Nw\n", MessageLise[Index]);
          Free (MessageLise[Index]);
        }
        Free (MessageLise);

        Print (" %=rW %76s| %=Nw\n", StartLine);
        Print ("\n");
        Print (" %=bW %76s| %=Nw\n", Interface->Name);
        Print (" %=Nw %76s| %=Nw\n", Interface->Explain);
        Print (" %=wN %76s| %=Nw\n", Interface->ItemNameTitle);
        GetCursorPosition (&Left, &Top);

        ITEM* Item = ItemList->GetStartItem (ItemList, NowIndex / Height * Height);
        for (int Index = 0; Index < Height; Index++) {
          if (Item != NULL) {
            Print ("   %3d. %20s -- %20s< %25s\n",(Index + 1) + (page * Height), Item->Name, Item->EnglishName, Item->Explain);
            InPageItemCount++;
            Item = Item->Next;
          }
          else {
            printf (" \n");
          }
        }

        Print (" %=cW %20s %=wN %62s %=Nw", Interface->Content,"%=wr(h)%=wN Help");

        while (true) {
          bool IsHelp = false;

          if (NowIndex == -1)
            NowIndex = 0;
          if (NowIndex >= InPageItemCount + page * Height)
            NowIndex = InPageItemCount + page * Height - 1;

          *Choice = InterfaceCursor ( 2, Top, Interface->Symbol, InPageItemCount, &NowIndex, Height, page, ItemCount, &IsHelp);
          if (IsHelp)
            break;
          if (((NowIndex) > InPageItemCount + page * Height - 1 && Item != NULL) || (NowIndex < (page) * Height && NowIndex != -1)) {
            page = NowIndex / Height;
            break;
          }
          if (*Choice != 0)
            break;
        }
        system ("CLS");
        if (*Choice != 0)
          break;

      }
      Free (StartLine);
      ItemList->FreeItem (ItemList);
    }
    Interface->FreeInterface (Interface);
  }
  SetCursorVisible (true);
}

void ShowUpdateAppSuccessMenu (DATABASE* DataBase, int *Choice){

  INTERFACE* Interface  = DataBase->GetInterface (DataBase, "UpdateAppSuccess");
  if (Interface != NULL) {
    ITEM*      ItemList   = DataBase->GetItemList (DataBase, Interface->Sequence);
    if (ItemList!=NULL) {
      int   Top = 0;
      int   Left = 0;
      int   page = 0;
      int   AllHeight = 15;
      int   ItemCount = ItemList->GetItemCount (ItemList);
      int   InPageItemCount  = 0;
      int   NowIndex = 0;

      char* StartLine            = DataBase->GetVariable (DataBase, "Line_Start");

      SetCursorVisible (false);

      system ("CLS");

      *Choice = *Choice - 1;
      if (*Choice >= 0)
        if (*Choice >= ItemCount)
          NowIndex = ItemCount - 1;
        else
          NowIndex = *Choice;
      else
        NowIndex = ItemCount - 1;
      page = NowIndex / AllHeight;

      while (ItemList != NULL) {
        int Height = AllHeight;
        InPageItemCount = 0;

        Print ("\n");
        Print (" %=RW %76s| %=Nw\n", Interface->Title);
        Print (" %=rW %76s| %=Nw\n", StartLine);

        int MessageLiseCount = 0;
        char** MessageLise = Interface->GetMessageLise (Interface, &MessageLiseCount);
        for (int Index = 0; Index < MessageLiseCount; Index++, Height--) {
          Print (" %=rW %76s| %=Nw\n", MessageLise[Index]);
          Free (MessageLise[Index]);
        }
        Free (MessageLise);

        Print (" %=rW %76s| %=Nw\n", StartLine);
        Print ("\n");
        Print (" %=bW %76s| %=Nw\n", Interface->Name);
        Print (" %=Nw %76s| %=Nw\n", Interface->Explain);
        Print (" %=wN %76s| %=Nw\n", Interface->ItemNameTitle);
        GetCursorPosition (&Left, &Top);

        ITEM* Item = ItemList->GetStartItem (ItemList, NowIndex / Height * Height);
        for (int Index = 0; Index < Height; Index++) {
          if (Item != NULL) {
            Print ("   %3d. %20s -- %25s< %20s\n",(Index + 1) + (page * Height), Item->Name, Item->EnglishName, Item->Explain);
            InPageItemCount++;
            Item = Item->Next;
          }
          else {
            printf (" \n");
          }
        }

        Print (" %=cW %20s %=wN %62s %=Nw", Interface->Content,"%=wr(h)%=wN Help");

        while (true) {
          bool IsHelp = false;

          if (NowIndex == -1)
            NowIndex = 0;
          if (NowIndex >= InPageItemCount + page * Height)
            NowIndex = InPageItemCount + page * Height - 1;

          *Choice = InterfaceCursor ( 2, Top, Interface->Symbol, InPageItemCount, &NowIndex, Height, page, ItemCount, &IsHelp);
          if (IsHelp)
            break;
          if (((NowIndex) > InPageItemCount + page * Height - 1 && Item != NULL) || (NowIndex < (page) * Height && NowIndex != -1)) {
            page = NowIndex / Height;
            break;
          }
          if (*Choice != 0)
            break;
        }
        system ("CLS");
        if (*Choice != 0)
          break;

      }
      Free (StartLine);
      ItemList->FreeItem (ItemList);
    }
    Interface->FreeInterface (Interface);
  }
  SetCursorVisible (true);
}
void ShowIsNewVersion (DATABASE* DataBase, int *Choice){

  INTERFACE* Interface  = DataBase->GetInterface (DataBase, "IsNewVersion");
  if (Interface != NULL) {
    ITEM*      ItemList   = DataBase->GetItemList (DataBase, Interface->Sequence);
    if (ItemList!=NULL) {
      int   Top = 0;
      int   Left = 0;
      int   page = 0;
      int   AllHeight = 15;
      int   ItemCount = ItemList->GetItemCount (ItemList);
      int   InPageItemCount  = 0;
      int   NowIndex = 0;

      char* StartLine            = DataBase->GetVariable (DataBase, "Line_Start");

      SetCursorVisible (false);

      system ("CLS");

      *Choice = *Choice - 1;
      if (*Choice >= 0)
        if (*Choice >= ItemCount)
          NowIndex = ItemCount - 1;
        else
          NowIndex = *Choice;
      else
        NowIndex = ItemCount - 1;
      page = NowIndex / AllHeight;

      while (ItemList != NULL) {
        int Height = AllHeight;
        InPageItemCount = 0;

        Print ("\n");
        Print (" %=RW %76s| %=Nw\n", Interface->Title);
        Print (" %=rW %76s| %=Nw\n", StartLine);

        int MessageLiseCount = 0;
        char** MessageLise = Interface->GetMessageLise (Interface, &MessageLiseCount);
        for (int Index = 0; Index < MessageLiseCount; Index++, Height--) {
          Print (" %=rW %76s| %=Nw\n", MessageLise[Index]);
          Free (MessageLise[Index]);
        }
        Free (MessageLise);

        Print (" %=rW %76s| %=Nw\n", StartLine);
        Print ("\n");
        Print (" %=bW %76s| %=Nw\n", Interface->Name);
        Print (" %=Nw %76s| %=Nw\n", Interface->Explain);
        Print (" %=wN %76s| %=Nw\n", Interface->ItemNameTitle);
        GetCursorPosition (&Left, &Top);

        ITEM* Item = ItemList->GetStartItem (ItemList, NowIndex / Height * Height);
        for (int Index = 0; Index < Height; Index++) {
          if (Item != NULL) {
            Print ("   %3d. %20s -- %25s< %20s\n",(Index + 1) + (page * Height), Item->Name, Item->EnglishName, Item->Explain);
            InPageItemCount++;
            Item = Item->Next;
          }
          else {
            printf (" \n");
          }
        }

        Print (" %=cW %20s %=wN %62s %=Nw", Interface->Content,"%=wr(h)%=wN Help");

        while (true) {
          bool IsHelp = false;

          if (NowIndex == -1)
            NowIndex = 0;
          if (NowIndex >= InPageItemCount + page * Height)
            NowIndex = InPageItemCount + page * Height - 1;

          *Choice = InterfaceCursor ( 2, Top, Interface->Symbol, InPageItemCount, &NowIndex, Height, page, ItemCount, &IsHelp);
          if (IsHelp)
            break;
          if (((NowIndex) > InPageItemCount + page * Height - 1 && Item != NULL) || (NowIndex < (page) * Height && NowIndex != -1)) {
            page = NowIndex / Height;
            break;
          }
          if (*Choice != 0)
            break;
        }
        system ("CLS");
        if (*Choice != 0)
          break;

      }
      Free (StartLine);
      ItemList->FreeItem (ItemList);
    }
    Interface->FreeInterface (Interface);
  }
  SetCursorVisible (true);
}
void UpdateApp (DATABASE* DataBase) {
  char* WgetFile        = DataBase->GetVariable (DataBase, "File_Wget");
  char* Announcement    = DataBase->GetVariable (DataBase, "Url_Announcement");
  char* TempFile        = DataBase->GetVariable (DataBase, "Folder_TempFile");

  Print (" ==> 正在檢查更新，請稍後..\n");
  RunCommand (WgetFile, Announcement, TempFile, true);
  Free (Announcement);

  if (GetFileSize(TempFile) > 0) {
    char* MyVersion = DataBase->GetVariable (DataBase, "Version");
    char* NewVersion = NULL;
    ReadVarFile (TempFile, "ComicBook_New_Version",0,&NewVersion);

    if (strcmp (MyVersion, NewVersion) != 0) {
      int Choice = 1;
      ShowContinueUpdateAppMenu (DataBase, &Choice);
      if (Choice == 1) {

        Print (" ==> 正在下載新版，請稍後..\n");
        char* DownloadUrl = NULL;
        char* FileName = NULL;
        ReadVarFile (TempFile, "ComicBook_New_Version_Download_Url",0,&DownloadUrl);
        ReadVarFile (TempFile, "ComicBook_New_Download_App_Name",0,&FileName);
        ClearFile (TempFile);
        RunCommand (WgetFile, DownloadUrl, FileName, true);
        system ("123");
        Free (DownloadUrl);
        Print (" ==> 正在下載新版完成!\n");

        Choice = 1;
        ShowUpdateAppSuccessMenu (DataBase, &Choice);
        if (Choice == 1) {
          system (FileName);
        }
        Free (FileName);
      }
    }
    else {
      int Choice = 1;
      ShowIsNewVersion (DataBase, &Choice);
    }
    Free (NewVersion);
    Free (MyVersion);
  }
  else {
    int Choice = 1;
    ShowIsNewVersion (DataBase, &Choice);
  }

  Free (TempFile);
  Free (WgetFile);
}

void ExplainApp (DATABASE* DataBase) {

  char* Version = DataBase->GetVariable (DataBase, "Version");
  char* MyVersion = MergeString ("程式版本 : %s", Version);

  Print ("\n");
  Print (" %=pW %76s| %=Nw\n"," ");
  Print (" %=pW %76s| %=Nw\n","[OA Comic Book]");
  Print (" %=pW %76s| %=Nw\n"," ");

  Print ("\n");

  Print ("  %s\n\n","程式名稱 : OA Comic Book");
  Print ("  %s\n\n",MyVersion);
  Print ("  %s\n\n","引用資源 : 本程式引用線上漫畫網站 http://www.8comic.com，還是請支持原創!");

  Print ("  %s\n"  ,"友情提示 : 由於本程式還在開發中，並非最完美階段，故請勿任意改變資料夾結構，");
  Print ("  %s\n\n","           以避免造成一些奇怪的錯誤!");
  Print ("  %s\n"  ,"免責聲明 : 本程式僅提供學術研究喔，你要拿去做啥用途，皆與原創者無關!");
  Print ("  %s\n"  ,"           此程式為個人創作的作品，你如果要拿去改成你的，儘管拿去吧!");
  Print ("  %s\n\n","           出問題別來問我!!");
  Print ("  %s\n","使用流程簡圖 :");
  Print ("  %s\n\n","    解壓縮 >> 執行 OA_ComicBook.exe >> 1. 更新漫畫檔 >> 2. 瀏覽漫畫");

  Print ("  %s\n\n","發布日期 : 2012/11/12 01:25");
  Print (" %=BW %76s %=Nw","請按任意鍵回到 主要選單。");

  SetCursorVisible (false);
  getch ();
  SetCursorVisible (true);


  Free (MyVersion);
  Free (Version);
}
int main (int argc, char* argv[]) {
  SetBufferWindowSameSize (WINDOW_SIZE_WIDTH,WINDOW_SIZE_HEIGHT);

  char DataBasePath[] = ".\\DataBase\\DataBase.db3";

  DATABASE* DataBase = NULL;
  LocateDataBaseStruct (&DataBase, DataBasePath);

  if (DataBase != NULL) {
    int Choice = 1;

    while (Choice != 8) {

      ShowMainMain (DataBase,&Choice);
      switch (Choice) {
        case 1:
          Update (DataBase);
          break;

        case 2:
          OpenBrowser (DataBase);
          break;

        case 3:
          Download (DataBase);
          break;

        case 4:
          ResetAll (DataBase);
          break;

        case 5:
          Folder (DataBase);
          break;

        case 6:
          UpdateApp (DataBase);
          break;

        case 7:
          ExplainApp (DataBase);
          break;
      }
    }
  }


/**/
  DataBase->FreeDataBase (DataBase);
  return 0;
}


#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>

#define   VAR_START_SYMBOL     0x0000005B
#define   VAR_END_SYMBOL       0x0000005D
#define   VAR_VALUE_SYMBOL     0x0000007C





bool CheckIsAllow (char   cmpchar, char*   AllowCharList) {
  for (int Index = 0; Index < strlen (AllowCharList) ; Index++)
    if(AllowCharList[Index] == cmpchar)
      return true;
  return false;
}
void GetInput (int MaxLength, char* AllowCharList, char** InputData) {
  int Key = 0;

  (*InputData) = (char *) malloc (sizeof (char) * (MaxLength + 1));

  for (int StringCount = 0; StringCount < MaxLength + 1 ; StringCount++) {
    Key = getch ();
    if (CheckIsAllow (Key, AllowCharList) && (StringCount < MaxLength)) {
      printf ("%c", Key);
      (*InputData)[StringCount] = Key;
    }
    else if (Key == 8) {
      if (StringCount > 0) {
        printf ("\b \b");
        StringCount = StringCount - 2;
      }
      else {
        StringCount--;
      }
    }
    else if (Key == 13) {
      if (StringCount == 0) {
        StringCount--;
      }
      else {
        (*InputData)[StringCount] = '\0';
        break;
      }
    }
    else {
      StringCount--;
    }
  }
}
//--------------------------------------------------------------
/*width -> many Unicode - ANSI*/
char* w2m(const wchar_t* wcs)
{
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
wchar_t* m2w(const char* mbs)
{
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
wchar_t * ANSIToUnicode( const char* str )
{
     int textlen ;
     wchar_t * result;
     textlen = MultiByteToWideChar( CP_ACP, 0, str,-1, NULL,0 );
     result = (wchar_t *)malloc((textlen+1)*sizeof(wchar_t));
     memset(result,0,(textlen+1)*sizeof(wchar_t));
     MultiByteToWideChar(CP_ACP, 0,str,-1,(LPWSTR)result,textlen );
     return result;
}

char * UnicodeToANSI( const wchar_t* str )
{
     char* result;
     int textlen;
     textlen = WideCharToMultiByte( CP_ACP, 0, str, -1, NULL, 0, NULL, NULL );
     result =(char *)malloc((textlen+1)*sizeof(char));
     memset( result, 0, sizeof(char) * ( textlen + 1 ) );
     WideCharToMultiByte( CP_ACP, 0, str, -1, result, textlen, NULL, NULL );
     return result;
}

wchar_t * UTF8ToUnicode( const char* str )
{
     int textlen ;
     wchar_t * result;
     textlen = MultiByteToWideChar( CP_UTF8, 0, str,-1, NULL,0 );
     result = (wchar_t *)malloc((textlen+1)*sizeof(wchar_t));
     memset(result,0,(textlen+1)*sizeof(wchar_t));
     MultiByteToWideChar(CP_UTF8, 0,str,-1,(LPWSTR)result,textlen );
     return result;
}

char * UnicodeToUTF8( const wchar_t* str )
{
     char* result;
     int textlen;
     textlen = WideCharToMultiByte( CP_UTF8, 0, str, -1, NULL, 0, NULL, NULL );
     result =(char *)malloc((textlen+1)*sizeof(char));
     memset(result, 0, sizeof(char) * ( textlen + 1 ) );
     WideCharToMultiByte( CP_UTF8, 0, str, -1, result, textlen, NULL, NULL );
     return result;
}
char* ANSIToUTF8(const char* str)
{
     return UnicodeToUTF8(ANSIToUnicode(str));
}

char* UTF8ToANSI(const char* str)
{
     return UnicodeToANSI(UTF8ToUnicode(str));
}
//--------------------------------------------------------------
int GetIntLength (int Num) {
  int Index = 1;
  for (Index = 1; (Num = Num/10)>0 ; Index++);
  return Index;
}
void MergeString (char** OutString, char* Format, ...) {
  (*OutString) = NULL;
  if (Format != NULL) {
    int      Length = 0;
    char*    FormatPoint = Format;
    va_list  args;
    char*    TempString;

    va_start (args, *Format);
    for (;*FormatPoint!='\0';*FormatPoint++)
      if (*FormatPoint =='%') {
        switch (*++FormatPoint){
          case 's':
            TempString = va_arg(args, char *);
            if (TempString != NULL)
              Length = Length + strlen (TempString);
            break;
          case 'd':
            Length = Length + GetIntLength (va_arg(args, int));
            break;
        }
      }
      else {
        Length++;
      }
    va_end(args);


    (*OutString) = (char*) malloc (sizeof (char) * (Length + 1));
    strcpy(*OutString, "");

    va_start(args, *Format);
    for (int NowLength = 0; *Format!='\0'; *Format++)
      if (*Format =='%') {
        switch (*++Format){
          case 's':
            TempString = va_arg(args, char *);
            if (TempString != NULL)
              sprintf (*OutString, "%s%s", *OutString, TempString);
            break;
          case 'd':
            sprintf (*OutString, "%s%d", *OutString, va_arg(args, int));
            break;
        }
      }
      else {
        NowLength = strlen (*OutString);
        (*OutString)[NowLength++] = *Format;
        (*OutString)[NowLength++] = '\0';
      }

    va_end(args);
  }
}

int GetFileSize (char* FileName) {
  FILE *FilePoint;
  int Size;
  if ((FilePoint = fopen (FileName, "r")) == NULL) {
    Size = 0;
  }
  else {
    fseek(FilePoint, 0, SEEK_END);
    Size = ftell(FilePoint);
    fseek(FilePoint, 0, SEEK_SET);
    fclose(FilePoint);
  }
  return Size;
}
void ReadVarFileLine (char* FilePath, char* Key, char**** Array){
  (*Array) = NULL;

  int    FileSize = 0;
  FileSize = GetFileSize (FilePath);

  if (FileSize > 0) {
    FILE   *FilePoint;
    if ((FilePoint = fopen (FilePath, "r")) != NULL) {
      char* Article = NULL;
      Article = (char*) malloc(sizeof (char) * (FileSize + 1));
      int ArrayLength = 0;
      int Index = 0;
      for (Index = 0; (Article[Index]=getc(FilePoint)) != EOF; Index++){
        if (Article[Index] == VAR_START_SYMBOL)
          ArrayLength++;
      }
      Article[Index] = '\0';
      fclose (FilePoint);

      (*Array) = (char***)malloc (sizeof (char**) * ArrayLength);

      for (Index = 0,ArrayLength = 0; Index < strlen (Article); Index++) {
        if (Article[Index] == VAR_START_SYMBOL){
          (*Array)[ArrayLength++] = (char**)malloc (sizeof (char*) * 5);
        }
      }
      free (Article);
    }
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

void WriteVarFile (char* FilePath, char* Key, int ValueIndex, char* Value){

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

        int StartPoint = 0;
        int EndPoint   = 0;

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


                    int ValueCount = 0;
                    for (int TempIndex = EndSymbolPoint; TempIndex <= StartSymbolPoint; TempIndex++) {
                      StartPoint = TempIndex;

                      for (TempIndex++; TempIndex <= StartSymbolPoint; TempIndex++) {
                        if (Article[TempIndex] == VAR_VALUE_SYMBOL) {

                          if ((ValueCount++) == ValueIndex) {
                            EndPoint = TempIndex;


                            for (StartPoint++; Article[StartPoint] == ' '; StartPoint++);
                            for (EndPoint--; Article[EndPoint] == ' '; EndPoint--);


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
        int Length = EndPoint - StartPoint + 1;

        if (Length >= strlen (Value)) {
          int ValueCount = 0;
          for (int Index = StartPoint; Index <= EndPoint; Index++)
            if (ValueCount < strlen (Value))
              Article[Index] = Value[ValueCount++];
            else
              Article[Index] = ' ';

          if ((FilePoint = fopen (FilePath, "w")) != NULL) {
            for (int Index = 0; Article[Index] != '\0'; Index++) {
              putc (Article[Index], FilePoint);
            }
            fclose(FilePoint);
          }
        }
        else {
          int   AddLength = strlen (Value) - Length;
          int   NewArticleLength = strlen (Article) + AddLength;
          char* NewArticle = NULL;
          int   NewArticleCount = 0;

          NewArticle = (char*) malloc (sizeof (char) * (NewArticleLength + 1));

          for (int Index = 0; Index < StartPoint && Index < strlen (Article); Index++)
            NewArticle[NewArticleCount++] = Article[Index];
          for (int Index = 0; Index < strlen (Value); Index++)
            NewArticle [NewArticleCount++] = Value[Index];
          for (int Index = EndPoint + 1; Index < strlen (Article); Index++)
            NewArticle [NewArticleCount++] = Article[Index];

          NewArticle [NewArticleCount++] = '\0';

          if ((FilePoint = fopen (FilePath, "w")) != NULL) {
            for (int Index = 0; Article[Index] != '\0'; Index++) {
              putc (Article[Index], FilePoint);
            }
            fclose(FilePoint);
          }
          free (NewArticle);
        }
        free (Article);
      }
    }
  }
}

void Print (char* StringFormat, ...){
   char *ptr=StringFormat;
   va_list args;

   va_start(args, StringFormat);

   for (;*ptr!='\0';*ptr++) {
     switch (*ptr){
       case '\\':
         switch (*++ptr){
           case 'n':
             Print ("\n");
             break;
         }
         break;
       case '%':
         switch (*++ptr){
           case 's':
             Print (va_arg(args, char *));
             break;
           case 'd':
             printf ("%d", va_arg(args, int));
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

             if (*(ptr + 1) == ' ')
                *++ptr;
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
void ClearFile (char* FileName){
  FILE* FilePoint = NULL;
  if ((FilePoint = fopen (FileName, "w")) != NULL) {
    fprintf (FilePoint, "");
    fclose (FilePoint);
  }
}
void RunCommand (char* WgetFilePath, char* Url, char* FileName){

  char* Command = NULL;
  MergeString (&Command, "%s %s -O %s", WgetFilePath, Url, FileName);

  ClearFile(FileName);
  //printf ("->%s<-\n",CommandFormat);
  system(Command);

  free (Command);
}
//--------------------------------------------------------------------

void SetCursorPosition (int x, int y)
{
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
//-------------------------------------
void InterfaceCursor (int Left, int Top, char *Key, int ItemCount, int *Choice)
{
  int X = Left;
  int Y = Top + *Choice - 1;
  bool IsExit = false;

  SetCursorVisible (false);
  while (!IsExit){
    if (Y < Top)
      Y = ItemCount + Top - 1;
    if (Y > ItemCount + Top - 1)
      Y = Top;

    SetCursorPosition (X,Y);
    printf ("%s\b",Key);

    if (getch () == 224) {
      switch (getch ()) {
        case 72:
          Y--;
          break;
        case 77:
          *Choice = Y - Top + 1;
          IsExit = true;
          break;
        case 80:
          Y++;
          break;
        case 75:
          *Choice = 0;
          IsExit = true;
          break;
      }
    }
    printf (" \b");
  }
  SetCursorVisible (true);
}
//--------------------------------------------------------------------

typedef struct   _PATH_            PATH;
typedef struct   _ITEM_            ITEM;
typedef struct   _INTERFACE_       INTERFACE;
typedef struct   _CHAPTER_         CHAPTER;
typedef struct   _COMIC_           COMIC;
typedef struct   _NOTE_            NOTE;
typedef struct   _ALL_SET_         ALL_SET;

typedef char* (*GET_PATH_FUNC_MODULE) (ALL_SET* This, char* Key);
typedef void  (*GET_INTERFACE_FUNC_MODULE) (ALL_SET* This, char* Key, INTERFACE** Interface);
typedef void  (*GET_COMIC_FUNC_MODULE) (ALL_SET* This, char* Sequence, COMIC** Comic);
typedef void  (*GET_CHAPTER_FUNC_MODULE) (COMIC* Comic, char* Index, CHAPTER** Chapter);
typedef void  (*FREE_ALL_PATH_FUNC_MODULE) (ALL_SET* This);
typedef void  (*FREE_ALL_INTERFACE_FUNC_MODULE) (ALL_SET* This);
typedef void  (*FREE_ALL_COMIC_FUNC_MODULE) (ALL_SET* This);

typedef char* (*GET_NOTE_FUNC_MODULE) (ALL_SET* This,char* Key);
typedef void (*FREE_ALL_NOTE_FUNC_MODULE) (ALL_SET* This);




typedef struct _PATH_ {
  char*      Name;
  char*      Path;
  _PATH_*    Next;
} PATH;

typedef struct _NOTE_ {
  char*      Key;
  char*      Value;
  _NOTE_*    Next;
} NOTE;

typedef struct _ITEM_ {
  char*             Name;
  char*             Explain;
  bool              IsEnable;
  _ITEM_*            Next;
} ITEM;

typedef struct _INTERFACE_ {
  char*             Name;
  char*             Title;
  char*             Content;
  char*             Explain;
  char*             ChoiceSymbol;
  ITEM*             Item;
  _INTERFACE_*      Next;
} INTERFACE;


typedef struct _CHAPTER_ {
  char*             Index;
  char*             Sid;
  char*             Did;
  char*             PicRange;
  char*             Code;
  _CHAPTER_*        Next;
} CHAPTER;


typedef struct _COMIC_ {
  char*                        Sequence;
  char*                        Name;
  CHAPTER*                     Chapter;
  GET_CHAPTER_FUNC_MODULE      GetChapter;

  _COMIC_*                     Next;


} COMIC;

typedef struct _ALL_SET_ {
  char*                                 VarFilePath;

  PATH*                                 Path;
  GET_PATH_FUNC_MODULE                  GetPath;
  FREE_ALL_PATH_FUNC_MODULE             FreeAllPath;

  INTERFACE*                            Interface;
  GET_INTERFACE_FUNC_MODULE             GetInterface;
  FREE_ALL_INTERFACE_FUNC_MODULE        FreeAllInterface;

  COMIC*                                Comic;
  GET_COMIC_FUNC_MODULE                 GetComic;
  FREE_ALL_COMIC_FUNC_MODULE            FreeAllComic;

  NOTE*                                 Note;
  GET_NOTE_FUNC_MODULE                  GetNote;
  FREE_ALL_NOTE_FUNC_MODULE             FreeAllNote;

} ALL_SET;
//--------------------------------------------------------------------------
void ShowMainMain (ALL_SET* AllSet, int *Choice) {
  INTERFACE* Interface = NULL;
  AllSet->GetInterface (AllSet, "MainMenu", &Interface);

  if (Interface != NULL) {
    int  Top = 0;
    int  Left = 0;

    system ("CLS");
    Print (" %=NY %s %=Nw\n", Interface->Title);
    Print ("%s\n", AllSet->GetNote(AllSet,"DoubleLine"));
    GetCursorPosition (&Left, &Top);

    int Index = 0;
    for (ITEM* Item = Interface->Item; Item != NULL; Item = Item->Next, Index++)
      Print ("    %=NW %d%=Nw . %s\n", (Index + 1), Item->Name);

    Print ("%=NW %s %=Nw\n", AllSet->GetNote(AllSet,"Line"));
    Print (" %=NR %s %=Nw\n", Interface->Explain);
    Print ("%s\n", AllSet->GetNote(AllSet,"DoubleLine"));

    InterfaceCursor ( 2, Top, Interface->ChoiceSymbol, Index, Choice);
    system ("CLS");
  }
}
//--------------------------------------------------------------------
void GetChapter (COMIC* Comic, char* Index, CHAPTER** Chapter) {
  if (Comic == NULL) {
    (*Chapter) = NULL;
  }
  else {
    CHAPTER* ChapterPtr = NULL;
    for (ChapterPtr = Comic->Chapter; (ChapterPtr != NULL) && (strcmp(ChapterPtr->Index,Index) != 0) ; ChapterPtr = ChapterPtr->Next);
    (*Chapter) = ChapterPtr;
  }
}
void GetComic (ALL_SET* This, char* Sequence, COMIC** Comic) {
  if (This == NULL) {
    (*Comic) = NULL;
  }
  else {
    COMIC* ComicPtr = NULL;
    for (ComicPtr = This->Comic; (ComicPtr != NULL) && (strcmp(ComicPtr->Sequence,Sequence) != 0) ; ComicPtr = ComicPtr->Next);
    (*Comic) = ComicPtr;
  }
}
void GetInterface (ALL_SET* This, char* Name, INTERFACE** Interface) {
  if (This == NULL) {
    (*Interface) = NULL;
  }
  else {
    INTERFACE* InterfacePtr = NULL;

    for (InterfacePtr = This->Interface; (InterfacePtr != NULL) && (strcmp(InterfacePtr->Name,Name) != 0) ; InterfacePtr = InterfacePtr->Next);

    (*Interface) = InterfacePtr;
  }
}
char* GetPath (ALL_SET* This, char* Key) {
  if (This == NULL) {
    return NULL;
  }
  else {
    PATH* PathPtr = NULL;
    for (PathPtr = This->Path; (PathPtr != NULL) && (strcmp(PathPtr->Name,Key) != 0) ; PathPtr = PathPtr->Next);

    if (PathPtr == NULL)
      return NULL;
    else
      return PathPtr->Path;
  }
}
char* GetNote (ALL_SET* This, char* Key) {
  if (This == NULL) {
    return NULL;
  }
  else {
    NOTE* NotePtr = NULL;
    for (NotePtr = This->Note; (NotePtr != NULL) && (strcmp(NotePtr->Key,Key) != 0) ; NotePtr = NotePtr->Next);

    if (NotePtr == NULL)
      return NULL;
    else
      return NotePtr->Value;
  }
}


void AddInterface (ALL_SET *AllSet , char* InterfaceName, int LanguageIndex) {
  INTERFACE* Interface = NULL;
  Interface = (INTERFACE*) malloc (sizeof (INTERFACE) * 1);

  char* Key = NULL;
  MergeString (&Interface->Name, InterfaceName);

  MergeString (&Key, "%s_Title", InterfaceName);
  ReadVarFile ((AllSet)->VarFilePath, Key, LanguageIndex, &Interface->Title);
  free (Key);

  MergeString (&Key, "%s_Content", InterfaceName);
  ReadVarFile ((AllSet)->VarFilePath, Key, LanguageIndex, &Interface->Content);
  free (Key);

  MergeString (&Key, "%s_Explain", InterfaceName);
  ReadVarFile ((AllSet)->VarFilePath, Key, LanguageIndex, &Interface->Explain);
  free (Key);

  MergeString (&Key, "%s_ChoiceSymbol", InterfaceName);
  ReadVarFile ((AllSet)->VarFilePath, Key, LanguageIndex, &Interface->ChoiceSymbol);
  free (Key);

  Interface->Item = NULL;
  for (int Index = 0; ;Index++) {
    MergeString (&Key, "%s_ItemName_%d", InterfaceName, Index);

    char*   ItemName = NULL;
    ReadVarFile ((AllSet)->VarFilePath, Key, LanguageIndex, &ItemName);
    free (Key);

    if (ItemName == NULL) {
      break;
    }
    else {
      ITEM* Item = NULL;
      Item = (ITEM*) malloc (sizeof (ITEM) * 1);
      Item->Name = ItemName;

      MergeString (&Key, "%s_ItemExplain_%d", InterfaceName, Index);
      ReadVarFile ((AllSet)->VarFilePath, Key, LanguageIndex, &Item->Explain);

      free (Key);

      Item->IsEnable = true;

      Item->Next = NULL;
      if (Interface->Item == NULL) {
        Interface->Item = Item;
      }
      else {
        ITEM* ItemPoint = NULL;
        for (ItemPoint = Interface->Item; ItemPoint->Next != NULL; ItemPoint = ItemPoint->Next) ;
        ItemPoint->Next = Item;
      }
    }
  }

  if ((AllSet)->Interface != NULL)
    Interface->Next = (AllSet)->Interface;
  else
    Interface->Next = NULL;

  (AllSet)->Interface = Interface;

  free (Key);
}

void AddPath (ALL_SET* AllSet, char* Key){

  PATH* Path = NULL;
  Path = (PATH*) malloc (sizeof (PATH) * 1);
  MergeString (&Path->Name, Key);
  ReadVarFile ((AllSet)->VarFilePath, Key, 0, &Path->Path);

  if ((AllSet)->Path != NULL)
    Path->Next = (AllSet)->Path;
  else
    Path->Next = NULL;

  (AllSet)->Path = Path;
}
void FreeAllComic (ALL_SET* AllSet) {
  for (AllSet->Comic; AllSet->Comic != NULL; AllSet->Comic = AllSet->Comic->Next){
    free (AllSet->Comic->Sequence);
    free (AllSet->Comic->Name);

    for (AllSet->Comic->Chapter; AllSet->Comic->Chapter != NULL; AllSet->Comic->Chapter = AllSet->Comic->Chapter->Next){
      free (AllSet->Comic->Chapter->Index);
      free (AllSet->Comic->Chapter->Sid);
      free (AllSet->Comic->Chapter->Did);
      free (AllSet->Comic->Chapter->PicRange);
      free (AllSet->Comic->Chapter->Code);
      free (AllSet->Comic->Chapter);
    }
     free (AllSet->Comic);
  }
}
void FreeAllInterface (ALL_SET* AllSet) {
  for (AllSet->Interface; AllSet->Interface != NULL; AllSet->Interface = AllSet->Interface->Next){
    free (AllSet->Interface->Name);
    free (AllSet->Interface->Title);
    free (AllSet->Interface->Content);
    free (AllSet->Interface->Explain);
    free (AllSet->Interface->ChoiceSymbol);

    for (AllSet->Interface->Item; AllSet->Interface->Item != NULL; AllSet->Interface->Item = AllSet->Interface->Item->Next){
      free (AllSet->Interface->Item->Name);
      free (AllSet->Interface->Item->Explain);
      free (AllSet->Interface->Item);
    }
    free (AllSet->Interface);
  }
}
void FreeAllPath (ALL_SET* AllSet) {
  for (AllSet->Path; AllSet->Path != NULL; AllSet->Path = AllSet->Path->Next){
    free (AllSet->Path->Name);
    free (AllSet->Path->Path);
    free (AllSet->Path);
  }
}
void FreeAllNote (ALL_SET* AllSet) {
  for (AllSet->Note; AllSet->Note != NULL; AllSet->Note = AllSet->Note->Next){
    free (AllSet->Note->Key);
    free (AllSet->Note->Value);
    free (AllSet->Note);
  }
}

void AddNote (ALL_SET* AllSet, char* Key){

  NOTE* Note = NULL;
  Note = (NOTE*) malloc (sizeof (NOTE) * 1);
  MergeString (&Note->Key, Key);
  ReadVarFile ((AllSet)->VarFilePath, Key, 0, &Note->Value);

  if ((AllSet)->Note != NULL)
    Note->Next = (AllSet)->Note;
  else
    Note->Next = NULL;

  (AllSet)->Note = Note;
}

void LocateSetStruct (ALL_SET** AllSet, char* VarFilePath) {

  if (GetFileSize (VarFilePath) > 0) {
    //0
    (*AllSet) = (ALL_SET*) malloc (sizeof (ALL_SET) * 1);

    //1
    MergeString (&(*AllSet)->VarFilePath, VarFilePath);

    //2
    (*AllSet)->Path = NULL;
    AddPath (*AllSet ,"File_Wget");
    AddPath (*AllSet ,"File_Curl");
    AddPath (*AllSet ,"File_AllComicCode");
    AddPath (*AllSet ,"File_ComicView");
    AddPath (*AllSet ,"File_JsTransform");
    AddPath (*AllSet ,"File_TestAccessIndex");
    AddPath (*AllSet ,"File_BrowserIndex");
    AddPath (*AllSet ,"Folder_Download");
    AddPath (*AllSet ,"File_Codes");
    AddPath (*AllSet ,"File_Html");
    AddPath (*AllSet ,"File_Show");
    AddPath (*AllSet ,"Url_Base");
    AddPath (*AllSet ,"Url_ComicView");
    AddPath (*AllSet ,"Url_Html");
    (*AllSet)->GetPath = GetPath;
    (*AllSet)->FreeAllPath = FreeAllPath;

    //3
    char* Language = NULL;
    ReadVarFile (VarFilePath, "LanguageIndex", 0, &Language);
    int LanguageIndex = atoi (Language);
    free (Language);

    (*AllSet)->Interface = NULL;
    AddInterface (*AllSet , "MainMenu", LanguageIndex);
    AddInterface (*AllSet , "ContinueUpDate", LanguageIndex);
    AddInterface (*AllSet , "ComicList", LanguageIndex);
    AddInterface (*AllSet , "ChapterList", LanguageIndex);
    (*AllSet)->GetInterface = GetInterface;
    (*AllSet)->FreeAllInterface = FreeAllInterface;


    //4
    (*AllSet)->Comic = NULL;

    for (int Index = 0; ; Index++) {
      char* Key = NULL;
      MergeString (&Key, "Comic_%d", Index);
      char*   Sequence = NULL;
      ReadVarFile ((*AllSet)->VarFilePath, Key, 0, &Sequence);
      free (Key);

      if (Sequence != NULL) {
        COMIC*  Comic = NULL;
        Comic = (COMIC*) malloc (sizeof (COMIC) * 1);
        Comic->Sequence = Sequence;

        MergeString (&Key, "Comic_%d", Index);
        ReadVarFile ((*AllSet)->VarFilePath, Key, 1, &Comic->Name);
        free (Key);

        Comic->Chapter = NULL;

        char* FilePath = (*AllSet)->GetPath(*AllSet, "File_Codes");
        MergeString (&FilePath, FilePath,Sequence);

        if (GetFileSize (FilePath) > 0) {
          for (int ChapterIndex = 0; ; ChapterIndex++) {
            printf ("Sequence %s, ChapterIndex %d\n",Sequence,ChapterIndex);
            MergeString (&Key, "Chapter_%d", ChapterIndex);
            char* Line = NULL;
            //ReadVarFileLine (FilePath, Key, 0, &Line);
            free (Key);

            if (Line != NULL) {
              CHAPTER*  Chapter = NULL;
              Chapter = (CHAPTER*) malloc (sizeof (CHAPTER) * 1);

              /*
              Chapter->Index = Index;

              MergeString (&Key, "Chapter_%d", ChapterIndex);
              ReadVarFile (FilePath, Key, 1, &Chapter->Sid);

              MergeString (&Key, "Chapter_%d", ChapterIndex);
              ReadVarFile (FilePath, Key, 2, &Chapter->Did);

              MergeString (&Key, "Chapter_%d", ChapterIndex);
              ReadVarFile (FilePath, Key, 3, &Chapter->PicRange);

              MergeString (&Key, "Chapter_%d", ChapterIndex);
              ReadVarFile (FilePath, Key, 4, &Chapter->Code);
              free (Key);
*/

              Chapter->Next = NULL;
              if (Comic->Chapter == NULL) {
                Comic->Chapter = Chapter;
              }
              else {
                CHAPTER* ChapterPoint = NULL;
                for (ChapterPoint = Comic->Chapter; ChapterPoint->Next != NULL; ChapterPoint = ChapterPoint->Next) ;
                ChapterPoint->Next = Chapter;
              }
            }
            else {
              break;
            }
          }
        }

        free (FilePath);

        Comic->GetChapter = GetChapter;




        Comic->Next = NULL;
        if ((*AllSet)->Comic == NULL) {
          (*AllSet)->Comic = Comic;
        }
        else {
          COMIC* ComicPoint = NULL;
          for (ComicPoint = (*AllSet)->Comic; ComicPoint->Next != NULL; ComicPoint = ComicPoint->Next) ;
          ComicPoint->Next = Comic;
        }

      }
      else {
        break;
      }
    }
    (*AllSet)->GetComic = GetComic;
    (*AllSet)->FreeAllComic = FreeAllComic;

    //5
    (*AllSet)->Note = NULL;
    AddNote (*AllSet, "UpdateStartWarning");
    AddNote (*AllSet, "DoubleLine");
    AddNote (*AllSet, "Line");
    (*AllSet)->GetNote = GetNote;
    (*AllSet)->FreeAllNote = FreeAllNote;

  }
  else {
    //0
    (*AllSet) = NULL;
  }
}
void FreeSetStruct (ALL_SET* AllSet) {
  if (AllSet != NULL) {
    //5
    AllSet->FreeAllNote (AllSet);

    //4
    AllSet->FreeAllComic (AllSet);

    //3
    AllSet->FreeAllInterface (AllSet);

    //2
    AllSet->FreeAllPath (AllSet);

    //1
    free (AllSet->VarFilePath);
    //0
    free (AllSet);
  }
}
//-----------------------------------------------------------------------------

void ShowContinueUpDateMenu (ALL_SET* AllSet, int *Choice){
  INTERFACE* Interface = NULL;
  AllSet->GetInterface (AllSet, "ContinueUpDate", &Interface);

  if (Interface != NULL) {
    int  Top = 0;
    int  Left = 0;

    system ("CLS");
    Print (" %=NY %s %=Nw\n", Interface->Title);
    Print ("%s\n", AllSet->GetNote(AllSet,"DoubleLine"));
    Print (" %=NR %s %=Nw\n", Interface->Content);
    Print ("%=NW %s %=Nw\n", AllSet->GetNote(AllSet,"Line"));
    GetCursorPosition (&Left, &Top);

    int Index = 0;
    for (ITEM* Item = Interface->Item; Item != NULL; Item = Item->Next, Index++)
      Print ("    %=NW %d%=Nw . %s\n", (Index + 1), Item->Name);

    Print ("%=NW %s %=Nw\n", AllSet->GetNote(AllSet,"Line"));
    Print (" %=NR %s %=Nw\n", Interface->Explain);
    Print ("%s\n", AllSet->GetNote(AllSet,"DoubleLine"));

    InterfaceCursor ( 2, Top, Interface->ChoiceSymbol, Index, Choice);
    system ("CLS");
  }
}

void TransformJs (char* FromFileName, char* ToFileName) {
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


  FileSize = GetFileSize (FromFileName);

  if (FileSize > 0) {
    if ((FilePoint = fopen (FromFileName, "r")) == NULL) {
      printf("開啟檔案錯誤!\n");
    }
    else {
      Article = (char*) malloc(sizeof (char) * (FileSize + 1));

      for (Index = 0; (Article[Index]=getc(FilePoint)) != EOF; Index++)
        if (Article[Index] == ' ')
          Index--;

      Article[Index] = '\0';
      fclose(FilePoint);

      UrlIndex = 0;
      ClearFile (ToFileName);

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

          if ((FilePoint = fopen (ToFileName, "a")) == NULL) {
            printf("開啟檔案錯誤!\n");
          }
          else {
            fprintf (FilePoint, "[%d]%s|\n", ConditionIndex, Url);
            fclose (FilePoint);
          }
          free (Url);
        }
        free (StartKey);
      }
      free (Article);
    }
  }
  else {
    printf("檔案內沒有任何文字!\n");
  }
}





void GetKeySearchValue (char* StartKey, char* EndKey, char** Value, char* FileName, bool IsIgnoreSpace){
  FILE   *FilePoint;
  int    FileSize = 0;
  char   *Article = NULL;
  int    Index = 0;
  int    KeyIndex = 0;
  int    Start = -1;
  int    End = -1;

  FileSize = GetFileSize (FileName);
  (*Value) = NULL;

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
        (*Value) = (char*) malloc (sizeof (char) * (Length + 1));

        for (Index = Start,KeyIndex = 0; Index < End; Index++)
          (*Value)[KeyIndex++] = Article[Index];
        (*Value)[KeyIndex] = '\0';
      }
      free (Article);
    }
  }
}

void DeCodes(char* Codes, char* FileName){
  FILE *FilePoint = NULL;
  int FileSize = 0;
  int Index = 0;
  int KeyNum = 0;
  int Chapter = 0;

  ClearFile (FileName);

  if ((FilePoint = fopen (FileName, "a")) != NULL) {
    for (Index = 0,KeyNum = 0; Index < strlen (Codes); Index++) {
      if (KeyNum == 0){
        fprintf (FilePoint, "[");
        fprintf (FilePoint, "Chapter_%d]", Chapter);
        for (; Index < strlen (Codes) && Codes[Index] != ' '; Index++) {
          fprintf (FilePoint, "%c", Codes[Index]);
        }
        fprintf (FilePoint, "|");
        KeyNum++;
      }
      else if (KeyNum > 0 && KeyNum < 4) {
        for (; Index < strlen (Codes) && Codes[Index] != ' '; Index++) {
          fprintf (FilePoint, "%c",Codes[Index]);
        }
        fprintf (FilePoint, "|");
        KeyNum++;
      }
      else {
        for (; Index < strlen (Codes) && Codes[Index] != '|'; Index++) {
          fprintf (FilePoint, "%c",Codes[Index]);
        }
        fprintf (FilePoint, "|\n");
        KeyNum = 0;
        Chapter++;
      }
    }
    fclose (FilePoint);
  }
}
void Update (ALL_SET* AllSet) {

  int Choice = 1;
  ShowContinueUpDateMenu (AllSet, &Choice);
  if (Choice == 1) {
    //RunCommand (AllSet->GetPath (AllSet, "File_Wget"), AllSet->GetPath (AllSet, "Url_Base"), AllSet->GetPath (AllSet, "File_TestAccessIndex"));
    if (GetFileSize (AllSet->GetPath (AllSet, "File_TestAccessIndex")) > 0) {
      //RunCommand (AllSet->GetPath (AllSet, "File_Wget"), AllSet->GetPath (AllSet, "Url_ComicView"), AllSet->GetPath (AllSet, "File_ComicView"));
      //TransformJs (AllSet->GetPath (AllSet, "File_ComicView"), AllSet->GetPath (AllSet, "File_JsTransform"));

      FILE *FilePoint = NULL;
      ClearFile(AllSet->GetPath (AllSet,"File_AllComicCode"));
      if ((FilePoint = fopen (AllSet->GetPath (AllSet,"File_AllComicCode"), "a")) != NULL) {
        putc (0xEF, FilePoint);
        putc (0xBB, FilePoint);
        putc (0xBF, FilePoint);

        fprintf (FilePoint, "%s\n", ANSIToUTF8 ("//"));
        fprintf (FilePoint, "%s\n", ANSIToUTF8 ("// 漫畫代碼,名稱,章節代碼"));
        fprintf (FilePoint, "%s\n", ANSIToUTF8 ("//"));
        fprintf (FilePoint, "%s\n", ANSIToUTF8 ("var gComicList = new Array ("));

        for (COMIC* Comic = AllSet->Comic; Comic != NULL; Comic = Comic->Next) {
          char* HtmlUrl = NULL;
          char* HtmlPath = NULL;
          char* ShowKeySatrtFormat = NULL;
          char* ShowKey = NULL;
          char* ShowBaseUrl = NULL;
          char* ShowUrl = NULL;
          char* ShowPath = NULL;

          char* Codes = NULL;
          char* CodePath = NULL;

          MergeString (&HtmlUrl, AllSet->GetPath (AllSet,"Url_Html"), Comic->Sequence);
          MergeString (&HtmlPath, AllSet->GetPath (AllSet,"File_Html"), Comic->Sequence);
          //RunCommand (AllSet->GetPath (AllSet, "File_Wget"), HtmlUrl, HtmlPath);
          free (HtmlUrl);

          MergeString (&ShowKeySatrtFormat, "cview('%s-1.html',", Comic->Sequence);
          GetKeySearchValue (ShowKeySatrtFormat,");",&ShowKey,HtmlPath,true);
          free (HtmlPath);
          free (ShowKeySatrtFormat);


          ReadVarFile (AllSet->GetPath (AllSet, "File_JsTransform"), ShowKey, 0, &ShowBaseUrl);
          MergeString (&ShowUrl, "%s%s.html", ShowBaseUrl, Comic->Sequence);
          free (ShowKey);
          free (ShowBaseUrl);


          MergeString (&ShowPath, AllSet->GetPath (AllSet,"File_Show"), Comic->Sequence);
          //RunCommand (AllSet->GetPath (AllSet, "File_Wget"), ShowUrl, ShowPath);
          free (ShowUrl);

          GetKeySearchValue ("codes=\"","\".split('|')",&Codes,ShowPath,false);
          free (ShowPath);

          MergeString (&CodePath, AllSet->GetPath (AllSet,"File_Codes"), Comic->Sequence);
          DeCodes (Codes,CodePath);
          free (CodePath);

          fprintf (FilePoint, "  new Array (%s, \"%s\", \"%s\")", Comic->Sequence, ANSIToUTF8 (Comic->Name), Codes);
          if (Comic->Next != NULL)
            fprintf (FilePoint, ",");
          fprintf (FilePoint, "\n");

          free (Codes);

        }
        fprintf (FilePoint, "%s\n", ANSIToUTF8 (");"));
        fclose (FilePoint);
      }
    }
  }
}


void Browser (ALL_SET* AllSet) {
  system (AllSet->GetPath(AllSet, "File_BrowserIndex"));
}

void ShowComicListMenu (ALL_SET* AllSet, int *Choice){
  INTERFACE* Interface = NULL;
  AllSet->GetInterface (AllSet, "ComicList", &Interface);

  if (Interface != NULL) {
    int  Top = 0;
    int  Left = 0;

    system ("CLS");
    Print (" %=NY %s %=Nw\n", Interface->Title);
    Print ("%s\n", AllSet->GetNote(AllSet,"DoubleLine"));
    GetCursorPosition (&Left, &Top);

    int Index = 0;
    COMIC* Comic = NULL;
    for (Comic = AllSet->Comic; Comic != NULL; Comic = Comic->Next, Index++)
      Print ("    %=NW %d%=Nw . %s\n", (Index + 1), Comic->Name);

    Print ("%=NW %s %=Nw\n", AllSet->GetNote(AllSet,"Line"));
    Print (" %=NR %s %=Nw\n", Interface->Explain);
    Print ("%s\n", AllSet->GetNote(AllSet,"DoubleLine"));
    InterfaceCursor ( 2, Top, Interface->ChoiceSymbol, Index, Choice);

    if ((*Choice) != 0) {
      for (Comic = AllSet->Comic,Index = 1; Comic != NULL && Index != (*Choice); Comic = Comic->Next, Index++);
      (*Choice) = atoi (Comic->Sequence);
    }
    system ("CLS");
  }
}

void ShowChapterListMenu (ALL_SET* AllSet,char* ComicSequence, int *Choice){
  INTERFACE* Interface = NULL;
  AllSet->GetInterface (AllSet, "ChapterList", &Interface);

  if (Interface != NULL) {
    int  Top = 0;
    int  Left = 0;

    system ("CLS");
    Print (" %=NY %s %=Nw\n", Interface->Title);
    Print ("%s\n", AllSet->GetNote(AllSet,"DoubleLine"));
    GetCursorPosition (&Left, &Top);

    int Index = 0;
    COMIC* Comic = NULL;
    AllSet->GetComic (AllSet,ComicSequence,&Comic);

    CHAPTER* Chapter = NULL;
    for (Chapter = Comic->Chapter; Chapter != NULL; Chapter= Chapter->Next, Index++)
      Print ("    %=NW %d%=Nw . %s\n", (Index + 1), Chapter->Index);

    Print ("%=NW %s %=Nw\n", AllSet->GetNote(AllSet,"Line"));
    Print (" %=NR %s %=Nw\n", Interface->Explain);
    Print ("%s\n", AllSet->GetNote(AllSet,"DoubleLine"));
    InterfaceCursor ( 2, Top, Interface->ChoiceSymbol, Index, Choice);

    if ((*Choice) != 0) {
      for (Chapter = Comic->Chapter,Index = 1; Chapter != NULL && Index != (*Choice); Chapter = Chapter->Next, Index++);
      (*Choice) = atoi (Chapter->Index);
    }
    system ("CLS");
  }
}
void Doenload (ALL_SET* AllSet) {
  int Choice = 1;
  while (Choice != 0) {
    ShowComicListMenu (AllSet, &Choice);
    if (Choice != 0) {
      char* ComicSequence = NULL;
      MergeString (&ComicSequence , "%d", Choice);

      Choice = 1;
      while (Choice != 0) {
        ShowChapterListMenu (AllSet, ComicSequence, &Choice);

        if (Choice != 0) {
          printf ("%d\n",Choice);
          system ("pause");
        }
      }

      free (ComicSequence);
    }
  }
}
int main(int argc, char * argv[]) {

  char VarFilePath[] = ".\\\\SetVar\\\\VarFile.var";
  ALL_SET* AllSet = NULL;


  char*** val =NULL;
  ReadVarFileLine (".\\\\PageCode\\\\Codes-102.var","Chapter_236", &val);
  printf ("%s\n",val[0][0]);
  free (val);

  system ("pause");


/*
  LocateSetStruct(&AllSet, VarFilePath);

  if (AllSet != NULL) {
    int Choice = 1;
    while (Choice != 8) {
      ShowMainMain (AllSet,&Choice);

      switch (Choice) {
        case 1:
          Update(AllSet);
          break;

        case 2:
          Browser(AllSet);
          break;

        case 3:
          Doenload(AllSet);
          break;

        case 4:
          //Function4(AllSet);
          break;

        case 5:
          //Function5(AllSet);
          break;

        case 6:
          //Function6(AllSet);
          break;

        case 7:
          //Function7(AllSet);
          break;
      }
    }
  }
  FreeSetStruct (AllSet);
/**/
  return 0;
}

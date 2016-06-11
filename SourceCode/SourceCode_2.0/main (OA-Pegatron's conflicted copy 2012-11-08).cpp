#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>
#include <time.h>
#include "sqlite3.h"

#define   VAR_START_SYMBOL     0x0000005B
#define   VAR_END_SYMBOL       0x0000005D
#define   VAR_VALUE_SYMBOL     0x0000007C
//---------------------------------------------------------------public
void FreeString (char* String) {
  if (String != NULL)
    free (String);
}
bool FileIsExists(const char* FileName){
    if (FILE* File = fopen(FileName, "r")){
        fclose(File);
        return true;
    }
    return false;
}
int GetIntLength (int Num) {
  int Index = 1;
  for (Index = 1; (Num = Num/10)>0 ; Index++);
  return Index;
}
char* MergeString (char* Format, ...) {
  char* OutString = NULL;
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
          case '%':
            Length++;
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
    for (int NowLength = 0; *Format!='\0'; *Format++)
      if (*Format =='%') {
        switch (*++Format){
          case 's':
            TempString = va_arg(args, char *);
            if (TempString != NULL)
              sprintf (OutString, "%s%s", OutString, TempString);
            break;
          case 'd':
            sprintf (OutString, "%s%d", OutString, va_arg(args, int));
            break;
          case '%':
            sprintf (OutString, "%s%%",OutString);
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
void RunCommand (char* WgetFile, char* Url, char* FileName){

  printf ("Get %s\n",Url);

  char* Command = MergeString ("%s %s -O %s -nv", WgetFile, Url, FileName);
  ClearFile(FileName);
  system(Command);
  free (Command);

  printf ("Get %s Sucess\n",Url);
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
//--------------------------------------------------------------UTF-8
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
//---------------------------------------------------------------DataBase-struct--


typedef struct   _DATA_BASE_    DATABASE;
typedef struct   _INTERFACE_    INTERFACE;
typedef struct   _ITEM_         ITEM;
typedef struct   _COMIC_        COMIC;
typedef struct   _CHAPTER_      CHAPTER;

typedef void       (*FUNC_MODULE_CHECK_DB_EXISTS)  (DATABASE* DataBase);
typedef char*      (*FUNC_MODULE_GET_VARIABLE)     (DATABASE* DataBase, char* Name);
typedef char*      (*FUNC_MODULE_GET_SHOW_BASE_URL)(DATABASE* DataBase, char* Key);
typedef INTERFACE* (*FUNC_MODULE_GET_INTERFACE)    (DATABASE* DataBase, char* Name);
typedef COMIC*     (*FUNC_MODULE_GET_COMIC)        (DATABASE* DataBase, char* ComicIndex);
typedef char*      (*FUNC_MODULE_GET_PAGE)         (DATABASE* DataBase, char* Name);
typedef CHAPTER*   (*FUNC_MODULE_GET_CHAPTER)      (DATABASE* DataBase, char* ParentSeq, char* ChapterIndex);

typedef COMIC*     (*FUNC_MODULE_GET_COMIC_LIST)   (DATABASE* DataBase);
typedef ITEM*      (*FUNC_MODULE_GET_ITEM_LIST)    (DATABASE* DataBase, char* InterfaceSeq);
typedef CHAPTER*   (*FUNC_MODULE_GET_CHAPTER_LIST) (DATABASE* DataBase, char* ParentSeq);


typedef bool       (*FUNC_MODULE_SET_VARIABLE)     (DATABASE* DataBase, char* Name, char* Value);
typedef bool       (*FUNC_MODULE_SET_PAGE)         (DATABASE* DataBase, char* Name, char* Content);
typedef bool       (*FUNC_MODULE_SET_CHAPTER)      (DATABASE* DataBase, char* ComicSeq, char* ChapterIndex, char* Sid, char* Did, char* PicRange, char* Code);
typedef bool       (*FUNC_MODULE_SET_BASE_URL)     (DATABASE* DataBase, char* Key, char* BaseUrl);

typedef void       (*FUNC_MODULE_FREE_DATABASE)    (DATABASE* DataBase);


typedef void       (*FUNC_MODULE_FREE_INTERFACE)   (INTERFACE* Interface);
typedef void       (*FUNC_MODULE_FREE_ITEM)        (ITEM* Item);
typedef void       (*FUNC_MODULE_FREE_COMIC)       (COMIC* Comic);
typedef void       (*FUNC_MODULE_FREE_CHAPTER)     (CHAPTER* Chapter);


typedef struct _DATA_BASE_ {
  char*                         DataBasePath;
  FUNC_MODULE_CHECK_DB_EXISTS   CheckDataBase;
  FUNC_MODULE_GET_VARIABLE      GetVariable;
  FUNC_MODULE_GET_INTERFACE     GetInterface;
  FUNC_MODULE_GET_COMIC         GetComic;
  FUNC_MODULE_GET_PAGE          GetPage;
  FUNC_MODULE_GET_CHAPTER       GetChapter;
  FUNC_MODULE_GET_SHOW_BASE_URL GetShowBaseUrl;

  FUNC_MODULE_GET_COMIC_LIST     GetComicList;
  FUNC_MODULE_GET_ITEM_LIST      GetItemList;
  FUNC_MODULE_GET_CHAPTER_LIST   GetChapterList;

  FUNC_MODULE_SET_VARIABLE      SetVariable;
  FUNC_MODULE_SET_PAGE          SetPage;
  FUNC_MODULE_SET_CHAPTER       SetChapter;
  FUNC_MODULE_SET_BASE_URL      SetShowBaseUrl;



  FUNC_MODULE_FREE_DATABASE     FreeDataBase;

} DATABASE;

typedef struct _INTERFACE_ {
  char*                         Sequence;
  char*                         Name;
  char*                         Title;
  char*                         Content;
  char*                         Explain;
  char*                         ChoiceSymbol;
  FUNC_MODULE_FREE_INTERFACE    FreeInterface;
} INTERFACE;

typedef struct _ITEM_ {
  char*                         ParentSeq;
  char*                         Name;
  FUNC_MODULE_FREE_ITEM         FreeItem;
  ITEM*                         Next;
} ITEM;

typedef struct _COMIC_ {
  char*                         Sequence;
  char*                         Index;
  char*                         Name;
  FUNC_MODULE_FREE_COMIC        FreeComic;
  COMIC*                        Next;
} COMIC;

typedef struct _CHAPTER_ {
  char*                         ParentSeq;
  char*                         Index;
  char*                         Sid;
  char*                         Did;
  char*                         PicRange;
  char*                         Code;
  FUNC_MODULE_FREE_CHAPTER      FreeChapter;
  CHAPTER*                      Next;
} CHAPTER;

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
  FreeString (DataBaseJournal);

  if (!FileIsExists (DataBase->DataBasePath)) {
    Result = sqlite3_open_v2(
               DataBase->DataBasePath,
               &DataBaseLink,
               SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
               NULL
               );

    if (Result == SQLITE_OK){
      char* Create[] = {
         "CREATE TABLE Variable    (Sequence INTEGER PRIMARY KEY, Name TEXT,            Value TEXT);"
        ,"CREATE TABLE Interface   (Sequence INTEGER PRIMARY KEY, Name TEXT,            Title TEXT,       Explain TEXT,     Symbol TEXT,   Content TEXT,   Language TEXT);"
        ,"CREATE TABLE Item        (Sequence INTEGER PRIMARY KEY, InterfaceSeq TEXT,    Name TEXT,        Language TEXT);"
        ,"CREATE TABLE Comic       (Sequence INTEGER PRIMARY KEY, ComicIndex TEXT,      Name TEXT);"
        ,"CREATE TABLE Chapter     (Sequence INTEGER PRIMARY KEY, ComicSeq TEXT,        ChapterIndex TEXT, Sid TEXT,         Did TEXT,      PicRange TEXT,  Code TEXT);"
        ,"CREATE TABLE ShowBaseUrl (Sequence INTEGER PRIMARY KEY, Key TEXT,             BaseUrl TEXT);"
      };

      for (int Index = 0; Index < 6; Index++) {
        Result = sqlite3_exec(DataBaseLink, Create[Index], 0, 0, &ErroeMessage);
        if (Result != SQLITE_OK) {
          printf ("Create Table Error! Index : %d, Message : %s\n", Index, ErroeMessage);
          break;
        }
      }

      char* InsertVariable[] = {
         "INSERT INTO Variable (Sequence, Name, Value) VALUES ( 1, 'File_Wget',            '.\\Command\\wget.exe');"
        ,"INSERT INTO Variable (Sequence, Name, Value) VALUES ( 2, 'File_AllComicCode',    '.\\JavaScrip\\AllComicCode.js');"
        ,"INSERT INTO Variable (Sequence, Name, Value) VALUES ( 3, 'Folder_TempFile',      '.\\TempFile\\TempFile.txt');"
        ,"INSERT INTO Variable (Sequence, Name, Value) VALUES ( 4, 'File_BrowserIndex',    '.\\index.html');"
        ,"INSERT INTO Variable (Sequence, Name, Value) VALUES ( 5, 'Folder_Download',      '.\\Download\\');"
        ,"INSERT INTO Variable (Sequence, Name, Value) VALUES ( 6, 'Url_WebBaseUrl',       'http://www.8comic.com/');"
        ,"INSERT INTO Variable (Sequence, Name, Value) VALUES ( 7, 'Url_ComicView',        'http://www.8comic.com/js/comicview.js');"
        ,"INSERT INTO Variable (Sequence, Name, Value) VALUES ( 8, 'Url_HtmlBaseUrl',      'http://www.8comic.com/html/');"
        ,"INSERT INTO Variable (Sequence, Name, Value) VALUES ( 9, 'LanguageIndex',        '0');"
        ,"INSERT INTO Variable (Sequence, Name, Value) VALUES (10, 'DoubleLine',           '====================================================');"
        ,"INSERT INTO Variable (Sequence, Name, Value) VALUES (11, 'Line',                 '----------------------------------------------------');"
      };
      for (int Index = 0; Index < 11; Index++) {
        Result = sqlite3_exec(DataBaseLink, InsertVariable[Index], 0, 0, &ErroeMessage);
        if (Result != SQLITE_OK) {
          printf ("INSERT Variable Error! Index : %d, Message : %s\n", Index, ErroeMessage);
          break;
        }
      }


      char* InsertInterface[] = {
         "INSERT INTO Interface (Sequence, Name, Title, Explain, Symbol, Content, Language) VALUES ( 1, 'MainMenu', '主要 選單', '↑↓鍵選擇，確認選擇 請在該項目前按→鍵!', '>', '主要功能 ', '0');"
        ,"INSERT INTO Interface (Sequence, Name, Title, Explain, Symbol, Content, Language) VALUES ( 2, 'IsContinueUpDate', '是否繼續', '↑↓鍵選擇，←鍵離開，確認選擇請在該項目內按→鍵!', '>', '警告! 更新當中，請勿任意的中斷結束此程式!\n 請問! 是否繼續執行更新!?', '0');"
        ,"INSERT INTO Interface (Sequence, Name, Title, Explain, Symbol, Content, Language) VALUES ( 3, 'ComicListMenu', '請選擇要下載的漫畫', '↑↓鍵選擇，←鍵離開，確認選擇請在該項目內按→鍵!', '>', '警告! 更新當中，請勿任意的中斷結束此程式!\n 請問! 是否繼續執行更新!?', '0');"
        ,"INSERT INTO Interface (Sequence, Name, Title, Explain, Symbol, Content, Language) VALUES ( 4, 'ChapterListMenu', '請選擇要下載的章節', '↑↓鍵選擇，←鍵離開，確認選擇請在該項目內按→鍵!', '>', '警告! 更新當中，請勿任意的中斷結束此程式!\n 請問! 是否繼續執行更新!?', '0');"
      };
      for (int Index = 0; Index < 4; Index++) {

        Result = sqlite3_exec(DataBaseLink, (InsertInterface[Index]), 0, 0, &ErroeMessage);
        if (Result != SQLITE_OK) {
          printf ("INSERT Interface Error! Index : %d, Message : %s\n", Index, ErroeMessage);
          break;
        }
      }

      char* InsertItem[] = {
          "INSERT INTO Item (Sequence, InterfaceSeq, Name, Language) VALUES ( 1, 1, '更新漫畫檔   -- Update', '0');"
         ,"INSERT INTO Item (Sequence, InterfaceSeq, Name, Language) VALUES ( 2, 1, '瀏覽漫畫     -- Browser', '0');"
         ,"INSERT INTO Item (Sequence, InterfaceSeq, Name, Language) VALUES ( 3, 1, '下載漫畫     -- Download', '0');"
         ,"INSERT INTO Item (Sequence, InterfaceSeq, Name, Language) VALUES ( 4, 1, '清除暫存檔案 -- Clear', '0');"
         ,"INSERT INTO Item (Sequence, InterfaceSeq, Name, Language) VALUES ( 5, 1, '下載資料夾   -- Download Folder', '0');"
         ,"INSERT INTO Item (Sequence, InterfaceSeq, Name, Language) VALUES ( 6, 1, '設定語言     -- Set Language', '0');"
         ,"INSERT INTO Item (Sequence, InterfaceSeq, Name, Language) VALUES ( 7, 1, '程式說明     -- Explain', '0');"
         ,"INSERT INTO Item (Sequence, InterfaceSeq, Name, Language) VALUES ( 8, 1, '離開本程式   -- Exit', '0');"

         ,"INSERT INTO Item (Sequence, InterfaceSeq, Name, Language) VALUES ( 9, 2, '是的! -- Yes!', '0');"
         ,"INSERT INTO Item (Sequence, InterfaceSeq, Name, Language) VALUES (10, 2, '不要! -- No!', '0');"
      };
      for (int Index = 0; Index < 10; Index++) {
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
  }
}

char* GetPage(DATABASE* DataBase, char* Name) {
  char*         Content = NULL;

  sqlite3_stmt  *stmt;
  int           Result = 0;
  sqlite3*      DataBaseLink;

  if (Name != NULL) {
    Result = sqlite3_open_v2(
               DataBase->DataBasePath,
               &DataBaseLink,
               SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
               NULL
               );

    if (Result == SQLITE_OK) {
      char* Select = MergeString ("SELECT Content FROM Page WHERE Name = '%s'", Name);
      Result = sqlite3_prepare(DataBaseLink, Select, -1, &stmt, NULL);
      if (Result == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
          Content = MergeString("%s", sqlite3_column_text(stmt, 0));
        }
      }
      sqlite3_finalize(stmt);
      FreeString (Select);
    }
    sqlite3_close(DataBaseLink);
  }
  return Content;
}
char* GetShowBaseUrl(DATABASE* DataBase, char* Key) {
  char*         Url = NULL;

  sqlite3_stmt  *stmt;
  int           Result = 0;
  sqlite3*      DataBaseLink;

  if (Key != NULL) {
    Result = sqlite3_open_v2(
               DataBase->DataBasePath,
               &DataBaseLink,
               SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
               NULL
               );

    if (Result == SQLITE_OK) {
      char* Select = MergeString ("SELECT BaseUrl FROM ShowBaseUrl WHERE Key = '%s'", Key);
      Result = sqlite3_prepare(DataBaseLink, Select, -1, &stmt, NULL);
      if (Result == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
          Url = MergeString("%s", sqlite3_column_text(stmt, 0));
        }
      }
      sqlite3_finalize(stmt);
      FreeString (Select);
    }
    sqlite3_close(DataBaseLink);
  }
  return Url;
}
char* GetVariable(DATABASE* DataBase, char* Name) {
  char*         Value = NULL;

  sqlite3_stmt  *stmt;
  int           Result = 0;
  sqlite3*      DataBaseLink;

  if (Name != NULL) {
    Result = sqlite3_open_v2(
               DataBase->DataBasePath,
               &DataBaseLink,
               SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
               NULL
               );

    if (Result == SQLITE_OK) {
      char* Select = MergeString ("SELECT Value FROM Variable WHERE Name = '%s'", Name);
      Result = sqlite3_prepare(DataBaseLink, Select, -1, &stmt, NULL);
      if (Result == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
          Value = MergeString("%s", sqlite3_column_text(stmt, 0));
        }
      }
      sqlite3_finalize(stmt);
      FreeString (Select);
    }
    sqlite3_close(DataBaseLink);
  }
  return Value;
}
void FreeInterface (INTERFACE* Interface) {
  if (Interface != NULL) {
    free (Interface->Sequence);
    free (Interface->Name);
    free (Interface->Title);
    free (Interface->Explain);
    free (Interface->ChoiceSymbol);
    free (Interface->Content);
    free (Interface);
  }
}
INTERFACE* GetInterface (DATABASE* DataBase, char* Name) {
  INTERFACE*    Interface = NULL;

  sqlite3_stmt  *stmt;
  int           Result = 0;
  sqlite3*      DataBaseLink;

  if (Name != NULL) {
    Result = sqlite3_open_v2(
               DataBase->DataBasePath,
               &DataBaseLink,
               SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
               NULL
               );

    if (Result == SQLITE_OK) {
      char* Select = MergeString ("SELECT Sequence,Name,Title,Explain,Symbol,Content FROM Interface WHERE Name = '%s' AND Language = '%s'", Name, DataBase->GetVariable (DataBase, "LanguageIndex"));
      Result = sqlite3_prepare(DataBaseLink, Select, -1, &stmt, NULL);
      if (Result == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
          Interface = (INTERFACE*) malloc (sizeof (INTERFACE) * 1);

          Interface->Sequence      = MergeString("%s", sqlite3_column_text(stmt, 0));
          Interface->Name          = MergeString("%s", sqlite3_column_text(stmt, 1));
          Interface->Title         = MergeString("%s", sqlite3_column_text(stmt, 2));
          Interface->Explain       = MergeString("%s", sqlite3_column_text(stmt, 3));
          Interface->ChoiceSymbol  = MergeString("%s", sqlite3_column_text(stmt, 4));
          Interface->Content       = MergeString("%s", sqlite3_column_text(stmt, 5));
          Interface->FreeInterface = FreeInterface;
        }
      }
      sqlite3_finalize(stmt);
      FreeString (Select);
    }
    sqlite3_close(DataBaseLink);
  }
  return Interface;
}


void FreeComic (COMIC* Comic) {
  if (Comic != NULL) {
    free (Comic->Sequence);
    free (Comic->Index);
    free (Comic->Name);
    if (Comic->Next != NULL)
      FreeComic (Comic->Next);
    free (Comic);
  }
}
COMIC* GetComic (DATABASE* DataBase, char* ComicIndex) {
  COMIC*    Comic = NULL;

  sqlite3_stmt  *stmt;
  int           Result = 0;
  sqlite3*      DataBaseLink;

  if (ComicIndex != NULL) {
    Result = sqlite3_open_v2(
               DataBase->DataBasePath,
               &DataBaseLink,
               SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
               NULL
               );

    if (Result == SQLITE_OK) {
      char* Select = MergeString ("SELECT Sequence,ComicIndex,Name FROM Comic WHERE ComicIndex = '%s'", ComicIndex);
      Result = sqlite3_prepare(DataBaseLink, Select, -1, &stmt, NULL);
      if (Result == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
          Comic = (COMIC*) malloc (sizeof (COMIC) * 1);

          Comic->Sequence  = MergeString("%s", sqlite3_column_text(stmt, 0));
          Comic->Index     = MergeString("%s", sqlite3_column_text(stmt, 1));
          Comic->Name      = MergeString("%s", sqlite3_column_text(stmt, 2));
          Comic->FreeComic = FreeComic;
          Comic->Next      = NULL;
        }
      }
      sqlite3_finalize(stmt);
      FreeString (Select);
    }
    sqlite3_close(DataBaseLink);
  }
  return Comic;
}

void FreeChapter (CHAPTER* Chapter) {
  if (Chapter != NULL) {
    free (Chapter->ParentSeq);
    free (Chapter->Index);
    free (Chapter->Sid);
    free (Chapter->Did);
    free (Chapter->PicRange);
    free (Chapter->Code);
    if (Chapter->Next != NULL)
      FreeChapter (Chapter->Next);
    free (Chapter);
  }
}
CHAPTER* GetChapter (DATABASE* DataBase, char* ParentSeq, char* ChapterIndex) {
  CHAPTER*    Chapter = NULL;

  sqlite3_stmt  *stmt;
  int           Result = 0;
  sqlite3*      DataBaseLink;

  if (ParentSeq != NULL) {
    Result = sqlite3_open_v2(
               DataBase->DataBasePath,
               &DataBaseLink,
               SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
               NULL
               );

    if (Result == SQLITE_OK) {
      char* Select = MergeString ("SELECT ComicSeq,ChapterIndex,Sid,Did,PicRange,Code FROM Chapter WHERE ComicSeq = '%s' AND ChapterIndex = '%S'", ParentSeq, ChapterIndex);
      Result = sqlite3_prepare(DataBaseLink, Select, -1, &stmt, NULL);
      if (Result == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
          Chapter = (CHAPTER*) malloc (sizeof (CHAPTER) * 1);

          Chapter->ParentSeq = MergeString("%s", sqlite3_column_text(stmt, 0));
          Chapter->Index     = MergeString("%s", sqlite3_column_text(stmt, 1));
          Chapter->Sid       = MergeString("%s", sqlite3_column_text(stmt, 2));
          Chapter->Did       = MergeString("%s", sqlite3_column_text(stmt, 3));
          Chapter->PicRange  = MergeString("%s", sqlite3_column_text(stmt, 4));
          Chapter->Code      = MergeString("%s", sqlite3_column_text(stmt, 5));

          Chapter->FreeChapter = FreeChapter;

          Chapter->Next = NULL;
        }
      }
      sqlite3_finalize(stmt);
      FreeString (Select);
    }
    sqlite3_close(DataBaseLink);
  }
  return Chapter;
}
void FreeDataBase (DATABASE* DataBase) {
  if (DataBase != NULL) {
    FreeString (DataBase->DataBasePath);
    free (DataBase);
  }
}
bool SetShowBaseUrl (DATABASE* DataBase, char* Key, char* BaseUrl) {
  bool IsSuccess = false;
  if ((Key != NULL) && (BaseUrl != NULL)) {
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
      FreeString (Sql);

      if (Result == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
          int RowCount = sqlite3_column_int (stmt, 0);

          if (RowCount > 0) {
            Sql = MergeString ("UPDATE ShowBaseUrl SET BaseUrl = '%s' WHERE Key = '%s'", BaseUrl, Key);
            Result = sqlite3_exec(DataBaseLink, Sql, 0, 0, &ErrorMessage);
            FreeString (Sql);
            if (Result == SQLITE_OK) {
              IsSuccess = true;
            }
          }
          else {
            Sql = MergeString ("INSERT INTO ShowBaseUrl (Key, BaseUrl) VALUES ('%s', '%s')", Key, BaseUrl);
            Result = sqlite3_exec(DataBaseLink, Sql, 0, 0, &ErrorMessage);
            FreeString (Sql);
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
bool SetVariable (DATABASE* DataBase, char* Name, char* Value) {
  bool IsSuccess = false;
  if ((Name != NULL) && (Value != NULL)) {
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
      char* Sql = MergeString ("SELECT COUNT (Sequence) FROM Variable WHERE Name = '%s'", Name);
      Result = sqlite3_prepare(DataBaseLink, Sql, -1, &stmt, NULL);
      FreeString (Sql);

      if (Result == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
          int RowCount = sqlite3_column_int (stmt, 0);

          if (RowCount > 0) {
            Sql = MergeString ("UPDATE Variable SET Value = '%s' WHERE Name = '%s'", Value, Name);
            Result = sqlite3_exec(DataBaseLink, Sql, 0, 0, &ErrorMessage);
            FreeString (Sql);
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
bool SetPage (DATABASE* DataBase, char* Name, char* Content) {
  bool IsSuccess = false;
  if ((Name != NULL) && (Content != NULL)) {
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
      char* Sql = MergeString ("SELECT COUNT (Sequence) FROM Page WHERE Name = '%s'", Name);
      Result = sqlite3_prepare(DataBaseLink, Sql, -1, &stmt, NULL);
      FreeString (Sql);

      if (Result == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
          int RowCount = sqlite3_column_int (stmt, 0);

          if (RowCount > 0) {
            Sql = MergeString ("UPDATE Page SET Content = '%s' WHERE Name = '%s'", Content, Name);

            Result = sqlite3_exec(DataBaseLink, Sql, 0, 0, &ErrorMessage);
            FreeString (Sql);
            if (Result == SQLITE_OK) {
              IsSuccess = true;
            }
          }
          else {
            Sql = MergeString ("INSERT INTO Page (Name, Content) VALUES ('%s', '%s')", Name, Content);
            Result = sqlite3_exec(DataBaseLink, Sql, 0, 0, &ErrorMessage);
            FreeString (Sql);
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
      FreeString (Sql);

      if (Result == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
          int RowCount = sqlite3_column_int (stmt, 0);

          if (RowCount > 0) {
            Sql = MergeString ("UPDATE Chapter SET Sid = '%s', Did = '%s', PicRange = '%s', Code = '%s'  WHERE ComicSeq = '%s' AND ChapterIndex = '%s'", Sid, Did, PicRange, Code, ComicSeq, ChapterIndex);
            Result = sqlite3_exec(DataBaseLink, Sql, 0, 0, &ErrorMessage);
            FreeString (Sql);
            if (Result == SQLITE_OK) {
              IsSuccess = true;
            }
          }
          else {
            Sql = MergeString ("INSERT INTO Chapter (ComicSeq, ChapterIndex, Sid, Did, PicRange, Code) VALUES ('%s', '%s', '%s', '%s', '%s', '%s')", ComicSeq, ChapterIndex, Sid, Did, PicRange, Code);
            Result = sqlite3_exec(DataBaseLink, Sql, 0, 0, &ErrorMessage);
            FreeString (Sql);
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

        Comic->Sequence  = MergeString("%s", sqlite3_column_text(stmt, 0));
        Comic->Index     = MergeString("%s", sqlite3_column_text(stmt, 1));
        Comic->Name      = MergeString("%s", sqlite3_column_text(stmt, 2));
        Comic->FreeComic = FreeComic;
        Comic->Next      = NULL;

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
    FreeString (Select);
  }
  sqlite3_close(DataBaseLink);

  return ComicStart;
}

void FreeItem (ITEM* Item) {
  if (Item != NULL) {
    free (Item->ParentSeq);
    free (Item->Name);
    if (Item->Next != NULL)
      FreeItem (Item->Next);
    free (Item);
  }
}
ITEM* GetItemList (DATABASE* DataBase, char* InterfaceSeq) {
  ITEM*         ItemStart = NULL;

  sqlite3_stmt  *stmt;
  int           Result = 0;
  sqlite3*      DataBaseLink;

  if (InterfaceSeq != NULL) {
    Result = sqlite3_open_v2(
               DataBase->DataBasePath,
               &DataBaseLink,
               SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
               NULL
               );

    if (Result == SQLITE_OK) {
      char* Select = MergeString ("SELECT Name FROM Item WHERE InterfaceSeq = '%s' AND Language = '%s'", InterfaceSeq, DataBase->GetVariable (DataBase, "LanguageIndex"));
      Result = sqlite3_prepare(DataBaseLink, Select, -1, &stmt, NULL);
      if (Result == SQLITE_OK) {
        for (ITEM* Item = NULL; sqlite3_step(stmt) == SQLITE_ROW; Item = NULL) {
          Item = (ITEM*) malloc (sizeof (ITEM) * 1);

          Item->ParentSeq      = MergeString (InterfaceSeq);
          Item->Name           = MergeString("%s", sqlite3_column_text(stmt, 0));
          Item->FreeItem       = FreeItem;

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
      FreeString (Select);
    }
    sqlite3_close(DataBaseLink);
  }
  return ItemStart;
}


CHAPTER* GetChapterList (DATABASE* DataBase, char* ParentSeq) {
  CHAPTER*    ChapterStart = NULL;

  sqlite3_stmt  *stmt;
  int           Result = 0;
  sqlite3*      DataBaseLink;

  if (ParentSeq != NULL) {
    Result = sqlite3_open_v2(
               DataBase->DataBasePath,
               &DataBaseLink,
               SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
               NULL
               );

    if (Result == SQLITE_OK) {
      char* Select = MergeString ("SELECT ComicSeq,ChapterIndex,Sid,Did,PicRange,Code FROM Chapter WHERE ComicSeq = '%s'", ParentSeq);
      Result = sqlite3_prepare(DataBaseLink, Select, -1, &stmt, NULL);
      if (Result == SQLITE_OK) {
        for (CHAPTER* Chapter = NULL; sqlite3_step(stmt) == SQLITE_ROW; Chapter = NULL) {
          Chapter = (CHAPTER*) malloc (sizeof (CHAPTER) * 1);

          Chapter->ParentSeq = MergeString("%s", sqlite3_column_text(stmt, 0));
          Chapter->Index     = MergeString("%s", sqlite3_column_text(stmt, 1));
          Chapter->Sid       = MergeString("%s", sqlite3_column_text(stmt, 2));
          Chapter->Did       = MergeString("%s", sqlite3_column_text(stmt, 3));
          Chapter->PicRange  = MergeString("%s", sqlite3_column_text(stmt, 4));
          Chapter->Code      = MergeString("%s", sqlite3_column_text(stmt, 5));

          Chapter->FreeChapter = FreeChapter;

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
      FreeString (Select);
    }
    sqlite3_close(DataBaseLink);
  }
  return ChapterStart;
}

void LocateDataBaseStruct (DATABASE** DataBase, char* DataBasePath) {

  (*DataBase) = (DATABASE*) malloc (sizeof (DATABASE) * 1);
  (*DataBase)->DataBasePath = MergeString (DataBasePath);

  (*DataBase)->CheckDataBase   = CheckDataBase;
  (*DataBase)->GetVariable     = GetVariable;
  (*DataBase)->GetInterface    = GetInterface;
  (*DataBase)->GetComic        = GetComic;
  (*DataBase)->GetPage         = GetPage;
  (*DataBase)->GetChapter      = GetChapter;
  (*DataBase)->GetShowBaseUrl  = GetShowBaseUrl;

  (*DataBase)->GetComicList    = GetComicList;
  (*DataBase)->GetItemList     = GetItemList;
  (*DataBase)->GetChapterList  = GetChapterList;

  (*DataBase)->SetVariable     = SetVariable;
  (*DataBase)->SetPage         = SetPage;
  (*DataBase)->SetChapter      = SetChapter;
  (*DataBase)->SetShowBaseUrl  = SetShowBaseUrl;

  (*DataBase)->FreeDataBase    = FreeDataBase;

  (*DataBase)->CheckDataBase (*DataBase);
}
//--------------------------------------------------------------------------
void ShowMainMain (DATABASE* DataBase, int *Choice) {
  INTERFACE* Interface  = DataBase->GetInterface (DataBase, "MainMenu");
  ITEM*      ItemList   = DataBase->GetItemList (DataBase, Interface->Sequence);
  char*      DoubleLine = DataBase->GetVariable (DataBase, "DoubleLine");
  char*      Line       = DataBase->GetVariable (DataBase, "Line");

  if (Interface != NULL) {
    int  Top = 0;
    int  Left = 0;

    system ("CLS");
    Print (" %=NY %s %=Nw\n", Interface->Title);
    Print ("%s\n", DoubleLine);
    GetCursorPosition (&Left, &Top);

    int Index = 0;
    for (ITEM* Item = ItemList; Item != NULL; Item = Item->Next, Index++)
      Print ("    %=NW %d%=Nw . %s\n", (Index + 1), Item->Name);

    Print ("%=NW %s %=Nw\n", Line);
    Print (" %=NR %s %=Nw\n", Interface->Explain);
    Print ("%s\n", DoubleLine);

    InterfaceCursor ( 2, Top, Interface->ChoiceSymbol, Index, Choice);
    system ("CLS");
  }
  Interface->FreeInterface (Interface);
  ItemList->FreeItem (ItemList);
  FreeString (DoubleLine);
  FreeString (Line);
}


void ShowContinueUpDateMenu (DATABASE* DataBase, int *Choice){
  INTERFACE* Interface  = DataBase->GetInterface (DataBase, "IsContinueUpDate");
  ITEM*      ItemList   = DataBase->GetItemList (DataBase, Interface->Sequence);
  char*      DoubleLine = DataBase->GetVariable (DataBase, "DoubleLine");
  char*      Line       = DataBase->GetVariable (DataBase, "Line");

  if (Interface != NULL) {
    int  Top = 0;
    int  Left = 0;

    system ("CLS");
    Print (" %=NY %s %=Nw\n", Interface->Title);
    Print ("%s\n", DoubleLine);
    Print (" %=NR %s %=Nw\n", Interface->Content);
    Print ("%=NW %s %=Nw\n", Line);
    GetCursorPosition (&Left, &Top);

    int Index = 0;
    for (ITEM* Item = ItemList; Item != NULL; Item = Item->Next, Index++)
      Print ("    %=NW %d%=Nw . %s\n", (Index + 1), Item->Name);

    Print ("%=NW %s %=Nw\n", Line);
    Print (" %=NR %s %=Nw\n", Interface->Explain);
    Print ("%s\n", DoubleLine);

    InterfaceCursor ( 2, Top, Interface->ChoiceSymbol, Index, Choice);
    system ("CLS");
  }
  Interface->FreeInterface (Interface);
  ItemList->FreeItem (ItemList);
  FreeString (DoubleLine);
  FreeString (Line);
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
            FreeString(Key);

            free (Url);
          }
          free (StartKey);
        }
        free (Article);
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

        printf ("====> Save Code to DataBase!\n");
        DataBase->SetChapter (DataBase, ComicSeq, ChapterIndex, Sid, Did, PicRange, Code);
        printf ("====> Save Success!\n");

      }
    }
  }
}
void Update (DATABASE* DataBase) {
  int Choice = 1;
  ShowContinueUpDateMenu (DataBase, &Choice);
  if (Choice == 1) {
    char* WgetFile    = DataBase->GetVariable (DataBase, "File_Wget");
    char* WebBaseUrl      = DataBase->GetVariable (DataBase, "Url_WebBaseUrl");
    char* TempFile        = DataBase->GetVariable (DataBase, "Folder_TempFile");
    printf ("==> Acess Web..\n");
    RunCommand (WgetFile, WebBaseUrl, TempFile);
    FreeString (WebBaseUrl);


    if (GetFileSize(TempFile) > 0) {
      printf ("==> Acess Success!\n");
      char* ComicViewUrl = DataBase->GetVariable (DataBase, "Url_ComicView");


      printf ("==> Get ComicView!\n");
      RunCommand (WgetFile, ComicViewUrl, TempFile);
      FreeString (ComicViewUrl);
      printf ("==> Get ComicView Success!\n");


      printf ("==> Transform ComicView!\n");
      TransformJs (DataBase, TempFile);
      printf ("==> Transform Success!\n");

      char*  HtmlBaseUrl = DataBase->GetVariable (DataBase, "Url_HtmlBaseUrl");
      COMIC* ComicList = DataBase->GetComicList (DataBase);

      FILE *FilePoint = NULL;
      char* AllComicCodeFile = DataBase->GetVariable (DataBase, "File_AllComicCode");
      ClearFile(AllComicCodeFile);

      if ((FilePoint = fopen (AllComicCodeFile, "a")) != NULL) {

        printf ("==> Write AllComicCodeFile Header!\n");
        putc (0xEF, FilePoint);
        putc (0xBB, FilePoint);
        putc (0xBF, FilePoint);

        fprintf (FilePoint, "%s\n", ANSIToUTF8 ("//"));
        fprintf (FilePoint, "%s\n", ANSIToUTF8 ("// 漫畫代碼,名稱,章節代碼"));
        fprintf (FilePoint, "%s\n", ANSIToUTF8 ("//"));
        fprintf (FilePoint, "%s\n", ANSIToUTF8 ("var gComicList = new Array ("));
        printf ("==> Write Header Success!\n");

        for (COMIC* Comic = ComicList; Comic != NULL; Comic = Comic->Next) {
          system ("cls");


          printf ("==> Get Html!\n");
          char* HtmlUrl = MergeString ("%s%s.html",HtmlBaseUrl, Comic->Index);
          RunCommand (WgetFile, HtmlUrl, TempFile);
          FreeString (HtmlUrl);
          printf ("==> Get Html Success!\n");


          printf ("==> Get ShowKey!\n");
          char* ShowKeySatrt = MergeString ("cview('%s-1.html',", Comic->Index);
          char* ShowKey = NULL;
          ShowKey = GetKeySearchValue (ShowKeySatrt,");",TempFile,true);
          FreeString (ShowKeySatrt);
          printf ("==> Get ShowKey Success!\n");


          printf ("==> Get ShowBaseUrl!\n");
          char* ShowBaseUrl = DataBase->GetShowBaseUrl (DataBase, ShowKey);
          FreeString (ShowKey);
          printf ("==> Get ShowBaseUrl Success!\n");

          printf ("==> Get Show!\n");
          char* ShowUrl = MergeString ("%s%s.html",ShowBaseUrl, Comic->Index);
          FreeString (ShowBaseUrl);
          RunCommand (WgetFile, ShowUrl, TempFile);
          FreeString (ShowUrl);
          printf ("==> Get Show Success!\n");


          printf ("==> Get Codes!\n");
          char* Codes = GetKeySearchValue ("codes=\"","\".split('|')",TempFile,false);
          printf ("==> Get Codes Success!\n");


          printf ("==> DeCodes!\n");
          DeCodes (DataBase, Codes, Comic->Sequence);
          printf ("==> DeCodes Success!\n");

          printf ("==> Write To AllComicCodeFile!\n");
          fprintf (FilePoint, "  new Array (%s, \"%s\", \"%s\")", Comic->Index, ANSIToUTF8 (Comic->Name), Codes);
          if (Comic->Next != NULL)
            fprintf (FilePoint, ",");
          fprintf (FilePoint, "\n");
          printf ("==> Write Success!\n");

          FreeString (Codes);
          ClearFile (TempFile);
        }
        fprintf (FilePoint, "%s\n", ANSIToUTF8 (");"));
        fclose (FilePoint);
      }
      FreeString (AllComicCodeFile);
      ComicList->FreeComic (ComicList);
      FreeString (HtmlBaseUrl);
    }
    else {
      printf ("==> Acess Failure!\n");
      // access error

    }
    FreeString (TempFile);
    FreeString (WgetFile);
  }
}

int main (int argc, char* argv[]) {


  char DataBasePath[] = ".\\\\DataBase\\\\DataBase.db3";

  //printf ("%d\n",time(NULL));

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
          //Browser(AllSet);
          break;

        case 3:
          //Doenload(AllSet);
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
/**/
  DataBase->FreeDataBase (DataBase);
  return 0;
}

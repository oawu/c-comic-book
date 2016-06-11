#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "windows.h"


//------------------------------------------------------------------------------ Function --
int GetIntLength (int Num) {
  int Index = 1;
  for (Index = 1; (Num = Num/10)>0 ; Index++);
  return Index;
}
void MergeString (char** OutString, char* Format, ...) {
  int      Length = 0;
  char*    FormatPoint = Format;
  va_list  args;
  
  va_start (args, *Format);
  for (;*FormatPoint!='\0';*FormatPoint++)
    if (*FormatPoint =='%') {
      switch (*++FormatPoint){
        case 's':
          Length = Length + strlen (va_arg(args, char *));
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
  
  (*OutString) = NULL;
  (*OutString) = (char*) malloc (sizeof (char) * (Length + 1));
  strcpy(*OutString, "");
  
  va_start(args, *Format);
  for (int NowLength = 0; *Format!='\0'; *Format++)
    if (*Format =='%') {
      switch (*++Format){
        case 's':
          sprintf (*OutString, "%s%s", *OutString, va_arg(args, char *));
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
void GetFileSize (int *Size, char FileName[]) {
  FILE *FilePoint;
  if ((FilePoint = fopen (FileName, "r")) == NULL) {
    *Size = 0;
  }
  else {
    fseek(FilePoint, 0, SEEK_END);
    *Size = ftell(FilePoint);
    fseek(FilePoint, 0, SEEK_SET);
    fclose(FilePoint);
  }
}
void GetSearchValue (char* FilePath, char* StartKey, char* EndKey,char** Value, bool IsIgnoreSpace) {
  //*Value = (char*) malloc (5);
  FILE   *FilePoint;
  int    FileSize = 0;
  char   *Article = NULL;
  
  int    Index = 0;
  int    KeyIndex = 0;
  int    Start = -1;
  int    End = -1;
  int    Length = 0;
  
  *Value = NULL;
  GetFileSize (&FileSize, FilePath);
        
  if (FileSize > 0) {
    if ((FilePoint = fopen (FilePath, "r")) != NULL) {
      Article = (char*) malloc(sizeof (char) * (FileSize + 1));
      for (Index = 0; (Article[Index]=getc(FilePoint)) != EOF; Index++)
        if ((IsIgnoreSpace) && (Article[Index] == ' '))
          Index--;
      Article[Index] = '\0';
      fclose(FilePoint);
      
      for (Index = 0, Start = -1, End = -1, KeyIndex = 0; Index < strlen (Article); Index++){ 
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
        Length = End - Start;
        *Value = (char*) malloc (sizeof (char) * (Length + 1));
        
        for (Index = Start, KeyIndex = 0; Index < End; Index++)
          (*Value)[KeyIndex++] = Article[Index];
        (*Value)[KeyIndex] = '\0';
        
      }
      free (Article);
    }
  }
}
//------------------------------------------------------------------------------ Struct --


typedef struct   _ALL_SET_       ALL_SET;
typedef struct   _VAR_FILE_      VAR_FILE;
typedef struct   _FILE_PATH_     FILE_PATH;
typedef struct   _FOLDER_PATH_   FOLDER_PATH;
typedef struct   _FILE_FORMAT_   FILE_FORMAT;

typedef void (*RW_FUNC_MODULE) (ALL_SET* This, char* FileName, char* Key, char** Value);


typedef struct _FILE_FORMAT_ {
  char*    Codes;
  char*    html;
  char*    show;
} FILE_FORMAT;


typedef struct _FOLDER_PATH_ {
  char*    Command;
  char*    Css;
  char*    Download;
  char*    Icon;
  char*    JavaScrip;
  char*    PageCode;
  char*    SetVar;
  char*    SourceCode;
} FOLDER_PATH;

typedef struct _FILE_PATH_ {
  char*    Wget;
  char*    AllVar;
  char*    ComicList;
  char*    ProgramSet;
  char*    ComicView;
  char*    TestAccessIndex;
  char*    JsTransform;
  char*    UpdateSuccess;
  char*    BrowserIndex;
} FILE_PATH;

typedef struct _VAR_FILE_ {
  RW_FUNC_MODULE    Read;
  RW_FUNC_MODULE    Write;
} VAR_FILE;

typedef struct _ALL_SET_ {
  char*            SetPath;
  VAR_FILE         VarFile;
  FILE_PATH        FilePath;
  FOLDER_PATH      FolderPath;
  FILE_FORMAT      FileFormat;
} ALL_SET;


void ReadVarFunc (ALL_SET* This, char* FileName, char* Key, char** Value){
  char* FilePath = NULL;
  MergeString (&FilePath, "%s%s", This->SetPath, FileName);
  
  char* StartKey = NULL;
  MergeString (&StartKey, "[%s|", Key);
  
  char* EndKey = NULL;
  MergeString (&EndKey, "]");
  
  GetSearchValue (FilePath, StartKey, EndKey, Value, true);
  
  free (EndKey);
  free (StartKey);
  free (FilePath);
}
void WriteVarFunc (ALL_SET* This, char* FileName, char* Key, char** Value){
}
void LocateStruct (ALL_SET** AllSet, char _Set_Path[]) {
  //Locate Block 0
  (*AllSet) = (ALL_SET*) malloc (sizeof (ALL_SET) * 1);
  
  //Locate Block 1
  (*AllSet)->SetPath = (char*) malloc (sizeof (char) * strlen (_Set_Path) + 1);
  strcpy ((*AllSet)->SetPath, _Set_Path);
  
  //Locate Block 2
  (*AllSet)->VarFile.Read = ReadVarFunc;
  (*AllSet)->VarFile.Write = WriteVarFunc;
  
  //Locate Block 3
  (*AllSet)->VarFile.Read(*AllSet, "FilePath.txt", "Wget",            &(*AllSet)->FilePath.Wget);
  
  (*AllSet)->VarFile.Read(*AllSet, "FilePath.txt", "AllVar",          &(*AllSet)->FilePath.AllVar);
  (*AllSet)->VarFile.Read(*AllSet, "FilePath.txt", "ComicList",       &(*AllSet)->FilePath.ComicList);
  (*AllSet)->VarFile.Read(*AllSet, "FilePath.txt", "ProgramSet",      &(*AllSet)->FilePath.ProgramSet);
  (*AllSet)->VarFile.Read(*AllSet, "FilePath.txt", "ComicView",       &(*AllSet)->FilePath.ComicView);
  (*AllSet)->VarFile.Read(*AllSet, "FilePath.txt", "TestAccessIndex", &(*AllSet)->FilePath.TestAccessIndex);
  (*AllSet)->VarFile.Read(*AllSet, "FilePath.txt", "JsTransform",     &(*AllSet)->FilePath.JsTransform);
  (*AllSet)->VarFile.Read(*AllSet, "FilePath.txt", "UpdateSuccess",   &(*AllSet)->FilePath.UpdateSuccess);
  (*AllSet)->VarFile.Read(*AllSet, "FilePath.txt", "BrowserIndex",    &(*AllSet)->FilePath.BrowserIndex);
  
  //(*AllSet)->FolderPath.SetVar = (char*) malloc (sizeof (char) * 5);
  //strcpy((*AllSet)->FolderPath.SetVar, "asdf");
}
void FreeStruct (ALL_SET* AllSet) {
  //free (AllSet->FolderPath.SetVar);
  
  
  free (AllSet->FilePath.BrowserIndex);
  free (AllSet->FilePath.UpdateSuccess);
  free (AllSet->FilePath.JsTransform);
  free (AllSet->FilePath.TestAccessIndex);
  free (AllSet->FilePath.ComicView);
  free (AllSet->FilePath.ProgramSet);
  free (AllSet->FilePath.ComicList);
  free (AllSet->FilePath.AllVar);
  free (AllSet->FilePath.Wget);
  free (AllSet->SetPath);
  
  free (AllSet);
}
int main ()
{
  char Path_ProgramSet[] = "..\\SetVar\\";
  
  ALL_SET* AllSet = NULL;
  
  
  //Locate
  
  
  LocateStruct(&AllSet, Path_ProgramSet);
  

  FreeStruct (AllSet);
  
  
  
  
  system ("PAUSE");
}

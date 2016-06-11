
#include <conio.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

char   gWgetFilePath[]                = ".\\Command\\wget.exe";
char   g8ComicBaseUrl[]               = "http://www.8comic.com/";
char   gTempFilePath[]                = ".\\TempFile\\TempFile.var";

int GetIntLength (int Num) {
  int Index = 1;
  for (Index = 1; (Num = Num / 10) > 0 ; Index++);
  return Index;
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

void ClearFile (char* FileName){
  FILE* FilePoint = NULL;
  if ((FilePoint = fopen (FileName, "w")) != NULL) {
    fprintf (FilePoint, "");
    fclose (FilePoint);
  }
}

void RunWgetCommand (char* WgetFile, char* Url, char* FileName, bool IsSsl){
  Print ("%=Nr  ======> Start Run Command To Get Data..%=Nw\n");

  ClearFile(FileName);

  char* Command = NULL;
  if (IsSsl)
    Command = MergeString ("%s %s -O %s --no-check-certificate", WgetFile, Url, FileName);
  else
    Command = MergeString ("%s %s -O %s", WgetFile, Url, FileName);


  STARTUPINFO           t_StartupInfo       = {0};
  PROCESS_INFORMATION   t_ProcessInformation = {0};

  t_StartupInfo.cb = sizeof(t_StartupInfo);
  bool bSuccess = CreateProcess (NULL, Command, NULL, NULL, TRUE, NORMAL_PRIORITY_CLASS, NULL, NULL, &t_StartupInfo, &t_ProcessInformation);

  if (bSuccess){
    DWORD dwRect = WaitForSingleObject(t_ProcessInformation.hProcess, 1 * 1000);
    switch (dwRect){
      case WAIT_OBJECT_0:
        Print ("%=Nr  ======> Run Command And Get Data Success!%=Nw\n");
        break;

      case WAIT_TIMEOUT:
        TerminateProcess(t_ProcessInformation.hProcess, 0);
        Print ("%=Ny  ======> Run Command And Get Data Failure!%=Nw\n");
        break;
    }
  }

  free (Command);
}

int main() {



    RunWgetCommand (gWgetFilePath, g8ComicBaseUrl, gTempFilePath, false);




    system("pause");
}

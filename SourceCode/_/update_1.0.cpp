#include "stdio.h"
#include "stdlib.h"
#include "String.h"
#include "windows.h"
#include "conio.h"
#include "OA_Color.h"
#include "OA_Print.h"
#include "OA_Cursor.h"

#define PAUSE getch();




void ShowMainMain (int *Choice) {
  int  InputLegth = 1;
  int  ItemCount = 6;
  int  ExitItem = 5;
  int  Top = 0;
  int  Left = 0;
  
  char *Item[6] = {"更新漫畫檔    --  Update",
                   "瀏覽漫畫      --  Browser",
                   "下載漫畫      --  Download",
                   "清除暫存檔案  --  Clear",
                   "下載資料夾    --  Download Folder",
                   "離開本程式    --  Exit"
                      };
                      
  Print (" %=NY Main Menu %=Nw\n");
  Print ("====================================================\n");
  
  GetCursorPosition (&Left, &Top);
  
  
  for (int  Index = 0; Index < ItemCount; Index++) {
    Print ("    %=NW %d%=Nw . %s\n", Index, Item[Index]);
  }
  Print ("%=NW ---------------------------------------------------- %=Nw\n");
  Print (" %=NR Please Press ←↑→↓ Choice! %=Nw\n");
  Print ("====================================================\n");
  
  MenuCursor ( 2, Top, ">", ItemCount, Choice);
  system ("CLS");
}

int main(int argc, char * argv[]){
  
  char FilePath_ProgramSet[] = "..\\SetVar\\ProgramSet.txt";
  bool IsUpdated = false;
  int MainMainChoice = 1;
  
  while (true) {
    ShowMainMain (&MainMainChoice);
    printf ("%d\n",MainMainChoice);
    PAUSE
    
    switch (MainMainChoice) {
      case 1:
      //  Update (FilePath_ProgramSet);
        break;
        
      case 2:
     //     RunIndexHtml (FilePath_ProgramSet);
        break;
        
      case 3:
     //     DownloadComic (FilePath_ProgramSet);
        break;
        
      case 4:
     //   DeleteTemporary (FilePath_ProgramSet);
        break;
        
      case 5:
     //   DownloadFolder (FilePath_ProgramSet);
        break;
        
      case 6:
        return 0;
        break;
    } 
    system ("CLS");
  } /* */
}

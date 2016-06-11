
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
void MenuCursor (int Left, int Top, char *Key, int ItemCount, int *Choice)
/*++
  進入回傳 第幾項目
  離開     回傳 0
  第一項目 回傳 1 
--*/
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

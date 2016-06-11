
void Print (char* StringFormat, ...)
/*++
  nbgcrpyw
  N,n => 無色     b => 藍     g => 綠     c => 青
  r => 紅         p => 紫     y => 黃     w => 灰
  
  大寫為亮, %=後的第一個字母為背景 第二個字母為文字
  第三個為空格吃掉，不為空格印出,空一格方便於判斷 
  
  ex :
    Print ("%=rG Hi,How a%XNw re You?");
    or
    Print ("%=rGHi,How a%XNwre You?");
--*/
{
   char *ptr=StringFormat;
   va_list args;
 
   va_start(args, StringFormat);
   
   for (;*ptr!='\0';*ptr++) {
     switch (*ptr){
       case '%':
         switch (*++ptr){
           case 's':
             printf ("%s", va_arg(args, char *));
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

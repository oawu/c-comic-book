
bool CheckIsAllow (char   cmpchar, char   AllowCharList[], int    AllowCharListCount) {
  int    Index = 0;
  bool   IsInList = false;
  
  for (Index = 0; Index < AllowCharListCount ; Index++) {
    if(AllowCharList[Index] == cmpchar){
      IsInList = true;
    }
  }
  return IsInList;
}

void GetInput (int MaxLength, char AllowCharList[], int AllowCharListCount, char InputData[]) {
  int    StringCount                  = 0;
  int    Key                          = 0;
  char   *String                      = NULL;
  bool   IsAllowChar                  = false;
  
  String = (char *) malloc (sizeof (char) * (MaxLength + 1));
  
  for (StringCount = 0; StringCount < MaxLength + 1 ; StringCount++) {
    Key = getch ();
    IsAllowChar = CheckIsAllow (Key, AllowCharList, AllowCharListCount);
    
    if (IsAllowChar && (StringCount < MaxLength)) {
      printf ("%c", Key);
      String[StringCount] = Key;
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
        String[StringCount] = '\0';
        break;
      }
    }
    else {
      StringCount--;
    }
  }
  strcpy (InputData, String);
  free (String);
}


#define NONE	      0x00000000

#define BLUE          0x00000001
#define GREEN         0x00000002
#define RED           0x00000004
#define INTENSITY     0x00000008
#define CYAN          BLUE  | GREEN
#define PURPLE        BLUE  | RED
#define YELLOW        GREEN | RED
#define GRAY          BLUE  | GREEN | RED

#define LIGHT_BLUE    BLUE   | INTENSITY
#define LIGHT_GREEN   GREEN  | INTENSITY
#define LIGHT_RED     RED    | INTENSITY
#define LIGHT_CYAN    CYAN   | INTENSITY
#define LIGHT_PURPLE  PURPLE | INTENSITY
#define LIGHT_YELLOW  YELLOW | INTENSITY
#define LIGHT_GRAY    GRAY   | INTENSITY

//--------------------------------------------------

#define B_BLUE          0x00000010
#define B_GREEN         0x00000020
#define B_RED           0x00000040
#define B_INTENSITY     0x00000080

#define B_CYAN          B_GREEN  | B_BLUE
#define B_PURPLE        B_RED    | B_BLUE
#define B_YELLOW        B_RED    | B_GREEN
#define B_GRAY          B_BLUE   | B_GREEN  | B_RED

#define B_LIGHT_BLUE    B_BLUE   | B_INTENSITY
#define B_LIGHT_GREEN   B_GREEN  | B_INTENSITY
#define B_LIGHT_RED     B_RED    | B_INTENSITY
#define B_LIGHT_CYAN    B_CYAN   | B_INTENSITY
#define B_LIGHT_PURPLE  B_PURPLE | B_INTENSITY
#define B_LIGHT_YELLOW  B_YELLOW | B_INTENSITY
#define B_LIGHT_GRAY    B_GRAY   | B_INTENSITY

#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
#include <SPI.h>
#include <Adafruit_VS1053.h>
#include <SD.h>

MCUFRIEND_kbv tft;

// Color definitions
#define BLACK   0x0000
#define WHITE   0xFFFF
#define RED     0xF800
#define GREEN   0x07E0
#define BLUE    0x001F
#define GOLD    0xEDE1
#define YELLOW  0xFFE0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define BACKGROUND 0x1067
#define LANE_COLOR  0xFFFF // Dark gray for lanes

// Game states
enum GameState { MAIN_MENU, START_SCREEN, PLAYING, END_SCREEN };
GameState currentState = MAIN_MENU;

// Lane X positions
int laneX[] = {25, 81, 137, 194};
int laneWidth = 42;

// Block spawn positions and lanes
int song = 0;
int notes = 0;
unsigned long current = 0;
unsigned long songStartTime = 0;
const unsigned long songDurations[3] = {98100, 135140,164260}; // Durations of your two songs in ms
bool gameActive = false;

const unsigned long pos[3][517][2] PROGMEM = {
  {{3000,0},  {3970,2},  {4450,3},  {5530,0},  {5800,1},   {6090,2}, {7160,1},  {7710,0},  {9390,3},  {10470,3},
  {10980,0}, {12070,0}, {12600,0}, {13650,1}, {13930,2}, {14290,3}, {15960,3}, {17130,1}, {17620,0}, {18760,3},
  {19080,2}, {19450,1}, {20390,2}, {20920,3}, {22500,0}, {23650,1}, {24140,2}, {25300,3}, {25800,3}, {29390,0},
  {30600,2}, {30890,3}, {31150,3}, {31460,2}, {31750,0}, {32640,3}, {33890,0}, {34180,1}, {34510,1}, {34780,0},
  {35050,3}, {35940,1}, {37130,3}, {37400,0}, {37690,3}, {37960,0}, {38240,1}, {38710,2}, {39210,3}, {44170,3},
  {45140,1}, {45620,0}, {46600,3}, {46870,2}, {47160,1}, {48230,2}, {48780,3}, {50460,0}, {51540,0}, {52050,3},
  {53140,3}, {53670,3}, {54720,2}, {55000,1}, {55360,0}, {57030,0}, {58200,2}, {58690,3}, {59830,0}, {60150,1},
  {60520,2}, {61460,1}, {61990,0}, {63570,3}, {64720,2}, {65210,1}, {66370,0}, {66870,0}, {70460,3}, {71670,1},
  {71960,0}, {72220,0}, {72530,1}, {72820,3}, {73710,0}, {74960,3}, {75250,2}, {75580,2}, {75850,3}, {76120,0},
  {77010,2}, {78200,0}, {78470,3}, {78760,0}, {79030,3}, {79310,2}, {79780,1}, {80280,0}, {85240,0}, {86210,2},
  {86690,3}, {87670,0}, {87940,1}, {88230,2}, {89300,1}, {89850,0}, {91530,3}, {92610,3}, {93120,0}, {94210,0},
  {94740,0}, {95790,1}, {96070,2}, {96430,3}, {98100,3}
},
  
  {{2400, 1}, {5500, 2}, {6000, 2}, {9550, 0}, {9990, 0}, {10200, 1}, {10560, 2}, {10860, 3}, {11200, 0},
  {11550, 3}, {12570, 2}, {13070, 3}, {13320, 2}, {14170, 0}, {14480, 1}, {14880, 0}, {15300, 1}, {15870, 3},
  {16690, 1}, {17050, 2}, {17600, 0}, {17930, 3}, {18230, 0}, {18560, 3}, {18860, 0}, {19900, 2}, {20350, 0},
  {20650, 1}, {20850, 2}, {21650, 0}, {21850, 2}, {22450, 1}, {22890, 2}, {23650, 1}, {24250, 3}, {24650, 2}, 
  {25150, 0}, {25650, 2}, {25950, 1}, {26250, 0}, {26550, 0}, {27680, 1}, {28150, 2}, {28620, 1}, {29150, 3},
  {29300, 3}, {29600, 2}, {29880, 1}, {30190, 0}, {31480, 2}, {31980, 2}, {32580, 1}, {33070, 0}, {33300, 0}, 
  {33560, 3}, {33770, 1}, {34870, 2}, {35370, 2}, {35760, 0}, {36270, 2}, {36770, 3}, {37190, 1}, {37600, 2}, 
  {38660, 0}, {38950, 0}, {39250, 2}, {40340, 3}, {40720, 3}, {41080, 1}, {42160, 0}, {42600, 3}, {43000, 2}, 
  {43260, 1}, {43560, 3}, {43930, 0}, {44170, 2}, {44530, 1}, {44860, 2}, {45540, 0}, {46450, 0}, {46940, 1},
  {47400, 2}, {47860, 3}, {48830, 1}, {49230, 1}, {49680, 2}, {49990, 2}, {50280, 3}, {51390, 1}, {51810, 1}, 
  {52090, 0}, {53330, 3}, {53650, 0}, {54110, 1}, {54660, 2}, {54950, 3}, {55560, 0}, {55880, 1}, {56520, 2}, 
  {57500, 1}, {57980, 2}, {58450, 0}, {58910, 1}, {59400, 3}, {59910, 2}, {60330, 1}, {60740, 2}, {62170, 3}, 
  {62450, 0}, {62660, 0}, {66260, 1}, {66540, 2}, {70380, 0}, {70610, 0}, {71020, 1}, {71500, 1}, {71910, 2}, 
  {72230, 2}, {72570, 3}, {73630, 0}, {73970, 0}, {74270, 0}, {74850, 1}, {75230, 2}, {75560, 3}, {75860, 3}, 
  {76230, 3}, {77660, 0}, {77930, 3}, {78350, 1}, {78830, 3}, {79310, 2}, {79850, 1}, {80940, 3}, {81150, 0}, 
  {81450, 1}, {81800, 2}, {82020, 0}, {82840, 2}, {83150, 3}, {83450, 1}, {83780, 1}, {85130, 2}, {85630, 0}, 
  {86110, 3}, {86570, 1}, {86860, 0}, {87050, 3}, {87270, 1}, {88700, 0}, {89160, 0}, {89440, 1}, {89830, 0}, 
  {90340, 0}, {90760, 1}, {92520, 2}, {92730, 1}, {92940, 0}, {93230, 3}, {93540, 2}, {94010, 3}, {94480, 1}, 
  {94690, 0}, {96230, 2}, {96770, 2}, {97170, 0}, {97570, 1}, {98060, 2}, {98460, 3}, {99560, 0}, {99880, 1}, 
  {100120, 2}, {101440, 3}, {101710, 2}, {102070, 1}, {103060, 2}, {103440, 0}, {103810, 1}, {104120, 3}, {104410, 3}, 
  {104730, 2}, {105030, 1}, {105370, 2}, {105720, 3}, {106350, 3}, {107410, 0}, {107800, 0}, {108270, 1}, {108730, 2}, 
  {109660, 3}, {110080, 3}, {110560, 2}, {110890, 1}, {111270, 3}, {112770, 1}, {113110, 2}, {113510, 1}, {114210, 3}, 
  {114590, 3}, {114930, 0}, {115190, 1}, {115530, 0}, {115870, 2}, {116170, 3}, {116500, 2}, {116770, 3}, {117500, 1}, 
  {118480, 1}, {118930, 2}, {119320, 0}, {119820, 2}, {120290, 3}, {120690, 2}, {121190, 0}, {121640, 1}, {123080, 3}, 
  {123360, 1}, {123620, 2}, {126970, 1}, {127180, 2}, {127460, 3}, {131590, 2}, {131920, 1}, {132370, 0}, {132830, 2}, 
  {134210, 2}, {134610, 3}, {135140, 1}}, 

  {
{1400,0}, {1940,1}, {2474,2}, {3010,3}, {3545,0}, {4260,1}, {4795,3}, {5331,0}, {5867,1},
{6402,2}, {7117,0}, {7474,1}, {7652,2}, {8010,3}, {8188,0}, {8367,1}, {8724,2}, {9081,3},
{9260,0}, {9438,1}, {9795,2}, {9974,3}, {10331,0}, {10510,1}, {10688,2}, {10867,3}, {11224,0}, {11581,1},
{11760,2}, {11938,3}, {12117,0}, {12295,1}, {12474,2}, {12831,3}, {13188,0}, {13367,1}, {13545,2}, {13724,3},
{13902,0}, {14260,1}, {14438,2}, {14617,3}, {14974,0}, {15152,1}, {15331,2}, {15688,3}, {16045,0}, {16224,1},
{16402,2}, {16760,3}, {17117,0}, {17295,1}, {17474,2}, {17831,3}, {18010,0}, {18188,1}, {18367,2}, {18545,3},
{18902,0}, {19260,1}, {19438,2}, {19617,3}, {19974,0}, {20152,1}, {20331,2}, {20688,3}, {20867,0}, {21045,1},
{21402,2}, {21760,3}, {21938,0}, {22117,1}, {22474,2}, {22831,3}, {23010,0}, {23188,1}, {23545,2}, {23724,3},
{23902,0}, {24260,1}, {24617,2}, {24974,3}, {25152,0}, {25331,1}, {25688,2}, {25867,3}, {26045,0}, {26402,1},
{26581,2}, {26760,3}, {26938,0}, {27117,1}, {27474,2}, {27831,3}, {28188,0}, {28545,1}, {28724,2}, {28902,3},
{29081,0}, {29260,1}, {29617,2}, {29974,3}, {30152,0}, {30331,1}, {30688,2}, {30867,3}, {31045,0}, {31402,1},
{31581,2}, {31760,3}, {32117,0}, {32295,1}, {32474,2}, {32831,3}, {33010,0}, {33188,1}, {33367,2}, {33545,3},
{33724,0}, {33902,1}, {34081,2}, {34617,3}, {34795,0}, {34974,1}, {35242,2}, {35510,3}, {35688,0}, {36045,1},
{36224,2}, {36581,3}, {36760,0}, {36938,1}, {37295,2}, {37474,3}, {37831,0}, {38099,1}, {38367,2}, {38545,3},
{38902,0}, {39081,1}, {39438,2}, {39617,3}, {39795,0}, {40152,1}, {40331,2}, {40510,3}, {40688,0}, {41045,1},
{41224,2}, {41581,3}, {41938,0}, {42117,1}, {42295,2}, {42474,3}, {42652,0}, {43010,1}, {43367,2}, {43545,3},
{43902,0}, {44081,1}, {44260,2}, {44617,3}, {44974,0}, {45152,1}, {45331,2}, {45510,3}, {45867,0}, {46224,1},
{46402,2}, {46670,3}, {46938,0}, {47117,1}, {47474,2}, {47652,3}, {48010,0}, {48188,1}, {48367,2}, {48724,3},
{48902,0}, {49260,1}, {49438,2}, {49617,3}, {49795,0}, {49974,1}, {50331,2}, {50510,3}, {50867,0}, {51045,1},
{51224,2}, {51581,3}, {51938,0}, {52295,1}, {52474,2}, {52831,3}, {53188,0}, {53367,1}, {53545,2}, {53724,3},
{54081,0}, {54438,1}, {54617,2}, {54795,3}, {54974,0}, {55331,1}, {55688,2}, {56045,3}, {56224,0}, {56402,1},
{56760,2}, {57117,3}, {57474,0}, {57652,1}, {57831,2}, {58188,3}, {58367,0}, {58545,1}, {61402,2}, {69795,3},
{69974,0}, {70152,1}, {70510,2}, {70688,3}, {71045,0}, {71224,1}, {71402,2}, {71581,3}, {71938,0}, {72117,1},
{72474,2}, {72652,3}, {72831,0}, {73010,1}, {73367,2}, {73545,3}, {73902,0}, {74260,1}, {74438,2}, {74795,3},
{74974,0}, {75331,1}, {75510,2}, {75688,3}, {75867,0}, {76045,1}, {76402,2}, {76581,3}, {76760,0}, {77117,1},
{77295,2}, {77474,3}, {77831,0}, {78010,1}, {78188,2}, {78545,3}, {78902,0}, {79260,1}, {79438,2}, {79617,3},
{79795,0}, {80152,1}, {80331,2}, {80510,3}, {80867,0}, {81045,1}, {81402,2}, {81760,3}, {81931,0}, {82117,1},
{82295,2}, {82474,3}, {82831,0}, {83188,1}, {83545,2}, {83902,3}, {84081,0}, {84260,1}, {84617,2}, {84795,3},
{84974,0}, {85152,1}, {85331,2}, {85688,3}, {86045,0}, {86402,1}, {86760,2}, {86938,3}, {87117,0}, {87474,1},
{87652,2}, {87831,3}, {88188,0}, {88545,1}, {88813,2}, {89081,3}, {89260,0}, {89617,1}, {89974,2}, {90152,3},
{90331,0}, {90510,1}, {90688,2}, {91045,3}, {91224,0}, {91581,1}, {91760,2}, {91938,3}, {92117,0}, {92385,1},
{92652,2}, {92831,3}, {93188,0}, {93367,1}, {93724,2}, {93902,3}, {94081,0}, {94438,1}, {94617,2}, {94974,3},
{95242,0}, {95510,1}, {95688,2}, {96045,3}, {96224,0}, {96581,1}, {96760,2}, {96938,3}, {97295,0}, {97474,1},
{97652,2}, {97831,3}, {98188,0}, {98367,1}, {98724,2}, {99081,3}, {99260,0}, {99438,1}, {99617,2}, {99795,3},
{100152,0}, {100510,1}, {100688,2}, {101045,3}, {101224,0}, {101402,1}, {101760,2}, {102117,3}, {102295,0}, {102474,1},
{102652,2}, {103010,3}, {103367,0}, {103545,1}, {103813,2}, {104081,3}, {104260,0}, {104617,1}, {104795,2}, {105152,3},
{105331,0}, {105510,1}, {105867,2}, {106045,3}, {106402,1}, {106581,2}, {106760,3}, {106938,0}, {107117,1},
{107474,2}, {107652,3}, {108010,0}, {108188,1}, {108367,2}, {108724,3}, {109081,0}, {109438,1}, {109617,2}, {109974,3},
{110331,0}, {110510,1}, {110688,2}, {110867,3}, {111224,0}, {111581,1}, {111760,2}, {111938,3}, {112117,0}, {112474,1},
{112831,2}, {113188,3}, {113367,0}, {113545,1}, {113902,2}, {114260,3}, {114617,0}, {114795,1}, {114974,2}, {115152,3},
{115331,0}, {115688,1}, {127117,2}, {127831,3}, {128188,0}, {128902,1}, {129260,2}, {129617,3}, {129795,0}, {129974,1},
{130688,2}, {131224,3}, {131402,0}, {131760,1}, {132117,2}, {132474,3}, {132831,0}, {133902,1}, {134081,2}, {134260,3},
{135331,0}, {135510,1}, {135688,2}, {136760,3}, {137117,0}, {137474,1}, {137831,2}, {138099,3}, {138367,0}, {138545,1},
{138902,2}, {139081,3}, {139438,0}, {139617,1}, {139795,2}, {140152,3}, {140331,0}, {140188,1}, {140956,2}, {141224,3},
{141402,0}, {141760,1}, {141938,2}, {142295,3}, {142474,0}, {142652,1}, {143010,2}, {143188,3}, {143367,0}, {143545,1},
{143902,2}, {144081,3}, {144438,0}, {144795,1}, {144974,2}, {145152,3}, {145331,0}, {145510,1}, {145867,2}, {146224,3},
{146402,0}, {146760,1}, {146938,2}, {147117,3}, {147474,0}, {147831,1}, {148010,2}, {148188,3}, {148367,0}, {148724,1},
{149081,2}, {149260,3}, {149527,0}, {149795,1}, {149974,2}, {150331,3}, {150510,0}, {150867,1}, {151045,2}, {151224,3},
{151581,0}, {151760,1}, {152117,2}, {152295,3}, {152474,0}, {152652,1}, {152831,2}, {153188,3}, {153367,0}, {153724,1},
{153902,2}, {154081,3}, {154438,0}, {154795,1}, {155152,2}, {155331,3}, {155688,0}, {156045,1}, {156224,2}, {156402,3},
{156581,0}, {156938,1}, {157295,2}, {157474,3}, {157652,0}, {157831,1}, {158188,2}, {158545,3}, {158902,0}, {159081,1},
{159260,2}, {159617,3}, {159974,0}, {160331,1}, {160510,2}, {160688,3}, {160867,0}, {161045,1}, {161402,2}, {164260,3}
  }
};

const int blockWidth = 21;
const int blockHeight = 20;
const int maxBlocks = 10;
const int LEFT_BUTTON = 34;
const int DOWN_BUTTON = 33;
const int UP_BUTTON = 32;
const int RIGHT_BUTTON = 31;
const int SCORE_AREA_WIDTH = 50;
const int SCORE_AREA_HEIGHT = 20;
int blockSpeed = 5;
int score = 0;

struct Block {
  int x;
  int y;
  bool active;
  int prevY; // Track previous position for proper erasing
};

// Button state tracking
bool leftButtonPressed = false;
bool downButtonPressed = false;
bool upButtonPressed = false;
bool rightButtonPressed = false;
bool anyButtonPressed = false;

Block blocks[maxBlocks];
bool spawnEventsProcessed[228] = {false};

void spawnBlock(int lane) {
  for (int i = 0; i < maxBlocks; i++) {
    if (!blocks[i].active) {
      blocks[i].x = laneX[lane];
      blocks[i].y = -blockHeight; // Start above screen
      blocks[i].prevY = -blockHeight;
      blocks[i].active = true;
      Serial.print("Spawned block in lane ");
      Serial.print(lane);
      Serial.print(" at x=");
      Serial.println(blocks[i].x);
      break;
    }
  }
}

void drawEndScreen() {
  tft.fillScreen(BACKGROUND);
  tft.setTextSize(3);
  tft.setTextColor(WHITE);
  
  tft.setCursor(50, 80);
  tft.print("GAME OVER");
  
  tft.setCursor(50, 130);
  tft.print("Score: ");
  tft.print(score);
  
  tft.setTextSize(2);
  tft.setCursor(50, 180);
  tft.print("Press any button");
  tft.setCursor(50, 210);
  tft.print("to return to menu");
}

void drawMainMenu() {
  tft.fillScreen(BACKGROUND);
  tft.setCursor(75,50);
  tft.setTextSize(5);
  tft.setTextColor(RED);
  tft.print("MP3");
  tft.setCursor(40,110);
  tft.setTextSize(7);
  tft.print("MEGA");
  tft.setCursor(65,250);
  tft.setTextSize(1);
  tft.setTextColor(WHITE);
  tft.print("PRESS ANY BUTTON TO");
  tft.setCursor(90,270);
  tft.setTextSize(2);
  tft.print("START");
}

void drawStartUp() {
  currentState = START_SCREEN;
  tft.fillScreen(BACKGROUND);
  tft.setTextSize(2);
  tft.setTextColor(WHITE);

  tft.setCursor(50, 80);
  tft.print("Zelda's Lullaby");
  tft.fillCircle(35, 82, 7, GREEN);

  tft.setCursor(50, 140);
  tft.print("Escape From \n    The City");
  tft.fillCircle(35, 142, 7, YELLOW);

  tft.setCursor(50, 200);
  tft.print("It's Going Down\n    Now");
  tft.fillCircle(35, 202, 7, RED);
}

void drawLanes() {
  for (int i = 0; i < 4; i++) {
    tft.fillRect(laneX[i] - (laneWidth-blockWidth)/2, 0, laneWidth, 280, LANE_COLOR);
  }
}

void startGame() {
  currentState = PLAYING;
  score = 0;
  songStartTime = millis();
  gameActive = true;
  
  for (int i = 0; i < maxBlocks; i++) blocks[i].active = false;
  for (int i = 0; i < notes; i++) spawnEventsProcessed[i] = false;

  tft.fillScreen(BLACK);
  drawLanes(); // Draw filled lanes
  
  tft.setTextSize(2);
  tft.setCursor(30, 300);
  tft.print("Score: ");
  tft.setCursor(110, 300);
  tft.print(score);

  Serial1.print(song);
  Serial.println("Sent to MP3 player at game start");
}

class BUTTON {
  public:
    void wired_button_state() {
      if (digitalRead(LEFT_BUTTON) || digitalRead(DOWN_BUTTON) || 
          digitalRead(UP_BUTTON) || digitalRead(RIGHT_BUTTON)) {
        if (!anyButtonPressed) {
          anyButtonPressed = true;
          if (currentState == MAIN_MENU) {
            drawStartUp();
          } 
          else if (currentState == START_SCREEN) {
            if (digitalRead(DOWN_BUTTON)){
              notes = 115;
              song = 1;
            }
            else if (digitalRead(LEFT_BUTTON)){
              notes = 228;
              song = 0;
            }
            startGame();
          }
          else if (currentState == END_SCREEN) {
            currentState = MAIN_MENU;
            drawMainMenu();
          }
        }
      } else {
        anyButtonPressed = false;
      }

      if (currentState != PLAYING) return;

      if (digitalRead(LEFT_BUTTON) && !leftButtonPressed) {
        leftButtonPressed = true;
        checkAndRemoveBlock(0);
      } else if (!digitalRead(LEFT_BUTTON)) {
        leftButtonPressed = false;
      }

      if (digitalRead(DOWN_BUTTON) && !downButtonPressed) {
        downButtonPressed = true;
        checkAndRemoveBlock(1);
      } else if (!digitalRead(DOWN_BUTTON)) {
        downButtonPressed = false;
      }

      if (digitalRead(UP_BUTTON) && !upButtonPressed) {
        upButtonPressed = true;
        checkAndRemoveBlock(2);
      } else if (!digitalRead(UP_BUTTON)) {
        upButtonPressed = false;
      }

      if (digitalRead(RIGHT_BUTTON) && !rightButtonPressed) {
        rightButtonPressed = true;
        checkAndRemoveBlock(3);
      } else if (!digitalRead(RIGHT_BUTTON)) {
        rightButtonPressed = false;
      }
    }

  private:
    void checkAndRemoveBlock(int lane) {
      for (int i = 0; i < maxBlocks; i++) {
        if (blocks[i].active && blocks[i].x == laneX[lane] && blocks[i].y >= 210 && blocks[i].y <= 280) {
          blocks[i].active = false;
          score++;
          // Erase block
          tft.fillRect(blocks[i].x, blocks[i].y, blockWidth, blockHeight, LANE_COLOR);
          // Update score
          tft.fillRect(110, 300, SCORE_AREA_WIDTH, SCORE_AREA_HEIGHT, BLACK);
          tft.setCursor(110, 300);
          tft.print(score);
          Serial.println("Block Hit!");
          return;
        }
      }
      Serial.println("Missed!");
    }
};

BUTTON button;

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
  uint16_t ID = tft.readID();
  tft.begin(ID);
  pinMode(LEFT_BUTTON, INPUT);
  pinMode(DOWN_BUTTON, INPUT);
  pinMode(UP_BUTTON, INPUT);
  pinMode(RIGHT_BUTTON, INPUT);
  randomSeed(analogRead(A0));
  drawMainMenu();
  Serial.println("Game initialized. Waiting for input...");
  current = current + millis();
}

void loop() {
  button.wired_button_state();

  if (currentState == PLAYING) {
    // Check if song has finished
    if (millis() >= songDurations[song]) {
      gameActive = false; // Stop spawning new blocks
    }

    // Spawn blocks when it's time, only if gameActive
    if (gameActive) {
      for (int i = 0; i < notes; i++) {
        unsigned long temp_time = pgm_read_word(&pos[song][i][0]);
        unsigned long temp_lane = pgm_read_word(&pos[song][i][1]);
        if (!spawnEventsProcessed[i] && millis() >= (temp_time + songStartTime)) {
          spawnBlock(temp_lane);
          spawnEventsProcessed[i] = true;
        }
      }
    }

    // Update active blocks and check if all blocks have fallen
    bool allBlocksFallen = true;
    for (int i = 0; i < maxBlocks; i++) {
      if (blocks[i].active) {
        allBlocksFallen = false;

        // Erase previous position (only if visible)
        if (blocks[i].prevY >= 0 && blocks[i].prevY <= 280) {
          tft.fillRect(blocks[i].x, blocks[i].prevY, blockWidth, blockHeight, LANE_COLOR);
        }

        // Update position
        blocks[i].prevY = blocks[i].y;
        blocks[i].y += blockSpeed;

        // Draw at new position (only if visible)
        if (blocks[i].y >= 0 && blocks[i].y <= 280) {
          tft.fillRect(blocks[i].x, blocks[i].y, blockWidth, blockHeight, RED);
        }

        // Remove block if it goes off screen
        if (blocks[i].y > 255) {
          // Erase one last time if needed
          if (blocks[i].y <= 280 + blockHeight) {
            tft.fillRect(blocks[i].x, blocks[i].y, blockWidth, blockHeight, LANE_COLOR);
          }
          blocks[i].active = false;
          Serial.print("Block removed at y=");
          Serial.println(blocks[i].y);
        }
      }
    }

    // If song ended and all blocks fallen, transition to end screen
    if (!gameActive && allBlocksFallen) {
      currentState = END_SCREEN;
      // Clear screen and draw only end screen once
      tft.fillScreen(BACKGROUND);
      drawEndScreen();
      return; // Stop further processing in loop this cycle
    }
  }

  delay(10);
}



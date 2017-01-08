#define ssd
//#define ili

#ifdef ssd
  #include <Wire.h>
  #include "Adafruit_SSD1306.h"
  Adafruit_SSD1306 display(A2);
  #define show display.display()
  #define set_brightness \
    display.dim(brightness*255/max_brightness)
  #define _sleep display.off()
  #define wake display.on()
#elif defined ili
  #include <SPI.h>
  #include "Adafruit_ILI9341.h"
  Adafruit_ILI9341 display(A2, A3, A4);
  #define show
  #define led_pin 3
  #define set_brightness \
    analogWrite(led_pin, brightness*255/max_brightness)
  #define _sleep analogWrite(led_pin, 0)
  #define wake set_brightness
#endif

#include "Adafruit_GFX.h"
#include "SL_Helper.h"

#define menu_n 5
#define settings_n 6
#define buttons_n 5

#define btn_up 0
#define btn_down 1
#define btn_left 2
#define btn_right 3
#define btn_c 4

#define batt_pin A1
#define buzz_pin 9

#ifdef ssd
  #define disp_w 128
  #define disp_h 64
  #define font_big 2
  #define font_small 1
#elif defined ili
  #define disp_w 240
  #define disp_h 320
  #define font_big 4
  #define font_small 3
#endif

#define font_b_w font_big*6
#define font_b_h font_big*8
#define font_s_w font_small*6
#define font_s_h font_small*8

#define GAME 1
#define GAME_OVER 2
#define MENU 3
#define SETTINGS 4
#define ABOUT 5
#define SLEEP 6

#define max_len 50
#define max_w 10
#define min_brightness 0
#define max_brightness 2

#define eat_sound_freq 2000
#define game_over_sound_freq 1000


static const byte PROGMEM  keksik[] = {
  B00000000, B00001110, B00000000, //1
  B00000000, B00011110, B00000000, //2
  B00000001, B11111101, B00000000, //3
  B00000110, B00111111, B00000000, //4
  B00011000, B10011111, B00000000, //5
  B00100000, B00001100, B10000000, //6
  B00100010, B00000000, B10000000, //7
  B01001000, B00000100, B01000000, //8
  B01000000, B01000000, B01000000, //9
  B10000000, B00000001, B00100000, //10
  B10000010, B00010000, B00100000, //11
  B10000000, B00000000, B00100000, //12
  B01100000, B01000011, B11000000, //13
  B00110000, B10100100, B10000000, //14
  B00101001, B00011000, B10000000, //15
  B00010110, B00000001, B00000000, //16
  B00010000, B00000001, B00000000, //17
  B00010000, B00000001, B00000000, //18
  B00001000, B00000010, B00000000, //19
  B00000111, B11111100, B00000000  //20
};


static const byte PROGMEM  snake[] = {
  B00000000, B00000111, B11000000, B00000000,
  B00000000, B00001000, B00100000, B00000000,
  B00000000, B00110000, B00011000, B00000000,
  B00000000, B11000000, B11100100, B00000000,
  B00000001, B00000001, B01000011, B00000000,
  B00000010, B00000000, B00000000, B10000000,
  B00000010, B00000000, B00000000, B10000000,
  B00000010, B00000000, B00000000, B10000000,
  B00000010, B00000000, B00000000, B10000000,
  B01111111, B01111011, B11111111, B11111110,
  B11000001, B10000110, B00000000, B00000011,
  B10000000, B10000100, B00000000, B00000001,
  B10000000, B00000000, B00000000, B00000001,
  B10000000, B00000000, B00000000, B00000001,
  B10000000, B00000000, B00000000, B00000001,
  B10000000, B00000000, B00000000, B00000001,
  B10000000, B00000000, B00000000, B00000001,
  B10000000, B00000000, B00000000, B00000001,
  B11000000, B00000000, B00000000, B00000011,
  B01111111, B11111111, B11111111, B11111110,
  B01000010, B00000000, B00000000, B01000000,
  B01000001, B00000000, B00000000, B10000000,
  B10100000, B10000000, B00000001, B00000000,
  B10010000, B01000000, B00000110, B00000000,
  B11001000, B00100000, B00001000, B00000000,
  B01100111, B10100000, B00000110, B00000000,
  B00110000, B01000000, B00000011, B11000000,
  B00011100, B10000000, B00000010, B00111100,
  B00000111, B00000000, B00000100, B00001000,
  B00000010, B00000000, B00001000, B11000100,
  B00000100, B00000000, B00001000, B00110011,
  B00001000, B00000000, B00001000, B00001001,
  B00001000, B00000000, B00000000, B00110001,
  B00010000, B00000000, B00000001, B11000001,
  B00010000, B00000000, B00000000, B00000001,
  B00010000, B00000000, B00000000, B00000001,
  B00010000, B00000000, B00000000, B00000011,
  B00001000, B00000000, B00000000, B00011100,
  B00000100, B00000000, B00000000, B11100000,
  B00000011, B11111111, B11111111, B00000000
};


byte brightness = 0;

byte buttons[] = {4, 5, 6, 7, 3};
bool last_states_buttons[buttons_n], processed_buttons[buttons_n];
byte buttons_pressed[buttons_n];

byte menu_select, keksik_pos, location = ABOUT;
char keksik_speed = 2;
byte body[max_len*2], body_n, food[3], dir;
unsigned int score, highscore = 0;
byte map_w, map_h, player_w, player_speed;
byte new_player_speed = 5, new_player_w = 5;
bool walls, sounds, bonuses, new_walls = 0, new_sounds = 0, new_bonuses = 1;
bool start = 0;
int last = millis();


void buttons_callback(); //216
void loop_game(); //226
void loop_game_over(); //244
void loop_menu(); //254
void loop_settings(); //273
void loop_sleep(); //312
void loop_about(); //320
bool move(); //325
void clear_snake(); //370
void draw_game(); //378
void redraw_game(); //388
void draw_game_over(); //406
void redraw_game_over(); //420
void draw_menu(); //427
void redraw_menu(); //438
void draw_settings(); //446
void redraw_settings(); //457
void draw_about(); //485
void draw_logo(); //496
bool check_in_body(byte x, byte y); //509
void put_food(); //517
void new_game(); //529
void game_over(); //544
void menu(); //553
void settings(); //560
void sleep(); //569
void about(); //575
void apply_settings(); //581


void setup() {
  //analogReference(INTERNAL);
  //attachInterrupt(0, buttons_callback, CHANGE);
  display.begin();
  display.dim(brightness*255/max_brightness);
  display.setTextColor(WHITE);
  randomSeed(analogRead(0));
  for (byte i = 0; i < buttons_n; i++) digitalWrite(buttons[i], 1);
  digitalWrite(batt_pin, 1);
  apply_settings();
  draw_logo();
}


void loop() {
  buttons_callback();
  if (location == GAME) loop_game();
  else if (location == GAME_OVER) loop_game_over();
  else if (location == MENU) loop_menu();
  else if (location == SETTINGS) loop_settings();
  else if (location == SLEEP) loop_sleep();
  else if (location == ABOUT) loop_about();
  for (byte i = 0; i < buttons_n; i++) processed_buttons[i] = 1;
}


void buttons_callback() {
  for (byte i = 0; i < buttons_n; i++) {
    if (digitalRead(buttons[i]) == last_states_buttons[i]) {
      last_states_buttons[i] = !last_states_buttons[i];
      processed_buttons[i] = 0;
    }
  }
}


void loop_game() {
  if (!processed_buttons[btn_up] && last_states_buttons[btn_up] && dir != 2) dir = 1;
  if (!processed_buttons[btn_down] && last_states_buttons[btn_down] && dir != 1) dir = 2;
  if (!processed_buttons[btn_left] && last_states_buttons[btn_left] && dir != 4) dir = 3;
  if (!processed_buttons[btn_right] && last_states_buttons[btn_right] && dir != 3) dir = 4;
  if (!processed_buttons[btn_c] && last_states_buttons[btn_c]) {
    menu();
    return;
  }
  int now = millis();
  if (now - last >= (10 - player_speed) * 10) {
    last = now;
    clear_snake();
    if(move()) redraw_game();
  }
}


void loop_game_over(){
  redraw_game_over();
  keksik_pos += keksik_speed;
  if (keksik_pos >= disp_h-20 && keksik_speed > 0 || keksik_pos <= 20 && keksik_speed < 0)
    keksik_speed = -keksik_speed;
  if(!processed_buttons[btn_c] && last_states_buttons[btn_c]) new_game();
  else delay(20);
}


void loop_menu(){
  byte old_menu = menu_select;
  if (!processed_buttons[btn_up] && last_states_buttons[btn_up]){
    if(menu_select == 1) menu_select = menu_n;
    else menu_select--;
  } else if (!processed_buttons[btn_down] && last_states_buttons[btn_down]){
    if(menu_select == menu_n) menu_select = 1;
    else menu_select++;
  }
  if (!processed_buttons[btn_c] && last_states_buttons[btn_c]) {
    if(menu_select == 1) new_game();
    else if (menu_select == 2 && !start) location = GAME;
    else if (menu_select == 3) settings();
    else if (menu_select == 4) sleep();
    else if (menu_select == 5) about();
  } else if(old_menu != menu_select) redraw_menu();
}


void loop_settings() {
  if (!processed_buttons[btn_up] && last_states_buttons[btn_up]){
    if(menu_select == 1) menu_select = settings_n;
    else menu_select--;
  }
  if (!processed_buttons[btn_down] && last_states_buttons[btn_down]){
    if(menu_select == settings_n) menu_select = 1;
    else menu_select++;
  }
  if (!processed_buttons[btn_left] && last_states_buttons[btn_left]) {
    if (menu_select == 1 && new_player_w > 1) new_player_w--;
    else if (menu_select == 2 && new_player_speed > 1) new_player_speed--;
    else if (menu_select == 3) new_walls = !new_walls;
    else if (menu_select == 4) new_sounds = !new_sounds;
    else if (menu_select == 5) new_bonuses = !new_bonuses;
    else if (menu_select == 6 && brightness>min_brightness){
      brightness--;
      set_brightness;
    }
  }
  if (!processed_buttons[btn_right] && last_states_buttons[btn_right]) {
    if (menu_select == 1 && new_player_w < 10) new_player_w++;
    else if (menu_select == 2 && new_player_speed < 10) new_player_speed++;
    else if (menu_select == 3) new_walls = !new_walls;
    else if (menu_select == 4) new_sounds = !new_sounds;
    else if (menu_select == 5) new_bonuses = !new_bonuses;
    else if (menu_select == 6 && brightness<max_brightness){
      brightness++;
      set_brightness;
    }
  }
  if (!processed_buttons[btn_c] && last_states_buttons[btn_c]) {
    apply_settings();
    start = 0;
    menu();
  } else redraw_settings();
}


void loop_sleep(){
  if (!processed_buttons[btn_c] && last_states_buttons[btn_c]) {
    wake;
    menu();
  }
}


void loop_about(){
  if (!processed_buttons[btn_c] && last_states_buttons[btn_c]) menu();
}


bool move() {
  byte head[2];
  if (dir == 1) {
    if(walls && body[1] == 0) return true;
    head[0] = body[0];
    if(body[1] > 0) head[1] = body[1] - 1;
    else head[1] = map_h-1;
  } else if (dir == 2) {
    if(walls && body[1] == map_h-1) return true;
    head[0] = body[0];
    if(body[1] < map_h-1) head[1] = body[1] + 1;
    else head[1] = 0;
  } else if (dir == 3) {
    if(walls && body[0] == 0) return true;
    if(body[0] > 0) head[0] = body[0] - 1;
    else head[0] = map_w-1;
    head[1] = body[1];
  } else if (dir == 4) {
    if(walls && body[0] == map_w-1) return true;
    if(body[0] < map_w-1) head[0] = body[0] + 1;
    else head[0] = 0;
    head[1] = body[1];
  } else return true;

  if (check_in_body(head[0], head[1])) {
    game_over();
    return false;
  }

  if (head[0] == food[0] && head[1] == food[1]) {
    if(sounds) tone(9, eat_sound_freq, 200);
    if(food[2] < 4) score += food[2];
    else if(food[2] == 4){
      body_n = (body_n>9)?(body_n-5):5;
      for(byte i=body_n*2-1; i<max_len*2-1; i++) body[i] = 0;
    }
    put_food();
    if (body_n < max_len) body_n++;
  }

  for (byte i = body_n * 2 - 1; i > 1; i--) body[i] = body[i - 2];
  body[0] = head[0]; body[1] = head[1];
}


void clear_snake(){
  for (byte i = 0; i < body_n * 2; i += 2) {
    display.fillRect(2+body[i]*player_w, font_b_h+2+body[i+1]*player_w, player_w, player_w, 0);
  }
  display.fillRect(2+food[0]*player_w, font_b_h+2+food[1]*player_w, player_w, player_w, 0);
}


void draw_game() {
  display.fillScreen(0);
  display.setTextSize(font_big);
  display.setCursor(0, 0);
  display.print("Очки: ");
  display.drawRect(0, font_b_h, disp_w, disp_h - font_b_h, 1);
  redraw_game();
}


void redraw_game(){
  display.fillRect(font_b_w*6, 0, disp_w-font_b_w*6, font_b_h, 0);
  display.setCursor(font_b_w*6, 0);
  display.setTextSize(font_big);
  display.print(score);
  display.setCursor(disp_w-font_s_w*4, 0);
  display.setTextSize(font_small);
  display.print(4*5.0/1024*analogRead(batt_pin), 1);
  for (byte i = 2; i < body_n * 2; i += 2) {
    display.drawRect(2+body[i]*player_w, font_b_h+2+body[i+1]*player_w, player_w, player_w, 1);
  }
  display.fillRect(2+body[0]*player_w, font_b_h+2+body[1]*player_w, player_w, player_w, 1);
  if(food[2] == 1) display.fillRect(2+food[0]*player_w, font_b_h+2+food[1]*player_w, player_w, player_w, 1);
  else display.fillCircle(2+food[0]*player_w+player_w/2, font_b_h+2+food[1]*player_w+player_w/2, player_w/2, 1);
  display.display();
}


void draw_game_over() {
  display.fillScreen(0);
  display.setTextSize(font_big);
  display.setCursor(font_b_w*2, 0);
  display.print(F("ИГРА\nОКОНЧЕНА"));
  display.setCursor(0, font_b_h*2+font_s_h);
  display.setTextSize(font_small);
  display.print(F("Очки: "));
  display.println(score);
  display.print(F("Рекрорд: "));
  display.println(highscore);
}


void redraw_game_over(){
  display.fillRect(font_b_w*8+(disp_w-font_b_w*8)/2, 0, 20, disp_h, 0);
  display.drawBitmap(font_b_w*8+(disp_w-font_b_w*8)/2, keksik_pos, keksik, 20, 20, 1);
  display.display();
}


void draw_menu(){
  display.fillScreen(0);
  display.setTextSize(font_big);
  display.setCursor((disp_w-font_b_w*4)/2, 0);
  display.println(F("меню"));
  display.setTextSize(font_small);
  display.println(F("  новая игра\n  продолжить\n  настройки\n  сон\n  авторы"));
  redraw_menu();
}


void redraw_menu(){
  display.fillRect(0, font_b_h, font_s_w*2, font_s_h*menu_n, 0);
  display.setCursor(0, font_b_h+(menu_select-1)*font_s_h);
  display.print(">");
  display.display();
}


void draw_settings() {
  display.fillScreen(0);
  display.setTextSize(font_big);
  display.setCursor((disp_w-font_b_w*9)/2, 0);
  display.print(F("настройки"));
  display.setCursor(0, font_b_h);
  display.setTextSize(font_small);
  display.print(F("  размер:\n  скорость:\n  стены:\n  звуки:\n  бонусы:\n  яркость:"));
}


void redraw_settings(){
  byte x = font_s_w*13;
  display.fillRect(0, font_b_h, font_s_w*2, font_s_h*settings_n, 0);
  display.fillRect(x, font_b_h, disp_w-x, disp_h, 0);
  display.setCursor(0, font_b_h+(menu_select-1)*font_s_h);
  display.print(">");
  
  display.setCursor(x, font_b_h);
  display.println(new_player_w);

  display.setCursor(x, font_b_h+font_s_h);
  display.println(new_player_speed);
  
  display.setCursor(x, font_b_h+font_s_h*2);
  display.println(new_walls?"да":"нет");
  
  display.setCursor(x, font_b_h+font_s_h*3);
  display.println(new_sounds?"да":"нет");
  
  display.setCursor(x, font_b_h+font_s_h*4);
  display.println(new_bonuses?"да":"нет");
  
  display.setCursor(x, font_b_h+font_s_h*5);
  display.println(brightness);
  display.display();
}


void draw_about(){
  display.fillScreen(0);
  display.setTextSize(font_big);
  display.setCursor((disp_w-font_b_w*6)/2, 0);
  display.println(F("авторы"));
  display.setTextSize(font_small);
  display.print(F("код и сборка: Тимоха\nдизайн кексика: Рита\nдизайн лого: Саня\nсооавтор: Саня\nтестеры: Саня и тд"));
  display.display();
}


void draw_logo(){
  display.fillScreen(0);
  display.setTextSize(font_big);
  display.setCursor((disp_w-font_b_w*6)/2, 0);
  display.print("ЗМЕЙКА");
  display.drawBitmap(48, 20, snake, 32, 40, 1);
  display.setTextSize(font_small);
  display.setCursor(50, 31);
  display.print("1.1.2");
  display.display();
}


bool check_in_body(byte x, byte y) {
  for (byte i = 0; i < body_n * 2; i += 2) {
    if (body[i] == x && body[i + 1] == y) return true;
  }
  return false;
}


void put_food() {
  byte x, y;
  do {
    x = random(map_w);
    y = random(map_h);
  } while (check_in_body(x, y));
  food[0] = x;
  food[1] = y;
  food[2] = bonuses?random(1, 5):1;
}


void new_game() {
  start = 1;
  location = GAME;
  dir = score = 0;
  body_n = 5;
  memset(body, 0, max_len*2);
  for (byte i = 0; i < 5; i++) {
    body[i * 2] = map_w / 2 + i;
    body[i * 2 + 1] = map_h / 2;
  }
  put_food();
  draw_game();
}


void game_over() {
  if(sounds) tone(9, game_over_sound_freq, 500);
  location = GAME_OVER;
  keksik_pos = 20;
  if(score > highscore) highscore = score;
  draw_game_over();
}


void menu(){
  location = MENU;
  menu_select = 1;
  draw_menu();
}


void settings() {
  new_player_speed = player_speed;
  new_player_w = player_w;
  location = SETTINGS;
  menu_select = 1;
  draw_settings();
}


void sleep(){
  location = SLEEP;
  _sleep;
}


void about(){
  location = ABOUT;
  draw_about();
}


void apply_settings() {
  player_w = new_player_w;
  player_speed = new_player_speed;
  walls = new_walls;
  sounds = new_sounds;
  bonuses = new_bonuses;
  map_w = (disp_w - 4) / player_w;
  map_h = (disp_h - 4 - font_b_h) / player_w;
}

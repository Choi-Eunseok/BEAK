#include <SoftwareSerial.h>
#include "OneButton.h"

SoftwareSerial bluetoothSerial(2, 3); //(RX, TX)

uint8_t const alphabet[] = {0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D};
uint8_t sa[] = {0xFD, 0x9, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
uint8_t const sar[] = {0xFD, 0x9, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
uint8_t vo[14] = {0xFD, 0x9, 0x1, 0x01, 0x0, 0xE2, 0x0, 0x0, 0x0, 0x0, 0x0};
uint8_t const vor[14] = {0xFD, 0x9, 0x1, 0x00, 0x0, 0x00, 0x0, 0x0, 0x0, 0x0, 0x0};

//#include "Keyboard.h"

int d[] = {4, 14, 24, 5, 15, 45, 46, 6, 56, 124, 125, 145, 245,
           126, 1235, 345, 234, 1345, 156, 34, 136, 1236, 13456, 346, 134, 1234, 146, 246, 2456, 135,
           1, 25, 35, 2, 26, 12, 3, 2356, 13, 23, 235, 236, 256, 356
          };


int i = 0; //for문을 위한 변수 i

int last[6] = {0}; //점자모양의 버튼이 두번 입력되지 않게 확인하는 변수
int last_check = 0; //입력버튼이 두번 입력되지 않게 확인하는 변수
int current[6] = {0}; //점자모양의 버튼이 두번 입력되지 않게 확인하는 변수
int current_check = 0; //입력버튼이 두번 입력되지 않게 확인하는 변수

int dot_arr[6] = {0};

OneButton left(8, true);
OneButton middle(9, true);
OneButton right(12, true);

long prev_dot = 0;
int prev_pos = 0;
long curr_dot = 0;
int curr_pos = 0;
int mode = 0;

void setup() {
  pinMode(5, INPUT_PULLUP);
  pinMode(6, INPUT_PULLUP);
  pinMode(7, INPUT_PULLUP);
  pinMode(A3, INPUT_PULLUP);
  pinMode(A2, INPUT_PULLUP);
  pinMode(A1, INPUT_PULLUP);

  pinMode(8, INPUT_PULLUP);
  pinMode(9, INPUT_PULLUP);
  pinMode(12, INPUT_PULLUP);

  pinMode(13, OUTPUT);
  Serial.begin(9600);

  left.attachDoubleClick(l_doubleclick);
  left.attachClick(l_singleclick);
  left.attachLongPressStop(l_longclick);
  middle.attachDoubleClick(m_doubleclick);
  middle.attachClick(m_singleclick);
  middle.attachLongPressStop(m_longclick);
  right.attachDoubleClick(r_doubleclick);
  right.attachClick(r_singleclick);
  right.attachLongPressStop(r_longclick);

  bluetoothSerial.begin(115200);

  bluetoothSerial.println("$$$");         // Command Mode
  delay(500);
  bluetoothSerial.println("CFI");         // Connect Immediately
  delay(500);
  bluetoothSerial.print("---");       // Exit
}

void loop() {
  current[0] = digitalRead(5);
  if (current[0] == 1 && last[0] == 0) {
    dot_arr[0] = 1;
    Serial.println("1"); //1번점 눌려짐
  }
  last[0] = current[0];

  current[1] = digitalRead(6);
  if (current[1] == 1 && last[1] == 0) {
    dot_arr[1] = 1;
    Serial.println("2"); //2번점 눌려짐
  }
  last[1] = current[1];

  current[2] = digitalRead(7);
  if (current[2] == 1 && last[2] == 0) {
    dot_arr[2] = 1;
    Serial.println("3"); //3번점 눌려짐
  }
  last[2] = current[2];

  current[3] = digitalRead(A3);
  if (current[3] == 1 && last[3] == 0) {
    dot_arr[3] = 1;
    Serial.println("4"); //4번점 눌려짐
  }
  last[3] = current[3];

  current[4] = digitalRead(A2);
  if (current[4] == 1 && last[4] == 0) {
    dot_arr[4] = 1;
    Serial.println("5"); //5번점 눌려짐
  }
  last[4] = current[4];

  current[5] = digitalRead(A1
                          );
  if (current[5] == 1 && last[5] == 0) {
    dot_arr[5] = 1;
    Serial.println("6"); //6번점 눌려짐
  }
  last[5] = current[5];

  left.tick();
  middle.tick();
  right.tick();
  delay(10);

}

void m_singleclick() {        //중간 버튼을 한번 눌렀을 때 입력된 점자를 토대로 글자 출력하는 함수
  curr_dot = 0;
  for (i = 0; i < 6; i++) {
    if (dot_arr[i] == 1) {
      if (curr_dot == 0) curr_dot = i + 1;
      else curr_dot = (curr_dot * 10) + (i + 1);
      dot_arr[i] = {0};
    }
  }
  Serial.println(curr_dot); //어느 버튼이 눌려졌는지 알려주는 변수 : curr_dot
  pos_check();
  output();
}

void m_doubleclick() {        //중간 버튼을 두번 눌렀을 때 보이스오버 현재 항목 읽기
  blueVOKey(0x04);
}

void l_singleclick() {        //왼쪽 버튼을 한번 눌렀을 때 점자 리셋
  for (i = 0; i < 6; i++) dot_arr[i] = 0;
}

void l_doubleclick() {        //왼쪽 버튼을 두번 눌렀을 때 보이스오버 이전 항목 읽기
  blueVOKey(0x50);
}

void r_singleclick() {        //오른쪽 버튼을 한번 눌렀을 때 엔터 입력시키는 함수
  sa[5] = 0x28;
  for (int j = 0; j < 11; j++) {
    bluetoothSerial.write(sa[j]);
  }
  delay(10);
  for (int j = 0; j < 11; j++) {
    bluetoothSerial.write(sar[j]);
  }
}

void r_doubleclick() {        //오른쪽 버튼을 두번 눌렀을 때 보이스오버 다음 항목 읽기
  blueVOKey(0x4F);
}

void m_longclick() {          //중간버튼을 길게 눌렀을때 스페이스를 입력시키는 함수
  sa[5] = 0x2C;
  for (int j = 0; j < 11; j++) {
    bluetoothSerial.write(sa[j]);
  }
  delay(10);
  for (int j = 0; j < 11; j++) {
    bluetoothSerial.write(sar[j]);
  }
}

void l_longclick() {          //왼쪽버튼을 길게 눌렀을때 백스페이스를 입력시키는 함수
  sa[5] = 0x2A;
  for (int j = 0; j < 11; j++) {
    bluetoothSerial.write(sa[j]);
  }
  delay(10);
  for (int j = 0; j < 11; j++) {
    bluetoothSerial.write(sar[j]);
  }
  curr_pos = prev_pos;
  
}

void r_longclick() {
}

void pos_check() { //초성인지 중성인지 종성인지 확인하는 함수
  int j = 0;
  for (j = 0; j < 44; j++) {
    if (d[j] == curr_dot) {
      if (j < 13) curr_pos = 1;
      else if (j < 30) curr_pos = 2;
      else curr_pos = 3;
    }
  }
}

void output() {
  if (mode == 0) {                                  //한글일때
    if (prev_pos == 3 && curr_pos != 3) {                         //이전에 초성을 입력하고 현재는 초성을 안 입력했을때
      if (prev_dot == 1)blueKey("r"); //Keyboard.write('r');                    //이전에 1번점을 눌렀을때 'ㄱ' 출력
      if (prev_dot == 3)blueKey("t"); //Keyboard.write('t');                    //이전에 3버넘을 눌렀을때 'ㅅ' 출력
    }


    //초성
    if (curr_dot == 4) {                //현재 4번점이 눌러졌을때
      if (prev_dot == 6) {              //이전에 된소리표인 6번 점을 눌렀을때
        //Keyboard.write('R');           //'ㄲ' 출력
        blueKey("R");
        prev_dot = 0;                   //'ㄲ'에 연달아서 'ㄱ'이 나오는 오류를 막기 위함 1
        return;                         //'ㄲ'에 연달아서 'ㄱ'이 나오는 오류를 막기 위함 2
      }
    } else if (curr_dot == 14) {        //현재 1,4번 점이 눌러졌을때
      //Keyboard.write('s');             //'ㄴ'출력
      blueKey("s");
    }
    else if (curr_dot == 24) {          //현재 2,4번점이 눌러졌을때
      if (prev_dot == 6) {              //이전에 된소리표인 6번 점을 눌렀을때
        //Keyboard.write('E');           //'ㄸ' 출력
        blueKey("E");
        prev_dot = 0;                   //'ㄸ'에 연달아서 'ㄷ'이 나오는 오류를 막기 위함 1
        return;                         //'ㄸ'에 연달아서 'ㄷ'이 나오는 오류를 막기 위함 2
      }
    } else if (curr_dot == 5) {         //현재 5번점이 눌러졌을때
      //Keyboard.write('f');             //'ㄹ' 출력
      blueKey("f");
    }
    else if (curr_dot == 15) {          //현재 1,5번점이 눌러졌을때
      //Keyboard.write('a');             //'ㅁ' 출력
      blueKey("a");
    }
    else if (curr_dot == 45) {          //현재 4,5번점이 눌러졌을때
      if (prev_dot == 6) {              //이전에 된소리표인 6번 점을 눌렀을때
        //Keyboard.write('Q');           //'ㅃ' 출력
        blueKey("Q");
        prev_dot = 0;                   //'ㅃ'에 연달아서 'ㅂ'이 나오는 오류를 막기 위함 1
        return;                         //'ㅃ'에 연달아서 'ㅂ'이 나오는 오류를 막기 위함 2
      }
    } else if (curr_dot == 6) {         //현재 6번점이 눌러졌을때
      if (prev_dot == 6) {              //이전에 된소리표인 6번점을 눌렀을때
        //Keyboard.write('T');           //'ㅆ' 출력
        blueKey("T");
        prev_dot = 0;                   //'ㅆ'에 연달아서 'ㅅ'이 나오는 오류를 막기 위함 1
        return;                         //'ㅆ'에 연달아서 'ㅅ'이 나오는 오류를 막기 위함 2
      }
    } else if (curr_dot == 46) {        //현재 4,6번점이 눌러졌을때
      if (prev_dot == 6) {              //이전에 된소리표인 6번점을 눌렀을때
        //Keyboard.write('W');           //'ㅉ' 출력
        blueKey("W");
        prev_dot = 0;                   //'ㅉ'에 연달아서 'ㅈ'이 나오는 오류를 막기 위함 1
        return;                         //'ㅉ'에 연달아서 'ㅈ'이 나오는 오류를 막기 위함 2
      }
    } else if (curr_dot == 56) {        //현재 5,6번점이 눌러졌을때
      //Keyboard.write('c');             //'ㅊ' 출력
      blueKey("c");
    }
    else if (curr_dot == 124) {         //현재 1,2,4번점이 눌러졌을때
      //Keyboard.write('z');             //'ㅋ' 출력
      blueKey("z");
    }
    else if (curr_dot == 125) {         //현재 1,2,5번점이 눌러졌을때
      //Keyboard.write('x');             //'ㅌ' 출력
      blueKey("x");
    }
    else if (curr_dot == 145) {         //현재 1,4,5번점이 눌러졌을때
      //Keyboard.write('v');             //'ㅍ' 출력
      blueKey("v");
    }
    else if (curr_dot == 245) {         //현재 2,4,5번점이 눌러졌을때
      //Keyboard.write('g');             //'ㅎ' 출력
      blueKey("g");
    }
    if (curr_pos == 2 && prev_pos == 1) {               //된소리표기가 가능한 초성들의 예사소리들을 입력하고 중성을 입력했을때
      if (prev_dot == 4) blueKey("r"); //Keyboard.write('r');          //이전에 4번점을 눌렀을때 'ㄱ' 출력
      if (prev_dot == 24) blueKey("e"); //Keyboard.write('e');         //이전에 2,4번점을 눌렀을때 'ㄷ' 출력
      if (prev_dot == 45) blueKey("q"); //Keyboard.write('q');         //이전에 4,5번점을 눌렀을때 'ㅂ' 출력
      if (prev_dot == 6) blueKey("t"); //Keyboard.write('t');          //이전에 6번점을 눌렀을때 'ㅅ' 출력
      if (prev_dot == 46) blueKey("w"); //Keyboard.write('w');         //이전에 4,6번점을 눌렀을때 'ㅈ' 출력
    }
    //초성 끝


    //중성
    if ((curr_pos == 2 && prev_pos != 1 && curr_dot != 1235) ||
        (curr_dot == 1235 && prev_dot == 1235)) {                 //초성을 치지 않고 모음을 쳤을 경우
      //Keyboard.write('d');                                       //모음 앞에 'ㅇ'을 입력해줌
      blueKey("d");
    }
    if (curr_dot != 1235) {                                       //현재 1,2,3,5번점을 누르지 않았을때(ㅒ, ㅙ, ㅞ, ㅟ 들의 첫번째 점자는 각자 다른 한개의 중성 점자와 겹치기 때문에
      //오류를 방지하기 위해 모음을 다 입력했을 상황에만 ㅑ,ㅘ,ㅜ,ㅝ가 출력되게함)
      if (prev_dot == 345) blueKey("i"); //Keyboard.write('i');                  //이전에 3,4,5번점을 눌렀을때 'ㅑ' 출력
      else if (prev_dot == 1236) blueKey("hk"); //Keyboard.print("hk");           //이전에 1,2,3,6번점을 눌렀을때 'ㅘ' 출력
      else if (prev_dot == 1234) blueKey("nj"); //Keyboard.print("nj");           //이전에 1,2,3,4번점을 눌렀을때 'ㅝ' 출력
      else if (prev_dot == 134) blueKey("n"); //Keyboard.write('n');             //이전에 1,3,4번점을 눌렀을때 'ㅜ' 출력
    }
    if (curr_dot == 126) {                                        //현재 1,2,6번점이 눌러졌을때
      //Keyboard.write('k');                                       //'ㅏ' 출력
      blueKey("k");
    }
    else if (curr_dot == 1235) {                                  //현재 1,2,3,5번점이 눌러졌을때 (1235는 ㅐ를 의미하지만, ㅒ와 ㅙ와 ㅞ와 ㅟ의 두번째 점자에 1235가 들어감)
      if (prev_dot == 345) blueKey("O"); //Keyboard.write('O');                  //이전에 3,4,5번점을 눌렀을때 'ㅒ' 출력
      else if (prev_dot == 1236) blueKey("ho"); //Keyboard.print("ho");           //이전에 1,2,3,6번점을 눌렀을때 'ㅙ' 출력
      else if (prev_dot == 1234) blueKey("np"); //Keyboard.print("np");           //이전에 1,2,3,4번점을 눌렀을때 'ㅞ' 출력
      else if (prev_dot == 134) blueKey("nl"); //Keyboard.print("nl");            //이전에 1,3,4번점을 눌렀을때 'ㅟ' 출력
      else blueKey("o"); //Keyboard.write('o');                                  //1,2,3,5번점을 한번만 눌렀을때 'ㅐ' 출력
    }
    else if (curr_dot == 234) {                                   //현재 2,3,4번점이 눌러졌을때
      //Keyboard.write('j');                                       //'ㅓ' 출력
      blueKey("j");
    }
    else if (curr_dot == 1345) {                                  //현재 1,3,4,5번점이 눌러졌을때
      //Keyboard.write('p');                                       //'ㅔ' 출력
      blueKey("p");
    }
    else if (curr_dot == 156) {                                   //현재 1,5,6번점이 눌러졌을때
      //Keyboard.write('u');                                       //'ㅕ' 출력
      blueKey("u");
    }
    else if (curr_dot == 34) {                                    //현재 3,4번점이 눌러졌을때
      //Keyboard.write('P');                                       //'ㅖ' 출력
      blueKey("P");
    }
    else if (curr_dot == 136) {                                   //현재 1,3,6번점이 눌러졌을때
      //Keyboard.write('h');                                       //'ㅗ' 출력
      blueKey("h");
    }
    else if (curr_dot == 13456) {                                 //현재 1,3,4,5,6번점이 눌러졌을때
      //Keyboard.print("hl");                                      //'ㅚ' 출력
      blueKey("hl");
    }
    else if (curr_dot == 346) {                                   //현재 3,4,6번점이 눌러졌을때
      //Keyboard.write('y');                                       //'ㅛ' 출력
      blueKey("y");
    }
    else if (curr_dot == 146) {                                   //현재 1,4,6번점이 눌러졌을때
      //Keyboard.write('b');                                       //'ㅠ' 출력
      blueKey("b");
    }
    else if (curr_dot == 246) {                                   //현재 2,4,6번점이 눌러졌을때
      //Keyboard.write('m');                                       //'ㅡ' 출력
      blueKey("m");
    }
    else if (curr_dot == 2456) {                                  //현재 2,4,5,6번점이 눌러졌을때
      //Keyboard.print("ml");                                      //'ㅢ' 출력
      blueKey("ml");
    }
    else if (curr_dot == 135) {                                   //현재 1,3,5번점이 눌러졌을때
      //Keyboard.write('l');                                       //'ㅣ' 출력
      blueKey("l");
    }
    //중성 끝


    //종성
    if (curr_dot == 1) {                                          //현재 1번점이 눌러졌을때
      if (prev_dot == 1) {                                        //이전에 1번점을 눌렀을때
        //Keyboard.write('R');                                     //'ㄲ' 출력
        blueKey("R");
        prev_dot = 0;                                             //'ㄲ'을 입력했는데 'ㄱㄲ'이 나오는 오류 막기위함 1
        prev_pos = curr_pos;                                      //'ㄲ'을 입력했는데 'ㄱㄲ'이 나오는 오류 막기위함 2
        return;                                                   //'ㄲ'을 입력했는데 'ㄱㄲ'이 나오는 오류 막기위함 3
      }
    } else if (curr_dot == 25) {                                  //현재 2,5번점이 눌러졌을때
      //Keyboard.write('s');                                       //'ㄴ' 출력
      blueKey("s");
    }
    else if (curr_dot == 35) {                                    //현재 3,5번점이 눌러졌을때
      //Keyboard.write('e');                                       //'ㄷ' 출력
      blueKey("e");
    }
    else if (curr_dot == 2) {                                     //현재 2번점이 눌러졌을때
      //Keyboard.write('f');                                       //'ㄹ' 출력
      blueKey("f");
    }
    else if (curr_dot == 26) {                                    //현재 2,6번점이 눌러졌을때
      //Keyboard.write('a');                                       //'ㅁ' 출력
      blueKey("a");
    }
    else if (curr_dot == 12) {                                    //현재 1,2번점이 눌러졌을때
      //Keyboard.write('q');                                       //'ㅂ' 출력
      blueKey("q");
    }
    else if (curr_dot == 3) {                                     //현재 3번점이 눌러졌을때
      if (prev_dot == 3) {                                        //이전에 3번점을 눌렀을때
        //Keyboard.write('T');                                     //'ㅆ' 출력
        blueKey("T");
        prev_dot = 0;                                             //'ㅆ'을 입력했는데 'ㅅㅆ'이 나오는 오류 막기위함 1
        prev_pos = curr_pos;                                      //'ㅆ'을 입력했는데 'ㅅㅆ'이 나오는 오류 막기위함 2
        return;                                                   //'ㅆ'을 입력했는데 'ㅅㅆ'이 나오는 오류 막기위함 3
      }
    } else if (curr_dot == 2356) {                                //현재 2,3,5,6번점이 눌러졌을때
      //Keyboard.write('d');                                       //'ㅇ' 출력
      blueKey("d");
    }
    else if (curr_dot == 13) {                                    //현재 1,3번점이 눌러졌을때
      //Keyboard.write('w');                                       //'ㅈ' 출력
      blueKey("w");
    }
    else if (curr_dot == 23) {                                    //현재 2,3번점이 눌러졌을때
      //Keyboard.write('c');                                       //'ㅊ' 출력
      blueKey("c");
    }
    else if (curr_dot == 235) {                                   //현재 2,3,5번점이 눌러졌을때
      //Keyboard.write('z');                                       //'ㅋ' 출력
      blueKey("z");
    }
    else if (curr_dot == 236) {                                   //현재 2,3,6번점이 눌러졌을때
      //Keyboard.write('x');                                       //'ㅌ' 출력
      blueKey("x");
    }
    else if (curr_dot == 256) {                                   //현재 2,5,6번점이 눌러졌을때
      //Keyboard.write('v');                                       //'ㅍ' 출력
      blueKey("v");
    }
    else if (curr_dot == 356) {                                   //현재 3,5,6번점이 눌러졌을때
      ////Keyboard.write('g');                                       //'ㅎ' 출력
      blueKey("g");
    }

  }


  prev_pos = curr_pos;                                          //현재 초성인지 중성인지 종성인지 판단하는 변수를 이전의 초중종성을 판단하는 변수로 저장
  prev_dot = curr_dot;                                          //현재의 점자를 이전의 점자로 저장
}

void blueKey(char arr[]) {                                      //출력해야하는 문자를 블루투스로 보내주는 함수
  for (int i = 0; i < sizeof(arr) / sizeof(char); i++) {
    if (arr[i] < 93) {
      sa[3] = 0x02;
      sa[5] = alphabet[arr[i] - 65];
    }
    else {
      sa[3] = 0x0;
      sa[5] = alphabet[arr[i] - 97];
    }
    for (int j = 0; j < 11; j++) {
      bluetoothSerial.write(sa[j]);
    }
    delay(10);
    for (int j = 0; j < 11; j++) {
      bluetoothSerial.write(sar[j]);
    }
  }
}

void blueVOKey(uint8_t in) {                                  //보이스 오버의 단축키을 블루투스로 보내주는 함수
  vo[6] = in;
  for (int j = 0; j < 11; j++) {
    bluetoothSerial.write(vo[j]);
  }
  delay(10);
  for (int j = 0; j < 11; j++) {
    bluetoothSerial.write(vor[j]);
  }
}

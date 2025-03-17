#include <FreqCount.h>
// FreqCountライブラリ D5に入力されたパルスをカウント
// https://qiita.com/hsgucci/items/eee5894e3651d0a8cb75
// https://www.arduinolibraries.info/libraries/freq-count

#define STRICT
// 未定義命令のチェックを厳密に行う
//#define SEST_MODE
// テスト用信号発生
//#define DEBUG_MODE
// デバッグモード

int i = 0;
String myString;
char command;

//volatile long int counter2;
//volatile long int counter3 = 0; //dummy
//volatile char command;

void(*resetFunc)(void) = 0;

void setup() {
  // put your setup code here, to run once:  char command;
  //Serial.begin(57600);
  Serial.begin(115200);
#ifdef TEST_MODE
  analogWrite(A3,128); // テストパルス Analog Pin3(A3) デューティー比50%(128/256)
#endif
}

void loop() {
  // put your main code here, to run repeatedly:
  int delta_t; //計数する時間(ミリ秒単位、デフォルト値1000)

  while ( Serial.available() == 0) { // キー入力待ち
  }
  command = Serial.read(); // 1文字読み出し c, t, s, r、それ以外の文字は親プログラムから送ってこないようにすること

#ifdef STRICT
  if(command != 'c' && command != 't' & command != 's' && command != 'r'){
    command = 'e'; //例外設定
  }
#endif
  
  //Serial.println(command);
  switch (command) {
    case 'c': //接続確認 "C"を返す
      //Serial.flush();
      myString = Serial.readString();
      Serial.println("C");
      Serial.flush();
      //resetFunc();
      break;
    case 's':
      myString = Serial.readString();
      proc_stop();
      break;
    case 'r':
      myString = Serial.readString();
      FreqCount.end();
      Serial.println("Reset"); // Arduinoリセット "Reset"を返す
      Serial.flush();
      resetFunc();
      break;
#ifdef STRICT
    case 'e': // 例外処理
      myString = Serial.readString();
      Serial.println("Invalid order"); // Arduinoリセット "Invalid order"を返す
      break;
#endif
    case 't': //タイマー間隔設定して開始, exapmle: t500 → 500 ms毎
      myString = Serial.readString();
      delta_t = myString.toInt();
      Serial.println(delta_t, DEC);
      FreqCount.begin(delta_t);
      while ( Serial.available() == 0) { //シリアルポート入力がない間カウント継続
        if (FreqCount.available()) {
          unsigned long count = FreqCount.read();
#ifdef DEBUG_MODE
          Serial.print(i);
          Serial.print("\t");
#endif
          Serial.print(count);
          Serial.print("\t");
          Serial.println("9999999"); //dummy data
#ifdef DEBUG_MODE
          i++;
#endif
          Serial.flush();
          //resetFunc();
        }
      }
      //Serial.println("Check step.");
      command = Serial.read();
      myString = Serial.readString();
      if (command == 's') {
        proc_stop();
      }
      break;
    default:
      break;
  }
}

void proc_stop(){
      FreqCount.end();
      Serial.println("S"); // カウンター停止 "S"を返す
      Serial.flush();
}

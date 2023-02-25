#include <M5Core2.h>
#include <Wire.h>
#include <AXP192.h>

//内部バッテリー取得用のエリア
AXP192 power;

//PIN アサイン設定用 C#は出力系、R#は入力系
#define C0 25
#define C1 26
#define C2 32
#define C3 33
#define R0 13
#define R1 14
#define R2 19
#define R3 27

void setup() {
  // put your setup code here, to run once:
  M5.begin();
  M5.Lcd.setTextFont(4);
  M5.Lcd.printf("DI TesterStart\n");

  //出力ポート設定
  pinMode(C0,OUTPUT);
  pinMode(C1,OUTPUT);
  pinMode(C2,OUTPUT);
  pinMode(C3,OUTPUT);

  //入力ポート設定
  pinMode(R0,INPUT_PULLUP);
  pinMode(R1,INPUT_PULLUP);
  pinMode(R2,INPUT_PULLUP);
  pinMode(R3,INPUT_PULLUP);

  //常時ROW出力
  digitalWrite(C0,LOW);
  digitalWrite(C1,LOW);
  digitalWrite(C2,LOW);
  digitalWrite(C3,LOW);

  //起動音
  M5.Spk.begin();
  M5.Spk.DingDong();
  delay(100);

}

//現在のインプット取得
int getNowInput(int *data )
{
  int inpImage = 0x0 ;
  //出力している場合にON(1)にしたいので、digitalReadの値を反転して保持
  inpImage = inpImage * 0x10 + ( data[0] = !digitalRead(R0) );
  inpImage = inpImage * 0x10 + ( data[1] = !digitalRead(R1) );
  inpImage = inpImage * 0x10 + ( data[2] = !digitalRead(R2) );
  inpImage = inpImage * 0x10 + ( data[3] = !digitalRead(R3) );

  return inpImage ;

}

int count = 0 ;
int oldImage = -1 ;
int bitOldImage[4] ;
int oldBatLevelPercent = 0 ;

void loop() {
  int nowImage =0 ;
  int bitImage[4] ;
  delay(100);
 
  nowImage = getNowInput( bitImage );

  if( oldImage != nowImage )
  {
    oldImage = nowImage ;
    M5.Spk.DingDong();
    M5.Lcd.clear(BLACK);
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextFont(4);
    M5.Lcd.setCursor(10,30);
    M5.Lcd.printf("InputChangeCount[%d]\n",count++);
    M5.Lcd.setTextFont(4);
    M5.Lcd.setCursor(10,60);
    M5.Lcd.printf("[NowState]");
    M5.Lcd.setTextFont(7);
    M5.Lcd.setCursor(50,90);
    for(int i = 0 ; i < 4 ; i++ )
    {
      if(bitImage[i])M5.Lcd.setTextColor(YELLOW); 
      else           M5.Lcd.setTextColor(WHITE); 
      M5.Lcd.printf("%d ",bitImage[i]);
    }
    M5.Lcd.setTextFont(4);
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setCursor(10,150);
    M5.Lcd.printf("[PreState]");
    M5.Lcd.setTextFont(7);
    M5.Lcd.setCursor(50,180);
    for(int i = 0 ; i < 4 ; i++ )
    {
      if(bitOldImage[i])M5.Lcd.setTextColor(YELLOW); 
      else              M5.Lcd.setTextColor(WHITE); 
      M5.Lcd.printf("%d ",bitOldImage[i]);
    }
    memcpy(bitOldImage , bitImage , sizeof(bitImage) );
    oldBatLevelPercent = 0 ;
  
  }
  //以下はバッテリー駆動時のため、バッテリーパーセンテージを表示
  //どっかで見た情報でMAX4.2V 3.0Vでスリープに入るらしいので、4.2を100%,3.2を0％と想定して組んでる。
  //あくまで目安
  float batLevel = power.GetBatVoltage() ;
  int batLevelPercent = ( batLevel - 3.2 )* 10000  / (42-32)  ;
  if( batLevelPercent != oldBatLevelPercent )
  {
    oldBatLevelPercent = batLevelPercent ;
    M5.Lcd.fillRect(0,0,319,26,BLACK);
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextFont(4);
    M5.Lcd.setCursor(10,10);
    M5.Lcd.printf("DiChecker BatLvl[%02d.%d%%]" , batLevelPercent / 10 , batLevelPercent % 10 );
 
  }
 

}

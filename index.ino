/*
 * @Author: liboqian
 * @Date: 2021-01-02 16:08:32
 * @LastEditTime: 2021-01-02 17:22:31
 * @LastEditors: Please set LastEditors
 * @Description: 智能车arduino部分，详细信息见readme.md
 * @FilePath: \index\index.ino
 */

#include "Arduino.h"
#include "Servo.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

struct CheDeng
{
    int LED;
} Left, Right;

SoftwareSerial mySoftwareSerial(10, 11); // 虚拟串口（RX, TX）
DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);
char inByte = 0;  //每次读取的字符
String temp = ""; //字符累加量
int angle = 0;    //角度值
int pos = 0, i = 0;
uint8_t state = 0x0;

Servo myservo1; //雨刷 pin9
Servo myservo2; //摄像头平台 pin12
Servo myservo3; //反光镜 pin13

void setup()
{
    Left.LED = 14;
    Right.LED = 15;
    mySoftwareSerial.begin(9600);
    Serial.begin(115200); //串口通讯波特率115200
    pinMode(Left.LED, OUTPUT);
    pinMode(Right.LED, OUTPUT);
    digitalWrite(Left.LED, LOW); //LOW状态指示灯灭
    digitalWrite(Right.LED, LOW);
    myservo1.attach(9);
    myservo2.attach(12);
    myservo3.attach(13);

    myservo1.write(0); //舵机初始化
    myservo2.write(90);
    myservo3.write(0);

    //正式运行时取消注释
    // if (!myDFPlayer.begin(mySoftwareSerial))
    // {
    //     Serial.println(F("error!"));
    //     while (true)
    //     {
    //         delay(0);
    //     }
    // }

    Serial.println(F("OK!"));
    myDFPlayer.volume(25); //设置音量，默认25
}

void loop()
{
    temp = "";
    while (Serial.available() > 0)
    {
        inByte = Serial.read(); //读取数据，串口一次只能读1个字符
        temp += inByte;         //把读到的字符存进临时变量里面缓存，
    }

    if (temp[0] >= '0' && temp[0] <= '9') //判断是否是角度
    {
        angle = temp.toInt();  //把变量字符串类型转成整型
        Serial.println(angle); //输出数据到串口上，以便观察
        myservo2.write(angle);
    }
    if (temp[0] == 'A') //开车灯
    {
        digitalWrite(Left.LED, HIGH);
        digitalWrite(Right.LED, HIGH);
        state = 0x1;
        Serial.println("ON");
    }
    if (temp[0] == 'B') //关车灯
    {
        digitalWrite(Left.LED, LOW);
        digitalWrite(Right.LED, LOW);
        state = 0x0;
        Serial.println("OFF");
    }
    if (temp[0] == 'J') //后视镜关
    {
        for (pos = 0; pos <= 90; pos += 1)
        {
            myservo3.write(pos);
            delay(5);
        }
    }
    if (temp[0] == 'K') //后视镜开
    {
        for (pos = 90; pos <= 0; pos -= 1)
        {
            myservo3.write(pos);
            delay(5);
        }
    }
    if (temp[0] == 'C') //左转向灯
    {
        uint8_t pinState = state;
        Serial.println("L_SHAN");
        for(int i=0;i<=10;i++)
        {
            digitalWrite(Left.LED, ~pinState);
            delay(500);
            digitalWrite(Left.LED, pinState);
            delay(500);
        }
    }
    if (temp[0] == 'D') //右转向灯
    {
        uint8_t pinState = state;
        Serial.println("R_SHAN");
        for(int i=0;i<=10;i++)
        {
            digitalWrite(Right.LED, ~pinState);
            delay(500);
            digitalWrite(Right.LED, pinState);
            delay(500);
        }
    }
    if (temp[0] == 'p') //暂停
    {
        myDFPlayer.pause(); 
        Serial.println("pause");
    }
    if (temp[0] == 's') //开始播放
    {
        myDFPlayer.start(); 
        Serial.println("start");
    }
    if (temp[0] == 'n') //下一首
    {
        myDFPlayer.next();
        Serial.println("next");
    }
    if (temp[0] == 'r') //上一首
    {
        myDFPlayer.previous(); 
        Serial.println("previous");
    }
}

void printDetail(uint8_t type, int value)   //MP3报错信息
{
    switch (type)
    {
    case TimeOut:
        Serial.println(F("Time Out!"));
        break;
    case WrongStack:
        Serial.println(F("Stack Wrong!"));
        break;
    case DFPlayerCardInserted:
        Serial.println(F("Card Inserted!"));
        break;
    case DFPlayerCardRemoved:
        Serial.println(F("Card Removed!"));
        break;
    case DFPlayerCardOnline:
        Serial.println(F("Card Online!"));
        break;
    case DFPlayerUSBInserted:
        Serial.println("USB Inserted!");
        break;
    case DFPlayerUSBRemoved:
        Serial.println("USB Removed!");
        break;
    case DFPlayerPlayFinished:
        Serial.print(F("Number:"));
        Serial.print(value);
        Serial.println(F(" Play Finished!"));
        break;
    case DFPlayerError:
        Serial.print(F("DFPlayerError:"));
        switch (value)
        {
        case Busy:
            Serial.println(F("Card not found"));
            break;
        case Sleeping:
            Serial.println(F("Sleeping"));
            break;
        case SerialWrongStack:
            Serial.println(F("Get Wrong Stack"));
            break;
        case CheckSumNotMatch:
            Serial.println(F("Check Sum Not Match"));
            break;
        case FileIndexOut:
            Serial.println(F("File Index Out of Bound"));
            break;
        case FileMismatch:
            Serial.println(F("Cannot Find File"));
            break;
        case Advertise:
            Serial.println(F("In Advertise"));
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
}

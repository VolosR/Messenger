#include "M5AtomS3.h"
#include <TFT_eSPI.h>
#include "WiFi.h"
#include <esp_now.h>
TFT_eSPI tft = TFT_eSPI();
TFT_eSprite spr = TFT_eSprite(&tft);

uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

//colors
unsigned short light=0x8F1F;  
unsigned short dark=0x0169;
unsigned short darkest=0x0062;
unsigned short selected=0x701D;
unsigned short foreground;
unsigned short background;
unsigned short txt1=0xDEFB;
char letters[31]="QWERTYUIOPASDFGHJKL<ZXCVBNM., ";
String myMsg="";

int maxLength=19;

int up=6 ;
int down= 5;
int left=8 ;
int right=38 ;
int ok=7 ;
int send=39;
int buttonMy=41;
int butt[7]={6,5,8,38,7,39,41};
int deb[7]={0};

int chosenX=0;
int chosenY=0;
int chosen=0;

bool isMy[4]={0};
bool isMyTMP[4]={0};

String allMsg[4]={"hello, how are you.A",
                  "hello, now are you.B",
                  "hello, zow are you.C",
                  "QWERTZUIOPASDFGHJKLY"};

String allMsgTMP[4]={"hello, how are you.A",
                  "hello, now are you.B",
                  "hello, zow are you.C",
                  "QWERTZUIOPASDFGHJKLY"};


String success;
char msgSent[21];
char msgRecieved[21];
esp_now_peer_info_t peerInfo;

// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  if (status ==0){
    success = "Delivery Success :)";
  }
  else{
    success = "Delivery Fail :(";
  }
}

// Callback when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&msgRecieved, incomingData, sizeof(msgRecieved));
  Serial.print("Bytes received: ");
  Serial.println(msgRecieved);
  sort();
  allMsg[3]=String(msgRecieved);
  isMy[3]=0;
}

   void sort()
   {
    for(int i=0;i<4;i++)
    {allMsgTMP[i]=allMsg[i];
    isMyTMP[i]=isMy[i];
    }
    for(int i=0;i<3;i++)
    {allMsg[i]=allMsgTMP[i+1];
    isMy[i]=isMyTMP[i+1];
    }
   }          

 void sendMsg()
 {
 sort();
 allMsg[3]=myMsg;
 isMy[3]=1;
 myMsg.toCharArray(msgSent, sizeof(msgSent));
 esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &msgSent, sizeof(msgSent));
 myMsg="";
 }                 

void setup() {

    pinMode(up,INPUT_PULLUP);
    pinMode(down,INPUT_PULLUP);
    pinMode(left,INPUT_PULLUP);
    pinMode(right,INPUT_PULLUP);
    pinMode(ok,INPUT_PULLUP);
    pinMode(send,INPUT_PULLUP);
    pinMode(buttonMy,INPUT_PULLUP);

    auto cfg = M5.config();
    AtomS3.begin(cfg);
    AtomS3.Display.setRotation(2);
    AtomS3.Display.setBrightness(14);
    spr.createSprite(128, 128);

     for(int i=0;i<4;i++)
     allMsg[i]="";

       Serial.begin(115200);

  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_send_cb(OnDataSent);
  
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
      
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
  esp_now_register_recv_cb(OnDataRecv);
}

void draw()
{
   spr.fillSprite(TFT_BLACK);
   spr.fillRoundRect(2,2,124,50,2,darkest);
   spr.fillRoundRect(4,70,120,14,2,dark);
   spr.fillRoundRect(2,52,80,14,2,darkest);
   spr.setTextColor(txt1,dark);
   spr.drawString(myMsg,5,73); 
   
    for(int j=0;j<3;j++)
    for(int i=0;i<10;i++)
    {
      if(i==chosenX && j==chosenY)
      {background=selected; foreground=TFT_WHITE;}
      else {
      background=light; foreground=TFT_BLACK;
      }
    spr.setTextColor(foreground,background);
    spr.fillRoundRect(5+(i*12),88+(j*14),10,12,2,background);  
    spr.drawString(String(letters[j*10+i]),5+(i*12)+2,88+(j*14)+2); 
    }

     spr.setTextColor(txt1,darkest);
     for(int j=0;j<4;j++)
     {
      if(isMy[j]==1) 
      spr.setTextColor(txt1,darkest);
      else
      spr.setTextColor(light,darkest);
     spr.drawString(allMsg[j],5,6+(j*12)); 
     }

     spr.setTextColor(TFT_ORANGE,TFT_BLACK);
     spr.drawString(String(myMsg.length())+"/20",94,57); 

     spr.drawFastHLine(8, 54, 62, 0x52AA);
     spr.setTextColor(0x39E7,darkest);
     spr.drawString("ESP NOW",10,57); 

     spr.fillCircle(126,2,6,TFT_RED);

    AtomS3.Display.pushImage(0, 0, 128, 128, (uint16_t*)spr.getPointer());
}

void loop() {

for(int i=0;i<7;i++)
if(digitalRead(butt[i])==0)
{
  if(deb[i]==0)
    {deb[i]=1;
      if(butt[i]==right)
      {chosenX++; if(chosenX>9) chosenX=0;}

       if(butt[i]==left)
      {chosenX--; if(chosenX<0) chosenX=9;}

        if(butt[i]==down)
      {chosenY++; if(chosenY>2) chosenY=0;}

       if(butt[i]==up)
      {chosenY--; if(chosenY<0) chosenY=2;}

       if(butt[i]==ok)
      {
        if(letters[chosenY*10+chosenX]=='<' && myMsg.length()<21)
        myMsg=myMsg.substring(0,myMsg.length()-1);
        else
        myMsg=myMsg+String(letters[chosenY*10+chosenX]);
      }

       if(butt[i]==send)
       { 
        sendMsg();    
       }
    }
}else deb[i]=0;

draw();
}
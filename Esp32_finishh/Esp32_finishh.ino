#include <WiFi.h>
#include <FirebaseESP32.h>
#include "esp_camera.h"
#include "esp_system.h"
#define FIREBASE_HOST "https://untrasonic-3320b-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "HkF1kNygfunBltIerb6He01fKuL853H0CMQeCqdT"
#define WIFI_SSID "AIS 4G Hi-Speed Home WiFi_103923"
#define WIFI_PASSWORD "50103923"

hw_timer_t *timer = NULL;
void IRAM_ATTR resetModule(){
    ets_printf("reboot\n");
    esp_restart();
}
#include <TridentTD_LineNotify.h>
// token line esp32camtus
#define LINE_TOKEN  "8cBn6flSaCro4dQAhYOojT0buALkLMaGIxya8ei144a"   

// Pin definition for CAMERA_MODEL_AI_THINKER
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

const int Led_Flash = 4;

boolean startTimer = false;
unsigned long time_now=0;
int time_capture=0;
int A=0;
int B=2;
FirebaseData firebaseData;
void setup() {

  Serial.begin(115200);
  WiFiConnection();

   pinMode(Led_Flash, OUTPUT);
    LINE.setToken(LINE_TOKEN);

    timer = timerBegin(0, 80, true); //timer 0, div 80Mhz 
    timerAttachInterrupt(timer, &resetModule, true);
    timerAlarmWrite(timer, 20000000, false); //set time in us 15s
    timerAlarmEnable(timer); //enable interrupt

  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG; 
  
  if(psramFound()){
// FRAMESIZE_ +
//QQVGA/160x120//QQVGA2/128x160//QCIF/176x144//HQVGA/240x176
//QVGA/320x240//CIF/400x296//VGA/640x480//SVGA/800x600//XGA/1024x768
//SXGA/1280x1024//UXGA/1600x1200//QXGA/2048*1536
    config.frame_size = FRAMESIZE_SXGA; 
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_QQVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }
  
  // Init Camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }
}

void loop() {
  timerWrite(timer, 0);
  long tme = millis();
  //Camera_capture();
  //delay(4000);
  if(Firebase.getInt(firebaseData, "/Int_A")){
    if(firebaseData.dataType() == "int"){
      A= firebaseData.intData();
      Serial.print("Int_A = "); 
      Serial.println(A);
      if( A == 1){    
        Camera_capture();
        Serial.println("Capture");
      }
    }
  }

    if(Firebase.getInt(firebaseData, "/Int_B")){
    if(firebaseData.dataType() == "int"){
      B= firebaseData.intData();
      Serial.print("Int_B = ");
      Serial.println(B);
      if( B == 0){    
        Camera_capture();
        Serial.println("Capture");
        Serial.println("ok.......................................");
      }
    }
  }
}

void Camera_capture() {
  digitalWrite(Led_Flash, HIGH);
  delay(100); 
  digitalWrite(Led_Flash, LOW);
  delay(100);
 digitalWrite(Led_Flash, HIGH);
  camera_fb_t * fb = NULL;
  delay(200); 
  // Take Picture with Camera
  fb = esp_camera_fb_get(); 
  if(!fb) {
    Serial.println("Camera capture failed");
    return;
  }
   digitalWrite(Led_Flash, LOW);
   Send_line(fb->buf,fb->len);
   esp_camera_fb_return(fb); 
  // Serial.println("Going to sleep now");
  // esp_deep_sleep_start();
  // Serial.println("This will never be printed");

}

void Send_line(uint8_t *image_data,size_t   image_size){
   LINE.notifyPicture("There’s been an accident please Help !",image_data, image_size);
   delay(1000);
  }
  void WiFiConnection(){
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  } 
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
}

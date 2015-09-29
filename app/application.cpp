#include <user_config.h>
#include <SmingCore/SmingCore.h>
#include <Libraries/LiquidCrystal/LiquidCrystal_I2C.h>
#include "NtpClientDelegateSystem.h"
#include <Wire.h>

// If you want, you can define WiFi settings globally in Eclipse Environment Variables

#define WIFI_SSID "OpenWrt" // Put you SSID and Password here
#define WIFI_PWD "testing123"

//#define WIFI_SSID "ekreative" // Put you SSID and Password here
//#define WIFI_PWD "yabloka346"

#define I2C_LCD_ADDR 0x26
LiquidCrystal_I2C lcd(I2C_LCD_ADDR, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

HttpServer server;
FTPServer ftp;
void onNtpReceive(NtpClient& client, time_t timestamp);

Timer printTimer;
ntpClientSystem *clockClient;

int PT2323_ADDRESS  = 74;
int PT2258_ADDRESS  = 68;
int DEVICE_REG_MODE1 = 0x00;
int DEVICE_REG_LEDOUT0 = 0x1d;

int MIN_ATTENUATION = 0;
int MAX_ATTENUATION = 79;

int CHAN_ALL  =  0;
int CHAN_FL   =  1;
int CHAN_FR   =  2;
int CHAN_RL   =  3;
int CHAN_RR   =  4;
int CHAN_SW   =  5;
int CHAN_CEN  =  6;

int PT2258_FL_1DB     =  0x90;
int PT2258_FL_10DB    =  0x80;
int PT2258_FR_1DB     =  0x50;
int PT2258_FR_10DB    =  0x40;
int PT2258_RL_1DB     =  0x70;
int PT2258_RL_10DB    =  0x60;
int PT2258_RR_1DB     =  0xB0;
int PT2258_RR_10DB    =  0xA0;
int PT2258_CEN_1DB    =  0x10;
int PT2258_CEN_10DB   =  0x00;
int PT2258_SW_1DB     =  0x30;
int PT2258_SW_10DB    =  0x20;
int PT2258_ALLCH_1DB  =  0xE0;
int PT2258_ALLCH_10DB =  0xD0;

int volumeFR = 74;
int volumeFL = 74;
int volumeRR = 74;
int volumeRL = 74;
int volumeCEN = 74;
int volumeSW = 74;
int volumeALLCH = 74;

int source = 0;
int mute = 0;
int backlight = 0;
float frequency = 0.0;	
int mixing = 1;
int enhance = 0;
	
	
void onIndex(HttpRequest &request, HttpResponse &response)
{
	TemplateFileStream *tmpl = new TemplateFileStream("index.html");
	auto &vars = tmpl->variables();
	//vars["counter"] = String(counter);
	response.sendTemplate(tmpl); // this template object will be deleted automatically
}

void onFile(HttpRequest &request, HttpResponse &response)
{
	String file = request.getPath();
	if (file[0] == '/')
		file = file.substring(1);

	if (file[0] == '.')
		response.forbidden();
	else
	{
		response.setCache(86400, true); // It's important to use cache for better performance.
		response.sendFile(file);
	}
}


void onLcd(HttpRequest &request, HttpResponse &response)
{
	int x = request.getQueryParameter("x").toInt();
	int y = request.getQueryParameter("y").toInt();
	String text = request.getQueryParameter("text");
	
	
	lcd.setCursor(0,y);
    lcd.print("                ");
	lcd.setCursor(x,y);
	lcd.print(text);
	
	JsonObjectStream* stream = new JsonObjectStream();
	JsonObject& json = stream->getRoot();
	json["status"] = (bool)true;
	response.sendJsonObject(stream);
}
void onVolume(HttpRequest &request, HttpResponse &response)
{
	
	int channel = request.getQueryParameter("channel").toInt();
	int value = 74 - request.getQueryParameter("value").toInt();
	int x10 = 0;
    int x1 = 0;
	
    if(channel == CHAN_ALL){
    	
       volumeALLCH = value;
       volumeFR = value;
       volumeFL = value;
       volumeRR = value;
       volumeRL = value;
       volumeCEN = value;
       volumeSW = value;
       volumeALLCH = value;

       x10 =  PT2258_ALLCH_10DB + (value / 10);
       x1 =   PT2258_ALLCH_1DB + (value % 10);

   }else
    if (channel == CHAN_FR){
       volumeFR = value;
       x10 =  PT2258_FR_10DB + (value / 10);
       x1 =   PT2258_FR_1DB + (value % 10);

   }else
    if ( channel == CHAN_FL){
       volumeFL = value;
       x10 =  PT2258_FL_10DB + (value / 10);
       x1 =   PT2258_FL_1DB + (value % 10);

   }else
    if ( channel == CHAN_RR){
       volumeRR = value;
       x10 =  PT2258_RR_10DB + (value / 10);
       x1 =   PT2258_RR_1DB + (value % 10);

   }else
    if ( channel == CHAN_RL){
       volumeRL = value;
       x10 =  PT2258_RL_10DB + (value / 10);
       x1 =   PT2258_RL_1DB + (value % 10);

   }else
    if (channel == CHAN_CEN){
       volumeCEN = value;
       x10 =  PT2258_CEN_10DB + (value / 10);
       x1 =   PT2258_CEN_1DB + (value % 10);

   }else
    if ( channel == CHAN_SW){
       volumeSW = value;
       x10 =  PT2258_SW_10DB + (value / 10);
       x1 =   PT2258_SW_1DB + (value % 10);
   }

	Wire.beginTransmission(PT2258_ADDRESS);
  	Wire.write(x1);
  	Wire.write(x10);
  	Wire.endTransmission();
	
	
	JsonObjectStream* stream = new JsonObjectStream();
	JsonObject& json = stream->getRoot();
	json["status"] = (bool)true;

	response.sendJsonObject(stream);
}

void onMute(HttpRequest &request, HttpResponse &response)
{
	mute = request.getQueryParameter("value").toInt();
	
	lcd.setCursor(0,1);
	
	if(mute==1){
		lcd.print("     Mute       ");	
	    Wire.beginTransmission(PT2323_ADDRESS);
	  	Wire.write(DEVICE_REG_MODE1);
	  	Wire.write(0xFF);
	  	Wire.endTransmission();
	}else{
		lcd.print("   Un Mute      ");	
	    Wire.beginTransmission(PT2323_ADDRESS);
	  	Wire.write(DEVICE_REG_MODE1);
	  	Wire.write(0xFE);
	  	Wire.endTransmission();
	}
	
	
	JsonObjectStream* stream = new JsonObjectStream();
	JsonObject& json = stream->getRoot();
	json["status"] = (bool)true;
	response.sendJsonObject(stream);
}
void onSource(HttpRequest &request, HttpResponse &response)
{
	source = request.getQueryParameter("source").toInt();
	int sourceID = 0;
    lcd.home ();                   // go home
    lcd.setCursor(0,0);
	
	if( source == 0){
       sourceID = 0xC7;
       lcd.print("     5.1        ");
    }else if( source == 1){
       sourceID =  0xCB;
       lcd.print("     AUX 2      ");
    }else if( source == 2){
       sourceID =  0xCA;
  	   lcd.print("     AUX 2      ");
    }else if( source == 3){
       sourceID = 0xC9;
	   lcd.print("     AUX 3      ");
    }else if( source == 4){
       sourceID = 0xC8;
	   lcd.print("     AUX 4      ");
    }
    
    Wire.beginTransmission(PT2323_ADDRESS);
  	Wire.write(DEVICE_REG_MODE1);
  	Wire.write(sourceID);
  	Wire.endTransmission();
    
	JsonObjectStream* stream = new JsonObjectStream();
	JsonObject& json = stream->getRoot();
	json["status"] = (bool)true;
	json["source"] = source;
	response.sendJsonObject(stream);
}
void onState(HttpRequest &request, HttpResponse &response)
{
	
	//systemTime = SystemClock.getSystemTimeString();
	
	int freq = request.getQueryParameter("value").toInt();
	JsonObjectStream* stream = new JsonObjectStream();
	JsonObject& json = stream->getRoot();
	json["status"] = (bool)true;
	
//	String timeString = SystemClock.getSystemTimeString();
	
	json["source"] = source;
	json["backlight"] = backlight;
	json["mixing"] = mixing;
	json["enhance"] = enhance;
	json["mute"] = mute;
	json["time"] =  SystemClock.getSystemTimeString().c_str();
	json["volumeFR"] = 74-volumeFR;
	json["volumeFL"] = 74-volumeFL;
	json["volumeRR"] = 74-volumeRR;
	json["volumeRL"] = 74-volumeRL;
	json["volumeCEN"] = 74-volumeCEN;
	json["volumeSW"] = 74-volumeSW;
	json["frequency"] = frequency;
	json["volumeALLCH"] = 74-volumeALLCH;
	response.sendJsonObject(stream);
}

void onBacklight(HttpRequest &request, HttpResponse &response)
{
	
	backlight = request.getQueryParameter("state").toInt();
	
	if(backlight==1){
		lcd.backlight();
	}else{
		lcd.noBacklight();
	}
	
	JsonObjectStream* stream = new JsonObjectStream();
	JsonObject& json = stream->getRoot();
	json["status"] = (bool)true;
	response.sendJsonObject(stream);
}
void onExtra(HttpRequest &request, HttpResponse &response)
{
	
	int state = request.getQueryParameter("state").toInt();
	if(state==1){
		mixing = 1;
		lcd.setCursor(0,1);
		lcd.print("   Mixed ON     ");	
		Wire.beginTransmission(PT2323_ADDRESS); 
		Wire.write(DEVICE_REG_MODE1);
		Wire.write(0x90);
		Wire.endTransmission();
	}else if(state==2){
		mixing = 0;
		lcd.setCursor(0,1);
		lcd.print("   Mixed OFF    ");	
		Wire.beginTransmission(PT2323_ADDRESS); 
		Wire.write(DEVICE_REG_MODE1);
		Wire.write(0x91);
		Wire.endTransmission();
	}else if(state==3){
		enhance = 1;
		lcd.setCursor(0,1);
		lcd.print("  Enhanced ON   ");	
		Wire.beginTransmission(PT2323_ADDRESS); 
		Wire.write(DEVICE_REG_MODE1);
		Wire.write(0xD0);
		Wire.endTransmission();
	}else if(state==4){
		enhance = 0;
		lcd.setCursor(0,1);
		lcd.print("  Enhanced OFF  ");	
		Wire.beginTransmission(PT2323_ADDRESS); 
		Wire.write(DEVICE_REG_MODE1);
		Wire.write(0xD1);
		Wire.endTransmission();
	}
	
	
	JsonObjectStream* stream = new JsonObjectStream();
	JsonObject& json = stream->getRoot();
	json["status"] = (bool)true;
	response.sendJsonObject(stream);
}



void onTune(HttpRequest &request, HttpResponse &response)
{
	
	frequency = request.getQueryParameter("freq").toFloat();
	
  	unsigned int frequencyB = 4 * (frequency * 1000000 + 225000) / 32768; 
	byte frequencyH = frequencyB >> 8;
	byte frequencyL = frequencyB & 0XFF;
	Wire.beginTransmission(0x60); 
	Wire.write(frequencyH);
	Wire.write(frequencyL);
	Wire.write(0xB0);
	Wire.write(0x10);
	Wire.write(0x00);
	Wire.endTransmission();
	delay(100);  
	
	JsonObjectStream* stream = new JsonObjectStream();
	JsonObject& json = stream->getRoot();
	json["status"] = (bool)true;
	response.sendJsonObject(stream);
	
}

void onPrintSystemTime() {
	lcd.setCursor(0,0);
	lcd.print(SystemClock.getSystemTimeString());	
}


void onNtpReceive(NtpClient& client, time_t timestamp) {
	SystemClock.setTime(timestamp);
	Serial.println(SystemClock.getSystemTimeString());
}



void startWebServer()
{
	server.listen(80);
	server.addPath("/", onIndex);
	server.addPath("/lcd", onLcd);
	server.addPath("/volume", onVolume);
	server.addPath("/mute", onMute);
	server.addPath("/source", onSource);
	server.addPath("/state", onState);
	server.addPath("/backlight", onBacklight);
	server.addPath("/tune", onTune);
	server.addPath("/extra", onExtra);
	
	server.setDefaultHandler(onFile);

	Serial.println("\r\n=== WEB SERVER STARTED ===");
	Serial.println(WifiStation.getIP());
    lcd.clear();
    lcd.setCursor(1,1);
	lcd.print(WifiStation.getIP());
}

void startFTP()
{
	if (!fileExist("index.html"))
		fileSetContent("index.html", "<h3>Please connect to FTP and upload files from folder 'web/build' (details in code)</h3>");
	ftp.listen(21);
	ftp.addUser("me", "123"); // FTP account
}

void connectOk()
{
	Serial.println("I'm CONNECTED");
	clockClient = new ntpClientSystem();
	startFTP();
	startWebServer();
}



void init()
{
	Wire.begin();	
	lcd.begin(16,2);               // initialize the lcd 

	for(int i = 0; i< 3; i++)
	{
		lcd.backlight();
		delay(150);
		lcd.noBacklight();
		delay(250);
	}
	lcd.backlight();
	
    lcd.setCursor(0,0);
    lcd.clear();
    lcd.print("    Music Box   ");
    lcd.setCursor(0,1);
    lcd.print("    Geek Labs   ");
	SystemClock.setTimeZone(3);
	printTimer.initializeMs(1000*60, onPrintSystemTime).start();
	
	Serial.begin(SERIAL_BAUD_RATE); // 115200 by default
	Serial.systemDebugOutput(true); // Enable debug output to serial
	WifiStation.enable(true);
	WifiStation.config(WIFI_SSID, WIFI_PWD);
	WifiAccessPoint.enable(false);
	
	Wire.beginTransmission(PT2258_ADDRESS); 
	Wire.write(0xC0);
	Wire.endTransmission();
		
	// Run our method when station was connected to AP
	WifiStation.waitConnection(connectOk);
}
#include <user_config.h>
#include <SmingCore/SmingCore.h>
#include <Libraries/LiquidCrystal/LiquidCrystal_I2C.h>
#include <Libraries/LiquidCrystal/LiquidCrystal_I2C.h>
#include <Wire.h>

// If you want, you can define WiFi settings globally in Eclipse Environment Variables

#define WIFI_SSID "OpenWrt" // Put you SSID and Password here
#define WIFI_PWD "testing123"

#define I2C_LCD_ADDR 0x26
LiquidCrystal_I2C lcd(I2C_LCD_ADDR, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

HttpServer server;
FTPServer ftp;

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

int volumeFR = 0;
int volumeFL = 0;
int volumeRR = 0;
int volumeRL = 0;
int volumeCEN = 0;
int volumeSW = 0;
int volumeALLCH = 0;



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
	int value = request.getQueryParameter("value").toInt();
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
	json["volumeFR"] = volumeFR;
	json["volumeFL"] = volumeFL;
	json["volumeRR"] = volumeRR;
	json["volumeRL"] = volumeRL;
	json["volumeCEN"] = volumeCEN;
	json["volumeSW"] = volumeSW;
	json["volumeALLCH"] = volumeALLCH;

	response.sendJsonObject(stream);
}
void onChannel(HttpRequest &request, HttpResponse &response)
{
	int freq = request.getQueryParameter("value").toInt();
	
	JsonObjectStream* stream = new JsonObjectStream();
	JsonObject& json = stream->getRoot();
	json["status"] = (bool)true;
	response.sendJsonObject(stream);
}
void onMute(HttpRequest &request, HttpResponse &response)
{
	int freq = request.getQueryParameter("value").toInt();
	JsonObjectStream* stream = new JsonObjectStream();
	JsonObject& json = stream->getRoot();
	json["status"] = (bool)true;
	response.sendJsonObject(stream);
}
void onSource(HttpRequest &request, HttpResponse &response)
{
	int source = request.getQueryParameter("source").toInt();
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
	
	int freq = request.getQueryParameter("value").toInt();
	JsonObjectStream* stream = new JsonObjectStream();
	JsonObject& json = stream->getRoot();
	json["status"] = (bool)true;
	response.sendJsonObject(stream);
}

void onBacklight(HttpRequest &request, HttpResponse &response)
{
	
	int backlight = request.getQueryParameter("state").toInt();
	
	if(backlight){
		lcd.backlight();
	}else{
		lcd.noBacklight();
	}
	
	JsonObjectStream* stream = new JsonObjectStream();
	JsonObject& json = stream->getRoot();
	json["status"] = (bool)true;
	response.sendJsonObject(stream);
}




void startWebServer()
{
	server.listen(80);
	server.addPath("/", onIndex);
	server.addPath("/lcd", onLcd);
	server.addPath("/volume", onVolume);
	server.addPath("/channel", onChannel);
	server.addPath("/mute", onMute);
	server.addPath("/source", onSource);
	server.addPath("/state", onState);
	server.addPath("/backlight", onBacklight);
	
	
	
	server.setDefaultHandler(onFile);

	Serial.println("\r\n=== WEB SERVER STARTED ===");
	Serial.println(WifiStation.getIP());
    lcd.setCursor(0,1);
    lcd.print(WifiStation.getIP());
	
	Serial.println("==============================\r\n");
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

	Serial.begin(SERIAL_BAUD_RATE); // 115200 by default
	Serial.systemDebugOutput(true); // Enable debug output to serial
	WifiStation.enable(true);
	WifiStation.config(WIFI_SSID, WIFI_PWD);
	WifiAccessPoint.enable(false);
	
	// Run our method when station was connected to AP
	WifiStation.waitConnection(connectOk);
}


#define ENABLE_RSSI true

#include "Arduino.h"
#include "LoRa_E220.h"
#include<TinyGPSPlus.h>
#include<SoftwareSerial.h>

SoftwareSerial ss(10,11);

TinyGPSPlus gps;

SoftwareSerial mySerial(4, 5); // Arduino RX <-- e220 TX, Arduino TX --> e220 RX
LoRa_E220 e220ttl(&mySerial, 3, 7, 6); // AUX M0 M1

void setup() {
  Serial.begin(9600); 
  e220ttl.begin();
    mySerial.begin(9600);
    ss.begin(9600);
  delay(500);
  e220ttl.sendMessage("hello");

  // Startup all pins and UART

//  If you have ever change configuration you must restore It

 // Serial.println("Hi, I'm going to send message!");
  // Send message
  ResponseStatus rs = e220ttl.sendMessage("Hello, world?");
  // Check If there is some problem of succesfully send
  Serial.println(rs.getResponseDescription());
}


  String strSatellite; //捕捉衛星数
  String strHdop; //精度
  String strDate; //日付
  String strTime; //時間
  String strLat; //緯度
  String strLng; //経度
  String strAge; //経過時間
 


void loop() {

  static const double LONDON_LAT = 51.508131, LONDON_LON = -0.128002;

  printSatellites(gps.satellites.value(), gps.satellites.isValid(), 5);
  printHdop(gps.hdop.hdop(), gps.hdop.isValid(), 6, 1);
  printLat(gps.location.lat(), gps.location.isValid(), 11, 6);
  printLng(gps.location.lng(), gps.location.isValid(), 12, 6);
  printAge(gps.location.age(), gps.location.isValid(), 5);
  printDateTime(gps.date, gps.time);
  
  printFloat(gps.altitude.meters(), gps.altitude.isValid(), 7, 2);
  printFloat(gps.course.deg(), gps.course.isValid(), 7, 2);
  printFloat(gps.speed.kmph(), gps.speed.isValid(), 6, 2);
  printStr(gps.course.isValid() ? TinyGPSPlus::cardinal(gps.course.deg()) : "*** ", 6);

  unsigned long distanceKmToLondon =
    (unsigned long)TinyGPSPlus::distanceBetween(
      gps.location.lat(),
      gps.location.lng(),
      LONDON_LAT, 
      LONDON_LON) / 1000;
  printInt(distanceKmToLondon, gps.location.isValid(), 9);

  double courseToLondon =
    TinyGPSPlus::courseTo(
      gps.location.lat(),
      gps.location.lng(),
      LONDON_LAT, 
      LONDON_LON);

  printFloat(courseToLondon, gps.location.isValid(), 7, 2);

  const char *cardinalToLondon = TinyGPSPlus::cardinal(courseToLondon);

  printStr(gps.location.isValid() ? cardinalToLondon : "*** ", 6);

  printInt(gps.charsProcessed(), true, 6);
  printInt(gps.sentencesWithFix(), true, 10);
  printInt(gps.failedChecksum(), true, 9);
  Serial.println();
  
  smartDelay(1000);

  if (millis() > 5000 && gps.charsProcessed() < 10)
    Serial.println(F("No GPS data received: check wiring"));

//GPSここまで



    delay(1000);


	// If something available
  if (e220ttl.available()>1) {
	  // read the String message
#ifdef ENABLE_RSSI
	ResponseContainer rc = e220ttl.receiveMessageRSSI();
#else
	ResponseContainer rc = e220ttl.receiveMessage();
#endif
	// Is something goes wrong print error
	if (rc.status.code!=1){
		Serial.println(rc.status.getResponseDescription());
	}else{
		// Print the data received
		Serial.println(rc.status.getResponseDescription());
		Serial.println(rc.data);
#ifdef ENABLE_RSSI
		Serial.print("RSSI: "); Serial.println(rc.rssi, DEC);
#endif
	}
  }
 // if (Serial.available()) {
	  e220ttl.sendMessage(strSatellite+','+strHdop+','+strLng+','+strLat);

//if send date or time +strTime,+strDate
    
   delay(1000);
//  }
}


static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (ss.available())
      gps.encode(ss.read());
  } while (millis() - start < ms);
}

static void printLat(float val, bool valid, int len, int prec)
{
    char latChar[32];
  if (!valid)
  {
    while (len-- > 1)
      Serial.print('*');
    Serial.print(' ');
    strLat = "NoData";
  }
  else
  {
    dtostrf(val,11,6,latChar);
    strLat = latChar+'\0';
    Serial.print(val, prec);
    int vi = abs((int)val);
    int flen = prec + (val < 0.0 ? 2 : 1); // . and -
    flen += vi >= 1000 ? 4 : vi >= 100 ? 3 : vi >= 10 ? 2 : 1;
    for (int i=flen; i<len; ++i)
      Serial.print(' ');
  }
  smartDelay(0);
}

static void printLng(float val, bool valid, int len, int prec)
{
  char lngChar[32];
  
  if (!valid)
  {
    while (len-- > 1)
      Serial.print('*');
    Serial.print(' ');
    strLng = "NoData";
  }
  else
  {
    dtostrf(val,12,6,lngChar);
    strLng = lngChar+'\0';
    Serial.print(val, prec);
    int vi = abs((int)val);
    int flen = prec + (val < 0.0 ? 2 : 1); // . and -
    flen += vi >= 1000 ? 4 : vi >= 100 ? 3 : vi >= 10 ? 2 : 1;
    for (int i=flen; i<len; ++i)
      Serial.print(' ');
  }
  smartDelay(0);
}

static void printHdop(float val, bool valid, int len, int prec)
{

  char hdopChar[32];
  if (!valid)
  {
    while (len-- > 1)
      Serial.print('*');
    Serial.print(' ');
    strHdop = "NoData";
  }
  else
  {
    dtostrf(val,6,1,hdopChar);
    strHdop = hdopChar+'\0';
    Serial.print(val, prec);
    int vi = abs((int)val);
    int flen = prec + (val < 0.0 ? 2 : 1); // . and -
    flen += vi >= 1000 ? 4 : vi >= 100 ? 3 : vi >= 10 ? 2 : 1;
    for (int i=flen; i<len; ++i)
      Serial.print(' ');
  }
  smartDelay(0);
}

static void printSatellites(unsigned long val, bool valid, int len)
{
  char sz[32] = "*****************";
  strSatellite = sz+'\0';
  if (valid)
    sprintf(sz, "%ld", val);
  sz[len] = 0;
  for (int i=strlen(sz); i<len; ++i)
    sz[i] = ' ';
  if (len > 0) 
    sz[len-1] = ' ';
  strSatellite = sz+'\0';
  Serial.print(sz);
  smartDelay(0);
}

static void printAge(unsigned long val, bool valid, int len)
{
  char sz[32] = "*****************";
  strAge = sz+'\0';
  if (valid)
    sprintf(sz, "%ld", val);
  sz[len] = 0;
  for (int i=strlen(sz); i<len; ++i)
    sz[i] = ' ';
  if (len > 0) 
    sz[len-1] = ' ';
   strAge = sz+'\0';
  Serial.print(sz);
  smartDelay(0);
}

static void printDateTime(TinyGPSDate &d, TinyGPSTime &t)
{
  if (!d.isValid())
  {
    Serial.print(F("********** "));
  }
  else
  {
    char sz[32];
    sprintf(sz, "%02d/%02d/%02d ", d.month(), d.day(), d.year());
    strDate = sz+'\0';
    Serial.print(sz);

  }
  
  if (!t.isValid())
  {
    Serial.print(F("******** "));
  }
  else
  {
    char sz[32];
    sprintf(sz, "%02d:%02d:%02d ", t.hour(), t.minute(), t.second());
    strTime = sz+'\0';
    Serial.print(sz);
  }

  printInt(d.age(), d.isValid(), 5);
  smartDelay(0);
}

static void printFloat(float val, bool valid, int len, int prec)
{
  if (!valid)
  {
    while (len-- > 1)
      Serial.print('*');
    Serial.print(' ');
  }
  else
  {
    Serial.print(val, prec);
    int vi = abs((int)val);
    int flen = prec + (val < 0.0 ? 2 : 1); // . and -
    flen += vi >= 1000 ? 4 : vi >= 100 ? 3 : vi >= 10 ? 2 : 1;
    for (int i=flen; i<len; ++i)
      Serial.print(' ');
  }
  smartDelay(0);
}

static void printInt(unsigned long val, bool valid, int len)
{
  char sz[32] = "*****************";
  if (valid)
    sprintf(sz, "%ld", val);
  sz[len] = 0;
  for (int i=strlen(sz); i<len; ++i)
    sz[i] = ' ';
  if (len > 0) 
    sz[len-1] = ' ';
  Serial.print(sz);
  smartDelay(0);
}


static void printStr(const char *str, int len)
{
  int slen = strlen(str);
  for (int i=0; i<len; ++i)
    Serial.print(i<slen ? str[i] : ' ');
  smartDelay(0);
}

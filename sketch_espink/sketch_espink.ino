#include <Wire.h>
#include <WiFi.h>
//#include <WiFiManager.h>
#include <HTTPClient.h>
#include <NetworkClientSecure.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>

/*------- ePaper ----------*/
#include <GxEPD2_BW.h>
#include <Fonts/FreeSansBold9pt7b.h>
#include <Fonts/FreeSansBold12pt7b.h>
#include <Fonts/FreeSans9pt7b.h>

// ESPink-Shelf-213 GDEM0213B74 -> 2.13" 122x250, SSD1680
GxEPD2_BW<GxEPD2_213_B74, GxEPD2_213_B74::HEIGHT> display(GxEPD2_213_B74(SS, 17, 16, 4)); 

#define NTP_SERVER     "pool.ntp.org"
#define UTC_OFFSET     1
#define UTC_OFFSET_DST 1

struct wifi_cred {
  String ssid;
  String pass;  
};

const char openmeteo[] = "https://api.open-meteo.com/v1/forecast?latitude=48.95&longitude=14.46&daily=weather_code,temperature_2m_max,temperature_2m_min,apparent_temperature_max,apparent_temperature_min,sunrise,sunset,rain_sum,showers_sum,precipitation_sum,precipitation_probability_max,wind_speed_10m_max&current=temperature_2m,relative_humidity_2m,apparent_temperature,precipitation,weather_code,cloud_cover,pressure_msl,wind_speed_10m,wind_direction_10m,wind_gusts_10m&timezone=Europe%2FBerlin&forecast_days=3&wind_speed_unit=ms";
String rootCACertificate = R"(-----BEGIN CERTIFICATE-----
MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw
TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh
cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4
WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu
ZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY
MTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAK3oJHP0FDfzm54rVygc
h77ct984kIxuPOZXoHj3dcKi/vVqbvYATyjb3miGbESTtrFj/RQSa78f0uoxmyF+
0TM8ukj13Xnfs7j/EvEhmkvBioZxaUpmZmyPfjxwv60pIgbz5MDmgK7iS4+3mX6U
A5/TR5d8mUgjU+g4rk8Kb4Mu0UlXjIB0ttov0DiNewNwIRt18jA8+o+u3dpjq+sW
T8KOEUt+zwvo/7V3LvSye0rgTBIlDHCNAymg4VMk7BPZ7hm/ELNKjD+Jo2FR3qyH
B5T0Y3HsLuJvW5iB4YlcNHlsdu87kGJ55tukmi8mxdAQ4Q7e2RCOFvu396j3x+UC
B5iPNgiV5+I3lg02dZ77DnKxHZu8A/lJBdiB3QW0KtZB6awBdpUKD9jf1b0SHzUv
KBds0pjBqAlkd25HN7rOrFleaJ1/ctaJxQZBKT5ZPt0m9STJEadao0xAH0ahmbWn
OlFuhjuefXKnEgV4We0+UXgVCwOPjdAvBbI+e0ocS3MFEvzG6uBQE3xDk3SzynTn
jh8BCNAw1FtxNrQHusEwMFxIt4I7mKZ9YIqioymCzLq9gwQbooMDQaHWBfEbwrbw
qHyGO0aoSCqI3Haadr8faqU9GY/rOPNk3sgrDQoo//fb4hVC1CLQJ13hef4Y53CI
rU7m2Ys6xt0nUW7/vGT1M0NPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNV
HRMBAf8EBTADAQH/MB0GA1UdDgQWBBR5tFnme7bl5AFzgAiIyBpY9umbbjANBgkq
hkiG9w0BAQsFAAOCAgEAVR9YqbyyqFDQDLHYGmkgJykIrGF1XIpu+ILlaS/V9lZL
ubhzEFnTIZd+50xx+7LSYK05qAvqFyFWhfFQDlnrzuBZ6brJFe+GnY+EgPbk6ZGQ
3BebYhtF8GaV0nxvwuo77x/Py9auJ/GpsMiu/X1+mvoiBOv/2X/qkSsisRcOj/KK
NFtY2PwByVS5uCbMiogziUwthDyC3+6WVwW6LLv3xLfHTjuCvjHIInNzktHCgKQ5
ORAzI4JMPJ+GslWYHb4phowim57iaztXOoJwTdwJx4nLCgdNbOhdjsnvzqvHu7Ur
TkXWStAmzOVyyghqpZXjFaH3pO3JLF+l+/+sKAIuvtd7u+Nxe5AW0wdeRlN8NwdC
jNPElpzVmbUq4JUagEiuTDkHzsxHpFKVK7q4+63SM1N95R1NbdWhscdCb+ZAJzVc
oyi3B43njTOQ5yOf+1CceWxG1bQVs5ZufpsMljq4Ui0/1lvh+wjChP4kqKOJ2qxq
4RgqsahDYVvTH9w7jXbyLeiNdd8XM2w9U/t7y0Ff/9yi0GE44Za4rF2LN9d11TPA
mRGunUHBcnWEvgJBQl9nJEiU0Zsnvgc/ubhPgXRR4Xq37Z0j4r7g1SgEEzwxA57d
emyPxgcYxn/eR44/KJ4EBs+lVDR3veyJm+kXQ99b21/+jh5Xos1AnX5iItreGCc=
-----END CERTIFICATE-----
)";


/* explanation of weather codes */
String get_weather_name(int code)
{
  if (code == 0) return "jasno";
  if (code == 1) return "skoro jasno";
  if (code == 2) return "castecne zatazeno";
  if (code == 3) return "zatazeno";
  if (code == 45 || code == 48) return "mlha";
  if (code == 51 || code == 53 || code == 55) return "mrholeni";
  if (code == 56 || code == 57) return "mrznouci mrholeni";
  if (code == 61) return "slaby dest";
  if (code == 63) return "dest";
  if (code == 65) return "silny dest";
  if (code == 66 || code == 67) return "mrznouci dest";
  if (code == 71) return "slabe snezeni";
  if (code == 73) return "snezeni";
  if (code == 75) return "silne snezeni";
  if (code == 77) return "kroupy";
  if (code == 80 || code == 81 || code == 82) return "prehanky";
  if (code == 85 || code == 86) return "snehove prehanky";
  if (code == 95) return "bourky";
  if (code == 96 || code == 99) return "silne bourky";
  return "";
}

/* return precipitation probability if weather code to rain */
String get_prec_prob(int code, float prob)
{
  if (code >= 61 && code <= 99 && prob >= 25) {
    return String(round(prob), 0) + "%";
  }
  else {
    return "";
  }
} 


/* return direction of wind from angle value */
String get_wind_name(float v_D)
{
  if (v_D >= 0 && v_D < 25 || v_D > 335)
    return "S";
  else if (v_D >= 25 && v_D <= 65)
    return "SV";
  else if (v_D > 65 && v_D < 115)
    return "V";
  else if (v_D >= 115 && v_D <= 155)
    return "JV";
  else if (v_D > 155 && v_D < 205)
    return "J";
  else if (v_D >= 205 && v_D <= 245)
    return "JZ";
  else if (v_D > 245 && v_D < 295)
    return "Z";
  else if (v_D >= 295 && v_D <= 335)
    return "SZ";
  else
    return "";
}

const char* parse_dt(String dt)
{
  String datum;
  dt.replace("T", " ");
  return dt.c_str();
}

/*------- Update ePaper ----------*/
void print_last_values(JsonDocument doc)
{
  display.setFont(&FreeSansBold9pt7b);
  display.setCursor(5, 20);
  display.printf("%s", parse_dt(doc["current"]["time"]));

  display.setFont(&FreeSansBold12pt7b);
  display.setCursor(160, 22);
  display.printf("%.1f 'C", (float) doc["current"]["temperature_2m"]);

  display.setFont(&FreeSans9pt7b);
  display.setCursor(5, 42);
  display.printf("%s %.0f m/s   %.0f %%   %.0f hPa",
    get_wind_name(doc["current"]["wind_direction_10m"]),
    round((float) doc["current"]["wind_speed_10m"]),
    round((float) doc["current"]["relative_humidity_2m"]),
    round((float) doc["current"]["pressure_msl"]));

  display.setCursor(5, 75);
  display.printf("Dnes: %.0f/%.0f 'C, %s %s",
    round((float) doc["daily"]["temperature_2m_max"][0]),
    round((float) doc["daily"]["temperature_2m_min"][0]),
    get_weather_name((int) doc["daily"]["weather_code"][0]),
    get_prec_prob((int) doc["daily"]["weather_code"][0],
                  (float) doc["daily"]["precipitation_probability_max"][0])
  );

  display.setCursor(5, 105);
  display.printf("Zitra: %.0f/%.0f 'C, %s %s",
    round((float) doc["daily"]["temperature_2m_max"][1]),
    round((float) doc["daily"]["temperature_2m_min"][1]),
    get_weather_name((int) doc["daily"]["weather_code"][1]),
    get_prec_prob((int) doc["daily"]["weather_code"][1],
                  (float) doc["daily"]["precipitation_probability_max"][1])
  );

  display.display(false);
  display.powerOff();
  digitalWrite(2, LOW);   // turn off the ePaper
}

void setup() {
  Serial.begin(115200);

  /*------- WiFi Manager init ----------
  bool wifi_res;
  WiFiManager wifiManager;
  //wifiManager.resetSettings();
  wifiManager.setConfigPortalTimeout(60); // seconds
  wifi_res = wifiManager.autoConnect("simplesp", "password");
  if (wifi_res) {
    Serial.println("WiFi connected");
  }
  else {
    Serial.println("WiFi failed, going to sleep");
    deep_sleep(360);
  }*/
  /*------- Static WiFi configuration */
  Serial.println("WiFi start");
  wifi_cred creds[] = {
    {"hotspot", "password"}
  };
  const int numCreds = sizeof(creds) / sizeof(creds[0]);
  for (int j = 0; j < numCreds; ++j) {
    Serial.println(creds[j].ssid);
    WiFi.disconnect(true, true);
    WiFi.mode(WIFI_OFF); // workaround as disconnect does not work correctly - https://github.com/espressif/arduino-esp32/issues/11742
    WiFi.begin(creds[j].ssid, creds[j].pass);
    delay(5000);
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("WiFi connected");
      break;
    }
    if (j == n - 1) {
      Serial.println("No WiFi available. Sleep 6 hours.");
      deep_sleep(360);
    }
  }

  /*------- Read CA certificate from SPIFFS -------
  if (!SPIFFS.begin(true)) {
    Serial.println("Could not mount SPIFFS");
    return;
  }
  File cacert = SPIFFS.open("/cacert.txt", "r");
  if (!cacert) {
    Serial.println("Could not read CA certificate");
    return;
  }
  while (cacert.available()) {
    rootCACertificate = cacert.readString();
  }
  cacert.close();*/

  /*------- Enable voltage for ePaper ----------*/
  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH);   // turn on the ePaper
  delay(100);

  Wire.begin(21, 22);
  while (!Serial) {
    ; // wait for serial
  }

  /*------- ePaper init ----------*/
  display.init(); // ePaper init
  display.setRotation(1);
  display.setTextColor(GxEPD_BLACK); // Set text color
  display.fillScreen(GxEPD_WHITE); // set the background to white
}
 
void loop() {
  NetworkClientSecure *client = new NetworkClientSecure;
  client->setCACert(rootCACertificate.c_str());
  HTTPClient https;

  int i = 0;
  int statusCode = -1;
  https.begin(*client, openmeteo);
  https.addHeader("Accept", "application/json");
  
  do {
    delay(i * 10000);
    statusCode = https.GET();
    ++i;
    if (statusCode != HTTP_CODE_OK) {
      Serial.printf("Https GET... failed, error: %s\n", https.errorToString(statusCode).c_str());
    }
  } while (i < 3 && statusCode != HTTP_CODE_OK);

  String response = https.getString();
  Serial.printf("Response status: %d, length: %d\n", statusCode, response.length());

  JsonDocument doc;
  deserializeJson(doc, response);
  print_last_values(doc);

  deep_sleep(15);
}

// start deep sleep for several minutes
void deep_sleep(int sleep_duration)
{
  // sleep for some time
  Serial.printf("Going to sleep for %d minutes", sleep_duration);
  Serial.flush();
  esp_sleep_enable_timer_wakeup(sleep_duration * 60 * 1E6);
  esp_deep_sleep_start();
}

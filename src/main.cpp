// #include <Arduino.h>
// #include <WiFi.h>
// #include <FirebaseESP32.h>

// // Provide the RTDB payload printing info and other helper functions.
// #include <addons/RTDBHelper.h>

// /* 1. Define the WiFi credentials */
// #define WIFI_SSID "uaifai-apolo"
// #define WIFI_PASSWORD "bemvindoaocesar"

// /* 2. Define the RTDB URL and database secret */
// #define DATABASE_URL "https://poolsystem2024-default-rtdb.firebaseio.com"
// #define DATABASE_SECRET "h3qXY75zemWd9hFkwdDRXHSTpP4neNlBAEvc4Uof"

// // define sound speed in cm/uS
// #define SOUND_SPEED 0.034
// #define CM_TO_INCH 0.393701

// /* 3. Define the Firebase Data object */
// FirebaseData fbdo;

// /* 4. Define the FirebaseAuth data for authentication data */
// FirebaseAuth auth;

// /* Define the FirebaseConfig data for config data */
// FirebaseConfig config;

// unsigned long previousMillisFirebase = 0;
// unsigned long previousMillisPotentiometer = 0;
// unsigned long previousMillisBlink = 0;
// const unsigned long blinkInterval = 500; // Blink interval in milliseconds

// long duration;
// float distanceCm;
// float distanceInch;

// const int TRIGGER_PIN = 18;
// const int ECHO_PIN = 21;
// const int LED_PIN1 = 14; // Corresponds to LED_PA8
// const int LED_PIN2 = 4;  // Corresponds to LED_PA9
// const int BUZZER_PIN = 19;
// const int BUTTON_PIN = 5;
// const int POTENTIOMETER_PIN = 34; // ADC1_CHANNEL_5

// int ledState1 = 0;
// int previousButtonState1 = HIGH;
// int ledState2 = 0;
// bool alertaAtivo = false;
// bool sistemaHabilitado = false;
// bool sistemaHabilitadoLocal = false;
// bool sistemaHabilitadoFirebase = false;
// float distInCm = 0;
// uint32_t adcValue = 0;
// uint32_t pwmDutyCycle = 128; // Initialize to mid-value

// void setup()
// {
//   Serial.begin(115200);
//   Serial.println("Starting...");

//   pinMode(TRIGGER_PIN, OUTPUT);
//   pinMode(ECHO_PIN, INPUT);
//   pinMode(LED_PIN1, OUTPUT);
//   pinMode(LED_PIN2, OUTPUT);
//   pinMode(BUZZER_PIN, OUTPUT);
//   pinMode(BUTTON_PIN, INPUT_PULLUP);

//   // Initialize PWM for buzzer
//   ledcSetup(0, 2000, 8); // Channel 0, 2 kHz frequency, 8-bit resolution
//   ledcAttachPin(BUZZER_PIN, 0);

//   // Initialize WiFi
//   WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
//   Serial.print("Connecting to Wi-Fi");
//   while (WiFi.status() != WL_CONNECTED)
//   {
//     Serial.print(".");
//     delay(300);
//   }
//   Serial.println();
//   Serial.print("Connected with IP: ");
//   Serial.println(WiFi.localIP());
//   Serial.println();

//   Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

//   /* Assign the database URL and database secret (required) */
//   config.database_url = DATABASE_URL;
//   config.signer.tokens.legacy_token = DATABASE_SECRET;

//   Firebase.reconnectNetwork(true);
//   fbdo.setBSSLBufferSize(4096, 1024);

//   /* Initialize the library with the Firebase auth and config */
//   Firebase.begin(&config, &auth);

//   // Initialize previousMillisBlink
//   previousMillisBlink = millis();
// }

// void handleUARTCommands()
// {
//   if (Serial.available() > 0)
//   {
//     char comando = Serial.read();
//     switch (comando)
//     {
//     case 'O':
//     case 'o':
//     case '1':
//       sistemaHabilitadoLocal = true;
//       Serial.println("System Enabled via UART");
//       break;

//     case 'F':
//     case 'f':
//     case '0':
//       sistemaHabilitadoLocal = false;
//       alertaAtivo = false; // Ensure alert is disabled
//       Serial.println("System Disabled via UART");
//       break;

//     default:
//       Serial.println("Invalid command");
//       break;
//     }
//   }
// }

// void handleButton()
// {
//   int buttonState1 = digitalRead(BUTTON_PIN);

//   // Serial.println("buttonState1: ");
//   // Serial.println(buttonState1);

//   // Serial.println("previousButtonState1: ");
//   // Serial.println(previousButtonState1);

//   if (buttonState1 == LOW && previousButtonState1 == HIGH)
//   {
//     // Adiciona um pequeno delay para debounce do botão
//     delay(50);

//     // Ler o valor atual de 'int' no Firebase
//     int currentValue;
//     if (Firebase.getInt(fbdo, F("/test/int"), &currentValue))
//     {
//       // Fazer toggle do valor (0 -> 1 ou 1 -> 0)
//       int newValue = currentValue == 0 ? 1 : 0;

//       // Atualizar o valor no Firebase
//       if (Firebase.setInt(fbdo, F("/test/int"), newValue))
//       {
//         Serial.printf("Updated int value to %d\n", newValue);
//       }
//       else
//       {
//         Serial.println(fbdo.errorReason());
//       }
//     }
//     else
//     {
//       Serial.println(fbdo.errorReason());
//     }
//   }

//   // Atualiza o estado anterior do botão
//   previousButtonState1 = buttonState1;
// }

// void handleSimpleSensorTest()
// {
//   // Clears the trigPin
//   digitalWrite(TRIGGER_PIN, LOW);
//   delayMicroseconds(2);
//   // Sets the TRIGGER_PIN on HIGH state for 10 micro seconds
//   digitalWrite(TRIGGER_PIN, HIGH);
//   delayMicroseconds(10);
//   digitalWrite(TRIGGER_PIN, LOW);

//   // Reads the echoPin, returns the sound wave travel time in microseconds
//   duration = pulseIn(ECHO_PIN, HIGH, 50000);

//   // Calculate the distance
//   distanceCm = duration * SOUND_SPEED / 2;

//   // Convert to inches
//   distanceInch = distanceCm * CM_TO_INCH;

//   // Prints the distance in the Serial Monitor
//   // Serial.print("Distance (cm): ");
//   // Serial.println(distanceCm);
//   // Serial.print("Distance (inch): ");
//   // Serial.println(distanceInch);
// }

// void handleSensor()
// {
//   if (sistemaHabilitado)
//   {
//     // Measure distance
//     digitalWrite(TRIGGER_PIN, LOW);
//     delayMicroseconds(2);
//     digitalWrite(TRIGGER_PIN, HIGH);
//     delayMicroseconds(10);
//     digitalWrite(TRIGGER_PIN, LOW);

//     duration = pulseIn(ECHO_PIN, HIGH, 300000);
//     distInCm = duration * SOUND_SPEED / 2;

//     Serial.println("Tempo bruto: ");
//     Serial.println(duration);

//     Serial.println("Distancia: ");
//     Serial.println(distInCm);

//     if (distInCm > 0 && distInCm < 20.0)
//     {
//       alertaAtivo = true;
//     }

//     if (alertaAtivo)
//     {
//       unsigned long currentMillis = millis();
//       if (currentMillis - previousMillisBlink >= blinkInterval)
//       {
//         previousMillisBlink = currentMillis;
//         // Toggle LED_PIN2
//         ledState2 = !ledState2;
//         digitalWrite(LED_PIN2, ledState2 ? HIGH : LOW);
//       }

//       // Activate buzzer
//       ledcWrite(0, pwmDutyCycle); // Set the duty cycle

//       // ENVIAR alerta para o firebase

//       int iVal = 0;

//       if (Firebase.getInt(fbdo, F("/test/alertstate"), &iVal))
//       {
//         // Serial.printf("Get alertstate ref... %d\n", iVal);

//         if (iVal == 0)
//         {
//           if (Firebase.setInt(fbdo, F("/test/alertstate"), 1))
//           {
//             Serial.printf("Updated alertstate value to %d\n", 1);
//           }
//           else
//           {
//             Serial.println(fbdo.errorReason());
//           }
//         }
//       }
//     }
//   }
//   else
//   {
//     alertaAtivo = false;
//     digitalWrite(LED_PIN2, LOW);
//     ledcWrite(0, 0); // Turn off buzzer
//   }
// }

// void handlePotentiometer()
// {
//   // Always update pwmDutyCycle to ensure buzzer works when alert is active
//   adcValue = analogRead(POTENTIOMETER_PIN);
//   pwmDutyCycle = map(adcValue, 0, 4095, 0, 255);
// }

// void handleFirebase()
// {
//   if (millis() - previousMillisFirebase > 500)
//   {
//     previousMillisFirebase = millis();

//     int iVal = 0;

//     if (Firebase.getInt(fbdo, F("/test/int"), &iVal))
//     {
//       // Serial.printf("Get int ref... %d\n", iVal);

//       // Update the Firebase control flag
//       sistemaHabilitadoFirebase = (iVal > 0);
//     }
//     else
//     {
//       Serial.println(fbdo.errorReason());
//       // Do not change sistemaHabilitadoFirebase on Firebase error
//     }
//   }
// }

// void updateSystemState()
// {
//   // Update the combined system state
//   sistemaHabilitado = sistemaHabilitadoLocal || sistemaHabilitadoFirebase;

//   // Control LED_PIN1 based on sistemaHabilitado
//   digitalWrite(LED_PIN1, sistemaHabilitado ? HIGH : LOW);

//   // If the system is disabled, ensure alerts are turned off
//   if (!sistemaHabilitado)
//   {
//     alertaAtivo = false;
//     digitalWrite(LED_PIN2, LOW);
//     ledcWrite(0, 0); // Turn off buzzer
//     int iVal = 0;

//     if (Firebase.getInt(fbdo, F("/test/alertstate"), &iVal))
//     {
//       // Serial.printf("Get alertstate ref... %d\n", iVal);

//       if (iVal == 1)
//       {
//         if (Firebase.setInt(fbdo, F("/test/alertstate"), 0))
//         {
//           Serial.printf("Updated alertstate value to %d\n", 0);
//         }
//         else
//         {
//           Serial.println(fbdo.errorReason());
//         }
//       }
//     }
//   }
// }

// void loop()
// {
//   handleUARTCommands();
//   // if (digitalRead(BUTTON_PIN))
//   // Serial.println("b press");
//   handleButton();

//   handleFirebase();
//   updateSystemState();

//   handlePotentiometer();

//   handleSensor();
//   // handleSimpleSensorTest();

//   // Small delay to prevent WDT reset
//   delay(1);
// }

#include <Arduino.h>
#include <WiFi.h>
#include <FirebaseESP32.h>

// Provide the RTDB payload printing info and other helper functions.
#include <addons/RTDBHelper.h>

/* 1. Define the WiFi credentials */
#define WIFI_SSID "uaifi-apolo"
#define WIFI_PASSWORD "bemvindoaocesar"

/* 2. Define the RTDB URL and database secret */
#define DATABASE_URL "https://poolsystem2024-default-rtdb.firebaseio.com"
#define DATABASE_SECRET "h3qXY75zemWd9hFkwdDRXHSTpP4neNlBAEvc4Uof"

// Define sound speed in cm/uS
#define SOUND_SPEED 0.034
#define CM_TO_INCH 0.393701

/* 3. Define the Firebase Data object */
FirebaseData fbdo;

/* 4. Define the FirebaseAuth data for authentication data */
FirebaseAuth auth;

/* Define the FirebaseConfig data for config data */
FirebaseConfig config;

unsigned long previousMillisFirebase = 0;
unsigned long previousMillisPotentiometer = 0;
unsigned long previousMillisBlink = 0;
const unsigned long blinkInterval = 500; // Blink interval in milliseconds

long duration;
float distanceCm;
float distanceInch;

const int TRIGGER_PIN = 18;
const int ECHO_PIN = 21;
const int LED_PIN1 = 14; // Corresponds to LED_PA8
const int LED_PIN2 = 4;  // Corresponds to LED_PA9
const int BUZZER_PIN = 19;
const int BUTTON_PIN = 5;
const int POTENTIOMETER_PIN = 34; // ADC1_CHANNEL_5

int ledState1 = 0;
int previousButtonState1 = HIGH;
int ledState2 = 0;
bool alertaAtivo = false;
bool sistemaHabilitado = false;
bool sistemaHabilitadoLocal = false;
bool sistemaHabilitadoFirebase = false;
float distInCm = 0;
float maxDistance = 20.0; // Default max distance (cm)
uint32_t adcValue = 0;
uint32_t pwmDutyCycle = 128; // Initialize to mid-value

void setup()
{
  Serial.begin(115200);
  Serial.println("Starting...");

  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(LED_PIN1, OUTPUT);
  pinMode(LED_PIN2, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // Initialize PWM for buzzer
  ledcSetup(0, 2000, 8); // Channel 0, 2 kHz frequency, 8-bit resolution
  ledcAttachPin(BUZZER_PIN, 0);

  // Initialize WiFi
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

  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

  /* Assign the database URL and database secret (required) */
  config.database_url = DATABASE_URL;
  config.signer.tokens.legacy_token = DATABASE_SECRET;

  Firebase.reconnectNetwork(true);
  fbdo.setBSSLBufferSize(4096, 1024);

  /* Initialize the library with the Firebase auth and config */
  Firebase.begin(&config, &auth);

  // Initialize previousMillisBlink
  previousMillisBlink = millis();
}

void handleFirebase()
{
  if (millis() - previousMillisFirebase > 2000)
  {
    previousMillisFirebase = millis();

    int iVal = 0;

    if (Firebase.getInt(fbdo, F("/test/int"), &iVal))
    {
      sistemaHabilitadoFirebase = (iVal > 0);
    }
    else
    {
      Serial.println(fbdo.errorReason());
    }

    // Get maxDistance from Firebase
    float distanceValue = 0;
    if (Firebase.getFloat(fbdo, F("/test/distance"), &distanceValue))
    {
      maxDistance = distanceValue;
      Serial.printf("Max distance: %.2f cm\n", maxDistance);
    }
    else
    {
      Serial.println(fbdo.errorReason());
    }
  }
}

void handleUARTCommands()
{
  if (Serial.available() > 0)
  {
    char comando = Serial.read();
    switch (comando)
    {
    case 'O':
    case 'o':
    case '1':
      sistemaHabilitadoLocal = true;
      Serial.println("System Enabled via UART");
      break;

    case 'F':
    case 'f':
    case '0':
      sistemaHabilitadoLocal = false;
      alertaAtivo = false; // Ensure alert is disabled
      Serial.println("System Disabled via UART");
      break;

    default:
      Serial.println("Invalid command");
      break;
    }
  }
}

void handleButton()
{
  int buttonState1 = digitalRead(BUTTON_PIN);

  // Serial.println("buttonState1: ");
  // Serial.println(buttonState1);

  // Serial.println("previousButtonState1: ");
  // Serial.println(previousButtonState1);

  if (buttonState1 == LOW && previousButtonState1 == HIGH)
  {
    // Adiciona um pequeno delay para debounce do botão
    delay(50);

    // Ler o valor atual de 'int' no Firebase
    int currentValue;
    if (Firebase.getInt(fbdo, F("/test/int"), &currentValue))
    {
      // Fazer toggle do valor (0 -> 1 ou 1 -> 0)
      int newValue = currentValue == 0 ? 1 : 0;

      // Atualizar o valor no Firebase
      if (Firebase.setInt(fbdo, F("/test/int"), newValue))
      {
        Serial.printf("Updated int value to %d\n", newValue);
      }
      else
      {
        Serial.println(fbdo.errorReason());
      }
    }
    else
    {
      Serial.println(fbdo.errorReason());
    }
  }

  // Atualiza o estado anterior do botão
  previousButtonState1 = buttonState1;
}

void handlePotentiometer()
{
  // Always update pwmDutyCycle to ensure buzzer works when alert is active
  adcValue = analogRead(POTENTIOMETER_PIN);
  pwmDutyCycle = map(adcValue, 0, 4095, 0, 255);
}

void handleSensor()
{
  if (sistemaHabilitado)
  {
    digitalWrite(TRIGGER_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIGGER_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIGGER_PIN, LOW);

    duration = pulseIn(ECHO_PIN, HIGH, 300000);
    distInCm = duration * SOUND_SPEED / 2;

    // Serial.println("Tempo bruto: ");
    // Serial.println(duration);

    // Serial.println("Distancia: ");
    // Serial.println(distInCm);

    if (distInCm > 0 && distInCm < maxDistance)
    {
      alertaAtivo = true;
    }

    if (alertaAtivo)
    {
      unsigned long currentMillis = millis();
      if (currentMillis - previousMillisBlink >= blinkInterval)
      {
        previousMillisBlink = currentMillis;
        ledState2 = !ledState2;
        digitalWrite(LED_PIN2, ledState2 ? HIGH : LOW);
      }

      ledcWrite(0, pwmDutyCycle); // Set the duty cycle

      int iVal = 0;

      if (Firebase.getInt(fbdo, F("/test/alertstate"), &iVal))
      {
        if (iVal == 0)
        {
          if (Firebase.setInt(fbdo, F("/test/alertstate"), 1))
          {
            Serial.printf("Updated alertstate value to %d\n", 1);
          }
          else
          {
            Serial.println(fbdo.errorReason());
          }
        }
      }
    }
  }
  else
  {
    alertaAtivo = false;
    digitalWrite(LED_PIN2, LOW);
    ledcWrite(0, 0); // Turn off buzzer
  }
}

void updateSystemState()
{
  // Update the combined system state
  sistemaHabilitado = sistemaHabilitadoLocal || sistemaHabilitadoFirebase;

  // Control LED_PIN1 based on sistemaHabilitado
  digitalWrite(LED_PIN1, sistemaHabilitado ? HIGH : LOW);

  // If the system is disabled, ensure alerts are turned off
  if (!sistemaHabilitado)
  {
    alertaAtivo = false;
    digitalWrite(LED_PIN2, LOW);
    ledcWrite(0, 0); // Turn off buzzer
    int iVal = 0;

    if (Firebase.getInt(fbdo, F("/test/alertstate"), &iVal))
    {
      // Serial.printf("Get alertstate ref... %d\n", iVal);

      if (iVal == 1)
      {
        if (Firebase.setInt(fbdo, F("/test/alertstate"), 0))
        {
          Serial.printf("Updated alertstate value to %d\n", 0);
        }
        else
        {
          Serial.println(fbdo.errorReason());
        }
      }
    }
  }
}

void loop()
{
  handleUARTCommands();
  handleButton();
  handleFirebase();
  updateSystemState();
  handlePotentiometer();
  handleSensor();
  delay(1);
}

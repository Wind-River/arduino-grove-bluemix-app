/* 
 * Copyright (c) 2016, Wind River Systems, Inc. 
 * 
 * Redistribution and use in source and binary forms, with or without modification, are 
 * permitted provided that the following conditions are met: 
 * 
 * 1) Redistributions of source code must retain the above copyright notice, 
 * this list of conditions and the following disclaimer. 
 * 
 * 2) Redistributions in binary form must reproduce the above copyright notice, 
 * this list of conditions and the following disclaimer in the documentation and/or 
 * other materials provided with the distribution. 
 * 
 * 3) Neither the name of Wind River Systems nor the names of its contributors may be 
 * used to endorse or promote products derived from this software without specific 
 * prior written permission. 
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE 
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE 
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */

/* 
 * DESCRIPTION
 * An example of collecting temperature data from a Grove Kit sensor (or, if the
 * kit is not available, generating some dummy data) and sending it to IBM 
 * Watson IoT cloud in either Quickstart or Registered mode. 
 * 
 * The code can be configured using the #define's a little lower down.
 * 
 * If using the Grove Start Kit then connect the Grove shield to the main board 
 * and connect the following Grove devices for this example:
 *    Connector A0 = temperature sensor
 *    I2C          = LCD display
 */

#include <Arduino-lite.h>

/* Set the following defines to configure the code:
 *  ENABLE_GROVE_KIT - define this if you have a Grove Starter Kit connected to 
 *                     the Galileo Gen2. Connect the LCD display to an I2C
 *                     connector and the temperature sensor to the A0 connector.
 *  ENABLE_BLUEMIX   - define this to enable connection to IBM Watson IoT. This 
 *                     will default to Quickstart mode unless the next macro
 *                     (ENABLE_BLUEMIX_REGISTERED) is also set.
 *  ENABLE_BLUEMIX_REGISTERED - define this macro if you are connecting to IBM
 *                     Watson IoT in registered mode. You must also define the 
 *                     previous macro (ENABLE_BLUEMIX). For Registered mode you
 *                     must also define the account details shown in the next
 *                     section.
 */
#define ENABLE_GROVE_KIT
#define ENABLE_BLUEMIX
#undef  ENABLE_BLUEMIX_REGISTERED   /* ENABLE_BLUEMIX must also be defined ! */

/* The following are only used (and must be set) if connecting to IBM Watson in 
 * Registered mode (i.e. if both the ENABLE_BLUEMIX and ENABLE_BLUEMIX_REGISTERED
 * macros are defined). The values of these will be provided during the IBM
 * Watson IoT registration process.
 */
#if defined(ENABLE_BLUEMIX) && defined(ENABLE_BLUEMIX_REGISTERED)
 char bmID[]    = "my_Board_ID";
 char bmType[]  = "my_Board_Type";
 char bmOrg[]   = "my_Org";
 char bmToken[] = "my_Token";
#endif   /* ENABLE_BLUEMIX && ENABLE_BLUEMIX_REGISTERED */

/* Include additional libraries we will be using... */
/* ... the Wire library */
#include <Wire.h>
/* ... the rgb_lcd (Grove LCD display) library */
#include <rgb_lcd.h>

/* This float variable will be used to store sensor or generated data */
float myData;

#ifdef ENABLE_BLUEMIX
 /* If we are using IBM Watson IoT then we need a couple more include files */
 #include <jansson.h>
 #include <BluemixVxWorksClient.h>
 /* We should also create some datapoints to send to IBM Watson IoT */
 #define IOT_OS2 "Wind River VxWorks"
 #define BOARD2 "Intel Galileo Gen2"
 /* and save them into a datapoint array */
 Datapoint os("iot_os", (char *)IOT_OS2);
 Datapoint board("board", (char *)BOARD2);
 Datapoint temp("temp", &myData);
 Datapoint* datapoints[] = { &os, &board, &temp, };
#endif   /* ENABLE_BLUEMIX */

#ifdef ENABLE_GROVE_KIT
 int temp_input = A0;
 char tstring[32];
 const int B = 3975;
 int input = 0;
 float resistance;
#else
 int q = 0;
#endif   /* !ENABLE_GROVE_KIT */

#ifdef ENABLE_BLUEMIX
#ifdef ENABLE_BLUEMIX_REGISTERED
/* Prepare IBM Watson IoT in Registered mode... */
/* ...this is our callback - called whenever a command is received from IBM Watson */
void myCallback (char* commandName, char* format, void* payload)
{
    double jsontemp;
    json_t *root;
    char fstring[32];

    if(strncmp(commandName, "overtemp", 8)) {
        Serial.println((char *)"------------ UNKNOWN COMMAND RECEIVED ------------");
		Serial.print((char *)"The command received :: ");
		Serial.println(commandName);
        sprintf(fstring, "COMMAND RECEIVED");
        lcd.print(fstring, 2);
    } else {
        Serial.println((char *)"------------ OVER TEMPERATURE WARNING ------------");
		Serial.print((char *)"The command received :: ");
		Serial.println(commandName);
        if(!strncmp(format, "json", 4)) {
            printf("json payload is: %s\n", (char *)payload);
            /* first convert the json string into a Jansson object... */
            if( NULL == (root = json_loads((char *)payload, 0, NULL)) ) {
                Serial.println((char *)"Error in json object formatting");
            } else {
                /* ...now specify the format and unpack the object to find value of temp */
                if (json_unpack(root, "{s:{s:f}}", "d", "temp", &jsontemp)) {
                    Serial.println((char *)"Error in json data or format");
                } else {
					Serial.print((char *)"Indicated temperature (from json data) is : ");
					Serial.println(jsontemp);
                    sprintf(fstring, "Input - OVERTEMP");
                    lcd.print(fstring, 1);
                    sprintf(fstring, "  Temp = %.2f", jsontemp);
                    lcd.print(fstring, 2);
                }
            }
        }    	
    }
    Serial.println((char *)"--------------------------------------------------");
}

BluemixVxWorksClientClass BluemixClient(bmID, bmType, bmOrg, bmToken, myCallback);

#else

/* Prepare IBM Watson IoT in Quickstart mode... */
char bmID[] = "";          /* empty string = find MAC from board */
BluemixVxWorksClientClass BluemixClient(bmID);

#endif   /* !ENABLE_BLUE_REGISTERED */
#endif   /* ENABLE_BLUEMIX */

void setup()
{
    pinMode(IO13, OUTPUT);          /* LED fixed on board */

    analogReadResolution(10);
    analogWriteResolution(8);

#ifdef ENABLE_GROVE_KIT
    Wire.begin();
    lcd.begin(16,2);                /* set LCD to 16 columns, 2 lines */
    lcd.setRGB(0x10, 0x10, 0xe0);   /* LCD backlight goes blue */
#endif   /* ENABLE_GROVE_KIT */

#ifdef ENABLE_BLUEMIX
/* Connect to IBM Watson IoT */
    BluemixClient.connect();
#ifndef ENABLE_BLUEMIX_REGISTERED
    Serial.println((char *)"Connected to IBM Watson IoT (Quickstart mode)");
#else
    Serial.println((char *)"Connected to IBM Watson IoT (Registered mode)");
#endif   /* ENABLE_BLUEMIX_REGISTERED */
#endif   /* ENABLE_BLUEMIX */
}

void loop()
{
    digitalWrite(IO13, LOW);                /* onboard LED off */
    delay(1000);                            /* 1 sec delay */
    digitalWrite(IO13, HIGH);               /* onboard LED on */

#ifdef ENABLE_GROVE_KIT
    /* Read temperature sensor */
	input = analogRead(temp_input);
    /* show raw A0 output on LCD */
    sprintf(tstring, "Input A0 = %d", input);
    lcd.print(tstring, 1);
    /* Calculate temperature from raw data */
	if (input==0) input=1;
	resistance = (10230000 - (input * 10000)) / input;
    myData = 1 / ( log(resistance/10000) / B + (1/298.15)) - 273.15;
    /* show temp on console */
    Serial.print((char *)"Analog temperature reading from A0 is : ");
    Serial.println(myData);
    /* show temp on LCD */
    sprintf(tstring, "  Temp = %.2f", myData);
    lcd.print(tstring, 2); 
#else
    /* Generate some dummy data */
    myData = 10 + (pow(abs((q % 7) + 1), 2.1));
    q++;
    /* show data on console */
    Serial.print((char *)"Generated output data is : ");
    Serial.println(myData);
#endif   /* !ENABLE_GROVE_KIT */

#ifdef ENABLE_BLUEMIX
    /* send datapoints to IBM Watson IoT */
    BluemixClient.publish(datapoints,3);
    delay(500);         /* publish to IBM Watson includes a 500ms yield delay */
#else
    delay(1000);
#endif   /* !ENABLE_BLUEMIX */

}

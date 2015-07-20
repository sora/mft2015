#include "cointray.h"
#include "ESP8266.h"
#include <SoftwareSerial.h>


SoftwareSerial wifiserial(3, 2);
ESP8266 wifi(wifiserial);

//char *hello = "GET / HTTP/1.1\r\nConnection: keep-alive\r\n\r\n";
char *hello = "GET / HTTP/1.1\r\nHost: www.baidu.com\r\nConnection: close\r\n\r\n";
String httpCmd = "GET / HTTP/1.1\r\n";

String cmd = "AT+CIPSEND=";

void setup() {
	httpCmd += "Host: ";
	httpCmd += HOST_NAME;
//	httpCmd += "\r\n";
//	httpCmd += "Connection: keep-alive";
	httpCmd += "\r\n\r\n";
	cmd += httpCmd.length();

	Serial.begin(9600);
	while (!Serial) {
		;
	}
	wifiserial.begin(9600);

	setupWifi();
}

void loop() {
	//int sensorValue0 = analogRead(A0);
	//Serial.println(sensorValue0);
	//Serial.println("-----------");

	connectTCP();
	sendCmd();
	//sendRequest();
hoge:
	sendRequest2();
	goto hoge;

	closeTCP();

	while(1);
}

void setupWifi() {
	if (debug) {
		Serial.print("FW Version:");
		Serial.println(wifi.getVersion().c_str());
		Serial.println("-----------");
	}

	if (wifi.setOprToStationSoftAP()) {
		Serial.println(F("to station + softap ok"));
	} else {
		Serial.println(F("to station + softap err"));
	}

	if (wifi.joinAP(SSID, PASSWORD)) {
		Serial.println(F("Join AP success"));
		Serial.println(F("IP:"));
		Serial.println( wifi.getLocalIP().c_str());
	} else {
		Serial.println("Join AP failure");
	}

	if (wifi.disableMUX()) {
		Serial.println(F("single ok"));
	} else {
		Serial.println("single err");
	}
	Serial.println("setup end");
}

static inline void connectTCP() {
	if (wifi.createTCP(HOST_NAME, HOST_PORT)) {
		Serial.println("create tcp ok");
	} else {
		Serial.println("create tcp err");
	}
}

static inline void sendRequest() {
	uint8_t buf[1024] = {0};
	uint32_t len, i;

	wifi.send((const uint8_t*)hello, strlen(hello));

	len = wifi.recv(buf, sizeof(buf), 10000);
	if (len > 0) {
		Serial.print("Received:[");
		for (i = 0; i < len; i++) {
			Serial.print((char)buf[i]);
		}
		Serial.print("]\r\n");
	} else {
		Serial.println("No recieved data");
	}
}

static inline void sendRequest2() {
	char c;

	wifiserial.print(cmd);
	wifiserial.print("\r\n");
	Serial.print("\r\n");

	if(wifiserial.find(">")) {
		wifiserial.print(httpCmd);
		delay(500);
		while (wifiserial.available()) {
			c = wifiserial.read();
			Serial.write(c);
			if (c == '\r') Serial.print('\n');
		}
		Serial.print("\r\n");
	}
}

static inline void sendCmd() {
	wifiserial.print(cmd);
	wifiserial.print("\r\n");
}

static inline void sendRequest3() {
	char c;

	if (wifiserial.find(">")) {
		wifiserial.print(httpCmd);
		delay(500);

		if (wifiserial.available()) {
			while (wifiserial.available()) {
				c = wifiserial.read();
				Serial.write(c);
				if(c == '\r') Serial.print('\n');
			}
			Serial.print("\r\n");
		} else {
			Serial.println("http failed");
		}
	}
}

static inline void closeTCP() {
	if (wifi.releaseTCP()) {
		Serial.println("release tcp ok");
	} else {
		Serial.println("release tcp err");
	}
}


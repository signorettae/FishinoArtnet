/*
This example will transmit a universe via Art-Net into the Network.
This example may be copied under the terms of the MIT license, see the LICENSE file for details

29/08/2019 - Modified by Emanuele Signoretta to work on Fishino boards.
*/

#include <Fishino.h>
#include <FishinoUdp.h>
#include <FishinoArtnet.h>
#include <SPI.h>


//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
// CONFIGURATION DATA    -- ADAPT TO YOUR NETWORK !!!
// DATI DI CONFIGURAZIONE -- ADATTARE ALLA PROPRIA RETE WiFi !!!
#ifndef __MY_NETWORK_H

// here pur SSID of your network
// inserire qui lo SSID della rete WiFi
#define MY_SSID ""

// here put PASSWORD of your network. Use "" if none
// inserire qui la PASSWORD della rete WiFi -- Usare "" se la rete non ￨ protetta
#define MY_PASS ""

// here put required IP address (and maybe gateway and netmask!) of your Fishino
// comment out this lines if you want AUTO IP (dhcp)
// NOTE : if you use auto IP you must find it somehow !
// inserire qui l'IP desiderato ed eventualmente gateway e netmask per il fishino
// commentare le linee sotto se si vuole l'IP automatico
// nota : se si utilizza l'IP automatico, occorre un metodo per trovarlo !
#define IPADDR  192, 168,   1, 255
#define GATEWAY 192, 168,   1, 1
#define NETMASK 255, 255, 255, 0

#endif
//                    END OF CONFIGURATION DATA                      //
//                       FINE CONFIGURAZIONE                         //
///////////////////////////////////////////////////////////////////////

// define ip address if required
// NOTE : if your network is not of type 255.255.255.0 or your gateway is not xx.xx.xx.1
// you should set also both netmask and gateway
#ifdef IPADDR
IPAddress Ip(IPADDR);
#ifdef GATEWAY
IPAddress gw(GATEWAY);
#else
IPAddress gw(ip[0], ip[1], ip[2], 1);
#endif
#ifdef NETMASK
IPAddress nm(NETMASK);
#else
IPAddress nm(255, 255, 255, 0);
#endif
#endif


// Artnet settings
FishinoArtnet artnet;
const int startUniverse = 0; // CHANGE FOR YOUR SETUP most software this is 1, some software send out artnet first universe as 0.
const char host[] = "2.1.1.1"; // CHANGE FOR YOUR SETUP your destination

// connect to wifi – returns true if successful or false if not
boolean ConnectWifi(void) {
	boolean state = true;
	// reset and test WiFi module
	// resetta e testa il modulo WiFi
	while (!Fishino.reset())
		Serial << F("Fishino RESET FAILED, RETRYING...\n");
	Serial << F("Fishino WiFi RESET OK\n");

	Fishino.setPhyMode(PHY_MODE_11N);

	// go into station mode
	// imposta la modalità stazione
	Fishino.setMode(STATION_MODE);

	// try forever to connect to AP
	// tenta la connessione finchè non riesce
	Serial << F("Connecting to AP...");
	while (!Fishino.begin(MY_SSID, MY_PASS)) {
		Serial << ".";
		delay(2000);
	}
	Serial << "OK\n";


	// setup IP or start DHCP client
	// imposta l'IP statico oppure avvia il client DHCP
#ifdef IPADDR
	Fishino.config(Ip, gw, nm);
#else
	Fishino.staStartDHCP();
#endif

	// wait till connection is established
	Serial << F("Waiting for IP...");
	while (Fishino.status() != STATION_GOT_IP) {
		Serial << ".";
		delay(500);
	}
	Serial << "OK\n";
	return state;
}

void setup() {
	Serial.begin(115200);
	ConnectWifi();
	artnet.begin(host);
	artnet.setLength(3);
	artnet.setUniverse(startUniverse);
}

void loop() {
	uint8_t i;
	uint8_t j;

	// set the first 3 byte to all the same value. A RGB lamp will show a ramp-up white.
	for (j = 0; j < 255; j++) {
		for (i = 0; i < 3; i++) {
			artnet.setByte(i, j);
		}
		// send out the Art-Net DMX data
		artnet.write();
		delay(100);
	}
}

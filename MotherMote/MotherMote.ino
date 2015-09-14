
// nRF24L01 to Arduino Uno
// MISO -> 12
// MOSI -> 11
// SCK -> 13
// CE -> 8
// CSN -> 7
// GND -> GND
// VCC -> 3V3

// Arduino ----------------------------------------
#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>
//--------------------------------------------------

// // ATtiny85-----------------------------------------
// #include <SPI85.h>
// #include <Mirf.h>
// #include <nRF24L01.h>
// #include <MirfHardwareSpi85Driver.h>
// //--------------------------------------------------

#define MAX_NODE_ID 25

//----------------
//packet_type = 1   => 	init_request
//packet_type = 2   =>  init_reply
//packet_type = 3   =>  data_packet
//----------------

struct packet_struct{
	byte packet_type;
	byte sensornode_id;
	byte old_id;
	byte mothermote_id;
	int wakeup_delay;
	int data;
};

static packet_struct receivedPacket;

void setup(){
	//Arduino------------------------------
	Mirf.spi = &MirfHardwareSpi;
	Serial.begin(9600);
	//


	// //ATtiny85-----------------------------
	// Mirf.cePin = PB4;
	// Mirf.csnPin = PB3;
	// Mirf.spi = &MirfHardwareSpi85;		
	// //-------------------------------------
	
	Mirf.init();
	Mirf.setRADDR((byte *) "cross");
	Mirf.payload = sizeof(packet_struct);
	Mirf.channel = 90;
	Mirf.config();	
}

void loop(){
	static packet_struct reply;
	// Mirf.flushRx();
	// Mirf.flushTx();
	Mirf.setRADDR((byte *) "cross");
	while(!Mirf.dataReady()){}
	if(Mirf.dataReady()){
		Serial.println("Packet received...");
		Mirf.getData((byte *) &receivedPacket);
		Serial.print("Packet type: ");
		Serial.println(receivedPacket.packet_type);
		Serial.print("Packet data: ");
		Serial.println(receivedPacket.data);
		reply.mothermote_id = 2;
		reply.packet_type = 2;
		reply.wakeup_delay = 3500;
		delay(100);
		Mirf.setTADDR((byte *) "cross");
		Mirf.send((byte *) &reply);
		while(Mirf.isSending()){}
		Serial.println("Packet reply sent");

	}	
	delay(100);
}

// 1: clear
// 0: not clear
boolean isChannelClear(){
	byte carrier_detect_reg_value = (byte) 0;        
        //RX mode
        Mirf.ceHi();
        Mirf.readRegister(CD, &carrier_detect_reg_value, sizeof(carrier_detect_reg_value));

        //Serial.print("carrier_detect_reg_value = ");
        //Serial.println(carrier_detect_reg_value, BIN);
        return carrier_detect_reg_value == 0;
}

int generateWakeUpDelay(){
	return random(1000, 2000);
}

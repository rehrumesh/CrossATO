
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
#define MOTHERMOTE_ID 1

//----------------
//packet_type = 0   =>  mothermote beacon broadcast
//packet_type = 1   => 	init_request
//packet_type = 2   =>  init_reply
//packet_type = 3   =>  data_packet
//----------------

struct packet_struct{
	byte packet_type;
	byte sensornode_id;
	byte mothermote_id;
	int wakeup_delay;
	int data;
	unsigned long broadcastTime;
};

static packet_struct receivedPacket;
int wakeup_delay_list[MAX_NODE_ID][2] = {{0,503},{0,617},{0,683},{0,757},{0,787},{0,823},{0,863},{0,911},{0,971},{0,1019},
			   {0,1069},{0,1153},{0,1193},{0,1223},{0,1297},{0,1361},{0,1399},{0,1439},{0,1481},{0,1531},
			   {0,1597},{0,1657},{0,1697},{0,1733},{0,1801}};

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
	Mirf.setRADDR((byte *) "cross");
	while(!Mirf.dataReady()){}
	if(Mirf.dataReady()){
		Serial.println("Packet received...");
		Mirf.getData((byte *) &receivedPacket);
		Serial.print("Packet type: ");
		Serial.println(receivedPacket.packet_type);
		Serial.print("Packet data: ");
		Serial.println(receivedPacket.data);
		if(receivedPacket.packet_type == 1){
			reply.mothermote_id = MOTHERMOTE_ID;
			reply.packet_type = 2;
			reply.wakeup_delay = generateWakeUpDelay(receivedPacket.sensornode_id);
			reply.sensornode_id = receivedPacket.sensornode_id;
			delay(100);
			Mirf.setTADDR((byte *) "cross");
			Mirf.send((byte *) &reply);
			while(Mirf.isSending()){}
			Serial.println("Packet reply sent");
		}else if(receivedPacket.packet_type == 3){
			Serial.print("broadcast : ");
			Serial.println(receivedPacket.broadcastTime);
		}
		

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

int generateWakeUpDelay(int sensornode_id){
	for(int i=0; i< MAX_NODE_ID; i++){
		if(wakeup_delay_list[i][0] == 0){
			wakeup_delay_list[i][0] = sensornode_id;
			return wakeup_delay_list[i][1];
		}
	}
	return -1;
}

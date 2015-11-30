
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

#define MAX_NODES 3
#define MOTHERMOTE_ID 1

//----------------
//packet_type = 0   =>  mothermote broadcast
//packet_type = 1   => 	new node request
//packet_type = 2   =>  new node reply
//packet_type = 3   =>  data
//----------------

struct packet_struct{
	byte id;	// mothermote id range = 1-20,  sensor node id range = 30+
	byte packet_type;
	int data;
};

static packet_struct packet;
static packet_struct reply;

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
	//Mirf.setRADDR((byte *) "cross");
	Mirf.payload = sizeof(packet_struct);
	Mirf.channel = 90;
	Mirf.config();	
}

void loop(){
	//initial sensor node set 
	packet.data = MAX_NODES;
	while(true){
		unsigned long cycle_start_time = millis();
		//create broadcast packet 
		//broadcast 
		//listen till timer expire
		//total cycle time = 20sec
		
		packet.packet_type = 0;
		packet.id = MOTHERMOTE_ID;
		
		Mirf.setTADDR((byte *) "cross");
		Mirf.send((byte *) &packet);
		while(Mirf.isSending()){};

		Mirf.setRADDR((byte *) "cross");
		while(millis() - cycle_start_time >20000){
			if(Mirf.dataReady()){
				Mirf.getData((byte *) &reply);
			}
		}
	}
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


int beacondataEncoder(int cycleTime, int numberOfNodes){
	int ans;
	ans = cycleTime<<5;
	return ans + numberOfNodes;	
}

void beacondataDecoder(int data){
	int a = data % B100000;
	int b = (data - a)>>5;
        Serial.println(a);
        Serial.println(b);
}
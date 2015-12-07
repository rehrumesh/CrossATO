
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
#define CYCLE_TIME_LENGTH 9000
#define FULL_CYCLE_TIME 10000

//----------------
//packet_type = 0   =>  mothermote broadcast
//packet_type = 1   => 	new node request
//packet_type = 2   =>  new node reply
//packet_type = 3   =>  data
//----------------

struct packet_struct{
	byte sender_id;	// mothermote id range = 1-20,  sensor node id range = 30+
	byte receiver_id;
	byte packet_type;
	int data;
};

struct decodedData{
	int cycleTime;
	int nodes;
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
	int current_nodes = MAX_NODES;
	while(true){
		unsigned long cycle_start_time = millis();
		//create broadcast packet 
		//broadcast 
		//listen till timer expire
		//total cycle time = 9sec
		//new node time = 1 seconds
		//
		
		packet.packet_type = 0;
		packet.sender_id = MOTHERMOTE_ID;
		packet.data = beacondataEncoder(CYCLE_TIME_LENGTH, current_nodes);
		packet.receiver_id = 0;
		
		Serial.println("Sending beacon packet.");
		Mirf.setTADDR((byte *) "cross");
		Mirf.send((byte *) &packet);
		while(Mirf.isSending()){};

		Mirf.setRADDR((byte *) "cross");
		Serial.println("***");
		Serial.println(millis());
		Serial.println(cycle_start_time);
		while(millis() - cycle_start_time <CYCLE_TIME_LENGTH){
			if(Mirf.dataReady()){
				Mirf.getData((byte *) &reply);
				if(reply.packet_type == 3 && reply.receiver_id == MOTHERMOTE_ID){	
					//Display data
					Serial.println("data received:----");
					Serial.print("sender_id : ");
					Serial.println(reply.sender_id);
					Serial.print("data : ");
					Serial.println(reply.data);
				}
			}
		}
		Serial.println("End of data cycle.");
		//wait for new node registration
		while(millis() - cycle_start_time <FULL_CYCLE_TIME){	//CYCLE_TIME_LENGTH + 1Second
			if(Mirf.dataReady()){
				Mirf.getData((byte *) &reply);
				if(reply.packet_type == 1 && reply.receiver_id == MOTHERMOTE_ID){	
					
					current_nodes = current_nodes + 1;
				}
			}
		}
		Serial.println("End of total cycle");
                delay(500);
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

void beacondataDecoder(int data, struct decodedData *temp){
	int a = data % B100000;
	int b = (data - a)>>5;
        temp->nodes = a;
        temp->cycleTime = b;
}

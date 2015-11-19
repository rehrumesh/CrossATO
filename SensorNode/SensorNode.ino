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
#define SENSOR_ID 24
//----------------
//packet_type = 0   =>  mothermote broadcast
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

static packet_struct init_request;
boolean isInitialized;
int wakeup_delay;
int sendingMessageNum;
int assignedMotherMote;

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
	Mirf.setTADDR((byte *) "cross");
	Mirf.payload = sizeof(packet_struct);
	Mirf.channel = 90;
	Mirf.config();	


	init_request.packet_type = 1;
	init_request.sensornode_id = SENSOR_ID;
	init_request.mothermote_id = 1;
	init_request.wakeup_delay = 1000;
	init_request.data = 1;

	isInitialized = false;
	wakeup_delay = 1000;
	sendingMessageNum = 1;
	assignedMotherMote = -1;
}

void loop(){
	static packet_struct reply;
	static packet_struct data_packet;
	unsigned long broadcastReceivedTime;
	unsigned long initializedTime;

	if(!isInitialized){
		while(!Mirf.dataReady());
		if(Mirf.dataReady()){
			Mirf.getData((byte *) &reply);
			if(reply.packet_type == 0){
				assignedMotherMote = reply.mothermote_id;
				broadcastReceivedTime =  reply.broadcastTime;

				unsigned long broad_time = millis();

				init_request.mothermote_id = assignedMotherMote;
				init_request.sensornode_id = SENSOR_ID;
				Mirf.setTADDR((byte *) "cross");
				Mirf.send((byte *) &init_request);

				while(Mirf.isSending());
				Mirf.setRADDR((byte *) "cross");
				while(!Mirf.dataReady());
				if(Mirf.dataReady()){
					Mirf.getData((byte *) &reply);
					if(reply.packet_type == 2){
						wakeup_delay = reply.wakeup_delay;
						initializedTime = millis();
						isInitialized = true;
												
					}
				}
			}
		}
	}else{
		Mirf.setTADDR((byte *) "cross");
		while(millis() - initializedTime > 52000){							
			data_packet.packet_type = 3;
			data_packet.data = 3456;
			data_packet.sensornode_id = SENSOR_ID;
			data_packet.mothermote_id = assignedMotherMote;
			Mirf.send((byte *) &data_packet);
			delay(wakeup_delay);
		}
		isInitialized = false;
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


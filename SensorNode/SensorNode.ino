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
#define SENSOR_ID 1
//----------------
//packet_type = 1   => 	init_request
//packet_type = 2   =>  data_packet
//----------------

struct packet_struct{
	byte packet_type;
	byte sensornode_id;
	byte mothermote_id;
	int wakeup_delay;
	int data;
};

static packet_struct init_request;
boolean isInitialized;
int wakeup_delay;

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
	init_request.data = 1234;

	isInitialized = false;
	wakeup_delay = 1000;
}

void loop(){
	static packet_struct reply;
	static packet_struct data_packet;

	if(!isInitialized){
		Mirf.setTADDR((byte *) "cross");
		Mirf.send((byte *) &init_request);
		while(Mirf.isSending()){};
		Serial.println("Packet sent..");
		Serial.print("Packet type: ");
		Serial.println(init_request.packet_type);
		Serial.print("Packet data: ");
		Serial.println(init_request.data);
		// Mirf.flushTx();
		// Mirf.flushRx();
		Mirf.setRADDR((byte *) "cross");
		Mirf.ceHi();
		unsigned long time = millis();
		while(!Mirf.dataReady()){
			if(millis() - time > 2500){
				break;
			}
		}
		if(Mirf.dataReady()){
			Serial.println("Packet reply received");
			Mirf.getData((byte *) &reply);
			if(reply.packet_type == 2 && reply.sensornode_id == SENSOR_ID){
				Serial.print("Packet type: ");
				Serial.println(reply.packet_type);
				Serial.print("Packet data: ");
				Serial.println(reply.data);
				Serial.print("Packet data: ");
				Serial.println(reply.wakeup_delay);

				wakeup_delay = reply.wakeup_delay;
				isInitialized = true;
			}
		}else{
			Serial.println("No Init reply received");
		}
	}else{
		data_packet.packet_type = 3;
		data_packet.sensornode_id = SENSOR_ID;
		data_packet.mothermote_id = 1;
		data_packet.data = 3152;

		Mirf.setTADDR((byte *) "cross");
		Mirf.send((byte *) &data_packet);
		while(Mirf.isSending()){};
		Serial.println("Data Packet sent..");
	}

		
	delay(wakeup_delay);
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


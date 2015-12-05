// Arduino --------------------------------------------
#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>
//-----------------------------------------------------

// // ATtiny85-----------------------------------------
// #include <SPI85.h>
// #include <Mirf.h>
// #include <nRF24L01.h>
// #include <MirfHardwareSpi85Driver.h>
// //--------------------------------------------------

#define MAX_NODE_ID 25
#define SENSOR_ID 0
#define ASSIGNED_MOTHERMOTE_ID 1
#define CYCLE_TIME_LENGTH 9000
#define FULL_CYCLE_TIME 10000

//----------------
//packet_type = 0   =>  mothermote broadcast
//packet_type = 1   => 	new node request
//packet_type = 2   =>  new node reply
//packet_type = 3   =>  data
//----------------

struct packet_struct{
	byte sender_id;	
	byte receiver_id;
	byte packet_type;
	int data;
};

struct decodedData{
	int windowTime;
	int nodes;
};

static packet_struct packet;
static packet_struct broadcast_packet;



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
	Mirf.payload = sizeof(packet_struct);
	Mirf.channel = 90;
	Mirf.config();	

}

void loop(){
	static boolean isInitialized = true;
	//unsigned long rtt_delay = 0;

	//generate data packet
	packet.sender_id = SENSOR_ID;
	packet.receiver_id = ASSIGNED_MOTHERMOTE_ID;
	packet.packet_type = 3;
	packet.data = 33;

	Mirf.setRADDR((byte *) "cross");
	while(Mirf.dataReady());
	if(Mirf.dataReady()){
		Mirf.getData((byte *) &broadcast_packet);
		if(broadcast_packet.packet_type == 0 && broadcast_packet.sender_id == ASSIGNED_MOTHERMOTE_ID){
			struct decodedData tmpDecodedData;
			beacondataDecoder(broadcast_packet.data, &tmpDecodedData);
			//calculate frame size
			int frameLength = tmpDecodedData.windowTime/tmpDecodedData.nodes;
			int numberOfWindows = CYCLE_TIME_LENGTH / tmpDecodedData.windowTime;
			
			delay(frameLength*SENSOR_ID);
			Mirf.setTADDR((byte *) "cross");
			for(int i = 0; i<numberOfWindows; i++){
				unsigned long tmpTime = millis();
				//turn on the radio
				Mirf.send((byte *) &packet);
				while(Mirf.isSending()){};

				//turn off the radio
				while(millis() - tmpTime < frameLength){}				
				delay(tmpDecodedData.windowTime - frameLength);
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


int beacondataEncoder(int windowTime, int numberOfNodes){
	int ans;
	ans = windowTime<<5;
	return ans + numberOfNodes;	
}

void beacondataDecoder(int data, struct decodedData *temp){
	int a = data % B100000;
	int b = (data - a)>>5;
        temp->nodes = a;
        temp->windowTime = b;
}
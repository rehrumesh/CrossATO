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
#define SENSOR_ID 24
//----------------
//packet_type = 0   =>  mothermote broadcast
//packet_type = 1   => 	init_request
//----------------

struct packet_struct{
	byte id;
	byte packet_type;
	int data;
};

static packet_struct packet;

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

}

void loop(){
		
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


#include <SPI85.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpi85Driver.h>

#define MAX_NODE_ID 25

struct packet_struct{
	byte packet_type;
	byte sensornode_id;
	byte old_id;
	byte mothermote_id;
	int wakeup_delay;
	int data;
};

static byte sensornode_id;
static byte mothermote_id;
static packet_struct init_req;
static packet_struct init_res;
static int wakeup_delay;

boolean isInitCompleted;

void setup(){
	isInitCompleted = false;
	sensornode_id = random(MAX_NODE_ID) + MAX_NODE_ID;
	wakeup_delay = 200;	//default sleep time

	Mirf.cePin = PB4;
	Mirf.csnPin = PB3;

	Mirf.spi = &MirfHardwareSpi85;	  
	Mirf.init();
	Mirf.setTADDR((byte *) "cross");
	Mirf.payload = sizeof(packet_struct);
	Mirf.channel = 50;
	Mirf.config();
}

void loop(){
	if(!isInitCompleted){
	 	initReq();
	}else{
	 	Mirf.setTADDR((byte *) "cross");

	 	packet_struct dPacket;
	 	dPacket.packet_type = 2;
	 	dPacket.sensornode_id = sensornode_id;
	 	dPacket.mothermote_id = mothermote_id;
	 	dPacket.old_id = 0;
	 	dPacket.data = 3456;
	 	dPacket.wakeup_delay = wakeup_delay;

	 	Mirf.send((byte *) &dPacket);
	 	while(Mirf.isSending());
	 }

	delay(wakeup_delay);

}

void initReq(){
	init_req.sensornode_id = sensornode_id;
	init_req.packet_type = 1;
	init_req.old_id = 0;
	init_req.mothermote_id = 0;
	init_req.wakeup_delay = 100;
	init_req.data = -1;

	//while(!isChannelClear());
	Mirf.send((byte *) &init_req);
	while(Mirf.isSending()){}
        Mirf.ceHi();
	Mirf.setRADDR((byte *) "cross");
	unsigned long time = millis();	
	do{		
		while(!Mirf.dataReady()){
			if ((millis() - time) > 1500) {
        			goto loopbreak;
                        }
		}
		if(Mirf.dataReady()){
			Mirf.getData((byte *) &init_res);
		}
		
	} while (init_res.old_id != init_req.sensornode_id);
	
	loopbreak:

        if(init_res.old_id != init_req.sensornode_id){
		sensornode_id = init_res.sensornode_id;
		wakeup_delay = init_res.wakeup_delay;
		mothermote_id = init_res.mothermote_id;
		isInitCompleted = true;
        }
        //isInitCompleted = true;
}

boolean isChannelClear(){
	byte carrier_detect_reg_value = (byte) 0;        
        //RX mode
        Mirf.ceHi();
        Mirf.readRegister(CD, &carrier_detect_reg_value, sizeof(carrier_detect_reg_value));

        //Serial.print("carrier_detect_reg_value = ");
        //Serial.println(carrier_detect_reg_value, BIN);
        return carrier_detect_reg_value == 0;
}


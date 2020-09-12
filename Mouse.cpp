//Oliver Cho 5/31/20
#include <iostream>
using namespace std;


//location of delta X2 in buffer
#define deltaX2 2
//location of delta Y2 in buffer
#define deltaY2 3
//location of Left button state in buffer
#define Left 4
//location of Right button state in buffer
#define Right 4
//location of delta X1 in buffer
#define deltaX1 4
//location of delta Y1 in buffer
#define deltaY1 4
//location of 3rd timestamp in buffer from right to left
#define timeStamp3 4
//location of 2nd timestamp in buffer from right to left
#define timeStamp2 5
//location of 1st timestamp in buffer from right to left
#define timeStamp1 6

//running timestamp for total time being collected from packets
uint32_t runningTimestamp = 0x0;   
//last added timestamp from the time being collected from packets
uint32_t lastAddedTimestamp = 0x0;

/*
Parameters: buffer: Pointer to an unsigned integer of 8 bits representing a packet from a mouse device
Return type: unsigned integer of 16 bits
Purpose: Go through the unsigned integer in the parameter and return the value of the Delta X section
*/
uint16_t getDeltaX(uint8_t *buffer) 
{
	//single out the value of the delta X in buffer Byte 4
	//use 0xC as a mask to single out the delta X value
	//shift to the right 2 bits due to delta X's location
  	uint8_t first = (buffer[deltaX1] & 0xC) >> 2;
  	//combine value of delta X in Byte 4 to value of delta X in Byte 2 into 16 bit value
  	uint16_t result=(first << 8) ^ buffer[deltaX2];

  	return result;
}

/*
Parameters: buffer: Pointer to an unsigned integer of 8 bits representing a packet from a mouse device
Return type: unsigned integer of 16 bits
Purpose: Go through the unsigned integer in the parameter and return the value of the Delta Y section
*/
uint16_t getDeltaY(uint8_t *buffer) 
{
	//single out the value of the delta Y in buffer Byte 4
	//use 0x30 as a mask to single out the delta Y value
	//shift to the right 4 bits due to delta Y's location
  	uint8_t first = (buffer[deltaY1] & 0x30) >> 4;
  	//combine value of delta Y in Byte 4 to value of delta Y in Byte 3 into 16 bit value
  	uint16_t result=(first << 8) ^ buffer[deltaY2];

  	return result;
}

/*
Parameters: buffer: Pointer to an unsigned integer of 8 bits representing a packet from a mouse device
Return type: unsigned char
Purpose: Go through the unsigned integer in the parameter and return the value of the Right button state section
*/
unsigned char getRight(uint8_t *buffer)
{
	//single out the value of right button state in buffer Byte 4
	//use 0x2 as a mask to single out the Right button state value
	return (buffer[Right] & 0x1);
}

/*
Parameters: buffer: Pointer to an unsigned integer of 8 bits representing a packet from a mouse device
Return type: unsigned char
Purpose: Go through the unsigned integer in the parameter and return the value of the Left button state section
*/
unsigned char getLeft(uint8_t *buffer) 
{
	//single out the value of left button state in buffer Byte 4
	//use 0x1 as a mask to single out the left button state value
	//shift to the right 1 bits due to left button state's location
	return (buffer[Left] & 0x2) >> 1;
}

/*
Parameters: buffer: Pointer to an unsigned integer of 8 bits representing a packet from a mouse device
Return type: unsigned integer of 32 bits
Purpose: Go through the unsinged integer in the parameter and return the value of the timestamp secion.
	The function can alsodetect rollover due to the 18 bit size of the timestamp packet and return the 
	proper time accordingly.
*/
uint32_t getTimestamp(uint8_t *buffer)
{
	//get timeStamp in buffer Byte 6
	uint8_t tstamp1=buffer[timeStamp1];
	//get timeStamp in buffer Byte 5
	uint8_t tstamp2=buffer[timeStamp2];
	//single out value of timestamp in buffer Byte 4
	//use 0xC0 as a mask to single out the timestamp value
	//shift to the right 6 bits dues to the timestamp's location
	unsigned char tstamp3=(buffer[timeStamp3] & 0xC0) >> 6;

	//comvbine the value of the timestamps
	//shift timestamp1 left 10 bits
	//include timepstamp2 with exclusive or
	//shift timestamp2 left 2 bits
	//include timestamp3 with exclusive or
	uint32_t newTimestamp = (tstamp1 << 10) ^ (tstamp2 << 2) ^ tstamp3;

	//If the new timestamp is less than or equal to the last time stamp then we 
	//have timestamp rollover as time cannot get smaller 
  	if (newTimestamp <= lastAddedTimestamp) {
    
   		//subtract the last time stamp from the maximum time to see how much time was lost
    	uint32_t rollover = 0x3FFFF - lastAddedTimestamp;
    	//Add the rollover time which was previously lost to the running time
    	runningTimestamp += rollover; 
    	//Reset the previously seen time
    	lastAddedTimestamp = 0x0;
  	}
  
  	//Update overall time by difference from last time,
  	runningTimestamp += (newTimestamp - lastAddedTimestamp); 
  	//then update last added timestamp. 
  	lastAddedTimestamp = newTimestamp;
  
  	return newTimestamp;
}

/*
Parameters: buffer: Pointer to an unsigned integer of 8 bits representing a packet from a mouse device
	length: Unsigned integer of 16 gits representing the length of the packet 
Return type: void
Purpose: Go through the unsigned integer "buffer" in the parameter and seperate out delta X, delta Y, 
	Left button state, Right button state, and Timestamp.Then print each of the section out in hex value.
*/
void handleReport(uint8_t *buffer, uint16_t length) 
{
	//if the entered length is not the expected 7
	if(length!=7)
	{
		cout<<"The length of the buffer must be 7 bytes"<<endl;
		return;
	}
	
	//get the value of Delta X and put it into dX
	uint16_t dX=getDeltaX(buffer);
	//get the value of Delta Y and put it into dY
	uint16_t dY=getDeltaY(buffer);
	//get the value of Left button state and put it into left
	unsigned char left = getLeft(buffer);
	//get the value of Right button state and put it into right
	unsigned char right = getRight(buffer);
	//get the value of Timestamp and put it into timestamp
	uint32_t timestamp = getTimestamp(buffer);

	//print it out
	printf("left: 0x%x\n", left);
  	printf("right: 0x%x\n", right);
  	printf("deltaX: 0x%x\n", dX);
  	printf("deltaY: 0x%x\n", dY);
  	printf("timestamp: 0x%x\n", timestamp);
  	printf("overall timestamp: 0x%x\n", runningTimestamp);
}
int main()
{
	uint16_t length = sizeof(uint8_t) * 7;
	uint16_t lengthOfSix = sizeof(uint8_t) * 6;
	uint16_t lengthOfEight = sizeof(uint8_t) * 8;
  	uint8_t *buffer0 = (uint8_t *) malloc(length);
  	uint8_t *buffer1 = (uint8_t *) malloc(length);
  	uint8_t *buffer2 = (uint8_t *) malloc(length);
  	uint8_t *buffer3 = (uint8_t *) malloc(lengthOfSix);
  	uint8_t *buffer4 = (uint8_t *) malloc(lengthOfEight);
  	
	buffer0[0] = 0xFF;
  	buffer0[1] = 0xFF;
  	buffer0[2] = 0xFF;
  	buffer0[3] = 0xFF;
  	buffer0[4] = 0xAA; 
  	buffer0[5] = 0xFF; 
  	buffer0[6] = 0x00;

  	buffer1[0] = 0x00;
  	buffer1[1] = 0x00;
  	buffer1[2] = 0x00;
  	buffer1[3] = 0x00;
  	buffer1[4] = 0x00;
  	buffer1[5] = 0x00;
  	buffer1[6] = 0x00;

  	buffer2[0] = 0xFF;
  	buffer2[1] = 0xFF;
  	buffer2[2] = 0xFF;
  	buffer2[3] = 0xFF;
  	buffer2[4] = 0xFF;
  	buffer2[5] = 0xFF;
  	buffer2[6] = 0xFF;

  	buffer3[0] = 0xFF;
  	buffer3[1] = 0xFF;
  	buffer3[2] = 0xFF;
  	buffer3[3] = 0xFF;
  	buffer3[4] = 0xAA;
  	buffer3[5] = 0xFF;

  	buffer4[0] = 0xFF;
  	buffer4[1] = 0xFF;
  	buffer4[2] = 0xFF;
  	buffer4[3] = 0xFF;
  	buffer4[4] = 0xAA;
  	buffer4[5] = 0xFF;
  	buffer4[6] = 0x00;
  	buffer4[7] = 0x00;


  	//test standard buffer
	cout<<"_____________buffer 0 result_________"<<endl;
  	handleReport(buffer0, length);

  	//test buffer of all 0's
  	cout<<"_____________buffer 1 result_________"<<endl;
  	handleReport(buffer1, length);

  	//test buffer of all 1's
  	cout<<"_____________buffer 2 result_________"<<endl;
  	handleReport(buffer2, length);

  	//test length less than 7
  	cout<<"_____________buffer 3 result_________"<<endl;
  	handleReport(buffer3, lengthOfSix);

  	//test length greater than 7
  	cout<<"_____________buffer 4 result_________"<<endl;
  	handleReport(buffer4, lengthOfEight);


	return 0;
};
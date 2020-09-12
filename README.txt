Oliver Cho 5/31/20

Question #1:
You are writing a driver for a new mouse. All incoming data is processed in a function named handleReport();

void handleReport(UInt8 *buffer, UInt16 length);

A mouse packet has the following structure:

B0: report format
B1: reserved
B2: dX (bits[7:0])
B3: dY (bits[7:0])
B4[1:0]: Left/Right button states
B4[3:2]: dX (bits[9:8])
B4[5:4]: dY (bits[9:8])
B4[7:6]: timestamp (ms) bits[1:0]
B5: timestamp (ms) bits[9:2]
B6: timestamp (ms) bits[17:10]

1) Write one or more functions that extract the mouse deltas, button state, and timestamp from the device.
2) The 18-bit packet timestamp reflects a device timestamp in milliseconds. It will roll over after a few minutes.
The host can accommodate a 32-bit timestamp that is updated with values from the incoming packet. Write code that
detects the timestamp rollover in the incoming packets, and extends it to 32 bits.

---------------------------------------------------------------------------------------------------------------

EXPLANATION

Files:
	Mouse.cpp

Mouse.cpp:
Inside Mouse.cpp is the function handleReport. The function handleReport is meant to take a mouse packet with the
structure included above. The function will decifer each bit section of the packet and decifer the information to
extract mouse deltas, button state, and timestamp from the device. The function handleReport can also determine whether
the amount of time has exceeded the amount the 18 bit packet can hold. The function will handle this as rollover and show
the total time in the overall time stamp. If the the length entered in the function parameter is not 7, the function will
print a statement to the command line and return. In the file Mouse.cpp the function handleReport is called in the main and
tested using a variety of scenarios.

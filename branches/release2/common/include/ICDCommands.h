#ifndef _ICD_COMMANDS_H
#define _ICD_COMMANDS_H

/*
* structure of packets
* (int)(int)(data) <- 1st int tells us the total size of the packet, meaning everything after the size field. 2nd int tells us what type of packet this is
* 	if type == command
*		(int)(args...) <- int is the command type (see below). args... are a variable list of arguments that are determined by the command type
* 	else if type == response
* 		(int)(data) <- int is the response type. data is determined by the response type
*/

/**
* Define packet types
*/
#define ICD_PACKET_TYPE_COMMAND 	1
#define ICD_PACKET_TYPE_RESPONSE	2

/**
* Command that is sent when the client wants to convert from ICD9 to ICD10
* Example payload:
*		(size)(ICD_PACKET_TYPE_COMMAND)(ICD_COMMAND_COVERT_9_TO_10)(int)(code goes here) <- int before code is the length of the code
*
* ICD_RESPONSE_CONVERT_9_TO_10 is the response to this message
* @see ICD_RESPONSE_CONVERT_9_TO_10
*/
#define ICD_COMMAND_CONVERT_9_TO_10 1

/**
* Response to the convert 9 to 10 command.
* Example payload:
*		(size)(ICD_PACKET_TYPE_RESPONSE)(ICD_RESPONSE_CONVERT_9_TO_10)(list of codes)
*
* See the CodeList file for functions to convert between code lists and byte buffers
* @see CodeList
*/
#define ICD_RESPONSE_CONVERT_9_TO_10 1

#endif

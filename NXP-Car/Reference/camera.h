/************************************************************
File: camera.h

Description: Header File for the camera module

Authors: Dakota Bassett and Amar Bhatt

Created: 11/20/2015

Last Edited: 12/9/2015
************************************************************/


#ifndef CAMERA_H_
#define CAMERA_H_


/************************************************************
 Library and Header File Includes
************************************************************/
#include "MK64F12.h"



/************************************************************
 Global Scope Function prototypes
************************************************************/
void init_camera( void );
void read_camera( uint16_t* cameradata );



/************************************************************
 Global Scope Variables
************************************************************/
extern volatile uint8_t run_main;



#endif /* CAMERA_H_ */

/* End of File */

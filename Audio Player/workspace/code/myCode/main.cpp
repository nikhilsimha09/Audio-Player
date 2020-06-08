/*
 * main.cpp
 */


#include "CAudioPlayerController.h"

int main (void)
{
	setvbuf(stdout, NULL, _IONBF, 0);
	CAudioPlayerController myController; 	// create the controller
	CUserInterfaceCmdIOW ui;				// create an user interface object for the controller (may be replaced by another ui type)
	ui.showMessage("Audio Player Started");
	myController.run(&ui);					// run the controller

	ui.showMessage("Bye!");
	return 0;
}





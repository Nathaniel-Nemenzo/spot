#include <spot/spot.h>

#include <iostream>
#include <string>
#include <thread>
#include <fstream>
#include <streambuf>
#include <assert.h>
#include <map>

#include <functional>
#include <sys/ioctl.h> //For FIONREAD.
#include <termios.h>

// main function for running Spot clients
int main(int argc, char *argv[]){
	std::cout<<"in main"<< std::endl;
	assert(argc == 3);

	// get username and password
	const std::string username = argv[1];
	const std::string password = argv[2];

	// create robot
	Robot robot("spot");

	// create a display

	// print id information
	std::cout << robot.getId() << std::endl;

	// authenticate robot
	robot.authenticate(username, password);

	// setup robot (initialize clients)
	robot.setup(); 
	
	Display display;
	while (true){
		robot.getImages();
		display.runDisplay(argc, argv);
		std::cout << "start loop again" << std::endl;
	}

	return 0;
}

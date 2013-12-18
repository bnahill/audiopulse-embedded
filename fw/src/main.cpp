/*!
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 
 @file main.cpp
 @brief Application entry point. Mostly glue logic.
 @author Ben Nahill <bnahill@gmail.com>
 */

#include <stdint.h>
#include <derivative.h>
#include <bsp/platform.h>

extern "C" {
	#include <RealTimerCounter.h>
	#include <audio_generator.h>
	void main();
}


/*!
 * @brief Application entry point
 */
void main(){
	// Perform late initializations
	Platform::lateInit();

	TimerQInitialize(0);
	//USB::audioClassInit();
	USB::hidClassInit();
	
	AK4621::start();

	while(true){
		static volatile uint32_t a = 0;
		a += 1;
		// Call the application task
		// USB::audioSend();
    }
} 


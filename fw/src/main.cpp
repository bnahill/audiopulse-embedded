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
#include <driver/platform.h>
#include <controller.h>
#include <input_dsp.h>

extern "C" {
	void main();
}


/*!
 * @brief Application entry point
 */
void main(){
	struct pt pt_dsp, pt_command_parser, pt_wavegen;

	PT_INIT(&pt_dsp);
	PT_INIT(&pt_command_parser);
	PT_INIT(&pt_wavegen);

	// Perform late initializations
	Platform::lateInit();

	USB::hidClassInit();
	
	AK4621::start();
	
	while(true){
		APulseController::pt_command_parser(&pt_command_parser);
		InputDSP::pt_dsp(&pt_dsp);
		APulseController::pt_wavegen(&pt_wavegen);
    }
} 


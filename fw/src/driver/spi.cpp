/*!
 (C) Copyright 2015 Ben Nahill

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

 @file spi.cpp
 @brief
 @author Ben Nahill <bnahill@gmail.com>
 */

#include "driver/spi.h"


void SPI::register_slave(SPI_slave &slave){
	if(slave.index != ~0)
		return;

	if(nslaves > 1)
		return;

	slave.index = nslaves;
	slave.NCS.configure((GPIOPin::mux_t)slave.mux, true);
	spi->CTAR[nslaves] = slave.CTAR;
	nslaves += 1;
}


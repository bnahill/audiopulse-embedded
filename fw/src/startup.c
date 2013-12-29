/*!
 (C) Copyright 2013, Ben Nahill
 
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
  
 @file startup.c
 @brief Memory initialization and startup code
 @author Ben Nahill <bnahill@gmail.com>
 */


#include <stdint.h>
#include <kinetis_sysinit.h>

extern void __CTOR_LIST__(void);
extern void __CTOR_END__(void);

extern void __DTOR_LIST__(void);
extern void __DTOR_END__(void);

extern uint32_t __START_BSS[];
extern uint32_t __END_BSS[];

extern uint32_t const _textdata;
extern uint32_t _data;
extern uint32_t _edata;

extern void main(void);
extern void __libc_init_array(void);

void __reset_vector(void){
	__libc_init_array();
	
	// Zero-fill the BSS
	for(uint32_t * ptr = __START_BSS; ptr != __END_BSS; ptr++){
		*ptr = 0;
	}
	
	{
		uint32_t *tp, *dp;
		tp = &_textdata;
		dp = &_data;
		while (dp < &_edata)
			*dp++ = *tp++;
	}
	
	// Call static constructors
	for(void (*fpp)(void) = &__CTOR_LIST__; fpp < &__CTOR_END__; fpp++){
		(*fpp)();
	}
	
	__init_hardware();
	
	// Run the application
	main();
	
	// Call destructors. Really, there is no good reason for this.
	for(void (*fpp)(void) = &__DTOR_LIST__; fpp < &__DTOR_END__; fpp++){
			(*fpp)();
	}
}


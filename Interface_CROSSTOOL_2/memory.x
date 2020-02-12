/************************************************************************/
/*      GCC/LLCM ARM Baremetal Software / (C) Toshio MURAI  2012-2016   */
/*      This is Free Software.   You can redistribute this software     */
/*      and/or modify it ,under the terms of the GNU General Public     */
/*      License version 3, or (at your option) any later version.       */
/*      or under the terms of the BSD 3-Clause License.	(Dual Licence)  */
/************************************************************************/
ENTRY (start);

MEMORY {
    ram (rxw) :   ORIGIN = 0x8000, LENGTH = 512K-32K	/*check	*/
}

SECTIONS
{
	.text : {
                vector.o(.rodata.i)
                vector.o(.rodata.v)
		*(.text*)
		*(.rodata*)
/*		etext = .;	*/
	}
	.ARM.exidx   :{
		__exidx_start = .;
		*(.ARM.exidx*) 
		__exidx_end = .;
	}
	etext = .;

	.data : AT (etext){
		sdata = .;
		*(.data)
		edata = .;
	}

	.bss : {
		sbss = .;
		*(.bss)
		*(COMMON)
		ebss = .;
	}

	_interruptStack =  ORIGIN(ram) + LENGTH(ram) -0x2000;

	_supervisorStack =  ORIGIN(ram) + LENGTH(ram);
}


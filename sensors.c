/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * check sensors response checksum
 * good sample: 2a 00 4b 46 ff ff 0e 10 84 : crc=84 YES
 * bad sample : ff ff ff ff ff ff ff ff ff : crc=c9 NO
 */
static int sensors_checksum(char *buffer) {
	if(strstr(buffer, "YES"))
		return 1;
	
	return 0;
}

/* 
 * extract temperature value in milli-degres celcius
 * sample: 2a 00 4b 46 ff ff 0e 10 84 t=20875
 * t= is the decimal value
 */
static int sensors_value(char *buffer) {
	char *str;
	
	if(!(str = strstr(buffer, " t=")))
		return 0;
	
	return atoi(str + 3);
}

/* read sensors current value */
float sensors_read(char *filename) {
	float value = -101;
	char buffer[1024];
	FILE *fp;
	
	if(!(fp = fopen(filename, "r"))) {
		// perror(filename);
		return 0;
	}
	
	/* reading first line: checksum */
	if(!(fgets(buffer, sizeof(buffer), fp))) {
		// perror("[-] fgets");
		goto finish;
	}
	
	/* checking checksum */
	if(!sensors_checksum(buffer)) {
		// fprintf(stderr, "[-] sensor %s: invalid checksum\n", filename);
		goto finish;
	}
	
	/* reading temperature value */
	if(!(fgets(buffer, sizeof(buffer), fp))) {
		// perror("[-] fgets");
		goto finish;
	}
	
	/* extracting temperature */
	value = sensors_value(buffer) / 1000.0;
	
	finish:
	fclose(fp);
	
	return value;
}

#ifndef __SENSURSES_H
	#define __SENSURSES_H
	
	//
	// +-----[ Label ]----+
	// |                  |
	// |      25.55°C     |
	// |                  |
	// |   10/10 - 22:57  |
	// +------------------+
	//
	
	#define BOX_WIDTH        20
	#define BOX_HEIGHT        6
	#define BOX_MARGIN_TOP    1
	#define BOX_MARGIN_LEFT   2
	
	typedef struct sensor_t {
		char *id;
		char *label;
		float value;
		time_t time;
		WINDOW *window;
		
	} sensor_t;
	
	typedef struct sensors_t {
		unsigned int length;
		unsigned int capacity;
		sensor_t *items;
		
	} sensors_t;
#endif

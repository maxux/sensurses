/*
 * 1-wire sensors ncurses 'responsive' interface
 * Copyright (C) 2012  DANIEL Maxime <root@maxux.net>
 *
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
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ncurses.h>
#include <signal.h>
#include <locale.h>
#include <time.h>
#include "rtinfo_ncurses.h"

sensors_t sensors;
int __maxy, __maxx;
int __lasty, __lastx;

void resetconsole() {
	clear();
	getmaxyx(stdscr, __maxy, __maxx);
	__lastx = 0;
	__lasty = 0;
}

void initconsole() {
	/* Init Console */
	initscr();		/* Init ncurses */
	cbreak();		/* No break line */
	noecho();		/* No echo key */
	start_color();		/* Enable color */
	use_default_colors();
	curs_set(0);		/* Disable cursor */
	keypad(stdscr, TRUE);
	scrollok(stdscr, 1);
	timeout(1000);
	setlocale(LC_CTYPE, "");
	
	resetconsole();
	
	/* Init Colors */
	init_pair(1, COLOR_WHITE,   -1);
	init_pair(2, COLOR_BLUE,    -1);
	init_pair(3, COLOR_YELLOW,  -1);
	init_pair(4, COLOR_RED,     -1);
	init_pair(5, COLOR_BLACK,   -1);
	init_pair(6, COLOR_CYAN,    -1);
	init_pair(7, COLOR_GREEN,   -1);
	init_pair(8, COLOR_MAGENTA, -1);
	init_pair(9, COLOR_WHITE,   -1);
	refresh();
}

void sighandler(int signal) {
	unsigned int i;
	
	switch(signal) {
		case SIGINT:
			endwin();
			exit(EXIT_SUCCESS);
		break;
		
		case SIGWINCH:
			for(i = 0; i < sensors.length; i++) {
				delwin(sensors.items[i].window);
				sensors.items[i].window = NULL;
			}
			
			endwin();
			resetconsole();
			refresh();
		break;
	}
}

void initialize(sensor_t *sensor) {
	if(BOX_WIDTH + __lastx >= __maxx) {
		__lasty += BOX_HEIGHT + BOX_MARGIN_TOP;
		__lastx  = 0;
	}
		
	sensor->window = newwin(BOX_HEIGHT, BOX_WIDTH, __lasty, __lastx);
	
	wattrset(sensor->window, (A_BOLD | COLOR_PAIR(5)));
	box(sensor->window, 0, 0);
	wrefresh(sensor->window);
	
	__lastx += BOX_WIDTH + BOX_MARGIN_LEFT;
}

void sensbox(sensor_t *sensor) {
	struct tm *ti;
	
	//
	// initialize box
	//
	if(!sensor->window)
		initialize(sensor);
	
	//
	// initialize data
	//
	ti = localtime(&sensor->time);
	
	//
	// rendering
	//
	wmove(sensor->window, 0, 2);
	wattrset(sensor->window, (A_BOLD | COLOR_PAIR(5)));
	wprintw(sensor->window, "[ %s ]", sensor->label);
	
	wmove(sensor->window, 2, (BOX_WIDTH / 2) - 3);
	
	if(sensor->value > 30)
		wattrset(sensor->window, (A_BOLD | COLOR_PAIR(4)));
		
	else if(sensor->value > 20)
		wattrset(sensor->window, (A_BOLD | COLOR_PAIR(3)));
		
	else wattrset(sensor->window, (A_BOLD | COLOR_PAIR(7)));
		
	wprintw(sensor->window, "%02.2f°C", sensor->value);
	
	wmove(sensor->window, 4, 3);
	wattrset(sensor->window, (COLOR_PAIR(2)));
	wprintw(
		sensor->window,
		"%02d/%02d    %02d:%02d",
		ti->tm_mday,
		ti->tm_mon + 1,
		ti->tm_hour,
		ti->tm_min
	);
	
	wrefresh(sensor->window);
}

int main(void) {
	unsigned int i;
	
	sensors.items    = (sensor_t *) malloc(sizeof(sensor_t) * 32);
	sensors.capacity = 32;
	sensors.length   = 3;
	
	sensors.items[0].id     = "12-23-45";
	sensors.items[0].label  = "Room 1";
	sensors.items[0].value  = 24.54;
	sensors.items[0].time   = 1436212541;
	sensors.items[0].window = NULL;
	
	sensors.items[1].id     = "12-##-45";
	sensors.items[1].label  = "Room 2";
	sensors.items[1].value  = 10.14;
	sensors.items[1].time   = 1436212541;
	sensors.items[1].window = NULL;
	
	sensors.items[2].id     = "@@-23-45";
	sensors.items[2].label  = "Room 3";
	sensors.items[2].value  = 34.54;
	sensors.items[2].time   = 1436212541;
	sensors.items[2].window = NULL;
	
	/* Handling Resize Signal */
	signal(SIGINT, sighandler);
	signal(SIGWINCH, sighandler);
	
	/* Initializing ncurses */
	initconsole();
	
	while(1) {
		for(i = 0; i < sensors.length; i++) {
			sensbox(&sensors.items[i]);
			sensors.items[i].value += 0.01;
		}
		
		usleep(1000000);
	}

	getchar();
	
	endwin();
	
	return 0;
}

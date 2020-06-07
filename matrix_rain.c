#ifndef CHECK_MEMORY
#include <curses.h>
#include <unistd.h>
#include <time.h>
#endif

#include <stdlib.h>
#include <string.h>

#define TEST_STRING "ABCDEFGHIJKLMNOPQRSTUVWXYZ"

enum { temp_speed = 5, temp_y = 0 };

enum { charset_min = 34, charset_max = 126 }; /* ASCII: [!-~] */

enum colors { white = 1, green = 2 };

enum { update_speed = 60000 };

typedef struct s
{
	int row;
	int column;
	int speed;
	char *text;
} streamer;

char *create_random_string(int min_size, int max_size)
{
	char *result;
	int rand_size = min_size + (int)((double)max_size * rand() / (RAND_MAX + 1.0));
	int i;
	result = malloc(sizeof(char) * (rand_size+1)); /* +1 for null byte */
	for(i = 0; i < rand_size; i++)
	{
		char random_char = charset_min + (int)((double)(charset_max - charset_min) * rand() / (RAND_MAX + 1.0));
		result[i] = random_char;
	}
	result[i] = '\0';
	return result;
}

streamer *create_streamer(int row, int column, int speed)
{
	streamer *st = malloc(sizeof(streamer));
	st->row = row;
	st->column = column;
	st->speed = speed;
	st->text = create_random_string(1, 20);
	return st;
}

streamer **create_array_of_streamers(int max_x, int max_y)
{
	streamer **arr = malloc(sizeof(streamer*) * max_x);
	int i;
	for(i = 0; i < max_x; i++)
	{
		int rand_speed = 1 + (int)(5.0 * rand() / (RAND_MAX + 1.0));
		int rand_row = (int)((double)max_y * rand() / (RAND_MAX + 1.0));
		arr[i] = create_streamer(rand_row, i, rand_speed);
	}
	return arr;
}

#ifndef CHECK_MEMORY
void show_streamer(streamer *st, int max_y)
{
	int i;
	int att = 0;
	int size = strlen(st->text);
	if(st->speed >= 3)
		att = A_BOLD;
	for(i = size - 1; i >= 0; i--)
	{
		if(i > 1)
		{
			attrset(COLOR_PAIR(green) | att);
		}
		else
		{
			attrset(COLOR_PAIR(white) | att);
		}
		if(st->row + size - i > max_y)
		{
			move((st->row + size - i - 1) % max_y, st->column);
			addch(st->text[i]);
		}
		else
		{
			move(st->row + size - 1 - i, st->column);
			addch(st->text[i]);
		}
		/* move(max_y-1, 20); */
		/* printw("%d", st->row); */
	}
}

void move_streamer(streamer *st, int max_y)
{
	int i;
	for(i = 0; i < st->speed; i++)
	{
		if(st->row + i >= max_y)
		{
			move((st->row + i) % max_y, st->column);
			addch(' ');
		}
		else
		{
			move(st->row + i, st->column);
			addch(' ');
		}
	}
	st->row += st->speed;
	if(st->row >= max_y)
	{
		st->row = st->row % max_y;
	}
}
#endif

void free_streamer(streamer *st)
{
	free(st->text);
	free(st);
}


int main()
{
#ifdef CHECK_MEMORY
	streamer **test_array = create_array_of_streamers(134, 32);
	int i;
	for(i = 0; i < 134; i++)
	{
		free_streamer(test_array[i]);
	}
	free(test_array);
	return 0;
#else
	srand(time(NULL));
#ifdef TEST_RANDOM_STRING
	char *test = create_random_string(1, 20);
	printf("%s\n", test);
	free(test);
#else
	int max_x, max_y;
	int key;

	initscr();
	getmaxyx(stdscr, max_y, max_x);
	start_color();
	init_pair(white, COLOR_WHITE, COLOR_BLACK);
	init_pair(green, COLOR_GREEN, COLOR_BLACK);
	cbreak();
	keypad(stdscr, 1);
	noecho();


#ifdef SINGLE_STREAM
	streamer *test = create_streamer(1, 0, 2);
	show_streamer(test, max_y);
	curs_set(0);
	refresh();
	while((key = getch()) != 27)
	{
		if(key == KEY_DOWN)
		{
			move_streamer(test, max_y);
			show_streamer(test, max_y);
			curs_set(0);
			refresh();
		}
	}
	free(test);
#else
	streamer **rain = create_array_of_streamers(max_x, max_y);
	while(true)
	{
		int i;
		for(i = 0; i < max_x; i++)
		{
			show_streamer(rain[i], max_y);
		}
		curs_set(0);
		refresh();
		for(i = 0; i < max_x; i++)
		{
			move_streamer(rain[i], max_y);
		}
		usleep(update_speed);
	}
#endif
	sleep(5);
	endwin();
#endif
	return 0;
#endif
}

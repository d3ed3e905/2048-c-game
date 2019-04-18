#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curses.h>
#include <ctype.h>
#include <time.h>
#include <sys/select.h>

#define FOREVER 1
#define TIMEOUT 10

WINDOW *win_name, *win_menu, *win_legend, *win_board,
	*win_matrix, *win_end_game, *win_bonus, *win_win;

// generare si initializare culori
void initialize_color()
{
	int i, j;
	start_color();
	init_color(39, 900, 0, 0);		  // ROSU APRINS
	init_color(40, 1000, 1000, 1000); // ALB
	init_color(20, 368, 635, 905);	// ALBASTRU DESCHIS
	init_color(21, 7, 490, 749);	  // ALBASTRU INCHIS
	init_color(22, 90, 827, 356);	 // VERDE IRLANDEZ
	init_color(23, 3, 752, 0);		  // VERDE 2
	init_color(24, 976, 764, 0);	  // GALBEN
	init_color(25, 917, 447, 66);	 // PORTOCALIU
	init_color(26, 721, 101, 160);	// ROSU
	init_color(27, 619, 0, 364);	  // VISINIU
	init_color(28, 341, 250, 486);	// MOV
	init_color(29, 239, 160, 388);	// INDIGO
	init_color(30, 450, 0, 0);		  // ROSU INCHIS
	init_color(31, 0, 1000, 0);		  // VERDE PUR
	init_color(32, 1000, 0, 0);		  // ROSU PUR

	init_pair(1, COLOR_GREEN, 0);
	init_pair(2, COLOR_YELLOW, 0);
	init_pair(3, COLOR_CYAN, 0);
	init_pair(4, COLOR_BLUE, 0);
	init_pair(5, COLOR_WHITE, 0);
	init_pair(6, COLOR_YELLOW, 0);
	init_pair(7, 40, COLOR_GREEN);
	init_pair(8, COLOR_RED, 0);
	init_pair(9, COLOR_WHITE, COLOR_WHITE);
	init_pair(21, 32, 0); // ROSU PUR PE NEGRU
	init_pair(22, 31, 0); // VERDE PUR PE NEGRU
	init_pair(23, 23, 0); // VERDE 2 PE NEGRU
	for (i = 10, j = 20; j <= 30; i++, j++)
		init_pair(i, 40, j);
}

//afiseaza "2048" ca titlu
void name_game()
{
	int i;
	win_name = newwin(30, 44, 0, 0);

	// afisare 2
	wattron(win_name, COLOR_PAIR(1));
	mvwprintw(win_name, 2, 5, "222222");
	mvwprintw(win_name, 6, 5, "222222");
	mvwprintw(win_name, 10, 5, "222222");
	for (i = 1; i <= 4; i++)
	{
		mvwprintw(win_name, 1 + i, 10, "2");
		mvwprintw(win_name, 5 + i, 5, "2");
	}
	wattroff(win_name, COLOR_PAIR(1));
	// afisare 0
	wattron(win_name, COLOR_PAIR(2));
	mvwprintw(win_name, 2, 15, "0000000");
	mvwprintw(win_name, 10, 15, "0000000");
	for (i = 1; i <= 8; i++)
	{
		mvwprintw(win_name, 1 + i, 21, "0");
		mvwprintw(win_name, 1 + i, 15, "0");
	}
	wattroff(win_name, COLOR_PAIR(2));
	// afisare 4
	wattron(win_name, COLOR_PAIR(3));
	mvwprintw(win_name, 6, 26, "444444");
	for (i = 1; i <= 5; i++)
	{
		mvwprintw(win_name, 5 + i, 31, "4");
		mvwprintw(win_name, 1 + i, 26, "4");
	}
	mvwprintw(win_name, 5, 31, "4");
	wattroff(win_name, COLOR_PAIR(3));
	// afisare 8
	wattron(win_name, COLOR_PAIR(4));
	mvwprintw(win_name, 2, 36, "8888888");
	mvwprintw(win_name, 6, 36, "8888888");
	mvwprintw(win_name, 10, 36, "8888888");
	for (i = 1; i <= 8; i++)
	{
		mvwprintw(win_name, 1 + i, 42, "8");
		mvwprintw(win_name, 1 + i, 36, "8");
	}
	wattroff(win_name, COLOR_PAIR(4));
	refresh();
	wrefresh(win_name);
	napms(300); // delay
}

// desenare bonus
void draw_bonus_face()
{
	win_bonus = newwin(11, 31, 2, 48);
	wattron(win_bonus, COLOR_PAIR(22) | A_BOLD);
	mvwprintw(win_bonus, 5, 6, "\\\\\\|///");
	mvwprintw(win_bonus, 6, 6, "|* . *|");
	mvwprintw(win_bonus, 7, 6, "|_ v _|");
	wattroff(win_bonus, COLOR_PAIR(22) | A_BOLD);
	mvwprintw(win_bonus, 6, 17, "x");
	wattron(win_bonus, COLOR_PAIR(2));
	mvwprintw(win_bonus, 8, 21, "bonus");
	wattroff(win_bonus, COLOR_PAIR(2));
	wrefresh(win_bonus);
}

/* Afiseaza pe ecran "Enter NAME: ".
Preia sirul introdus de jucator si
afiseaza mesajul "Hello, (sir)!" */
void enter_name()
{
	char s[20] = {}, aux[8] = "Hello, ", aux2[2] = "!";
	mvwprintw(win_name, 13, 2, "Enter NAME: ");
	wrefresh(win_name);
	curs_set(1);
	move(13, 14);
	echo();
	getnstr(s, 20);
	noecho();
	curs_set(0);
	strcat(aux, s);
	strcat(aux, aux2);
	wmove(win_name, 13, 2);
	wclrtoeol(win_name);
	wrefresh(win_name);
	wattron(win_name, A_BOLD);
	mvwprintw(win_name, 13, (44 - strlen(aux)) / 2, aux);
	wattroff(win_name, A_BOLD);
	wrefresh(win_name);
}

// creare si afisare meniu
void create_menu(char *game_menu[])
{
	win_menu = newwin(6, 40, 14, 3);
	box(win_menu, 0, 0);
	mvwprintw(win_menu, 1, 2, "%s", game_menu[0]);
	mvwprintw(win_menu, 2, 2, "%s", game_menu[1]);
	mvwprintw(win_menu, 3, 2, "%s", game_menu[2]);
	wrefresh(win_menu);
}

// deselectare optiuni meniu
void update_menu(int i, char *game_menu[])
{
	int j;
	for (j = 1; j <= 3; j++)
		if (i != j)
			mvwprintw(win_menu, j, 2, "%s", game_menu[j - 1]);
}

// permite selectarea optiunilor
int select_menu(int i, char *game_menu[])
{
	int c;
	/* retiparire a optiunilor
	cu selectare implicita pe NEW GAME*/
	i = 1;
	mvwprintw(win_menu, 1, 2, "%s", game_menu[0]);
	mvwprintw(win_menu, 2, 2, "%s", game_menu[1]);
	mvwprintw(win_menu, 3, 2, "%s", game_menu[2]);
	wmove(win_menu, 1, 2);
	wattron(win_menu, A_STANDOUT);
	wprintw(win_menu, game_menu[0]);
	wattroff(win_menu, A_STANDOUT);
	wrefresh(win_menu);

	while (FOREVER)
	{
		refresh();
		wrefresh(win_menu);
		c = getch();
		if (c == '\n')
			return i;
		if (c == KEY_DOWN)
		{
			if (i == 3)
				i = 1;
			else
				i++;
			wattron(win_menu, A_STANDOUT);
			mvwprintw(win_menu, i, 2, "%s", game_menu[i - 1]);
			wattroff(win_menu, A_STANDOUT);
			update_menu(i, game_menu);
			wrefresh(win_menu);
		}
		if (c == KEY_UP)
		{
			if (i == 1)
				i = 3;
			else
				i--;
			wattron(win_menu, A_STANDOUT);
			mvwprintw(win_menu, i, 2, "%s", game_menu[i - 1]);
			wattroff(win_menu, A_STANDOUT);
			update_menu(i, game_menu);
			wrefresh(win_menu);
		}
		refresh();
	}
}

//afiseaza mesaj de eroare pentru optiunea RESUME
void write_resume()
{
	wattron(win_menu, COLOR_PAIR(8));
	mvwprintw(win_menu, 2, 16, "Game not started.");
	mvwprintw(win_menu, 3, 17, "Select NEW GAME.");
	wattroff(win_menu, COLOR_PAIR(8));
	wrefresh(win_menu);
}

//afiseaza mesajul "SUCCES!" la inceperea unui nou joc
void write_newgame()
{
	wattron(win_menu, COLOR_PAIR(6));
	mvwprintw(win_menu, 1, 16, "Good LUCK!");
	wattroff(win_menu, COLOR_PAIR(6));
	wrefresh(win_menu);
}

// stergere mesaj de eroare la RESUME
void delete_resume()
{
	wmove(win_menu, 2, 12);
	wclrtoeol(win_menu);
	wmove(win_menu, 3, 14);
	wclrtoeol(win_menu);
	box(win_menu, 0, 0);
	wrefresh(win_menu);
}

// interogare cu DA/NU la iesirea din joc
void write_quit()
{
	wattron(win_menu, COLOR_PAIR(1));
	mvwprintw(win_menu, 2, 17, "Quit game ?");
	mvwprintw(win_menu, 3, 16, "YES(y) / NO(n)");
	wattroff(win_menu, COLOR_PAIR(1));
}

// stergere interogare cu DA/NU la iesirea din joc
void delete_quit()
{
	wmove(win_menu, 2, 15);
	wclrtoeol(win_menu);
	wmove(win_menu, 3, 16);
	wclrtoeol(win_menu);
	box(win_menu, 0, 0);
	wrefresh(win_menu);
}

// decide daca se paraseste jocul sau nu
int decision_quit()
{
	int c;
	while (FOREVER)
	{
		c = getch();
		if (c == 'y')
			return 1;
		if (c == 'n')
			return 0;
	}
}

// stergere mesaj "SUCCES!" afisat la NEW GAME
void delete_newgame()
{
	wmove(win_menu, 1, 13);
	wclrtoeol(win_menu);
	box(win_menu, 0, 0);
	wrefresh(win_menu);
}

// afiseaza legenda de comenzi valide
void legend()
{
	win_legend = newwin(10, 40, 20, 3);
	wrefresh(win_legend);
	mvwaddstr(win_legend, 2, 2, "Combine cells using:");
	mvwaddstr(win_legend, 2, 24, "      ^      ");
	mvwaddstr(win_legend, 4, 24, "<     v     >");
	mvwaddstr(win_legend, 6, 2, "For MENU, press Q.");
	mvwaddstr(win_legend, 7, 2, "To  UNDO, press U.");
	mvwaddstr(win_legend, 9, 2, "For ");
	wattron(win_legend, A_BOLD);
	mvwaddstr(win_legend, 9, 6, "BONUS");
	wattroff(win_legend, A_BOLD);
	mvwaddstr(win_legend, 9, 11, ", combine");
	wattron(win_legend, A_BOLD);
	mvwaddstr(win_legend, 9, 21, "6");
	wattroff(win_legend, A_BOLD);
	mvwaddstr(win_legend, 9, 23, "cells in a row.");
	wrefresh(win_legend);
}

// afisare data si ora curenta
void display_time()
{
	time_t t;
	struct tm *area;
	t = time(NULL);
	area = localtime(&t);
	mvwprintw(win_board, 4, 2, "%02d.%02d.%d  %02d:%02d:%02d",
			  area->tm_mday, area->tm_mon + 1, area->tm_year + 1900,
			  area->tm_hour, area->tm_min, area->tm_sec);
	wrefresh(win_board);
}

// afisare fereastra pentru scor si data
void create_board(int row, int col)
{
	win_board = newwin(6, 40, row, col);
	mvwprintw(win_board, 1, 2, "SCORE");
	mvwprintw(win_board, 2, 2, "TOTAL SCORE");
	display_time();
	box(win_board, 0, 0);
	wrefresh(win_board);
}

//stabileste culoarea celulei din tabla de joc
int color_fill(int nri, int nrj, int a[][5])
{
	switch (a[nri][nrj])
	{
	case 1: // pentru celule libere
		return 9;
	case 2:
		return 10;
	case 4:
		return 11;
	case 8:
		return 12;
	case 16:
		return 13;
	case 32:
		return 14;
	case 64:
		return 15;
	case 128:
		return 16;
	case 256:
		return 17;
	case 512:
		return 18;
	case 1024:
		return 19;
	case 2048:
		return 20;
	default:
		return 0;
	}
}

/*returneaza modul de tiparire al valorilor in celula
(asezare in patratel) */
int retype_matrix(int nri, int nrj, int a[][5])
{
	if ((a[nri][nrj] >= 9) && (a[nri][nrj] <= 512))
		return 2;
	else if ((a[nri][nrj] >= 1) && (a[nri][nrj] <= 8))
		return 3;
	else
		return 1;
}

//colorare tabla de joc
void fill_matrix(int a[][5])
{
	int i, j, n, k, nri = 0, nrj = 0, col, type;
	// col --> retine culoarea
	// type --> retine modul de tiparire
	for (i = 1; i < 16; i = i + 4)
	{
		nri++;
		nrj = 0;
		for (j = 2; j < 25; j = j + 7)
		{
			nrj++;
			col = color_fill(nri, nrj, a);

			// deschidere atribut culoare
			if (col != 9)
				wattron(win_matrix, COLOR_PAIR(col) | A_BOLD);
			else
				wattron(win_matrix, COLOR_PAIR(col));

			// tiparire
			for (k = 0; k < 3; k++)
				for (n = 0; n < 6; n++)
					mvwprintw(win_matrix, k + i, n + j, " ");
			type = retype_matrix(nri, nrj, a);
			mvwprintw(win_matrix, i + 1, j + type, "%d", a[nri][nrj]);

			// inchidere atribut culoare
			if (col != 9)
				wattroff(win_matrix, COLOR_PAIR(col) | A_BOLD);
			else
				wattroff(win_matrix, COLOR_PAIR(col));
		}
	}
	wrefresh(win_matrix);
}

//creare tabla de joc
void create_matrix()
{
	win_matrix = newwin(17, 31, 13, 48);
	box(win_matrix, 0, 0);
	wrefresh(win_matrix);
}

/*genereaza pozitiile primelor doua valori de pe tabla
astfel incat acestea sa nu coincida*/
void random_position(int poz[])
{
	int random_number, nr;
	do
	{
		nr = 0;
		srand(time(NULL));
		while (nr != 4)
		{
			random_number = rand() % 5;
			if (random_number != 0)
			{
				poz[nr] = random_number;
				nr++;
			}
		}
	} while ((poz[0] == poz[2]) && (poz[1] == poz[3]));
}

// genereaza primele doua valori de pe tabla
void random_numbers(int n[], int num)
{
	int random_number, nr = 0;
	srand(time(NULL));
	while (nr != num)
	{
		random_number = rand() % 5;
		if ((random_number == 2) || (random_number == 4))
		{
			n[nr] = random_number;
			nr++;
		}
	}
}

void initialize_matrix(int n[], int poz[], int a[][5])
{
	int i, j;
	//initializare centru matrice cu unu
	for (i = 1; i <= 4; i++)
		for (j = 1; j <= 4; j++)
			a[i][j] = 1;

	random_numbers(n, 2);
	random_position(poz);

	//pune numerele generate pe pozitiile corespunzatoare in matrice
	a[poz[0]][poz[1]] = n[0];
	a[poz[2]][poz[3]] = n[1];
}

// stergere mesaj "You lose + game over"
void delete_endgame()
{
	wmove(win_end_game, 4, 1);
	wclrtoeol(win_end_game);
	wmove(win_end_game, 6, 1);
	wclrtoeol(win_end_game);
	wrefresh(win_end_game);
}

// initializare variabile si ferestre pentru joc nou
void new_game(int n[], int poz[], int a[][5], long int *score, int *bonus_score)
{
	*score = 0;
	*bonus_score = 0;
	n[0] = n[1] = 0;
	poz[0] = poz[1] = poz[2] = poz[3] = 0;
	legend();
	nodelay(stdscr, TRUE);
	create_board(14, 3);
	initialize_matrix(n, poz, a);
	create_matrix();
	fill_matrix(a);
	wattron(win_bonus, COLOR_PAIR(2) | A_BOLD);
	mvwprintw(win_bonus, 6, 20, "%d", *bonus_score);
	wattroff(win_bonus, COLOR_PAIR(2) | A_BOLD);
	draw_bonus_face();
}

// mutare elemente in STANGA (Vest)
void move_left(int a[][5])
{
	int aux[5], i, j, k, nr;
	for (i = 1; i <= 4; i++)
	{
		nr = 0;
		for (j = 1; j <= 4; j++)
			if (a[i][j] != 1)
			{
				nr++;
				aux[nr] = a[i][j];
			}
		for (k = 1; k <= 4; k++)
			if (k <= nr)
				a[i][k] = aux[k];
			else
				a[i][k] = 1;
	}
}

// mutare elemente in DREAPTA (Est)
void move_right(int a[][5])
{
	int aux[5], i, j, k, nr;
	for (i = 1; i <= 4; i++)
	{
		nr = 0;
		for (j = 1; j <= 4; j++)
			if (a[i][j] != 1)
			{
				nr++;
				aux[nr] = a[i][j];
			}
		for (k = 1; k <= 4; k++)
			if (k <= nr)
				a[i][5 - k] = aux[nr - k + 1];
			else
				a[i][5 - k] = 1;
	}
}

// mutare elemente in SUS (Nord)
void move_up(int a[][5])
{
	int aux[5], i, j, k, nr;
	for (j = 1; j <= 4; j++)
	{
		nr = 0;
		for (i = 1; i <= 4; i++)
			if (a[i][j] != 1)
			{
				nr++;
				aux[nr] = a[i][j];
			}
		for (k = 1; k <= 4; k++)
			if (k <= nr)
				a[k][j] = aux[k];
			else
				a[k][j] = 1;
	}
}

// mutare elemente in JOS (Sud)
void move_down(int a[][5])
{
	int aux[5], i, j, k, nr;
	for (j = 1; j <= 4; j++)
	{
		nr = 0;
		for (i = 1; i <= 4; i++)
			if (a[i][j] != 1)
			{
				nr++;
				aux[nr] = a[i][j];
			}
		for (k = 1; k <= 4; k++)
			if (k <= nr)
				a[5 - k][j] = aux[nr - k + 1];
			else
				a[5 - k][j] = 1;
	}
}

// combina la STANGA (VEST)
void combine_left(int a[][5], long int *score, int *bonus_score)
{
	//k = nr. de combinari
	int i, j, k = 0;
	for (i = 1; i <= 4; i++)
	{
		j = 1;
		while (j <= 3)
		{
			if ((a[i][j] == a[i][j + 1]) && (a[i][j] != 1))
			{
				a[i][j] = 2 * a[i][j];
				k++;
				*score = *score + a[i][j];
				a[i][j + 1] = 1;
				j = j + 2;
			}
			else
				j++;
		}
	}
	if (k >= 3)
		*bonus_score = *bonus_score + 1;
}

// combina la DREAPTA (EST)
void combine_right(int a[][5], long int *score, int *bonus_score)
{
	// k = nr. de combinari
	int i, j, k = 0;
	for (i = 1; i <= 4; i++)
	{
		j = 4;
		while (j >= 2)
		{
			if ((a[i][j] == a[i][j - 1]) && (a[i][j] != 1))
			{
				a[i][j] = 2 * a[i][j];
				k++;
				*score = *score + a[i][j];
				a[i][j - 1] = 1;
				j = j - 2;
			}
			else
				j--;
		}
	}
	if (k >= 3)
		*bonus_score = *bonus_score + 1;
}

// combina in SUS (NORD)
void combine_up(int a[][5], long int *score, int *bonus_score)
{
	// k = nr. de combinari
	int i, j, k = 0;
	for (j = 1; j <= 4; j++)
	{
		i = 1;
		while (i <= 3)
		{
			if ((a[i][j] == a[i + 1][j]) && (a[i][j] != 1))
			{
				a[i][j] = 2 * a[i][j];
				*score = *score + a[i][j];
				a[i + 1][j] = 1;
				i = i + 2;
				k++;
			}
			else
				i++;
		}
	}
	if (k >= 3)
		*bonus_score = *bonus_score + 1;
}

// combina in JOS (SUD)
void combine_down(int a[][5], long int *score, int *bonus_score)
{
	// k = nr. de combinari
	int i, j, k = 0;
	for (j = 1; j <= 4; j++)
	{
		i = 4;
		while (i >= 2)
		{
			if ((a[i][j] == a[i - 1][j]) && (a[i][j] != 1))
			{
				a[i][j] = 2 * a[i][j];
				*score = *score + a[i][j];
				a[i - 1][j] = 1;
				i = i - 2;
				k++;
			}
			else
				i--;
		}
	}
	if (k >= 3)
		*bonus_score = *bonus_score + 1;
}

// genereaza pozitia pentru noul numar de adaugat pe tabla
void new_random_position(int poz[], int a[][5])
{
	/* search_i memoreaza i-urile celulelor libere
	search_j memoreaza j-urile celulelor libere 
	k= nr. celulelor libere */
	int random_number, i, j, k = 0, search_i[16], search_j[16];
	for (i = 1; i <= 4; i++)
		for (j = 1; j <= 4; j++)
			if (a[i][j] == 1) // daca celula e libera
			{
				search_i[k] = i;
				search_j[k] = j;
				k++;
			}
	srand(time(NULL));
	random_number = rand() % k;
	poz[0] = search_i[random_number]; // i-ul noului element
	poz[1] = search_j[random_number]; // j-ul noului element
}

// adaugare element nou pe tabla de joc
void add_number(int n[], int poz[], int a[][5])
{
	random_numbers(n, 1);
	new_random_position(poz, a);
	a[poz[0]][poz[1]] = n[0];
}

// copiaza matricea a in b
void copy_matrix(int a[][5], int b[][5])
{
	/* a = inainte de mutare
	b = dupa mutare */
	int i, j;
	for (i = 1; i <= 4; i++)
		for (j = 1; j <= 4; j++)
			b[i][j] = a[i][j];
}

// afisare mesaje --> joc pierdut
void write_lose_game()
{
	win_end_game = newwin(11, 31, 2, 48);
	wattron(win_end_game, COLOR_PAIR(21) | A_BOLD);
	mvwprintw(win_end_game, 4, 9, "YOU LOSE... :(");
	wattroff(win_end_game, COLOR_PAIR(21) | A_BOLD);
	wattron(win_end_game, COLOR_PAIR(2) | A_BOLD);
	mvwprintw(win_end_game, 6, 11, "GAME OVER");
	wattroff(win_end_game, COLOR_PAIR(2) | A_BOLD);
	wrefresh(win_end_game);
}

// afisare mesaje --> joc castigat
void write_win_game()
{
	win_win = newwin(11, 31, 2, 48);
	wattron(win_win, COLOR_PAIR(22) | A_BOLD);
	mvwprintw(win_win, 4, 10, "YOU WIN  :)");
	wattroff(win_win, COLOR_PAIR(22) | A_BOLD);
	wattron(win_win, COLOR_PAIR(2) | A_BOLD);
	mvwprintw(win_win, 6, 11, "GOOD JOB!");
	wattroff(win_win, COLOR_PAIR(2) | A_BOLD);
	wrefresh(win_win);
}

// decide daca jucatorul a pierdut
int decision_lose_game(int a[][5], int b[][5])
{
	/*Compara matricea a cu matricea b
	Daca matricele sunt identice si nu exista celule libere =>
	=> return 1 (JOC PIERDUT)*/
	int i, j, nr = 0;
	for (i = 1; i <= 4; i++)
		for (j = 1; j <= 4; j++)
			if ((a[i][j] != 1) && (a[i][j] == b[i][j]))
				nr++;
	if (nr == 16)
		return 1;
	else
		return 0;
}

// decide daca se adauga un numar nou
void decision_add_number(int n[], int poz[], int a[][5], int b[][5])
{
	int i, j, ok = 0;
	for (i = 1; i <= 4; i++)
		for (j = 1; j <= 4; j++)
			if (a[i][j] != b[i][j])
				ok = 1;
	if (ok == 1)
		add_number(n, poz, a);
}

// memorare numar de celule libere
void count_free(int x, int nr[], int a[][5])
{
	int i, j, k = 0;
	for (i = 1; i <= 4; i++)
		for (j = 1; j <= 4; j++)
			if (a[i][j] == 1)
				k++;
	nr[x] = k;
}

/* decide daca se POATE adauga un numar nou sau nu
cand nu a fost apasata nicio tasta */
int decision_add_number_NO_EVENTS(int a[][5], int b[][5])
{
	int i, j, ok = 0;
	for (i = 1; i <= 4; i++)
		for (j = 1; j <= 4; j++)
			if (a[i][j] != b[i][j])
				ok = 1;
	if (ok == 1)
		return 1; // se poate adauga numar
	else
		return -1; // nu se poate adauga numar
}

int play_game(int choose, char *game_menu[], int a[][5], int poz[], int n[],
			  long int *score, long int *total_score, int *bonus_score)
{
	int i, j, c, ok_left, ok_right, ok_up, ok_down, ok, nfds, sel,
		maxim, poz_maxim = 0, move_verif[3] = {0}, undo, up = 0, game_over = 0,
			   initial_score = 0, b[5][5], d[5][5], nr[4] = {0}, copy_matrix_a[5][5],
			   var_null2;
	long int copy_score, copy_total_score, var_null;
	struct timeval timeout;
	/*
	c = tasta apasata.
	ok_left/right/up/down = 1 daca jocul s-a terminat.
	ok=1 daca in intervalul de timp dat de TIMEOUT se apasa o tasta.
	nfds = numarul de elemente din multimea read_descriptors.
	maxim = nr. maxim de celule libere
	move_verif [?] = 1 => se poate adauga numar
	move_verif [?] = -1 => NU se poate adauga numar
	up=0 =>optiunea UNDO nu este accesibila pt ca nu s-a facut nicio mutare
	*/
	fd_set read_descriptors;
	nfds = 1;
	FD_ZERO(&read_descriptors);
	FD_SET(0, &read_descriptors);
	timeout.tv_sec = 1;
	timeout.tv_usec = 0;
	//afisare initiala scor bonus
	wattron(win_bonus, COLOR_PAIR(2) | A_BOLD);
	mvwprintw(win_bonus, 6, 20, "%d", *bonus_score);
	wattroff(win_bonus, COLOR_PAIR(2) | A_BOLD);
	wrefresh(win_bonus);

	while (FOREVER)
	{
		ok_left = ok_right = ok_up = ok_down = 0;
		ok = 0;
		undo = 0;
		var_null = var_null2 = 0;
		/* verifica daca exista mutari disponibile,
		deci daca jocul s-a incheiat sau nu : */
		// verificare mutare disponibila STANGA
		copy_matrix(a, d);
		move_left(d);
		combine_left(d, &var_null, &var_null2);
		move_left(d);
		decision_add_number(n, poz, d, a);
		ok_left = decision_lose_game(a, d);
		// verificare mutare disponibila DREAPTA
		copy_matrix(a, d);
		move_right(d);
		combine_right(d, &var_null, &var_null2);
		move_right(d);
		decision_add_number(n, poz, d, a);
		ok_right = decision_lose_game(a, d);
		// verificare mutare disponibila SUS
		copy_matrix(a, d);
		move_up(d);
		combine_up(d, &var_null, &var_null2);
		move_up(d);
		decision_add_number(n, poz, d, a);
		ok_up = decision_lose_game(a, d);
		// verificare mutare disponibila JOS
		copy_matrix(a, d);
		move_down(d);
		combine_down(d, &var_null, &var_null2);
		move_down(d);
		decision_add_number(n, poz, d, a);
		ok_down = decision_lose_game(a, d);
		// verificare joc pierdut
		if ((ok_left == 1) && (ok_right == 1) && (ok_up == 1) && (ok_down == 1))
		{
			game_over = 1;
			delwin(win_bonus);
			write_lose_game();
		}
		// verificare joc castigat
		for (i = 1; i <= 4; i++)
			for (j = 1; j <= 4; j++)
				if (a[i][j] == 2048)
				{
					game_over = 1;
					delwin(win_bonus);
					write_win_game();
				}
		// revenire la meniu daca jocul s-a incheiat
		if (game_over == 1)
		{
			touchwin(win_menu);
			wrefresh(win_menu);
			choose = select_menu(choose, game_menu);
			return choose;
		}

		initial_score = *score;
		wattron(win_board, COLOR_PAIR(2) | A_BOLD);
		mvwprintw(win_board, 1, 10, "%ld", *score);
		mvwprintw(win_board, 2, 15, "%ld", *total_score);
		wattroff(win_board, COLOR_PAIR(2) | A_BOLD);
		wrefresh(win_board);

		for (i = TIMEOUT; i >= 0; i--)
		{
			FD_SET(0, &read_descriptors);
			timeout.tv_usec = 0;
			timeout.tv_sec = 1;
			sel = select(nfds, &read_descriptors, NULL, NULL, &timeout);
			display_time();
			//afisare timp ramas pentru mutare
			wattron(win_board, COLOR_PAIR(21) | A_BOLD);
			mvwprintw(win_board, 2, 35, "%02d", i);
			wattroff(win_board, COLOR_PAIR(21) | A_BOLD);
			wrefresh(win_board);
			if (sel == 1) // timpul nu a expirat
			{
				c = getch();
				ok = 1;
				break;
			}
		}
		if (ok == 1) //a fost apasata o tasta
		{
			display_time();
			switch (tolower(c))
			{
			case 'q': //revine la meniu
				touchwin(win_menu);
				wrefresh(win_menu);
				choose = select_menu(choose, game_menu);
				return choose;
			case 'u': // UNDO
				if (up != 0)
				{
					copy_matrix(copy_matrix_a, a);
					fill_matrix(a);
					*score = copy_score;
					*total_score = copy_total_score;
					wrefresh(win_matrix);
					undo = 1;
				}
				break;
			}
			switch (c)
			{
			case KEY_LEFT:
				copy_matrix(a, copy_matrix_a);
				copy_score = *score;
				copy_total_score = *total_score;
				copy_matrix(a, b);
				move_left(a);
				combine_left(a, score, bonus_score);
				move_left(a);
				decision_add_number(n, poz, a, b);
				up++;
				break;
			case KEY_RIGHT:
				copy_matrix(a, copy_matrix_a);
				copy_score = *score;
				copy_total_score = *total_score;
				copy_matrix(a, b);
				move_right(a);
				combine_right(a, score, bonus_score);
				move_right(a);
				decision_add_number(n, poz, a, b);
				up++;
				break;
			case KEY_UP:
				copy_matrix(a, copy_matrix_a);
				copy_score = *score;
				copy_total_score = *total_score;
				copy_matrix(a, b);
				move_up(a);
				combine_up(a, score, bonus_score);
				move_up(a);
				decision_add_number(n, poz, a, b);
				up++;
				break;
			case KEY_DOWN:
				copy_matrix(a, copy_matrix_a);
				copy_score = *score;
				copy_total_score = *total_score;
				copy_matrix(a, b);
				move_down(a);
				combine_down(a, score, bonus_score);
				move_down(a);
				decision_add_number(n, poz, a, b);
				up++;
				break;
			}
		}

		if (ok == 0)
		{
			/* Nicio tasta apasata => calculare mutare care
			elibereaza cele mai multe celule */
			// mutare disponibila STANGA
			copy_matrix(a, d);
			move_left(d);
			combine_left(d, &var_null, &var_null2);
			move_left(d);
			move_verif[0] = decision_add_number_NO_EVENTS(d, a);
			count_free(0, nr, d);
			// mutare disponibila DREAPTA
			copy_matrix(a, d);
			move_right(d);
			combine_right(d, &var_null, &var_null2);
			move_right(d);
			move_verif[1] = decision_add_number_NO_EVENTS(d, a);
			count_free(1, nr, d);
			// mutare disponibila SUS
			copy_matrix(a, d);
			move_up(d);
			combine_up(d, &var_null, &var_null2);
			move_up(d);
			move_verif[2] = decision_add_number_NO_EVENTS(d, a);
			count_free(2, nr, d);
			// mutare disponibila JOS
			copy_matrix(a, d);
			move_up(d);
			combine_up(d, &var_null, &var_null2);
			move_up(d);
			move_verif[3] = decision_add_number_NO_EVENTS(d, a);
			count_free(3, nr, d);

			maxim = -2;
			poz_maxim = -2;
			for (i = 0; i <= 3; i++)
				if ((maxim < nr[i]) && (move_verif[i] != -1))
				{
					maxim = nr[i]; //nr maxim de celule libere
					poz_maxim = i;
				}

			switch (poz_maxim)
			{
			case 0: // STANGA
				copy_matrix(a, copy_matrix_a);
				copy_score = *score;
				copy_total_score = *total_score;
				copy_matrix(a, b);
				move_left(a);
				combine_left(a, score, bonus_score);
				move_left(a);
				decision_add_number(n, poz, a, b);
				up++;
				break;
			case 1: // DREAPTA
				copy_matrix(a, copy_matrix_a);
				copy_score = *score;
				copy_total_score = *total_score;
				copy_matrix(a, b);
				move_right(a);
				combine_right(a, score, bonus_score);
				move_right(a);
				decision_add_number(n, poz, a, b);
				up++;
				break;
			case 2: // SUS
				copy_matrix(a, copy_matrix_a);
				copy_score = *score;
				copy_total_score = *total_score;
				copy_matrix(a, b);
				move_up(a);
				combine_up(a, score, bonus_score);
				move_up(a);
				decision_add_number(n, poz, a, b);
				up++;
				break;
			case 3: // JOS
				copy_matrix(a, copy_matrix_a);
				copy_score = *score;
				copy_total_score = *total_score;
				copy_matrix(a, b);
				move_down(a);
				combine_down(a, score, bonus_score);
				move_down(a);
				decision_add_number(n, poz, a, b);
				up++;
				break;
			}
		}
		fill_matrix(a);
		if (undo == 0)
			*total_score = *total_score + *score - initial_score;

		//afisare scoruri
		wattron(win_board, COLOR_PAIR(2) | A_BOLD);
		mvwprintw(win_board, 1, 10, "%ld", *score);
		mvwprintw(win_board, 2, 15, "%ld", *total_score);
		wattroff(win_board, COLOR_PAIR(2) | A_BOLD);
		wrefresh(win_board);
		wattron(win_bonus, COLOR_PAIR(2) | A_BOLD);
		mvwprintw(win_bonus, 6, 20, "%d", *bonus_score);
		wattroff(win_bonus, COLOR_PAIR(2) | A_BOLD);
		wrefresh(win_bonus);
	}
}

int main()
{
	int choose = 0, ok = 0, n[2] = {0}, poz[4] = {0}, a[5][5], bonus_score;
	long int score, total_score = 0;
	char *game_menu[3] = {"New Game", "Resume", "Quit"};

	initscr();
	clear();
	noecho();
	cbreak();
	curs_set(0);
	keypad(stdscr, TRUE);
	initialize_color();

	name_game();							 //afiseaza "2048"
	enter_name();							 //citire nume jucator
	create_menu(game_menu);					 //afisare meniu
	choose = select_menu(choose, game_menu); //selectare optiuni din meniu

	while (FOREVER)
	{
		if (choose == 3) //QUIT
		{
			delete_newgame();
			delete_resume();
			write_quit();
			wrefresh(win_menu);
			if ((decision_quit()) == 0)
			{
				wrefresh(win_menu);
				delete_quit();
				choose = select_menu(choose, game_menu);
				wrefresh(win_menu);
			}
			else
				break;
		}
		if (choose == 2) //RESUME
		{
			if (ok == 0)
			{
				write_resume();
				choose = select_menu(choose, game_menu);
				wrefresh(win_menu);
			}
			else
			{
				delete_newgame();
				touchwin(win_board);
				wrefresh(win_board);
				wrefresh(win_matrix);
				choose = play_game(choose, game_menu, a, poz, n, &score, &total_score, &bonus_score);
			}
		}
		if (choose == 1) //NEW GAME
		{
			ok = 1;
			delwin(win_end_game);
			write_newgame();
			delete_resume();
			napms(900); //DELAY
			delete_newgame();
			new_game(n, poz, a, &score, &bonus_score);
			choose = play_game(choose, game_menu, a, poz, n, &score, &total_score, &bonus_score);
			getch();
		}
	}
	delwin(win_name);
	delwin(win_menu);
	delwin(win_legend);
	delwin(win_board);
	delwin(win_matrix);
	delwin(win_end_game);
	delwin(win_bonus);
	delwin(win_win);
	endwin();
	return 0;
}

/*
 * In The Name Of God
 * ========================================
 * [] File Name : ai.c
 *
 * [] Creation Date : 26-02-2016
 *
 * [] Created By : Parham Alvani (parham.alvani@gmail.com)
 * =======================================
*/
/*
 * Copyright (c) 2016 Parham Alvani.
*/

#include "game.h"
#include "board.h"
#include "player.h"
#include "ai.h"

#include <stdlib.h>

static double minimax(struct game *g) {
	/* How is the position like for player (their turn) on board? */
	enum state s = game_state(g);

	if (s == WIN)
		return 1;
	else if (s == LOSS)
		return -1;
	else if (s == DRAW)
		return 0;

	struct move m = {-1, -1};
	/* Losing moves are preferred to no move */
	double score = 0;
	int sum = 0, moves = 0;
	int i, j;
	/* For all moves */
	for (i = 0; i < g->b->row; i++) {
		for (j = 0; j < g->b->col; j++) {
			/* If legal move */
			if (board_get_cell(g->b, i, j) == 0) {
				/* Try the move */
				game_move(g, i, j);
				moves++;

				double current = minimax(g);

				if (current < 0) {
					sum++;
				}

				/* Pick the one that's worst for the opponent */
				if(score == 0 || -current > score) {
					score = -current;
					m.row = i;
					m.col = j;
				}

				/* Reset board after try */
				game_move_back(g, i, j);
			}
		}
	}

	if(m.row == -1 && m.col == -1)
		return 0;
	return ((double) sum / moves);
}

static struct move move(struct player_c *p, struct move enemy_move) {
	struct game *g = (struct game *) p->context;
	
	/* check for enemy_move */
	if (enemy_move.row != -1 && enemy_move.col != -1)
		game_move(g, enemy_move.row, enemy_move.col);

	struct move m = {-1, -1};
	/* Losing moves are preferred to no move */
	int score = 0;
	int i, j;
	/* For all moves */
	for (i = 0; i < g->b->row; i++) {
		for (j = 0; j < g->b->col; j++) {
			/* If legal move */
			if (board_get_cell(g->b, i, j) == 0) {
				/* Try the move */
				game_move(g, i, j);
#ifdef DEBUG
	printf("==> AI minimax root:\n");
	board_print(g->b, stdout);
	printf("--\n");
#endif
				double current = minimax(g);
#ifdef DEBUG
	printf("game result: %g\n", current);
#endif

				/* Pick the one that's worst for the opponent */
				if(score == 0 || current == -1) {
					score = -current;
					m.row = i;
					m.col = j;
				}

				/* Reset board after try */
				game_move_back(g, i, j);
			}
		}
	}

	/* do our move */
	game_move(g, m.row, m.col);

	return m;
}

struct player_c *ai_player_new(enum player p, int row, int col)
{
	struct player_c *new = malloc(sizeof(struct player_c));
	new->do_move = move;
	new->context = game_new(p, row, col);

	return new;
}

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define BOARD_LIMIT 80
#define BOARD_MINUMUM 25
#define RANDOM 0.20 /* чтобы 20% поля были заполнены случайными обитаемыми ячейками */
#define MAX_ITERATION 5000
#define BIRTH_CONDITION 3
#define SURVIVE_CONDITION_1 3 /* включает саму ячейку, поэтому +1 к условию правила */
#define SURVIVE_CONDITION_2 4 /* ключает саму ячейку, поэтому +1 к условию правила */
#define ANIMATE 1

//int создает доску  (int user_size);
void set_board (int board[][BOARD_LIMIT], int h, int w);
void randomise_board (int board[][BOARD_LIMIT], int h, int w);
void print_board (int board[][BOARD_LIMIT], int h, int w);
void life_or_death (int board[][BOARD_LIMIT], int next_board[][BOARD_LIMIT], int k, int j, int h, int w);
int count_neighbours (int board[][BOARD_LIMIT], int i, int j, int h, int w);
void update_board (int board[][BOARD_LIMIT], int next_board[][BOARD_LIMIT], int k, int j, int h, int w);
int check_if_equal(int board[][BOARD_LIMIT], int next_board[][BOARD_LIMIT], int h, int w);

/* main программа */
int
main(void)
{
        int i, k=0, j=0, count=1;
        int h = 25, w = 80;
        int board[BOARD_LIMIT][BOARD_LIMIT];
        int next_board[BOARD_LIMIT][BOARD_LIMIT];

        //user_size = create_board(user_size); /* обновляет размер платы в зависимости от ввода пользователя */
        set_board(board, h, w); /* инициализировать текущую доску (поле) */
        randomise_board(board, h, w);
        set_board(next_board, h, w); /* инициализировать следующую доску до нуля */
        printf("\nIntial board:\n");
        print_board(board, h, w);
        printf("\nThe Game of Life begins:\n");

        for (i = 0; i < MAX_ITERATION; i++) {
                life_or_death (board, next_board, k, j, h, w); /* обновить следующую доску */
                if (check_if_equal(board, next_board, h, w) == 1) {
                        i = MAX_ITERATION;
                } else {
                        printf("\nIteration number: %d\n", count);
                        print_board(next_board, h, w);
                        update_board(board, next_board, k, j, h, w);     /* обновляет текущую доску */
                        count++;
                }
        }

        printf("The Game of Life has ended.\n");

        return 0;
}

/* проверяет, равны ли доски */
int
check_if_equal(int board[][BOARD_LIMIT], int next_board[][BOARD_LIMIT], int h, int w)
{
        int k, j;
  
        for (k = 0; k < h; k++) {
                for (j = 0; j < w; j++) {
                        if (board[k][j] != next_board[k][j]) {
                                return 0;
                        }
                }
        }
        return (1);
}

/*user-defined board
int
create_board(int board_size)
{
        printf("Добро пожаловать в мою реализацию игры жизни Конвея\n");
        printf("Пожалуйста, убедитесь, что вы используете этот терминал в полноэкранном режиме для достижения наилучших результатов.!\n");
        printf("У вас может быть любой размер от 25 до 80. Насколько большой должна быть доска? \n");

        if (scanf("%d", &board_size) != 1) {
                printf("Произошла ошибка при вводе. Пожалуйста, введите целое число. Попробуйте снова.\n");
        } else if (board_size > BOARD_LIMIT || board_size < BOARD_MINUMUM) {
                printf("Это выходит за рамки наших целей\n");
        } else {
                printf("Теперь размер платы %d\n", board_size);
        }

        return board_size;
}

устанавливает значения доски на ноль - очищает ненужные значения */
void
set_board (int board[][BOARD_LIMIT], int h, int w)
{
        int k, j;

        for (k = 0; k < h; k++) {
                for (j = 0; j < w; j++) {
                        board[k][j] = 0;
                  }
        }
}

/* рандомизировать доску */
void
randomise_board(int board[][BOARD_LIMIT], int h, int w)
{
        int k, i, j;
  
        srand((unsigned)time(NULL)); /* установить случайное начальное число */

        for (k = 0; k < (h * w * RANDOM); k++) {
                i = rand() % (h);
                j = rand() % (w);
                board[i][j] = 1;
        }
}

/* печатает доску */
void
print_board(int board[][BOARD_LIMIT], int h, int w)
{
        int i, j;

        for (i = 0; i < h; i++) {
                for (j = 0; j < w; j++) {
                        if (board[i][j] == 1) {
                                putchar ('*');
                        } else {
                                putchar(' ');
                        }
                }
                printf("\n");
        }

#if ANIMATE == 1
        sleep(1);
#endif
}

/* считает соседей данной ячейки */
/* см. примечание к строке 174 для схемы индексной таблицы. */
int
count_neighbours(int board[][BOARD_LIMIT], int k, int j, int h, int w)
{
        int row_indx, column_indx, value, neighbour_count;
        value = 0;
        neighbour_count = 0; /* сбросить счетчик для новой ячейки */

        for (row_indx = -1; row_indx <= 1; row_indx++) {
                for (column_indx = -1; column_indx <= 1; column_indx++) {
                        if ((k + row_indx >= 0) &&
                            (j + column_indx >= 0) &&
                            (k + row_indx < h) &&
                            (j + column_indx < w)) {
                                /* присвоить значение ячейки (0 или 1) значению переменной */
                                value = board[k + row_indx][j + column_indx];
                                  /* значение определяется на основе ячейки.
                                  если есть соседняя ячейка,
                                  значение = 1, иначе значение = 0
                                  */
                                if (value == 1) {
                                        neighbour_count++;
                                }
                        }
                }
        }

        return neighbour_count;
}

/* выполнять правила жизни или смерти */
/* Если клетка обитаема и имеет ровно 2 или 3 соседей - Выживает (ячейка остается обитаемой)
 * Если клетка обитаема и имеет менее 2 или более 3 соседей - Смерть (клетка становится необитаемой)
 * Если клетка необитаемая имеет ровно 3 соседей - Рождение (клетка становится обитаемой) */

void
life_or_death(int board[][BOARD_LIMIT], int next_board[][BOARD_LIMIT], int k, int j, int h, int w)
{
        int neighbour_count;

        set_board(next_board, h, w);

        for (k = 0; k < h; k++) {
                for (j = 0; j < w; j++) {
                        neighbour_count = count_neighbours(board, k, j, h, w);
                        if (board[k][j] == 0) {
                                /* ячейка необитаема, проверьте условия
                                для рождения встречаются
                                */
                                if (neighbour_count == BIRTH_CONDITION) {
                                        next_board[k][j] = 1;   /* birth */
                                } else {
                                        next_board[k][j] =0;
                                }
                        } else if (board[k][j] == 1) {
                                /* ячейка существует, проверьте, выжила она или умерла */
                                if (neighbour_count == SURVIVE_CONDITION_1 ||
                                    neighbour_count == SURVIVE_CONDITION_2) {
                                        /* выживание */
                                        next_board[k][j] = board[k][j];
                                } else {
                                        /* смерть */
                                        next_board[k][j] = 0;
                                }
                        }
                }
        }
        printf("\n");
}

/* обновляет плату для следующей итерации*/
void
update_board(int board[][BOARD_LIMIT], int next_board[][BOARD_LIMIT], int k, int j, int h, int w)
{
        for (k = 0; k < h; k++) {
                for (j = 0; j < w; j++) {
                        board[k][j] = next_board[k][j];
                }
        }
}

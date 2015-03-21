#include "local_search2.h"
#include <time.h>

typedef struct s_List List;
	struct s_List {
		List *next;
		int qconf;
	};

	//List *list = NULL;

	List * list_create(int q) {
		List * list= malloc(sizeof(list));
		if (list)
		{
			list->qconf = q;
			list->next = NULL;
		}
		return list;
	}

	List * list_adhead(List * old,int data) {
		List *list = list_create(data);
		if(list)
			list->next = old;
		return list;
	}

	List * list_append(List *list, int data) {
	    List **plist = &list;
	    while (*plist)
	       plist = &(*plist)->next;
	    *plist = list_create(data);
	    if (*plist)
	       return list;
	    else
	       return NULL;
	}
	List * list_remove_first(List *list) {
		List *first = list;
		list = list->next;
		free(first);
		return list;
	}
	void list_destroy(List *list) {
		while (list)
			list = list_remove_first(list);
	}
	int list_length(List *list) {
		int length = 0;
		while (list)
		{
			length++;
			list = list->next;
		}
		return length;
	}
	void list_affiche(List * list) {
		while (list) {
			//printf("hello world ");
			printf("%d ",list->qconf);
			list = list->next;

		}
	}
	int cb_conflict_list(const cb_t * cb,List ** tabli) {
		if (!cb->size) {
                log_err("Call cb_conflicts on non valid cb");
                return -1;
        }
        int conflict = 0;
        for (size_t i = 0; i < cb->size; i++)
        	if (cb->queens[i] != -1)
        		for (size_t j = i+1; j < cb->size; ++j)
        			if (cb->queens[j] != -1 &&
                    	(cb->queens[i] == cb->queens[j] ||
                        abs(i - j) == abs(cb->queens[i] - cb->queens[j]))
                    ) {

        					tabli[i] = list_adhead(tabli[i],j);
        					tabli[j] = list_adhead(tabli[j],i);

        					++conflict;
        			}

        return conflict;
	}
	int cb_conflicts_point_list(const cb_t * cb, List ** tabli, size_t ligne, size_t col) {
		int conflict = 0;
		u32 queen;
		for (size_t i = 0; (i < cb->size) && ((queen = cb->queens[i]) != -1); ++i) {
                if (    i != col &&
                        abs(i - col) == abs(queen - ligne)
                   ) {
                   	tabli[i] = list_adhead(tabli[i],col);
                   	tabli[col] = list_adhead(tabli[col],i);
                        ++conflict;
                   }
        }

        return conflict;
	}
	bool list_isinside(List * list, int x) {
		while (list) {
			if (list->qconf == x)
				return true;
			else list = list->next;
		}
		return false;
	}
	void init_test(cb_t * cb)//debug
	{

			size_t queens[10]={ 0, 5, 4, 1, 2, 3, 9, 7, 8, 6};
			for (int i=0; i < 10 ; i++) {
				cb->queens[i] = queens[i];
			}



	}
	/*size_t dame_max_conflit(size_t taille, List **tablist) {
		size_t max = 0;
		for (size_t i = 0; i < taille ; i++) {
			if(list_length(tablist[i]) > max)
				max = i;
		}
	}*/
	void init_diagonal(const cb_t * cb,size_t * diag_pos,size_t * diag_neg,size_t * conflict_pos, size_t * conflict_neg) {
		for(size_t i=0; i < cb->size ; i++) {

			diag_pos[diagp(i,cb->queens[i],cb->size)] ++;
			if (diag_pos[diagp(i,cb->queens[i],cb->size)] > 1)
				conflict_pos[diagp(i,cb->queens[i],cb->size)]++;

			diag_neg[diagm(i,cb->queens[i],cb->size)] ++;
			if (diag_neg[diagm(i,cb->queens[i],cb->size)] > 1)
				conflict_neg[diagm(i,cb->queens[i],cb->size)]++;
		}
	}
	void add_queen(size_t size,size_t i, size_t j,size_t * diag_pos,size_t * diag_neg,size_t * conflict_pos, size_t * conflict_neg) {
		diag_pos[i +j]++;
		diag_neg[i - j + size]++;
		if (diag_pos[i+j] > 1)
			conflict_pos[i+j] ++;
		if (diag_neg[i - j + size] > 1)
			conflict_neg[i - j + size] ++;

	}
	void sup_queen(size_t size, size_t i,  size_t j, size_t * diag_pos,size_t * diag_neg,size_t * conflict_pos, size_t * conflict_neg) {
		//if (diag_pos[i + j] && diag_neg[i -j + size] > 0) {
			//if (diag_pos[i + j] > 0)
			diag_pos[i + j]--;
			//else
			//	diag_pos[i + j] = 0;

			//if (diag_neg[i - j + size] > 0)
				diag_neg[i - j + size]--;
			//else
			//	diag_neg[i - j + size] = 0;

			if (diag_pos[i + j] <= 1)
				conflict_pos[i + j]= 0;
			else
				conflict_pos[i + j]--;
			if (diag_neg[i - j + size] <= 1)
				conflict_neg[i - j + size] = 0;
			else
				conflict_neg[i - j + size]--;
		//}

	}

	void swap_d(size_t size, size_t i,size_t qi,size_t j,size_t qj,size_t * diag_pos,size_t * diag_neg,size_t * conflict_pos, size_t * conflict_neg) {
		//if (diag_pos[i + qi] > 0 && diag_neg[i - qi + size] > 0 && diag_pos[j + qj] > 0 && diag_neg[j - qj + size] > 0 )
		//{
		sup_queen(size,i,qi,diag_pos,diag_neg,conflict_pos,conflict_neg);
		sup_queen(size,j,qj,diag_pos,diag_neg,conflict_pos,conflict_neg);
		add_queen(size,i,qj,diag_pos,diag_neg,conflict_pos,conflict_neg);
		add_queen(size,j,qi,diag_pos,diag_neg,conflict_pos,conflict_neg);
		//}
	}
	bool queen_under_atak(size_t size, size_t i,size_t j, size_t * diag_pos, size_t * diag_neg) {
		return (diag_pos[i + j] > 1 || diag_neg[i - j + size] > 1);
	}
	void affiche_diag(size_t size,size_t * diag_pos) {
		printf("affichage ?\n");
		for (size_t i=0 ; i < size; i++) {
			printf("%d, ",diag_pos[i]);
		}
	}
	size_t sum_tab(size_t size, size_t * conflict_pos) {
		size_t value = 0;
		for (size_t i = 0; i < size; ++i)
		{
			value += conflict_pos[i];
		}
		return value;
	}
	size_t nb_conflict(size_t size, size_t * conflict_neg, size_t * conflict_pos) {
		return (sum_tab(size,conflict_neg)+sum_tab(size,conflict_pos));
	}

	void init_rand2(cb_t * cb) {

		size_t tail = cb->size;
		//srand(time(NULL));
		size_t *free_col = malloc(cb->size * sizeof(size_t));

		for (size_t i = 0 ; i < cb->size ; i++) {
			free_col[i]=i;
		}

		size_t indice = 0;
		while (indice < cb->size) {
			size_t rando = rand()%(tail);

			cb->queens[indice] = free_col[rando];
			//size_t tmp = alea;
			free_col[rando] = free_col[tail-1];
			--tail;
			++indice;


		}



		free(free_col);


	}





int local_search2(cb_t * cb) {
	size_t size = cb->size;
	time_t	before = time(NULL);
	//List * list = NULL;

	printf("l'initialisation a commencé \n");
	//cb_init_rand(cb,cb->size);
	init_rand2(cb);


	printf("l'initialisation est terminé %zu\n", time(NULL) - before);

	srand(time(NULL));
	//List * tablist[cb->size];
	size_t* diag_pos = malloc((size * 2)-1 * sizeof(size_t));
	size_t* diag_neg = malloc((size * 2)-1 * sizeof(size_t));


	size_t* conflict_pos = malloc((size * 2)-1 * sizeof(size_t));
	size_t* conflict_neg = malloc((size * 2)-1 * sizeof(size_t));





	for (size_t i=0; i < (cb->size*2) -1  ; i++) {
			diag_pos[i] = 0;
			//diag_pos[i + cb->size] = 0;
			conflict_pos[i] = 0;
			//conflict_pos[i + cb->size] = 0;
			diag_neg[i] = 0;
			//diag_neg[i + cb->size] = 0;
			conflict_neg[i] = 0;
			//conflict_neg[i + cb->size] = 0;

	}

	init_diagonal(cb, diag_pos, diag_neg, conflict_pos, conflict_neg);
	size_t c2 = nb_conflict(size*2 -1 ,conflict_neg,conflict_pos);

	unsigned long int swap =0;
	while (c2 > 0) {
		size_t dame_actuel = /*dame_max_conflit(cb->size,tablist)*/rand()%(cb->size);
		size_t qi = cb->queens[dame_actuel];
		if (queen_under_atak(size,dame_actuel,qi,diag_pos,diag_neg )) {
		//int cf_da = list_length(tablist[dame_actuel]);
		//if (cf_da > 0) {
			size_t r1 = rand()%(cb->size);
			size_t qj = cb->queens[r1];
			if (r1 != dame_actuel ) {

				swap++;
				swap_d(size,dame_actuel,qi,r1,qj,diag_pos,diag_neg,conflict_pos,conflict_neg);
				cb_swap(cb,dame_actuel,r1);
				//init_diagonal(cb, diag_pos, diag_neg, conflict_pos, conflict_neg);
				int ctmp =  nb_conflict(size*2 -1,conflict_neg,conflict_pos);
				if (ctmp > c2) {
				//if (nb_conflict(size*2,conflict_neg,conflict_pos) > c2) {
					swap_d(size,r1,qi,dame_actuel,qj,diag_pos,diag_neg,conflict_pos,conflict_neg);
					cb_swap(cb,r1,dame_actuel);
					//init_diagonal(cb, diag_pos, diag_neg, conflict_pos, conflict_neg);

				}
				else {
					c2 = ctmp;
					//printf("%d\n",c2);


				}
					//c2 = nb_conflict(size*2,conflict_neg,conflict_pos);
				//int cconflit = cb_conflicts_point_list(cb,tablist,dame_actuel,r1);
				//if (list_length(tablist[dame_actuel]) < cf_da)
					//dame_actuel++;
				//else
					//cb_swap(cb,r1,dame_actuel);
			}

		}
	}
	//affiche_diag(size*2-1,diag_pos);
	//affiche_diag(size*2-1,diag_neg);
	printf("il y a eu %lu swap mon captain \n",swap);

	//int c1 = cb_conflicts(cb,buf);



	free(diag_pos);
	free(diag_neg);
	free(conflict_pos);
	free(conflict_neg);

	return 0;



}

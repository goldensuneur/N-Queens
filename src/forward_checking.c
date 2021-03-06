#include "forward_checking.h"

extern int fw_rec(cb_t* cb, bf_t* domains, size_t col, size_t * calls) {
        ++*calls;
        //weare not at the end, now we have to select where to place the colth queens
        if (col < cb->size) {
                size_t  queen_place = -1;
                size_t  min_dom = -1;
                size_t  min_dom_val = -1;
                size_t  tmp_count;
                size_t  delta_size = 0;
                u8      delta[MAX_QUEENS];
                //ROW_MASK, DIAG1_MASK and DIAG2_MASK, save  if we need to revert the bit or not

                //just to be safe, but this should not happen
                if (cb->queens[col] != -1)
                        return 1;

                //now we get our first available queen in the remaining domain
                while ((queen_place = bf_get_next_setted(&domains[col], queen_place)) < cb->size) {
                        //we place the queen in the first available row
                        cb->queens[col] = queen_place;

                        //we update the domains of unplaced queens
                        for (delta_size = 0; delta_size < cb->size; ++delta_size) {

                                delta[delta_size] = 0;

                                if (cb->queens[delta_size] != -1)
                                        continue;

                                if (bf_get(domains[delta_size].field, queen_place)) {
                                        delta[delta_size] |= ROW_MASK;
                                        bf_unset(domains[delta_size].field, queen_place);
                                }

                                if (queen_place + (col - delta_size) < cb->size &&
                                    bf_get(domains[delta_size].field, queen_place + (col - delta_size))
                                    ) {
                                        delta[delta_size] |= DIAG1_MASK;
                                        bf_unset(domains[delta_size].field, queen_place + (col - delta_size));
                                }

                                if (queen_place - (col - delta_size) < cb->size &&
                                    bf_get(domains[delta_size].field, queen_place - (col - delta_size))
                                    ) {
                                        delta[delta_size] |= DIAG2_MASK;
                                        bf_unset(domains[delta_size].field, queen_place - (col - delta_size));
                                }

                                tmp_count = bf_count(&domains[delta_size]);

                                //the domain is empty, no need to go further, clean and test the next place
                                if (tmp_count == 0)
                                        goto restore;

                                //if the domain is smaller we save it for the next step
                                if (min_dom == -1 || tmp_count < min_dom_val) {
                                        min_dom = delta_size;
                                        min_dom_val = tmp_count;
                                }
                        }


                        //we check the smallest domain first, cause it's better !
                        if (!fw_rec(cb, domains, min_dom, calls))
                                return 0;


restore:
                        //else we have to undo what we have done and try the next place
                        cb->queens[col] = -1;
                        min_dom = -1;


                        for (size_t i = 0; i < cb->size && i <= delta_size; ++i) {
                                if (delta[i] & ROW_MASK)
                                        bf_set(domains[i].field, queen_place);

                                //no need to check if value is in bound, it has already been done when set
                                if (delta[i] & DIAG1_MASK)
                                        bf_set(domains[i].field, queen_place + (col - i));

                                //idem
                                if (delta[i] & DIAG2_MASK)
                                        bf_set(domains[i].field, queen_place - (col - i));
                        }

                }
                return 1;

        }
        //we are at the bottom, only one choice, evaluate the leaf
        else
                return cb_validates_full(cb);
}


int forward(cb_t* cb) {
        bf_t    domains[MAX_QUEENS];
        size_t  calls = 0;

        if (cb->size > MAX_QUEENS) {
                log_err("This method can't be used with size > %d.", MAX_QUEENS);
                return 1;
        }

        //we set the cb->size first bits to 1
        for (size_t i = 0; i < cb->size; ++i) {
                bf_init_from(&domains[i], cb->size);
                bf_not(&domains[i]);
        }

        int res = fw_rec(cb, domains, 0, &calls);

        printf("Returns in %zu calls\n", calls);

        return res;
}

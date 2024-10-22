#ifndef GOFISH_H
#define GOFISH_H
#include "deck.h"

/*
   Define any prototype functions
   for gofish.h here.
*/

// Used to print the hand of the target
void print_hand(struct player *target);

// print the book
void print_book(struct player *target, const char *label);

// add directly to the book
void add_book_directly(struct player *target, const char *rank);

// add specific card to hand
void add_specific_card(struct player *target, char suit, const char *rank);

// get the cards for printout
void get_matching_cards(struct player *target, const char *rank, char *matching_cards);

// count the amount in each book
int count_books(struct player *target);

// check if deck is empty
int empty_deck();

#endif

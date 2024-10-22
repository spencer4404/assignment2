#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h> // for the randomness

#include "card.h"
#include "deck.h"

/*
 * Variable: deck_instance
 * -----------------------
 *
 * Go Fish uses a single deck
 */
struct deck deck_instance;

// Implement shuffle function
int shuffle()
{
    char suits[] = {'C', 'D', 'H', 'S'};                                                // fill array of suits
    char *ranks[] = {"2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K", "A"}; // fill rank array

    int i = 0; // initialize index of the array

    // loop through the suits and ranks and create the 52 cards
    for (int s = 0; s < 4; s++)
    {
        for (int r = 0; r < 13; r++)
        {
            deck_instance.list[i].suit = suits[s];        // assign the suit at s to the deck at i
            strcpy(deck_instance.list[i].rank, ranks[r]); // string copy the rank to attach it to the suit
            i++;
        }
    }

    // use current time for the random number
    srand(time(NULL));
    deck_instance.top_card = 0; // initialize top card

    // Fisher-Yates shuffle
    for (int i = 51; i > 0; i--) // start at card 52 and work backwards
    {
        int j = rand() % (i + 1); // generate random index

        // swap the current card with random generated card (before or at current card)
        struct card temp = deck_instance.list[i];
        deck_instance.list[i] = deck_instance.list[j];
        deck_instance.list[j] = temp;
    }
    // for (int i = 0; i < 16; i++)
    // {
    //     printf("Card %d: Suit = %c, Rank = %s\n", i + 1, deck_instance.list[i].suit, deck_instance.list[i].rank);
    // }
    return 0;
}

// Dealing Cards
int deal_player_cards(struct player *target)
{
    int dealt; // initialize dealt cards

    // begin dealing
    for (dealt = 0; dealt < 7; dealt++)
    {
        // deal to the target
        add_card(target, next_card());
    }
    return 0;
}

// Get a pointer to the next card, removing the current card from the deck
struct card *next_card()
{

    // Check if there are cards left in the deck
    if (deck_size() == 0)
    {
        return NULL;
    }
    // Get a pointer to the top card
    struct card *card_to_return = &deck_instance.list[deck_instance.top_card];

    deck_instance.top_card++;

    return card_to_return;
}

// Return remaining cards
size_t deck_size()
{
    return 52 - deck_instance.top_card;
}
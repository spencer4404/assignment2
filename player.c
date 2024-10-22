#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include "player.h"
#include "deck.h"

/*
 * Instance Variables: user, computer
 * ----------------------------------
 *
 *  We only support 2 users: a human and a computer
 */
struct player user;
struct player computer;

// In player.c
void init_player(struct player *p)
{
    p->card_list = NULL;
    p->hand_size = 0;
    for (int i = 0; i < 7; i++)
    {
        p->book[i][0] = '\0'; // Initialize each book slot to an empty string
    }
}

// drawing cards
int add_card(struct player *target, struct card *new_card)
{
    // allocate memory for new hand
    struct hand *new_node = (struct hand *)malloc(sizeof(struct hand));

    // set new card
    new_node->top = *new_card;
    new_node->next = NULL;

    // If hand is empty, add as first card
    if (target->card_list == NULL)
    {
        target->card_list = new_node;
    }
    else
    {
        // Else, add it as the last item in the linked list
        struct hand *current = target->card_list;
        // get to the end of the list
        while (current->next != NULL)
        {
            current = current->next;
        }
        current->next = new_node;
    }
    // increment hand size
    target->hand_size++;

    return 0;
}

// removing cards
int remove_card(struct player *target, struct card *old_card)
{
    // Check if the hand is empty
    if (target->card_list == NULL)
    {
        return -1; // Return non-zero for error
    }

    struct hand *current = target->card_list;
    struct hand *previous = NULL;

    while (current != NULL)
    {
        // Check if the current card matches old_card
        if (current->top.suit == old_card->suit && strcmp(current->top.rank, old_card->rank) == 0)
        {
            // Card found; remove it from the list
            if (previous == NULL)
            {
                // Removing the first card in the list
                target->card_list = current->next;
            }
            else
            {
                // Removing a card in the middle or end of the list
                previous->next = current->next;
            }

            // Free the memory of the removed card
            free(current);
            target->hand_size--;

            return 0; // Return 0 to indicate successful removal
        }

        // Move to the next card
        previous = current;
        current = current->next;
    }

    return -1; // Return non-zero if the card was not found
}

// check hand and see if theres matches
const char *check_add_book(struct player *target)
{
    const char *ranks[] = {"2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K", "A"};
    int rank_count[13] = {0}; // array to count rank occurences

    // Traverse target hand to count occurrences of ranks
    struct hand *current = target->card_list;
    while (current != NULL)
    {
        // Find index of current card's rank
        for (int i = 0; i < 13; i++)
        {
            if (strcmp(current->top.rank, ranks[i]) == 0)
            {
                rank_count[i]++;
                break; // break once a match is found
            }
        }
        current = current->next;
    }

    // check if theres all 4 matching cards
    for (int i = 0; i < 13; i++)
    {
        if (rank_count[i] == 4)
        {
            for (int j = 0; j < 7; j++)
            {
                if (target->book[j][0] == '\0') // find empty spot in the book
                {
                    strcpy(target->book[j], ranks[i]);
                    break;
                }
            }

            // Remove cards of this rank from the players hand
            current = target->card_list;
            while (current != NULL)
            {
                struct hand *next = current->next;
                if (strcmp(current->top.rank, ranks[i]) == 0)
                {
                    remove_card(target, &current->top);
                }
                current = next;
            }
            return ranks[i]; // return the character of the rank that was removed
        }
    }
    return NULL; // return null if no card was added
}

// search the target hand for the given suit
int search(struct player *target, const char *rank)
{
    struct hand *current = target->card_list; // Start at first card in the hand

    // Traverse the hand
    while (current != NULL)
    {
        // Compare the ranks
        // printf("Checking card: %s of %c\n", current->top.rank, current->top.suit);

        if (strcmp(current->top.rank, rank) == 0) // compare each rank
        {
            // printf("Rank %s found in hand\n", rank);
            return 1; // rank found in the hand
        }
        current = current->next; // move to next card
    }
    // printf("rank %s not found in hand\n", rank);
    return 0; // rank not found
}

// Transfer wanted cards
int transfer_cards(struct player *src, struct player *dest, const char *rank, char *transferred_cards)
{
    struct hand *current = src->card_list; // look at source card list
    struct hand *next_node;
    int amount = 0; // track amount of cards transferred

    // initialize the transferred cards
    transferred_cards[0] = '\0';

    // traverse source's hand
    while (current != NULL)
    {
        next_node = current->next; // keep track of the next card

        // check if the current card matched the wanted rank
        if (strcmp(current->top.rank, rank) == 0)
        {
            char card_str[10]; // hold rank and suit
            sprintf(card_str, "%s%c ", current->top.rank, current->top.suit);
            strcat(transferred_cards, card_str);

            // Add matching card to destination player
            add_card(dest, &current->top);

            // remove card from source player's hand
            remove_card(src, &current->top);
            src->hand_size--;

            amount++; // increment amount of cards
        }
        current = next_node; // next card please!
    }
    if (amount == 0)
    {
        // printf("0\n");
        return 0; // no cards found
    }
    // printf("%d\n", amount);
    return amount; // return amount of transferred cards
}

// Check if a book has 7 cards in it
int game_over(struct player *target)
{
    int amount = 0;

    // Traverse target book array
    for (int i = 0; i < 7; i++)
    {
        if (target->book[i][0] != '\0') // Check book slot
        {
            amount++;
        }

        if (amount == 7)
        {
            // printf("1\n");
            return 1; // Game over
        }
    }
    // printf("0\n");
    return 0; // Game not over
}

// Function to clear the whole hand
int reset_player(struct player *target)
{
    while (target->card_list != NULL)
    {
        // Use remove_card to remove the top card until the list is empty
        remove_card(target, &target->card_list->top);
    }
    return 0;
}

// Function to reset the books for a player
void reset_books(struct player *target)
{
    // Iterate through the book array and reset each book entry
    for (int i = 0; i < 7; i++)
    {
        target->book[i][0] = '\0'; // Clear each book slot
    }
}

// Computers turn
char *computer_play(struct player *target)
{
    struct hand *current = target->card_list;
    char available_ranks[13][3]; // store up to 13 ranks of length 3
    int count = 0;

    // Traverse through the hand
    while (current != NULL)
    {
        // Avoid duplicates by checking if the rank is already collected
        int already_in_list = 0;
        for (int i = 0; i < count; i++)
        {
            if (strcmp(available_ranks[i], current->top.rank) == 0)
            {
                already_in_list = 1;
                break;
            }
        }
        // If the rank is not already in the list, add it
        if (!already_in_list)
        {
            strcpy(available_ranks[count], current->top.rank);
            count++;
        }

        current = current->next;
    }
    // Make sure there are cards to choose from (error case)
    if (count == 0)
    {
        return NULL;
    }
    // randomly select rank
    srand(time(NULL));
    int random_index = rand() % count;

    // return the rank
    return strdup(available_ranks[random_index]);
}

// User's turn
char *user_play(struct player *target)
{
    struct hand *current = target->card_list;
    char available_ranks[13][3]; // store up to 13 ranks of length 3
    int count = 0;
    char rank[3];

    // Traverse through the hand
    while (current != NULL)
    {
        // Avoid duplicates by checking if the rank is already collected
        int already_in_list = 0;
        for (int i = 0; i < count; i++)
        {
            if (strcmp(available_ranks[i], current->top.rank) == 0)
            {
                already_in_list = 1;
                break;
            }
        }
        // If the rank is not already in the list, add it
        if (!already_in_list)
        {
            strcpy(available_ranks[count], current->top.rank);
            count++;
        }

        current = current->next;
    }
    // Make sure there are cards to choose from (error case)
    if (count == 0)
    {
        printf("No cards in player's hand");
        return NULL;
    }
    // Step 3: Ask the user for input and validate it
    while (1)
    {
        printf("Player 1's turn, enter a Rank: ");
        scanf("%2s", rank); // Read a rank from the user (up to 2 characters)

        // Check if the entered rank is in the user's hand (available_ranks array)
        int found = 0;
        for (int i = 0; i < count; i++)
        {
            if (strcmp(available_ranks[i], rank) == 0)
            {
                found = 1;
            }
        }
        if (found == 1)
        {
            return strdup(rank);
        }
        else
        {
            printf("Invalid rank! Please enter a rank from your hand.\n");
        }
    }
}

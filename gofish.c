#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "gofish.h"

// Function to add a specific card to the player's hand
void add_specific_card(struct player *target, char suit, const char *rank)
{
  struct card new_card;
  new_card.suit = suit;
  strcpy(new_card.rank, rank);

  add_card(target, &new_card);
}

// function to add a card to the book directly
void add_book_directly(struct player *target, const char *rank)
{
  for (int i = 0; i < 7; i++)
  {
    if (target->book[i][0] == '\0') // Find the first empty book slot
    {
      strcpy(target->book[i], rank); // Copy the rank into the book slot
      break;
    }
  }
}

// print the current hand -- only needed for user player as you dont look at other persons hand
void print_hand(struct player *target) // take a target just to check the other hand
{
  struct hand *current = target->card_list;
  char hand[256] = "";

  // check if hand is empty
  if (current == NULL)
  {
    printf("Player 1's Hand - \n");
    return;
  }

  // traverse linked list, add the cards to the hand output
  while (current != NULL)
  {
    // Temporary buffer to hold the current card's string representation
    char card[32]; // Adjust size if needed, enough to hold rank, suit, and possible space

    // Format the card string
    sprintf(card, "%s%c ", current->top.rank, current->top.suit);

    // Concatenate the card to the hand string
    strcat(hand, card);
    current = current->next; // move to next card
  }
  // Print the hand
  printf("Player 1's Hand - %s\n", hand);
}

// print the books
void print_book(struct player *target, const char *label)
{
  bool book_empty = true;
  printf("%s's Book - ", label);

  for (int i = 0; i < 7; i++)
  {
    if (target->book[i] != NULL)
    {
      printf("%s ", target->book[i]);
      book_empty = false;
    }
  }
  if (book_empty)
  {
    printf("%s's Book - ", label);
  }
  printf("\n");
}

// for printing what each player has
void get_matching_cards(struct player *target, const char *rank, char *matching_cards)
{
  struct hand *current = target->card_list;
  matching_cards[0] = '\0'; // Initialize as empty string

  while (current != NULL)
  {
    if (strcmp(current->top.rank, rank) == 0)
    {
      char card_str[10];
      sprintf(card_str, "%s%c ", current->top.rank, current->top.suit);
      strcat(matching_cards, card_str);
    }
    current = current->next;
  }
}

// for printing score
int count_books(struct player *target)
{
  int count = 0;

  // Traverse the book array to count filled books
  for (int i = 0; i < 7; i++)
  {
    if (target->book[i][0] != '\0') // If a book exists in the slot
    {
      count++;
    }
  }

  return count; // Return the number of books
}

// check if deck is empty
int empty_deck()
{
  if (deck_size() <= 0)
  {
    return 1; // Deck is empty
  }
  else
  {
    return 0; // Deck is not empty
  }
}

// MAIN FUNCTION //
int main(int args, char *argv[])
{
  char *player_1_select = NULL;
  char *computer_select = NULL;
  char again;
  char transferred_cards[256];
  int search_user;
  int search_computer;
  char player_cards[256];
  bool winner = false;
  bool game = true;

  while (game)
  {
    reset_player(&user);
    reset_player(&computer);
    reset_books(&user);
    reset_books(&computer);

    // Step 1: Shuffle the deck and deal the cards
    printf("Shuffling deck...\n\n");
    shuffle();
    deal_player_cards(&user);
    deal_player_cards(&computer);

    // Print the hands and books
    print_hand(&user);
    print_hand(&computer);
    print_book(&user, "Player 1");
    print_book(&computer, "Player 2");

    while (!winner)
    {
      // === user's Turn ===
      bool player_1_turn = true;
      while (player_1_turn)
      {
        // user selects a rank
        player_1_select = user_play(&user);

        // Draw a card if hand is empty
        if (player_1_select == NULL)
        {
          printf("Hand is empty, must draw a card\n");
          // user draws a card
          struct card *new_card = next_card();
          if (new_card != NULL)
          {
            add_card(&computer, new_card);
            printf("    - Player 1 drew a %s of %c\n", new_card->rank, new_card->suit);
            // no longer their turn
            break;
          }
        }

        // Ensure Player 1 has at least one card of the selected rank
        if (search(&user, player_1_select) == 0)
        {
          printf("Error - must have at least one card from rank to play\n");
          print_hand(&user);
          print_hand(&computer);
          print_book(&user, "Player 1");
          print_book(&computer, "Player 2");
          continue; // Ask for another input
        }

        // Search in the computer's hand
        search_computer = search(&computer, player_1_select);

        if (search_computer)
        {
          // If a match is found, transfer cards
          get_matching_cards(&user, player_1_select, player_cards);
          transfer_cards(&computer, &user, player_1_select, transferred_cards);
          printf("    - Player 2 has: %s\n", transferred_cards);
          printf("    - Player 1 has: %s\n", player_cards);
        }
        else
        {
          // No match, go fish
          printf("    - Player 2 has no %s's\n", player_1_select);
          printf("    - Go Fish, Player 1 draws a card\n");

          // Player draws a card as long as deck isnt empty

          struct card *new_card = next_card();
          if (new_card != NULL)
          {
            add_card(&user, new_card);
            printf("    - Player 1 drew a %s of %c\n", new_card->rank, new_card->suit);

            // If the drawn card matches the requested rank, Player 1 gets another turn
            if (strcmp(new_card->rank, player_1_select) == 0)
            {
              printf("    - Player 1 drew the card they requested! Another turn.\n");
              print_hand(&user);
              print_hand(&computer);
              print_book(&user, "Player 1");
              print_book(&computer, "Player 2");

              // check if this causes game winner
              if (game_over(&user))
              {
                // Count the number of books for both players
                int user_books = count_books(&user);
                int computer_books = count_books(&computer);
                printf("Player 1 Wins! %d-%d\n", user_books, computer_books);
                winner = true;
                // end the game
                break;
              }

              continue; // Go again
            }
          }
          else
          {
            // end turn if the deck is empty
            printf("Empty deck!\n");
            break;
          }

          player_1_turn = false; // End Player 1's turn
        }

        // Check if Player 1 completes a book
        const char *new_book = check_add_book(&user);
        if (new_book)
        {
          printf("    - Player 1 books %s\n", new_book);
        }

        // Check if Player 1 has won after their turn
        if (game_over(&user))
        {
          // Count the number of books for both players
          int user_books = count_books(&user);
          int computer_books = count_books(&computer);
          printf("Player 1 Wins! %d-%d\n", user_books, computer_books);
          winner = true;
        }

        if (winner)
        {
          break;
        }

        // Print the hand and books
        print_hand(&user);
        print_hand(&computer);
        print_book(&user, "Player 1");
        print_book(&computer, "Player 2");
      }

      // === Computer's Turn ===
      bool computer_turn = true;
      // additional condition in case player 1 already won
      while (computer_turn && !winner)
      {
        // Computer selects a random rank
        computer_select = computer_play(&computer);
        // check if hand is empty
        if (computer_select == NULL)
        {
          printf("Hand is empty, must draw a card\n");
          // Computer draws a card
          struct card *new_card = next_card();
          if (new_card != NULL)
          {
            add_card(&computer, new_card);
            printf("    - Player 2 drew a %s of %c\n", new_card->rank, new_card->suit);
            // no longer their turn
            break;
          }
        }
        printf("Player 2's turn, enter a Rank: %s\n", computer_select);

        // Computer searches Player 1's hand
        search_user = search(&user, computer_select);

        if (search_user)
        {
          // If a match is found, transfer cards
          get_matching_cards(&computer, computer_select, player_cards);
          transfer_cards(&user, &computer, computer_select, transferred_cards);
          printf("    - Player 2 has %s\n", player_cards);
          printf("    - Player 1 has %s\n", transferred_cards);
        }
        else
        {
          // No match, go fish
          printf("    - Player 1 has no %s's\n", computer_select);
          printf("    - Go Fish, Player 2 draws a card\n");

          // Computer draws a card
          struct card *new_card = next_card();
          if (new_card != NULL)
          {
            add_card(&computer, new_card);
            // printf("    - Player 2 drew a %s of %c\n", new_card->rank, new_card->suit);
            // If the drawn card matches the requested rank, Computer gets another turn
            if (strcmp(new_card->rank, computer_select) == 0)
            {
              printf("    - Player 2 drew the card they requested! Another turn.\n");
              continue; // Go again
            }
          }
          else
          {
            // case for empty deck, end turn
            printf("Empty deck!\n");
            break;
          }

          computer_turn = false; // End computer's turn
        }

        // Check if the computer completes a book
        const char *new_book = check_add_book(&computer);
        if (new_book)
        {
          printf("    - Player 2 books %s\n", new_book);
        }

        // Check if the computer has won
        if (game_over(&computer))
        {
          // Count the number of books for both players
          int user_books = count_books(&user);
          int computer_books = count_books(&computer);
          printf("Player 2 Wins! %d-%d\n", user_books, computer_books);
          winner = true;
        }

        if (winner)
        {
          break;
        }

        // Print the hand and books
        print_hand(&user);
        print_hand(&computer);
        print_book(&user, "Player 1");
        print_book(&computer, "Player 2");
      }
    }

    // ask to play again
    printf("Do you want to play again [Y/N]: ");
    scanf(" %c", &again);
    if (again == 'N')
    {
      game = false;
    }
    // reset winner
    winner = false;
  }

  return 0;
}

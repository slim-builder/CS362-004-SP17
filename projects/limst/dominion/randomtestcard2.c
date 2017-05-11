#include "dominion.h"
#include "dominion_helpers.h"
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "rngs.h"
#include <math.h>
#include <stdlib.h>

#define DEBUG 0
#define NOISY_TEST 1
#define ITERATIONS 10000

void asserttrue(int result, int *successes, int *failures) {
  if (result == 0)
    (*failures)++;
  else
    (*successes)++;
}

void checkSmithyCard(int hp, int p, struct gameState *post, int *successes, int *failures) {
  struct gameState pre;
  memcpy (&pre, post, sizeof(struct gameState));

  int r, i;
    
  r = playSmithy (hp, p, post);

  // Test whether or not player's hand, deck, and discard pile are correct
  // If player's deck has three or more cards
  if (pre.deckCount[p] >= 3) {
    pre.hand[p][hp] = pre.deck[p][pre.deckCount[p]-3];
    pre.hand[p][pre.handCount[p]] = pre.deck[p][pre.deckCount[p]-1];
    pre.hand[p][pre.handCount[p]+1] = pre.deck[p][pre.deckCount[p]-2];
    pre.hand[p][pre.handCount[p]+2] = -1;
    pre.handCount[p] += 2;
    pre.deckCount[p] -= 3;
  }
  // If combination of player's deck and discard pile has three or more cards
  else if (pre.deckCount[p] + pre.discardCount[p] >= 3) {
    pre.hand[p][hp] = post->hand[p][hp];
    for (i = 0; i < pre.deckCount[p]; i++)
      pre.hand[p][pre.handCount[p]+i] = pre.deck[p][pre.deckCount[p]-1-i];
    for (i = pre.deckCount[p]; i < 2; i++)
    pre.hand[p][pre.handCount[p]+i] = post->hand[p][pre.handCount[p]+i];
    pre.hand[p][pre.handCount[p]+2] = -1;
    pre.handCount[p] += 2;
    pre.deckCount[p] = pre.discardCount[p] - (3 - pre.deckCount[p]);
    // A new deck is formed using a shuffled discard pile 
    memcpy(pre.deck[p], post->deck[p], sizeof(int) * pre.discardCount[p]);
    for (i = 0; i < pre.discardCount[p]; i++)
      pre.discard[p][i] = -1;
    pre.discardCount[p] = 0;
  }
  // If combination of player's deck and discard pile has two cards
  // and both deck and discard pile are non-empty.
  else if (pre.discardCount[p] > 0 && pre.deckCount[p] > 0) {
    pre.hand[p][hp] = post->hand[p][hp];
    for (i = 0; i < pre.deckCount[p]; i++)
      pre.hand[p][pre.handCount[p]+i] = pre.deck[p][pre.deckCount[p]-1-i];
    for (i = 0; i < pre.discardCount[p]-1; i++)
      pre.hand[p][pre.handCount[p]+pre.deckCount[p]+i] = post->hand[p][pre.handCount[p]+pre.deckCount[p]+i];
    pre.hand[p][pre.handCount[p]+pre.deckCount[p]+pre.discardCount[p]-1] = -1;
    pre.handCount[p] += pre.deckCount[p]+pre.discardCount[p]-1;
    // A new deck is formed using a shuffled discard pile 
    memcpy(pre.deck[p], post->deck[p], sizeof(int) * pre.discardCount[p]);
    for (i = 0; i < pre.discardCount[p]; i++)
      pre.discard[p][i] = -1;
    pre.discardCount[p] = 0;
    pre.deckCount[p] = 0;
  }
  // If player's deck has one or two cards and the discard pile is empty
  else if (pre.discardCount[p] == 0 && pre.deckCount[p] > 0) {
    pre.hand[p][hp] = pre.deck[p][0];
    for (i = 0; i < pre.deckCount[p]-1; i++)
      pre.hand[p][pre.handCount[p]+i] = pre.deck[p][pre.deckCount[p]-1-i];
    pre.hand[p][pre.handCount[p]+pre.deckCount[p]-1] = -1;
    pre.handCount[p] += pre.deckCount[p]-1;
    pre.deckCount[p] = 0;
  }
  // If player's discard pile has one or two cards and the deck is empty
  else if (pre.discardCount[p] > 0 && pre.deckCount[p] == 0) {
    pre.hand[p][hp] = post->hand[p][hp];
    for (i = 0; i < pre.discardCount[p]-1; i++)
      pre.hand[p][pre.handCount[p]+i] = post->hand[p][pre.handCount[p]+i];
    pre.hand[p][pre.handCount[p]+pre.discardCount[p]-1] = -1;
    pre.handCount[p] += pre.discardCount[p]-1;
    // A new deck is formed using a shuffled discard pile 
    memcpy(pre.deck[p], post->deck[p], sizeof(int) * pre.discardCount[p]);
    for (i = 0; i < pre.discardCount[p]; i++)
      pre.discard[p][i] = -1;
    pre.discardCount[p] = 0;
  }
  // If player's deck and discard pile are both empty
  else {
    pre.hand[p][hp] = pre.hand[p][pre.handCount[p]-1];
    pre.hand[p][pre.handCount[p]-1] = -1;
    pre.handCount[p]--;
  }

  // Test effect on playedCards array
  pre.playedCardCount++;
  pre.playedCards[pre.playedCardCount-1] = smithy;

  // Test card effect function return value
  asserttrue(r == 0, successes, failures);

  // Test whether or not the card effect function makes the correct changes
  // to the game state
  asserttrue(memcmp(&pre, post, sizeof(struct gameState)) == 0, successes, failures);
}

int main () {

  int i, n, p, hp;
  int successes = 0, failures = 0;

  struct gameState G;

  printf ("Testing Smithy Card.\n");

  printf ("RANDOM TESTS.\n");

  SelectStream(2);
  PutSeed(3);

  for (n = 0; n < ITERATIONS; n++) {
    // Randomly assign the values of game state data structure
    for (i = 0; i < sizeof(struct gameState); i++) {
      ((char*)&G)[i] = floor(Random() * 256);
    }
    // Randomly pick the number of players from two to MAX_PLAYERS
    G.numPlayers = floor(Random() * (MAX_PLAYERS-1)) + 2;
    // Randomly pick the player who plays the card under test
    p = floor(Random() * G.numPlayers);
    // Randomly pick the hand count.
    // Since the Smithy card results in the player drawing three
    // cards into their hand, the maximum hand count must be
    // MAX_HAND-3.  The minimum hand count must be one since the player
    // must have the card under test in their hand.
    G.handCount[p] = floor(Random() * (MAX_HAND-3)) + 1;
    // Randomly pick the deck and discard counts.
    // The sum of the hand, deck, and discard count should be less than
    // MAX_DECK.
    G.deckCount[p] = floor(Random() * (MAX_DECK+1-G.handCount[p]));
    G.discardCount[p] = floor(Random() * (MAX_DECK+1-G.handCount[p]-G.deckCount[p]));
    // Randomly pick the played card count whose initial maximum should be
    // MAX_DECK-1 since the played card count will be incremented by one
    // due to playing the card under test.
    G.playedCardCount = floor(Random() * MAX_DECK);
    // Randomly pick among valid card values for the hand, deck, and
    // discard piles
    for (i = 0; i < G.handCount[p]; i++)
      G.hand[p][i] = floor(Random() * (treasure_map + 1));
    for (i = 0; i < G.deckCount[p]; i++)
      G.deck[p][i] = floor(Random() * (treasure_map + 1));
    for (i = 0; i < G.discardCount[p]; i++)
      G.discard[p][i] = floor(Random() * (treasure_map + 1));
    // Randomly pick the hand position of the card under test and
    // place the card under test in the player's hand at the assigned
    // hand position.
    hp = floor(Random() * G.handCount[p]);
    G.hand[p][hp] = smithy;
    // Verify that the Smithy card effect function is correct
    checkSmithyCard(hp, p, &G, &successes, &failures);
  }

  printf ("%d TESTS SUCCESSFUL\n", successes);
  printf ("%d TESTS FAILED\n", failures);

  return 0;
}

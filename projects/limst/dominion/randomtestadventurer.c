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

void checkAdventurerCard(int hp, int p, struct gameState *post, int *successes, int *failures) {
  struct gameState pre;
  memcpy (&pre, post, sizeof(struct gameState));

  int r;
  int dummy = gold;
  int temphand[MAX_DECK];
    
  r = playAdventurer (hp, p, post, 0, temphand, 0, 0);

  int i;
  int deckTreasCount = 0;
  int discardTreasCount = 0;
  int firstTreasDeckPos = -1, secondTreasDeckPos = -1;
  // Count the number of treasures in the deck.
  // Also, find the memory location of the first two treasures
  // in the deck.
  for (i = pre.deckCount[p]-1; i >= 0; i--) {
    if (pre.deck[p][i] >= copper && pre.deck[p][i] <= gold) {
      if (deckTreasCount == 0)
        firstTreasDeckPos = i;
      else if (deckTreasCount == 1)
        secondTreasDeckPos = i;
      deckTreasCount++;
    }
  }
  // Count the number of treasures in the discard
  for (i = 0; i < pre.discardCount[p]; i++) {
    if (pre.discard[p][i] >= copper && pre.discard[p][i] <= gold) {
      discardTreasCount++;
    }
  }

  // Test whether or not the player's hand, deck, and discard pile are correct.
  // If there at least two treasures in the deck
  if (deckTreasCount >= 2) {
    pre.hand[p][hp] = pre.deck[p][secondTreasDeckPos];
    pre.hand[p][pre.handCount[p]] = pre.deck[p][firstTreasDeckPos];
    pre.hand[p][pre.handCount[p]+1] = -1;
    pre.handCount[p]++;

    memcpy(pre.discard[p] + pre.discardCount[p], post->discard[p] + pre.discardCount[p], sizeof(int)*(pre.deckCount[p]-secondTreasDeckPos-2));
    pre.discardCount[p] += pre.deckCount[p]-secondTreasDeckPos-2;

    pre.deckCount[p] = secondTreasDeckPos;
  }
  // If there less than two treasures in the deck, but greater
  // than or equal to two treasures in the combination of the deck
  // and the discard pile
  else if (deckTreasCount + discardTreasCount >= 2) {
    if (deckTreasCount == 1) {
      pre.hand[p][pre.handCount[p]] = pre.deck[p][firstTreasDeckPos];
      pre.hand[p][hp] = post->hand[p][hp];
    }
    else { // no treasure in deck, treasures are only in discard
      pre.hand[p][pre.handCount[p]] = post->hand[p][pre.handCount[p]];
      pre.hand[p][hp] = post->hand[p][hp];
    }
    pre.hand[p][pre.handCount[p]+1] = -1;
    pre.handCount[p]++;

    memcpy(pre.deck[p], post->deck[p], sizeof(int) * pre.discardCount[p]);
    pre.deckCount[p] = post->deckCount[p];

    memcpy(pre.discard[p], post->discard[p], sizeof(int) * MAX_DECK);
    pre.discardCount[p] = post->discardCount[p];
  }
  // If there is exactly one treasure in the combination of the deck
  // and the discard pile
  else if (deckTreasCount + discardTreasCount == 1) {
    pre.hand[p][pre.handCount[p]] = -1;
    pre.hand[p][pre.handCount[p]+1] = post->hand[p][pre.handCount[p]+1];
    if (deckTreasCount == 1) {
      pre.hand[p][hp] = pre.deck[p][firstTreasDeckPos];
    }
    else {
      pre.hand[p][hp] = post->hand[p][hp];
    }

    memcpy(pre.deck[p], post->deck[p], sizeof(int) * pre.discardCount[p]);
    pre.deckCount[p] = post->deckCount[p];

    memcpy(pre.discard[p], post->discard[p], sizeof(int) * MAX_DECK);
    pre.discardCount[p] = post->discardCount[p];
  }
  else { // no treasures in both the deck and discard pile
    pre.hand[p][hp] = pre.hand[p][pre.handCount[p]-1];
    pre.hand[p][pre.handCount[p]-1] = -1;
    pre.hand[p][pre.handCount[p]] = post->hand[p][pre.handCount[p]];
    pre.handCount[p]--;
    
    memcpy(pre.deck[p], post->deck[p], sizeof(int) * pre.discardCount[p]);
    pre.deckCount[p] = post->deckCount[p];

    memcpy(pre.discard[p], post->discard[p], sizeof(int) * MAX_DECK);
    pre.discardCount[p] = post->discardCount[p];
  }

  // Test effect on playedCards array
  pre.playedCardCount++;
  pre.playedCards[pre.playedCardCount-1] = adventurer;

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

  printf ("Testing Adventurer Card.\n");

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
    // Since the Adventurer card results in the player gaining two
    // treasures into their hand, the maximum hand count must be
    // MAX_HAND-2.  The minimum hand count must be one since the player
    // must have the card under test in their hand.
    G.handCount[p] = floor(Random() * (MAX_HAND-2)) + 1;
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
    G.hand[p][hp] = adventurer;
    // Verify that the Adventurer card effect function is correct
    checkAdventurerCard(hp, p, &G, &successes, &failures);
  }

  printf ("%d TESTS SUCCESSFUL\n", successes);
  printf ("%d TESTS FAILED\n", failures);

  return 0;
}

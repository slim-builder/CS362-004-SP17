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

void checkAdventurerCard(int hp, int p, struct gameState *post) {
  struct gameState pre;
  memcpy (&pre, post, sizeof(struct gameState));

  int r;
  int temphand[MAX_DECK];
    
  r = playAdventurer (hp, p, post, 0, temphand, 0, 0);

  int i;
  int deckTreasCount = 0;
  int discardTreasCount = 0;
  int firstTreasDeckPos = -1, secondTreasDeckPos = -1;
  for (i = pre.deckCount[p]-1; i >= 0; i--) {
    if (pre.deck[p][i] >= copper && pre.deck[p][i] <= gold) {
      if (deckTreasCount == 0)
        firstTreasDeckPos = i;
      else if (deckTreasCount == 1)
        secondTreasDeckPos = i;
      deckTreasCount++;
    }
  }
  for (i = 0; i < pre.discardCount[p]; i++) {
    if (pre.discard[p][i] >= copper && pre.discard[p][i] <= gold) {
      discardTreasCount++;
    }
  }

  if (deckTreasCount >= 2) {
    pre.hand[p][hp] = pre.deck[p][secondTreasDeckPos];
    pre.hand[p][pre.handCount[p]] = pre.deck[p][firstTreasDeckPos];
    pre.hand[p][pre.handCount[p]+1] = -1;
    pre.handCount[p]++;

    memcpy(pre.discard[p] + pre.discardCount[p], post->discard[p] + pre.discardCount[p], sizeof(int)*(pre.deckCount[p]-secondTreasDeckPos-2));
    pre.discardCount[p] += pre.deckCount[p]-secondTreasDeckPos-2;

    pre.deckCount[p] = secondTreasDeckPos;
  }
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
  else { // no treasures
    // Test discardCard function effects on player's hand
    pre.hand[p][hp] = pre.hand[p][pre.handCount[p]-1];
    pre.hand[p][pre.handCount[p]-1] = -1;
    pre.hand[p][pre.handCount[p]] = post->hand[p][pre.handCount[p]];
    pre.handCount[p]--;
    
    memcpy(pre.deck[p], post->deck[p], sizeof(int) * pre.discardCount[p]);
    pre.deckCount[p] = post->deckCount[p];

    memcpy(pre.discard[p], post->discard[p], sizeof(int) * MAX_DECK);
    pre.discardCount[p] = post->discardCount[p];
  }

  // Test discardCard function effects on playedCards array
  pre.playedCardCount++;
  pre.playedCards[pre.playedCardCount-1] = adventurer;

  assert (r == 0);

  assert(memcmp(&pre, post, sizeof(struct gameState)) == 0);
}

int main () {

  int i, n, p, hp;

  struct gameState G;

  printf ("Testing Adventurer Card.\n");

  printf ("RANDOM TESTS.\n");

  SelectStream(2);
  PutSeed(3);

  for (n = 0; n < 2000; n++) {
    for (i = 0; i < sizeof(struct gameState); i++) {
      ((char*)&G)[i] = floor(Random() * 256);
    }
    G.numPlayers = floor(Random() * (MAX_PLAYERS-1)) + 2;
    p = floor(Random() * G.numPlayers);
    G.handCount[p] = floor(Random() * (MAX_HAND-2)) + 1;
    G.deckCount[p] = floor(Random() * (MAX_DECK+1-G.handCount[p]));
    G.discardCount[p] = floor(Random() * (MAX_DECK+1-G.handCount[p]-G.deckCount[p]));
    G.playedCardCount = floor(Random() * MAX_DECK);
    hp = floor(Random() * G.handCount[p]);
    G.hand[p][hp] = adventurer;
    checkAdventurerCard(hp, p, &G);
  }

  printf ("ALL TESTS OK\n");

  return 0;
}

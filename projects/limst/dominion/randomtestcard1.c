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

void checkCouncilRoomCard(int hp, int p, struct gameState *post) {
  struct gameState pre;
  memcpy (&pre, post, sizeof(struct gameState));

  int r, i, j;
    
  r = playCouncilRoom (hp, p, post);

  if (pre.deckCount[p] >= 4) {
    pre.hand[p][hp] = pre.deck[p][pre.deckCount[p]-4];
    pre.hand[p][pre.handCount[p]] = pre.deck[p][pre.deckCount[p]-1];
    pre.hand[p][pre.handCount[p]+1] = pre.deck[p][pre.deckCount[p]-2];
    pre.hand[p][pre.handCount[p]+2] = pre.deck[p][pre.deckCount[p]-3];
    pre.hand[p][pre.handCount[p]+3] = -1;
    pre.handCount[p] += 3;
    pre.deckCount[p] -= 4;
  }
  else if (pre.deckCount[p] + pre.discardCount[p] >= 4) {
    pre.hand[p][hp] = post->hand[p][hp];
    for (i = 0; i < pre.deckCount[p]; i++)
      pre.hand[p][pre.handCount[p]+i] = pre.deck[p][pre.deckCount[p]-1-i];
    for (i = pre.deckCount[p]; i < 3; i++)
    pre.hand[p][pre.handCount[p]+i] = post->hand[p][pre.handCount[p]+i];
    pre.hand[p][pre.handCount[p]+3] = -1;
    pre.handCount[p] += 3;
    pre.deckCount[p] = pre.discardCount[p] - (4 - pre.deckCount[p]);
    memcpy(pre.deck[p], post->deck[p], sizeof(int) * pre.discardCount[p]);
    for (i = 0; i < pre.discardCount[p]; i++)
      pre.discard[p][i] = -1;
    pre.discardCount[p] = 0;
  }
  else if (pre.discardCount[p] > 0 && pre.deckCount[p] > 0) {
    pre.hand[p][hp] = post->hand[p][hp];
    for (i = 0; i < pre.deckCount[p]; i++)
      pre.hand[p][pre.handCount[p]+i] = pre.deck[p][pre.deckCount[p]-1-i];
    for (i = 0; i < pre.discardCount[p]-1; i++)
      pre.hand[p][pre.handCount[p]+pre.deckCount[p]+i] = post->hand[p][pre.handCount[p]+pre.deckCount[p]+i];
    pre.hand[p][pre.handCount[p]+pre.deckCount[p]+pre.discardCount[p]-1] = -1;
    pre.handCount[p] += pre.deckCount[p]+pre.discardCount[p]-1;
    memcpy(pre.deck[p], post->deck[p], sizeof(int) * pre.discardCount[p]);
    for (i = 0; i < pre.discardCount[p]; i++)
      pre.discard[p][i] = -1;
    pre.discardCount[p] = 0;
    pre.deckCount[p] = 0;
  }
  else if (pre.discardCount[p] == 0 && pre.deckCount[p] > 0) {
    pre.hand[p][hp] = pre.deck[p][0];
    for (i = 0; i < pre.deckCount[p]-1; i++)
      pre.hand[p][pre.handCount[p]+i] = pre.deck[p][pre.deckCount[p]-1-i];
    pre.hand[p][pre.handCount[p]+pre.deckCount[p]-1] = -1;
    pre.handCount[p] += pre.deckCount[p]-1;
    pre.deckCount[p] = 0;
  }
  else if (pre.discardCount[p] > 0 && pre.deckCount[p] == 0) {
    pre.hand[p][hp] = post->hand[p][hp];
    for (i = 0; i < pre.discardCount[p]-1; i++)
      pre.hand[p][pre.handCount[p]+i] = post->hand[p][pre.handCount[p]+i];
    pre.hand[p][pre.handCount[p]+pre.discardCount[p]-1] = -1;
    pre.handCount[p] += pre.discardCount[p]-1;
    memcpy(pre.deck[p], post->deck[p], sizeof(int) * pre.discardCount[p]);
    for (i = 0; i < pre.discardCount[p]; i++)
      pre.discard[p][i] = -1;
    pre.discardCount[p] = 0;
  }
  else {
    pre.hand[p][hp] = pre.hand[p][pre.handCount[p]-1];
    pre.hand[p][pre.handCount[p]-1] = -1;
    pre.handCount[p]--;
  }

  for (i = 0; i < pre.numPlayers; i++) {
    if (i != p) {
      if (pre.deckCount[i] > 0) {
        pre.hand[i][pre.handCount[i]] = pre.deck[i][pre.deckCount[i]-1];
        pre.deckCount[i]--;
        pre.handCount[i]++;
      }
      else if (pre.discardCount[i] > 0) {
        pre.hand[i][pre.handCount[i]] = post->hand[i][pre.handCount[i]];
        pre.handCount[i]++;
        pre.deckCount[i] = pre.discardCount[i]-1;
        memcpy(pre.deck[i], post->deck[i], sizeof(int) * pre.discardCount[i]);
        for (j = 0; j < pre.discardCount[i]; j++)
          pre.discard[i][j] = -1;
        pre.discardCount[i] = 0;
      }
    }
  }

  // Test discardCard function effects on playedCards array
  pre.playedCardCount++;
  pre.playedCards[pre.playedCardCount-1] = council_room;

  // Increase numBuys by one
  pre.numBuys++;

  assert (r == 0);

  assert(memcmp(&pre, post, sizeof(struct gameState)) == 0);
}

int main () {

  int i, n, p, hp, player;

  struct gameState G;

  printf ("Testing CouncilRoom Card.\n");

  printf ("RANDOM TESTS.\n");

  SelectStream(2);
  PutSeed(3);

  for (n = 0; n < 2000; n++) {
    for (i = 0; i < sizeof(struct gameState); i++) {
      ((char*)&G)[i] = floor(Random() * 256);
    }
    G.numPlayers = floor(Random() * (MAX_PLAYERS-1)) + 2;
    p = floor(Random() * G.numPlayers);
    for (player = 0; player < G.numPlayers; player++) {
      if (player == p)
        G.handCount[player] = floor(Random() * (MAX_HAND-4)) + 1;
      else
        G.handCount[player] = floor(Random() * MAX_HAND);
      G.deckCount[player] = floor(Random() * (MAX_DECK+1-G.handCount[player]));
      G.discardCount[player] = floor(Random() * (MAX_DECK+1-G.handCount[player]-G.deckCount[player]));
    }
    G.playedCardCount = floor(Random() * MAX_DECK);
    hp = floor(Random() * G.handCount[p]);
    G.hand[p][hp] = council_room;
    checkCouncilRoomCard(hp, p, &G);
  }

  printf ("ALL TESTS OK\n");

  return 0;
}

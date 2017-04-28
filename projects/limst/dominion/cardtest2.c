/* -----------------------------------------------------------------------
 * Unit test for playAdventurer function in dominion.c
 * -----------------------------------------------------------------------
 */

#include "dominion.h"
#include "dominion_helpers.h"
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "rngs.h"
#include "interface.h"

// set NOISY_TEST to 0 to remove printfs from output
#define NOISY_TEST 0
#define SUPPRESS_SUCCESS 1
void asserttrue(int test, int *globalFailFlag) {
    if (test == 0) { // test failed
        printf("TEST FAILED\n");
        *globalFailFlag = 1;
    }
    else { // test passed
#if (SUPPRESS_SUCCESS == 0)
        printf("TEST SUCCESSFULLY PASSED\n");
#endif
    }
}

int main() {
    int globalFailFlag = 0; // status flag indicates if any test failed
    int i;
    int seed = 1000;
    int numPlayer = 4;
    int p, r, deckCount, discardCount, handCount = 5, handPos;
    int treasureCardCount, disTreas, deckTreas;
    int k[10] = {adventurer, council_room, feast, gardens, mine
               , remodel, smithy, village, baron, great_hall};
    struct gameState G;
    // arrays of all coppers, silvers, and golds

    int estates[MAX_HAND];
    int duchies[MAX_HAND];
    int provinces[MAX_HAND];
    for (i = 0; i < MAX_HAND; i++)
    {
        estates[i] = estate;
        duchies[i] = duchy;
        provinces[i] = province;
    }

    int s;
    int drawntreasure = 0, temphand[MAX_DECK], z = 0, cardDrawn = 0;
    printf ("TESTING playAdventurer():\n");

    for (p = 0; p < numPlayer; p++)
    {
        for (deckCount = 0; deckCount <= 10; deckCount++)
        {
            for (discardCount = 0; discardCount <= 10; discardCount++)
            {
                for (handPos = 0; handPos < handCount; handPos++)
                {
                  for (disTreas = copper; disTreas <= gold; disTreas++)
                  {
                    for (deckTreas = copper; deckTreas <= gold; deckTreas++)
                    {
                    if (deckCount + discardCount < 2)
                        continue;
#if (NOISY_TEST == 1)
                printf("Test player %d playing a Adventurer card.\n", p);
#endif
                memset(&G, 23, sizeof(struct gameState));   // clear the game state
                r = initializeGame(numPlayer, k, seed, &G); // initialize a new game
                for (s = 0; s < numPlayer; s++) { // empty all hands, decks, and discard piles to maximum to check that the other player's states will not be changed
                    G.handCount[s] = 0;
                    G.discardCount[s] = 0;
                    G.deckCount[s] = 0;
                }
                for (s = 0; s <= treasure_map; s++)
                    G.supplyCount[s] = 0;
                G.handCount[p] = handCount;
                memcpy(G.hand[p], estates, sizeof(int) * handCount);
                G.hand[p][handPos] = adventurer;
                G.discardCount[p] = discardCount;
                memcpy(G.discard[p], duchies, sizeof(int) * discardCount);
                G.deckCount[p] = deckCount;
                memcpy(G.deck[p], provinces, sizeof(int) * deckCount);
                if (discardCount > 0 && deckCount > 0) {
                    G.discard[p][discardCount-1] = disTreas;
                    G.deck[p][deckCount-1] = deckTreas;
                }
                else if (discardCount > 0) {
                    G.discard[p][0] = disTreas;
                    G.discard[p][discardCount-1] = disTreas;
                } 
                else if (deckCount > 0) {
                    G.deck[p][0] = deckTreas;
                    G.deck[p][deckCount-1] = deckTreas;
                } 

                r = playAdventurer(handPos, p, &G, drawntreasure, temphand, z, cardDrawn); // Call function under test
#if (NOISY_TEST == 1)
                printf("playAdventurer return value = %d, expected = %d\n", r, 0);
#endif
                asserttrue(r == 0, &globalFailFlag); // check if playAdventurer return value is correct
                treasureCardCount = 0;
                for (s = 0; s < G.handCount[p]; s++) {
                    if (G.hand[p][s] >= copper && G.hand[p][s] <= gold)
                        treasureCardCount++;
#if (NOISY_TEST == 1)
                    printf("G.hand[p][s] = %d, expected != %d\n", G.hand[p][s], adventurer);
#endif
                    asserttrue(G.hand[p][s] != adventurer, &globalFailFlag);
                }
#if (NOISY_TEST == 1)
                printf("treasureCardCount = %d, expected = %d\n", treasureCardCount, 2);
#endif
                asserttrue(treasureCardCount == 2, &globalFailFlag); // check if discard count is one
#if (NOISY_TEST == 1)
                printf("G.handCount[p] = %d, expected = %d\n", G.handCount[p], handCount+1);
#endif
                asserttrue(G.handCount[p] == handCount+1, &globalFailFlag); // check if discard count is one
                for (r = 0; r <= treasure_map; r++) {
#if (NOISY_TEST == 1)
                    printf("G.supplyCount[r] = %d, expected = %d\n", G.supplyCount[r], 0);
#endif
                    asserttrue(G.supplyCount[r] == 0, &globalFailFlag); // check if discard count is one
                }
                for (r = 0; r < numPlayer; r++) {
#if (NOISY_TEST == 1)
                    if (r != p)
                        printf("G.handCount[r] = %d, expected = %d\n", G.handCount[r], 0);
#endif
                    if (r != p)
                        asserttrue(G.handCount[r] == 0, &globalFailFlag); // check if all other players' hands are empty
#if (NOISY_TEST == 1)
                    if (r != p)
                        printf("G.deckCount[r] = %d, expected = %d\n", G.deckCount[r], 0);
#endif
                    if (r != p)
                        asserttrue(G.deckCount[r] == 0, &globalFailFlag); // check if all other players' decks are empty
#if (NOISY_TEST == 1)
                    if (r != p)
                        printf("G.discardCount[r] = %d, expected = %d\n", G.discardCount[r], 0);
#endif
                    if (r != p)
                        asserttrue(G.discardCount[r] == 0, &globalFailFlag); // check if all other players' decks are empty
                }
                    }
                  }
                }
            }
        }
    }
    printf("******* playAdventurer function CARD TEST #2 EXECUTION IS COMPLETE *******\n");
    printf("RESULT: ");
    if (globalFailFlag == 0)
        printf("ALL TESTS PASSED\n");
    else
        printf("AT LEAST ONE TEST FAILED\n");

    return 0;
}

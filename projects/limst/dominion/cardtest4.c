/* -----------------------------------------------------------------------
 * Unit test for playSeaHag function in dominion.c
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
    int curseCount = numPlayer-1;
    int k[10] = {adventurer, council_room, feast, gardens, mine
               , remodel, smithy, village, baron, great_hall};
    struct gameState G;
    // arrays of all coppers, silvers, and golds
    int coppers[MAX_HAND];
    int silvers[MAX_HAND];
    int golds[MAX_HAND];
    for (i = 0; i < MAX_HAND; i++)
    {
        coppers[i] = copper;
        silvers[i] = silver;
        golds[i] = gold;
    }

    int s;
    printf ("TESTING playSeaHag():\n");

    for (p = 0; p < numPlayer; p++)
    {
        for (deckCount = 0; deckCount <= 2; deckCount++)
        {
            for (discardCount = 0; discardCount <= 2; discardCount++)
            {
                for (handPos = 0; handPos < handCount; handPos++)
                {
                if (deckCount + discardCount < 1)
                    continue;
#if (NOISY_TEST == 1)
                printf("Test player %d playing a Sea Hag card with deck = %d, discard = %d.\n", p, deckCount, discardCount);
#endif
                memset(&G, 23, sizeof(struct gameState));   // clear the game state
                r = initializeGame(numPlayer, k, seed, &G); // initialize a new game
                for (s = 0; s < numPlayer; s++) { // empty all hands, decks, and discard piles to maximum to check that the other player's states will not be changed
                    G.handCount[s] = 0;
                    G.discardCount[s] = discardCount;
                    memcpy(G.discard[s], silvers, sizeof(int) * discardCount);
                    G.deckCount[s] = deckCount;
                    memcpy(G.deck[s], golds, sizeof(int) * deckCount);
                }
                for (s = 0; s <= treasure_map; s++)
                    G.supplyCount[s] = 0;
                G.supplyCount[curse] = curseCount;

                G.handCount[p] = handCount;
                memcpy(G.hand[p], coppers, sizeof(int) * handCount);
                G.hand[p][handPos] = sea_hag;
                G.discardCount[p] = 0;
                G.deckCount[p] = 0;

                G.playedCardCount = 0;
                G.playedCards[0] = copper;

                r = playSeaHag(handPos, p, &G); // Call function under test
#if (NOISY_TEST == 1)
                printf("playSeaHag return value = %d, expected = %d\n", r, 0);
#endif
                asserttrue(r == 0, &globalFailFlag); // check if playSeaHag return value is correct
#if (NOISY_TEST == 1)
                printf("G.supplyCount[curse] = %d, expected = %d\n", G.supplyCount[curse], 0);
#endif
                asserttrue(G.supplyCount[curse] == 0, &globalFailFlag); // check if playSeaHag return value is correct
                for (s = 0; s < G.handCount[p]; s++) {
#if (NOISY_TEST == 1)
                    printf("G.hand[p][s] = %d, expected != %d\n", G.hand[p][s], sea_hag);
#endif
                    asserttrue(G.hand[p][s] != sea_hag, &globalFailFlag);
                }
#if (NOISY_TEST == 1)
                printf("G.handCount[p] = %d, expected = %d\n", G.handCount[p], handCount-1);
#endif
                asserttrue(G.handCount[p] == handCount-1, &globalFailFlag); // check if discard count is one
                for (s = 0; s < numPlayer; s++) {
                    if (s != p) {
                        if (deckCount > 0) {
#if (NOISY_TEST == 1)
                            printf("G.deckCount[s] = %d, expected = %d\n", G.deckCount[s], deckCount);
#endif
                            asserttrue(G.deckCount[s] == deckCount, &globalFailFlag); // check if discard count is one
                        }
                        else {
#if (NOISY_TEST == 1)
                            printf("G.deckCount[s] = %d, expected = %d\n", G.deckCount[s], discardCount);
#endif
                            asserttrue(G.deckCount[s] == discardCount, &globalFailFlag); // check if discard count is one
                        }
#if (NOISY_TEST == 1)
                        printf("G.deck[s][G.deckCount[s]-1] = %d, expected = %d\n", G.deck[s][G.deckCount[s]-1], curse);
#endif
                        asserttrue(G.deck[s][G.deckCount[s]-1] == curse, &globalFailFlag); // check if discard count is one
                    }
                }
                for (r = 0; r < numPlayer; r++) {
#if (NOISY_TEST == 1)
                    if (r != p)
                        printf("G.handCount[r] = %d, expected = %d\n", G.handCount[r], 0);
#endif
                    if (r != p)
                        asserttrue(G.handCount[r] == 0, &globalFailFlag); // check if all other players' hands are empty
#if (NOISY_TEST == 1)
                    if (r == p)
                        printf("G.discardCount[r] = %d, expected = %d\n", G.discardCount[r], 0);
#endif
                    if (r == p)
                        asserttrue(G.discardCount[r] == 0, &globalFailFlag); // check if all other players' hands are empty
#if (NOISY_TEST == 1)
                    if (r == p)
                        printf("G.deckCount[r] = %d, expected = %d\n", G.deckCount[r], 0);
#endif
                    if (r == p)
                        asserttrue(G.deckCount[r] == 0, &globalFailFlag); // check if all other players' hands are empty
                }
#if (NOISY_TEST == 1)
                        printf("G.playedCardCount = %d, expected = %d\n", G.playedCardCount, 1);
#endif
                        asserttrue(G.playedCardCount == 1, &globalFailFlag); // check if discard count is one
#if (NOISY_TEST == 1)
                        printf("G.playedCards[0] = %d, expected = %d\n", G.playedCards[0], sea_hag);
#endif
                        asserttrue(G.playedCards[0] == sea_hag, &globalFailFlag); // check if discard count is one

                }
            }
        }
    }
    printf("******* playSeaHag function CARD TEST #4 EXECUTION IS COMPLETE *******\n");
    printf("RESULT: ");
    if (globalFailFlag == 0)
        printf("ALL TESTS PASSED\n");
    else
        printf("AT LEAST ONE TEST FAILED\n");

    return 0;
}

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
#define NOISY_TEST 1
void asserttrue(int test, int *globalFailFlag, int verbose) {
    if (test == 0) { // test failed
        printf("- TEST FAILED\n");
        *globalFailFlag = 1;
    }
    else { // test passed
#if (NOISY_TEST == 1)
        if (verbose)
            printf("- TEST SUCCESSFULLY PASSED\n");
#endif
    }
}

int main() {
    int globalFailFlag = 0; // status flag indicates if any test failed
    int i;
    int seed = 1000;
    int numPlayer = 4;
    int p, r, deckCount, discardCount, handCount = 5, handPos;
    int curseCount;
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

    for (p = 0; p < numPlayer; p++) // test for each player
    {
        for (deckCount = 0; deckCount <= 2; deckCount++) // test for different deck counts
        {
            for (discardCount = 0; discardCount <= 2; discardCount++) // test for different discard counts
            {
                for (handPos = 0; handPos < handCount; handPos++) // test playing sea_hag for each hand position
                {
#if (NOISY_TEST == 1)
                printf("Test player %d playing a Sea Hag card with deck = %d, discard = %d.\n", p, deckCount, discardCount);
#endif
                memset(&G, 23, sizeof(struct gameState));   // clear the game state
                r = initializeGame(numPlayer, k, seed, &G); // initialize a new game
                for (s = 0; s < numPlayer; s++) { // initialize other player's decks and discards to test sea_hag effects on the other players
                    G.handCount[s] = 0;
                    G.discardCount[s] = discardCount;
                    memcpy(G.discard[s], silvers, sizeof(int) * discardCount);
                    G.deckCount[s] = deckCount;
                    memcpy(G.deck[s], golds, sizeof(int) * deckCount);
                }
                for (s = 0; s <= treasure_map; s++)
                    G.supplyCount[s] = 0;
                curseCount = numPlayer - 1;
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
                printf("playSeaHag return value = %d, expected = %d ", r, 0);
#endif
                asserttrue(r == 0, &globalFailFlag, 1); // check if playSeaHag return value is correct
#if (NOISY_TEST == 1)
                printf("G.supplyCount[curse] = %d, expected = %d ", G.supplyCount[curse], 0);
#endif
                asserttrue(G.supplyCount[curse] == 0, &globalFailFlag, 1); // check if curse supply count is zero
                for (s = 0; s < G.handCount[p]; s++) {
#if (NOISY_TEST == 1)
                    printf("G.hand[p][s] = %d, expected != %d ", G.hand[p][s], sea_hag);
#endif
                    asserttrue(G.hand[p][s] != sea_hag, &globalFailFlag, 1); // check if sea_hag is removed from player's hand
                }
#if (NOISY_TEST == 1)
                printf("G.handCount[p] = %d, expected = %d ", G.handCount[p], handCount-1);
#endif
                asserttrue(G.handCount[p] == handCount-1, &globalFailFlag, 1); // check if hand count is correct
                for (s = 0; s < numPlayer; s++) { // each other player
                    if (s != p) {
                        if (deckCount > 0) { // after playing the sea_hag card, the deck and discard counts will be different depending on the original deck and discard counts
#if (NOISY_TEST == 1)
                            printf("G.deckCount[s] = %d, expected = %d ", G.deckCount[s], deckCount);
#endif
                            asserttrue(G.deckCount[s] == deckCount, &globalFailFlag, 1); // check if deck count is unchanged
#if (NOISY_TEST == 1)
                            printf("G.discardCount[s] = %d, expected = %d ", G.discardCount[s], discardCount+1);
#endif
                            asserttrue(G.discardCount[s] == discardCount+1, &globalFailFlag, 1); // check if discard count is increased by one
                        }
                        else if (discardCount > 0) { // new deck used to be old discard
#if (NOISY_TEST == 1)
                            printf("G.deckCount[s] = %d, expected = %d ", G.deckCount[s], discardCount);
#endif
                            asserttrue(G.deckCount[s] == discardCount, &globalFailFlag, 1); // check if deck count is the old discard count since the new deck used to be the old discard
#if (NOISY_TEST == 1)
                            printf("G.discardCount[s] = %d, expected = %d ", G.discardCount[s], 1);
#endif
                            asserttrue(G.discardCount[s] == 1, &globalFailFlag, 1); // check if discard count is one
                        }
                        else { // case of empty deck and empty discard
#if (NOISY_TEST == 1)
                            printf("G.deckCount[s] = %d, expected = %d ", G.deckCount[s], 1);
#endif
                            asserttrue(G.deckCount[s] == 1, &globalFailFlag, 1); // check if deck count is one (one curse card)
#if (NOISY_TEST == 1)
                            printf("G.discardCount[s] = %d, expected = %d ", G.discardCount[s], 0);
#endif
                            asserttrue(G.discardCount[s] == 0, &globalFailFlag, 1); // check if discard remains empty
                        }
#if (NOISY_TEST == 1)
                        printf("G.deck[s][G.deckCount[s]-1] = %d and G.deck[s][0] = %d, expected = %d ", G.deck[s][G.deckCount[s]-1], G.deck[s][0], curse);
#endif
                        asserttrue(G.deck[s][G.deckCount[s]-1] == curse || G.deck[s][0] == curse, &globalFailFlag, 1); // check if top of deck is a curse card - since it is unknown how the draw card function is implemented, the top of deck could be the first entry or last entry in deck array 
                    }
                }
                for (r = 0; r < numPlayer; r++) {
#if (NOISY_TEST == 1)
                    if (r != p)
                        printf("G.handCount[r] = %d, expected = %d ", G.handCount[r], 0);
#endif
                    if (r != p)
                        asserttrue(G.handCount[r] == 0, &globalFailFlag, 1); // check if all other players' hands are empty
#if (NOISY_TEST == 1)
                    if (r == p)
                        printf("G.discardCount[r] = %d, expected = %d ", G.discardCount[r], 0);
#endif
                    if (r == p)
                        asserttrue(G.discardCount[r] == 0, &globalFailFlag, 1); // check if player's discard remains unchanged
#if (NOISY_TEST == 1)
                    if (r == p)
                        printf("G.deckCount[r] = %d, expected = %d ", G.deckCount[r], 0);
#endif
                    if (r == p)
                        asserttrue(G.deckCount[r] == 0, &globalFailFlag, 1); // check if player's deck remains unchanged
                }
#if (NOISY_TEST == 1)
                        printf("G.playedCardCount = %d, expected = %d ", G.playedCardCount, 1);
#endif
                        asserttrue(G.playedCardCount == 1, &globalFailFlag, 1); // check if played card count is one
#if (NOISY_TEST == 1)
                        printf("G.playedCards[0] = %d, expected = %d ", G.playedCards[0], sea_hag);
#endif
                        asserttrue(G.playedCards[0] == sea_hag, &globalFailFlag, 1); // check played card array contains sea_hag

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

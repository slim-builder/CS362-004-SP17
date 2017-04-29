/* -----------------------------------------------------------------------
 * Unit test for buyCard function in dominion.c
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
    int p, r, supplyPos;
    int k[10] = {adventurer, council_room, feast, gardens, mine
               , remodel, smithy, village, baron, great_hall};
    struct gameState G;
//    int maxHandCount = 5;
    // arrays of all coppers, silvers, and golds
//    int coppers[MAX_HAND];
//    int silvers[MAX_HAND];
//    int golds[MAX_HAND];
    for (i = 0; i < MAX_HAND; i++)
    {
//        coppers[i] = copper;
//        silvers[i] = silver;
//        golds[i] = gold;
    }

    int s;
#if (NOISY_TEST == 1)
    char cardname[256];
#endif
    printf ("TESTING buyCard():\n");

    for (p = 0; p < numPlayer; p++) // test for each player
    {
        for (supplyPos = 0; supplyPos <= treasure_map; supplyPos++) // test for each card type
        {
#if (NOISY_TEST == 1)
                cardNumToName(supplyPos, cardname);
                printf("Test player %d buying a %s card.\n", p, cardname);
#endif
                memset(&G, 23, sizeof(struct gameState));   // clear the game state
                r = initializeGame(numPlayer, k, seed, &G); // initialize a new game
                for (s = 0; s < numPlayer; s++) { // empty all hands, decks, and discard piles to check that the other player's states will not be changed
                    G.handCount[s] = 0;
                    G.discardCount[s] = 0;
                    G.deckCount[s] = 0;
                }
                for (s = 0; s <= treasure_map; s++) {
                    G.supplyCount[s] = 0; // Supply count of all cards to zero
                }

                G.whoseTurn = p; // Set current player
                G.coins = 10; // Set coins to ten
                G.numBuys = 1;
                G.phase = 0;

                r = buyCard(supplyPos, &G); // Call function under test

#if (NOISY_TEST == 1)
                printf("buyCard return value = %d, expected = %d ", r, -1);
#endif
                asserttrue(r == -1, &globalFailFlag, 1); // check if buyCard return value is correct when supply count of card is zero
                G.supplyCount[supplyPos] = 1;  // Set supply count of the card type to be bought to one

                G.whoseTurn = p; // Set current player
                G.coins = 0; // Set coins to zero
                G.numBuys = 1;
                G.phase = 0;

                r = buyCard(supplyPos, &G); // Call function under test
                if (supplyPos != curse && supplyPos != copper) {
#if (NOISY_TEST == 1)
                    printf("buyCard return value = %d, expected = %d ", r, -1);
#endif
                    asserttrue(r == -1, &globalFailFlag, 1); // check if buyCard return value is correct when coins is zero
                }
                else {
#if (NOISY_TEST == 1)
                    printf("buyCard return value = %d, expected = %d ", r, 0);
#endif
                    asserttrue(r == 0, &globalFailFlag, 1); // check if buyCard return value is correct when coins is zero
                    G.discardCount[p] = 0;
                }
                G.supplyCount[supplyPos] = 1;  // Set supply count of the card type to be bought to one

                G.whoseTurn = p; // Set current player
                G.coins = 10; // Set coins to ten
                G.numBuys = 0;
                G.phase = 0;

                r = buyCard(supplyPos, &G); // Call function under test

#if (NOISY_TEST == 1)
                printf("buyCard return value = %d, expected = %d ", r, -1);
#endif
                asserttrue(r == -1, &globalFailFlag, 1); // check if buyCard return value is correct when numBuys is zero

                G.supplyCount[supplyPos] = 1;  // Set supply count of the card type to be bought to one

                G.whoseTurn = p; // Set current player
                G.coins = 10; // Set coins to ten
                G.numBuys = 1;
                G.phase = 0;

                r = buyCard(supplyPos, &G); // Call function under test

#if (NOISY_TEST == 1)
                printf("buyCard return value = %d, expected = %d ", r, 0);
#endif
                asserttrue(r == 0, &globalFailFlag, 1); // check if buyCard return value is correct
#if (NOISY_TEST == 1)
                printf("G.whoseTurn = %d, expected = %d ", G.whoseTurn, p);
#endif
                asserttrue(G.whoseTurn == p, &globalFailFlag, 1); // check if the player whose turn it is remains unchanged
#if (NOISY_TEST == 1)
                printf("G.coins = %d, expected = %d ", G.coins, 10 - getCost(supplyPos));
#endif
                asserttrue(G.coins == 10 - getCost(supplyPos), &globalFailFlag, 1); // check if the cost of the card is deducted properly from the total number of coins after buying a card
#if (NOISY_TEST == 1)
                printf("G.numBuys = %d, expected = %d ", G.numBuys, 0);
#endif
                asserttrue(G.numBuys == 0, &globalFailFlag, 1); // check if number of buys is zero
#if (NOISY_TEST == 1)
                printf("G.phase = %d, expected = %d ", G.phase, 1);
#endif
                asserttrue(G.phase == 1, &globalFailFlag, 1); // check if turn phase is one
#if (NOISY_TEST == 1)
                printf("G.discard[p][0] = %d, expected = %d ", G.discard[p][0], supplyPos);
#endif
                asserttrue(G.discard[p][0] == supplyPos, &globalFailFlag, 1); // check if the player's discard pile only has the bought card since the discard pile initially is empty
#if (NOISY_TEST == 1)
                printf("G.discardCount[p] = %d, expected = %d ", G.discardCount[p], 1);
#endif
                asserttrue(G.discardCount[p] == 1, &globalFailFlag, 1); // check if discard count is one
                for (r = 0; r <= treasure_map; r++) {
#if (NOISY_TEST == 1)
                    printf("G.supplyCount[r] = %d, expected = %d ", G.supplyCount[r], 0);
#endif
                    asserttrue(G.supplyCount[r] == 0, &globalFailFlag, 1); // check if supply count is reduced by one after buying the card
                }
                for (r = 0; r < numPlayer; r++) {
#if (NOISY_TEST == 1)
                    printf("G.handCount[r] = %d, expected = %d ", G.handCount[r], 0);
#endif
                    asserttrue(G.handCount[r] == 0, &globalFailFlag, 1); // check if all other players' hands are empty
#if (NOISY_TEST == 1)
                    printf("G.deckCount[r] = %d, expected = %d ", G.deckCount[r], 0);
#endif
                    asserttrue(G.deckCount[r] == 0, &globalFailFlag, 1); // check if all other players' decks are empty
#if (NOISY_TEST == 1)
                    if (r != p)
                        printf("G.discardCount[r] = %d, expected = %d ", G.discardCount[r], 0);
#endif
                    if (r != p)
                        asserttrue(G.discardCount[r] == 0, &globalFailFlag, 1); // check if all other players' discards are empty
                }
        }
    }
    printf("******* buyCard function UNIT TEST #2 EXECUTION IS COMPLETE *******\n");
    printf("RESULT: ");
    if (globalFailFlag == 0)
        printf("ALL TESTS PASSED\n");
    else
        printf("AT LEAST ONE TEST FAILED\n");

    return 0;
}

/* -----------------------------------------------------------------------
 * Unit test for gainCard function in dominion.c
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
    int p, r, supplyPos, toFlag;
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
    printf ("TESTING gainCard():\n");

    for (p = 0; p < numPlayer; p++)
    {
        for (supplyPos = 0; supplyPos <= treasure_map; supplyPos++)
        {
            for (toFlag = 0; toFlag <= 2; toFlag++)
            {
#if (NOISY_TEST == 1)
                cardNumToName(supplyPos, cardname);
                printf("Test player %d gaining a %s card.\n", p, cardname);
#endif
                memset(&G, 23, sizeof(struct gameState));   // clear the game state
                r = initializeGame(numPlayer, k, seed, &G); // initialize a new game
                for (s = 0; s < numPlayer; s++) { // empty all hands, decks, and discard piles to maximum to check that the other player's states will not be changed
                    G.handCount[s] = 0;
                    G.discardCount[s] = 0;
                    G.deckCount[s] = 0;
                }
                for (s = 0; s <= treasure_map; s++) {
                    G.supplyCount[s] = 0; // Supply count of all cards to zero
                }

                r = gainCard(supplyPos, &G, toFlag, p); // Call function under test
#if (NOISY_TEST == 1)
                printf("gainCard return value = %d, expected = %d\n", r, -1);
#endif
                asserttrue(r == -1, &globalFailFlag); // check if gainCard return value is correct when supply count of card is zero
                for (s = 0; s < numPlayer; s++) { // empty all hands, decks, and discard piles to maximum to check that the other player's states will not be changed
                    G.handCount[s] = 0;
                    G.discardCount[s] = 0;
                    G.deckCount[s] = 0;
                }
                G.supplyCount[supplyPos] = 1;  // Set supply count of the card type to be bought to one

                r = gainCard(supplyPos, &G, toFlag, p); // Call function under test
#if (NOISY_TEST == 1)
                printf("gainCard return value = %d, expected = %d\n", r, 0);
#endif
                asserttrue(r == 0, &globalFailFlag); // check if gainCard return value is correct when coins is zero
                if (toFlag == 2) {
#if (NOISY_TEST == 1)
                    printf("G.hand[p][0] = %d, expected = %d\n", G.hand[p][0], supplyPos);
#endif
                    asserttrue(G.hand[p][0] == supplyPos, &globalFailFlag); // check if the player's discard pile only has the bought card since the discard pile initially is empty
#if (NOISY_TEST == 1)
                    printf("G.handCount[p] = %d, expected = %d\n", G.handCount[p], 1);
#endif
                    asserttrue(G.handCount[p] == 1, &globalFailFlag); // check if discard count is one
                }
                else if (toFlag == 1) {
#if (NOISY_TEST == 1)
                    printf("G.deck[p][0] = %d, expected = %d\n", G.deck[p][0], supplyPos);
#endif
                    asserttrue(G.deck[p][0] == supplyPos, &globalFailFlag); // check if the player's discard pile only has the bought card since the discard pile initially is empty
#if (NOISY_TEST == 1)
                    printf("G.deckCount[p] = %d, expected = %d\n", G.deckCount[p], 1);
#endif
                    asserttrue(G.deckCount[p] == 1, &globalFailFlag); // check if discard count is one
                }
                else {
#if (NOISY_TEST == 1)
                    printf("G.discard[p][0] = %d, expected = %d\n", G.discard[p][0], supplyPos);
#endif
                    asserttrue(G.discard[p][0] == supplyPos, &globalFailFlag); // check if the player's discard pile only has the bought card since the discard pile initially is empty
#if (NOISY_TEST == 1)
                    printf("G.discardCount[p] = %d, expected = %d\n", G.discardCount[p], 1);
#endif
                    asserttrue(G.discardCount[p] == 1, &globalFailFlag); // check if discard count is one
                }
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
    printf("******* gainCard function UNIT TEST #4 EXECUTION IS COMPLETE *******\n");
    printf("RESULT: ");
    if (globalFailFlag == 0)
        printf("ALL TESTS PASSED\n");
    else
        printf("AT LEAST ONE TEST FAILED\n");

    return 0;
}

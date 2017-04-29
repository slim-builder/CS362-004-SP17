/* -----------------------------------------------------------------------
 * Unit test for drawCard function in dominion.c
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
    int p, r, deckCount, discardCount, handCount;
    int k[10] = {adventurer, council_room, feast, gardens, mine
               , remodel, smithy, village, baron, great_hall};
    struct gameState G;
    int maxHandCount = 5;
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
    int retValue, disValue, deckValue;
    printf ("TESTING drawCard():\n");

    for (p = 0; p < numPlayer; p++) // test for each player
    {
        for (deckCount = 0; deckCount <= 10; deckCount++) // test for different deck counts
        {
            for (discardCount = 0; discardCount <= 10; discardCount++) // test for different discard counts
            {
                for (handCount = 0; handCount < maxHandCount; handCount++) // test for different hand counts
                {
#if (NOISY_TEST == 1)
                printf("Test player %d drawing a card when deck = %d, discard = %d, hand = %d.\n", p, deckCount, discardCount, handCount);
#endif
                memset(&G, 23, sizeof(struct gameState));   // clear the game state
                r = initializeGame(numPlayer, k, seed, &G); // initialize a new game
                for (s = 0; s < numPlayer; s++) { // empty all hands, decks, and discard piles to check that the other player's states will not be changed
                    G.handCount[s] = 0;
                    G.discardCount[s] = 0;
                    G.deckCount[s] = 0;
                }
                G.handCount[p] = handCount; // initialize size of player's hand, discard, and deck to verify changes after drawing a card
                G.discardCount[p] = discardCount;
                G.deckCount[p] = deckCount;
                memcpy(G.hand[p], coppers, sizeof(int) * handCount);
                memcpy(G.discard[p], silvers, sizeof(int) * discardCount);
                memcpy(G.deck[p], golds, sizeof(int) * deckCount);

                r = drawCard(p, &G); // Call function under test

                if (discardCount == 0 && deckCount == 0)
                    retValue = -1;
                else
                    retValue = 0;
#if (NOISY_TEST == 1)
                printf("drawCard return value = %d, expected = %d ", r, retValue);
#endif
                asserttrue(r == retValue, &globalFailFlag, 1); // check if drawCard return value is correct when supply count of card is zero
                if (r == -1)
                    continue;

                if (deckCount == 0) // expected discardCount is different depending on whether the deck is initially empty or not.  Check if a shuffled discard pile is used as the new deck when the old deck is empty
                    disValue = 0;
                else
                    disValue = discardCount;
#if (NOISY_TEST == 1)
                printf("G.discardCount[p] = %d, expected = %d ", G.discardCount[p], disValue); 
#endif
                asserttrue(G.discardCount[p] == disValue, &globalFailFlag, 1); // check if discard count is correct
                if (deckCount != 0) {
                    for (s = 0; s < discardCount; s++) {
#if (NOISY_TEST == 1)
                        printf("G.discard[p][s] = %d, expected = %d ", G.discard[p][s], silver);
#endif
                        asserttrue(G.discard[p][s] == silver, &globalFailFlag, 1); // check if discard pile cards are correct
                    }
                }
                if (deckCount == 0) // expected deck count is based on whether or not the deck is initially empty or not
                    deckValue = discardCount - 1;
                else
                    deckValue = deckCount - 1;
#if (NOISY_TEST == 1)
                printf("G.deckCount[p] = %d, expected = %d ", G.deckCount[p], deckValue);
#endif
                asserttrue(G.deckCount[p] == deckValue, &globalFailFlag, 1); // check if deck count is correct
                if (deckCount != 0) {  // the expected card type depends on whether the deck is initially empty or not since the discard pile cards have a different type than the deck pile cards
                    for (s = 0; s < deckCount-1; s++) {
#if (NOISY_TEST == 1)
                        printf("G.deck[p][s] = %d, expected = %d ", G.deck[p][s], gold);
#endif
                        asserttrue(G.deck[p][s] == gold, &globalFailFlag, 1); // check if deck cards are correct
                    }
                }
                else {
                    for (s = 0; s < discardCount-1; s++) {
#if (NOISY_TEST == 1)
                        printf("G.deck[p][s] = %d, expected = %d ", G.deck[p][s], silver);
#endif
                        asserttrue(G.deck[p][s] == silver, &globalFailFlag, 1); // check if deck cards are correct
                    }
                }
#if (NOISY_TEST == 1)
                printf("G.handCount[p] = %d, expected = %d ", G.handCount[p], handCount + 1);
#endif
                asserttrue(G.handCount[p] == handCount + 1, &globalFailFlag, 1); // check if hand count is one
                if (deckCount != 0) {
#if (NOISY_TEST == 1)
                    printf("G.hand[p][handCount] = %d, expected = %d ", G.hand[p][handCount], gold);
#endif
                    asserttrue(G.hand[p][handCount] == gold, &globalFailFlag, 1); // check if the drawn card is in the player's hand
                }
                else {
#if (NOISY_TEST == 1)
                    printf("G.hand[p][handCount] = %d, expected = %d ", G.hand[p][handCount], silver);
#endif
                    asserttrue(G.hand[p][handCount] == silver, &globalFailFlag, 1);// check if the drawn card is in the player's hand
                }
                for (r = 0; r < numPlayer; r++) {
#if (NOISY_TEST == 1)
                    if (r != p)
                        printf("G.handCount[r] = %d, expected = %d ", G.handCount[r], 0);
#endif
                    if (r != p)
                        asserttrue(G.handCount[r] == 0, &globalFailFlag, 1); // check if all other players' hands are empty
#if (NOISY_TEST == 1)
                    if (r != p)
                        printf("G.deckCount[r] = %d, expected = %d ", G.deckCount[r], 0);
#endif
                    if (r != p)
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
        }
    }
    printf("******* drawCard function UNIT TEST #3 EXECUTION IS COMPLETE *******\n");
    printf("RESULT: ");
    if (globalFailFlag == 0)
        printf("ALL TESTS PASSED\n");
    else
        printf("AT LEAST ONE TEST FAILED\n");

    return 0;
}

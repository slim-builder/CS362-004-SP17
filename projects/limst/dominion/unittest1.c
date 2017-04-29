/* -----------------------------------------------------------------------
 * Unit test for endTurn function in dominion.c
 * -----------------------------------------------------------------------
 */

#include "dominion.h"
#include "dominion_helpers.h"
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "rngs.h"

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
    int p, r, handCount;
    int k[10] = {adventurer, council_room, feast, gardens, mine
               , remodel, smithy, village, baron, great_hall};
    struct gameState G;
    int maxHandCount = 5;
    // arrays of all coppers, silvers, and golds
    int coppers[MAX_HAND];
    int silvers[MAX_HAND];
//    int golds[MAX_HAND];
    for (i = 0; i < MAX_HAND; i++)
    {
        coppers[i] = copper;
        silvers[i] = silver;
//        golds[i] = gold;
    }

    int q;
    int s;
    printf ("TESTING endTurn():\n");
    for (p = 0; p < numPlayer; p++) // test for each player
    {
        for (handCount = 0; handCount <= maxHandCount; handCount++) // test for each hand count
        {
                // q represents the next player who will be taking their turn
                if (p < numPlayer-1)
                    q = p + 1;
                else
                    q = 0;
#if (NOISY_TEST == 1)
                printf("Test player %d with %d hand card(s).\n", p, handCount);
#endif
                memset(&G, 23, sizeof(struct gameState));   // clear the game state
                r = initializeGame(numPlayer, k, seed, &G); // initialize a new game
                for (s = 0; s < numPlayer; s++) { // empty all hands, decks, and discard piles to check that the other player's states will not be changed
                    G.handCount[s] = 0;
                    G.discardCount[s] = 0;
                    G.deckCount[s] = 0;
                }
                G.handCount[p] = handCount;                 // set the number of cards on hand
                G.discardCount[p] = handCount;                 // set the number of cards in discard pile
                G.deckCount[q] = handCount + 5;                 // set the number of cards in deck of next player
                memcpy(G.hand[p], coppers, sizeof(int) * handCount); // set all hand cards to copper
                memcpy(G.discard[p], coppers, sizeof(int) * handCount); // set all discarded cards to copper
                memcpy(G.deck[q], silvers, sizeof(int) * (handCount+5)); // set all deck cards of next player to silver
                G.whoseTurn = p; // Set current player

                r = endTurn(&G); // Call function under test

#if (NOISY_TEST == 1)
                printf("endTurn return value = %d, expected = %d ", r, 0);
#endif
                asserttrue(r == 0, &globalFailFlag, 1); // check if endTurn return value is correct
#if (NOISY_TEST == 1)
                printf("G.whoseTurn = %d, expected = %d ", G.whoseTurn, q);
#endif
                asserttrue(G.whoseTurn == q, &globalFailFlag, 1); // check if the next player to execute a turn is correct
#if (NOISY_TEST == 1)
                printf("G.handCount[p] = %d, expected = %d ", G.handCount[p], 0); 
#endif
                asserttrue(G.handCount[p] == 0, &globalFailFlag, 1); // check if the current player's hand is empty
                for (r = 0; r < numPlayer; r++) {
#if (NOISY_TEST == 1)
                    if (r != q)
                        printf("G.handCount[%d] = %d, expected = %d ", r, G.handCount[r], 0);
#endif
                    if (r != q)
                        asserttrue(G.handCount[r] == 0, &globalFailFlag, 1); // check if all other players' hands are empty to check that their hand states are unchanged by the endTurn function
#if (NOISY_TEST == 1)
                    if (r != p)
                        printf("G.discardCount[%d] = %d, expected = %d ", r, G.discardCount[r], MAX_DECK);
#endif
                    if (r != p)
                        asserttrue(G.discardCount[r] == 0, &globalFailFlag, 1); // check if all other players' discard piles are empty to check that their discard states are unchanged by the endTurn function
#if (NOISY_TEST == 1)
                    if (r != q)
                        printf("G.deckCount[%d] = %d, expected = %d ", r, G.deckCount[r], 0);
#endif
                    if (r != q)
                        asserttrue(G.deckCount[r] == 0, &globalFailFlag, 1); // check if all other players' discard piles are max size to check that their discard states are unchanged by the endTurn function
                }
#if (NOISY_TEST == 1)
                printf("G.discardCount[p] = %d, expected = %d ", G.discardCount[p], 2*handCount);
#endif
                asserttrue(G.discardCount[p] == handCount + handCount, &globalFailFlag, 1); // check if the current player's discard pile size is increased by the size of the player's hand
                for (r = 0; r < 2*handCount; r++) {
#if (NOISY_TEST == 1)
                    printf("G.discard[p][%d] = %d, expected = %d ", r, G.discard[p][r], copper);
#endif
                    asserttrue(G.discard[p][r] == copper, &globalFailFlag, 1); // check if the player's discard pile only has coppers since the player's hand only contained coppers and the discard pile initially only has coppers.  In other words, make sure that only the player's hand cards were discarded.
                }
#if (NOISY_TEST == 1)
                printf("G.deckCount[p] = %d, expected = %d ", G.deckCount[q], handCount);
#endif
                asserttrue(G.deckCount[q] == handCount, &globalFailFlag, 1); // check if the next player's deck is reduced by five.  In other words, the next player drew a hand of five cards from their deck
#if (NOISY_TEST == 1)
                printf("G.handCount[q] = %d, expected = %d ", G.handCount[q], 5);
#endif
                asserttrue(G.handCount[q] == 5, &globalFailFlag, 1); // check if the next player's number of hand cards is correct
                for (r = 0; r < 5; r++) {
#if (NOISY_TEST == 1)
                    printf("G.hand[q][%d] = %d, expected = %d ", r, G.hand[q][r], silver);
#endif
                    asserttrue(G.hand[q][r] == silver, &globalFailFlag, 1); // check if the next player's hand has only silvers since the next player's deck has only silvers.  In other words, make sure that new player's hand cards were drawn from the new player's deck.
                }
#if (NOISY_TEST == 1)
                printf("G.outpostPlayed = %d, expected = %d ", G.outpostPlayed, 0);
#endif
                asserttrue(G.outpostPlayed == 0, &globalFailFlag, 1); // check if outpostPlayed reset to zero
#if (NOISY_TEST == 1)
                printf("G.phase = %d, expected = %d ", G.phase, 0);
#endif
                asserttrue(G.phase == 0, &globalFailFlag, 1); // check if phase reset to zero
#if (NOISY_TEST == 1)
                printf("G.numActions = %d, expected = %d ", G.numActions, 1);
#endif
                asserttrue(G.numActions == 1, &globalFailFlag, 1); // check if numActions reset to one
#if (NOISY_TEST == 1)
                printf("G.numBuys = %d, expected = %d ", G.numBuys, 1);
#endif
                asserttrue(G.numBuys == 1, &globalFailFlag, 1); // check if numBuys reset to one
#if (NOISY_TEST == 1)
                printf("G.playedCardCount = %d, expected = %d ", G.playedCardCount, 0);
#endif
                asserttrue(G.playedCardCount == 0, &globalFailFlag, 1); // check playedCardCount reset to zero
#if (NOISY_TEST == 1)
                printf("G.coins = %d, expected = %d ", G.coins, 10);
#endif
                asserttrue(G.coins == 10, &globalFailFlag, 1); // check if the number of coins is correct based on the next player's hand of five silver cards
        }
    }
    printf("******* endTurn function UNIT TEST #1 EXECUTION IS COMPLETE *******\n");
    printf("RESULT: ");
    if (globalFailFlag == 0)
        printf("ALL TESTS PASSED\n");
    else
        printf("AT LEAST ONE TEST FAILED\n");

    return 0;
}

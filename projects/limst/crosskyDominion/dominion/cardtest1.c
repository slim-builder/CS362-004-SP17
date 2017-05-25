/* -----------------------------------------------------------------------
 * Unit test for playSmithy function in dominion.c
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
    int goldCardCount, silverCardCount;
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
    printf ("TESTING playSmithy():\n");

    for (p = 0; p < numPlayer; p++) // test for each player
    {
        for (deckCount = 0; deckCount <= 3; deckCount++) // test for different deck counts
        {
            for (discardCount = 0; discardCount <= 3; discardCount++) // test for different discard counts
            {
                for (handPos = 0; handPos < handCount; handPos++) // test smithy in each hand position
                {
#if (NOISY_TEST == 1)
                printf("Test player %d playing a Smithy card.\n", p);
#endif
                memset(&G, 23, sizeof(struct gameState));   // clear the game state
                r = initializeGame(numPlayer, k, seed, &G); // initialize a new game
                for (s = 0; s < numPlayer; s++) { // empty all hands, decks, and discard piles to check that the other player's states will not be changed
                    G.handCount[s] = 0;
                    G.discardCount[s] = 0;
                    G.deckCount[s] = 0;
                }
                for (s = 0; s <= treasure_map; s++) // empty supply piles
                    G.supplyCount[s] = 0;

                G.handCount[p] = handCount; // initialize player's hand, deck, and discard
                memcpy(G.hand[p], coppers, sizeof(int) * handCount);
                G.hand[p][handPos] = smithy;
                G.discardCount[p] = discardCount;
                memcpy(G.discard[p], silvers, sizeof(int) * discardCount);
                G.deckCount[p] = deckCount;
                memcpy(G.deck[p], golds, sizeof(int) * deckCount);

                G.playedCardCount = 0; // set played card count to zero
                G.playedCards[0] = copper; // temp placeholder

                r = playSmithy(&G, handPos, p); // Call function under test
#if (NOISY_TEST == 1)
                printf("playSmithy return value = %d, expected = %d ", r, 0);
#endif
                asserttrue(r == 0, &globalFailFlag, 1); // check if playSmithy return value is correct
                for (s = 0; s < G.handCount[p]; s++) {
#if (NOISY_TEST == 1)
                    printf("G.hand[p][s] = %d, expected != %d ", G.hand[p][s], smithy); 
#endif
                    asserttrue(G.hand[p][s] != smithy, &globalFailFlag, 1); // check that smithy card is removed from hand
                }
                if (deckCount >= 3) {
                    goldCardCount = 0;
                    for (s = 0; s < handCount+2; s++) {
                        if (G.hand[p][s] == gold)
                            goldCardCount++;
                    }
#if (NOISY_TEST == 1)
                    printf("goldCardCount = %d, expected = %d ", goldCardCount, 3); 
#endif
                    asserttrue(goldCardCount == 3, &globalFailFlag, 1); // since deck is full of gold cards, there should be three gold cards in the player's hand after smithy is played
#if (NOISY_TEST == 1)
                    printf("G.handCount[p] = %d, expected = %d ", G.handCount[p], handCount-1+3);
#endif
                    asserttrue(G.handCount[p] == handCount-1+3, &globalFailFlag, 1); // check if hand count is correct
#if (NOISY_TEST == 1)
                    printf("G.deckCount[p] = %d, expected = %d ", G.deckCount[p], deckCount-3);
#endif
                    asserttrue(G.deckCount[p] == deckCount-3, &globalFailFlag, 1); // check if deck count is correct
#if (NOISY_TEST == 1)
                    printf("G.discardCount[p] = %d, expected = %d ", G.discardCount[p], discardCount);
#endif
                    asserttrue(G.discardCount[p] == discardCount, &globalFailFlag, 1); // check if discard count is correct
                    for (s = 0; s < deckCount-3; s++) {
#if (NOISY_TEST == 1)
                        printf("G.deck[p][s] = %d, expected = %d ", G.deck[p][s], gold);
#endif
                        asserttrue(G.deck[p][s] == gold, &globalFailFlag, 1); // check if deck is unchanged after playing smithy
                    }
                    for (s = 0; s < discardCount; s++) {
#if (NOISY_TEST == 1)
                        printf("G.discard[p][s] = %d, expected = %d ", G.discard[p][s], silver);
#endif
                        asserttrue(G.discard[p][s] == silver, &globalFailFlag, 1); // check if discard is unchanged after playing smithy
                    }
                }
                else if (deckCount + discardCount >= 3) { // expected numbers of gold and silver cards in hand depending on size of deck and discard
                    goldCardCount = 0;
                    silverCardCount = 0;
                    for (s = 0; s < handCount+2; s++) {
                        if (G.hand[p][s] == gold)
                            goldCardCount++;
                        if (G.hand[p][s] == silver)
                            silverCardCount++;
                    }
#if (NOISY_TEST == 1)
                    printf("goldCardCount = %d, expected = %d ", goldCardCount, deckCount);
#endif
                    asserttrue(goldCardCount == deckCount, &globalFailFlag, 1); // check if number of gold cards from deck into player's hand is correct
#if (NOISY_TEST == 1)
                    printf("silverCardCount = %d, expected = %d ", silverCardCount, 3 - deckCount);
#endif
                    asserttrue(silverCardCount == 3 - deckCount, &globalFailFlag, 1); // check if number of silver cards from deck into player's hand is correct
#if (NOISY_TEST == 1)
                    printf("G.handCount[p] = %d, expected = %d ", G.handCount[p], handCount-1+3);
#endif
                    asserttrue(G.handCount[p] == handCount-1+3, &globalFailFlag, 1); // check if hand count is correct
#if (NOISY_TEST == 1)
                    printf("G.deckCount[p] = %d, expected = %d ", G.deckCount[p], deckCount+discardCount-3);
#endif
                    asserttrue(G.deckCount[p] == deckCount+discardCount-3, &globalFailFlag, 1); // check if deck count is correct, in this case, the sum of both the deck count and discard count minus three
#if (NOISY_TEST == 1)
                    printf("G.discardCount[p] = %d, expected = %d ", G.discardCount[p], 0);
#endif
                    asserttrue(G.discardCount[p] == 0, &globalFailFlag, 1); // check if discard count is correct
                    for (s = 0; s < deckCount+discardCount-3; s++) {
#if (NOISY_TEST == 1)
                        printf("G.deck[p][s] = %d, expected = %d ", G.deck[p][s], silver);
#endif
                        asserttrue(G.deck[p][s] == silver, &globalFailFlag, 1); // check if discard is unchanged after playing smithy
                    }
                }
                else {
#if (NOISY_TEST == 1)
                    printf("G.handCount[p] = %d, expected = %d ", G.handCount[p], handCount-1+deckCount+discardCount);
#endif
                    asserttrue(G.handCount[p] == handCount-1+deckCount+discardCount, &globalFailFlag, 1); // check if hand count is correct, in this case, the number of drawn cards is less than three since the sum of deck count and discard count is less than three
                }
                for (r = 0; r <= treasure_map; r++) {
#if (NOISY_TEST == 1)
                    printf("G.supplyCount[r] = %d, expected = %d ", G.supplyCount[r], 0);
#endif
                    asserttrue(G.supplyCount[r] == 0, &globalFailFlag, 1); // check if supply count is correct
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
                        asserttrue(G.discardCount[r] == 0, &globalFailFlag, 1); // check if all other players' decks are empty
                }
#if (NOISY_TEST == 1)
                        printf("G.playedCardCount = %d, expected = %d ", G.playedCardCount, 1);
#endif
                        asserttrue(G.playedCardCount == 1, &globalFailFlag, 1); // check if played card count is one
#if (NOISY_TEST == 1)
                        printf("G.playedCards[0] = %d, expected = %d ", G.playedCards[0], smithy);
#endif
                        asserttrue(G.playedCards[0] == smithy, &globalFailFlag, 1); // check if played card array contains smithy
                }
            }
        }
    }
    printf("******* playSmithy function CARD TEST #1 EXECUTION IS COMPLETE *******\n");
    printf("RESULT: ");
    if (globalFailFlag == 0)
        printf("ALL TESTS PASSED\n");
    else
        printf("AT LEAST ONE TEST FAILED\n");

    return 0;
}

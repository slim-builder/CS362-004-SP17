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
    int disTreasCount, deckTreasCount, disTreas, deckTreas, treasNum;
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

    for (p = 0; p < numPlayer; p++) // test for each player
    {
        for (deckCount = 0; deckCount <= 10; deckCount++) // test for different deck counts
        {
            for (discardCount = 0; discardCount <= 10; discardCount++) // test for different discard counts
            {
                for (handPos = 0; handPos < handCount; handPos++) // test adventurer in each hand position
                {
                  for (disTreas = copper; disTreas <= gold; disTreas++) // test for each treasure type in discard pile 
                  {
                    for (deckTreas = copper; deckTreas <= gold; deckTreas++) // test for each treasure type in deck pile
                    {
                    if (deckCount + discardCount == 0) // set number of treasures to be found depending on size of deck and discard
                        treasNum = 0;
                    else if (deckCount + discardCount == 1)
                        treasNum = 1;
                    else
                        treasNum = 2;
                    if (disTreas == deckTreas)
                        continue;
#if (NOISY_TEST == 1)
                printf("Test player %d playing a Adventurer card with deck = %d and discard = %d.\n", p, deckCount, discardCount);
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
                memcpy(G.hand[p], estates, sizeof(int) * handCount);
                G.hand[p][handPos] = adventurer;
                G.discardCount[p] = discardCount;
                memcpy(G.discard[p], duchies, sizeof(int) * discardCount);
                G.deckCount[p] = deckCount;
                memcpy(G.deck[p], provinces, sizeof(int) * deckCount);
                if (discardCount > 0 && deckCount > 0) { // initialize placement of the two or less treasures depending on size of deck and discard
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

                G.playedCardCount = 0; // set played card count to zero
                G.playedCards[0] = copper; // temp placeholder
 
                r = playAdventurer(handPos, p, &G, drawntreasure, temphand, z, cardDrawn); // Call function under test
#if (NOISY_TEST == 1)
                printf("playAdventurer return value = %d, expected = %d ", r, 0);
#endif
                asserttrue(r == 0, &globalFailFlag, 1); // check if playAdventurer return value is correct
                disTreasCount = 0;
                deckTreasCount = 0; // count number of two different treasure types in the player's hand
                for (s = 0; s < G.handCount[p]; s++) {
                    if (G.hand[p][s] == disTreas)
                        disTreasCount++;
                    if (G.hand[p][s] == deckTreas)
                        deckTreasCount++;
#if (NOISY_TEST == 1)
                    printf("G.hand[p][s] = %d, expected != %d ", G.hand[p][s], adventurer);
#endif
                    asserttrue(G.hand[p][s] != adventurer, &globalFailFlag, 1); // check if adventurer card is removed from hand
                }
#if (NOISY_TEST == 1)
                printf("disTreasCount + deckTreasCount = %d, expected = %d ", disTreasCount + deckTreasCount, treasNum);
#endif
                asserttrue(disTreasCount + deckTreasCount == treasNum, &globalFailFlag, 1); // check if total treasure count is correct
#if (NOISY_TEST == 1)
                printf("G.handCount[p] = %d, expected = %d ", G.handCount[p], handCount-1+treasNum);
#endif
                asserttrue(G.handCount[p] == handCount-1+treasNum, &globalFailFlag, 1); // check if hand count is correct
                if (discardCount == 0 && deckCount >= 2) { // multiple cases to test for correct discard count
#if (NOISY_TEST == 1)
                    printf("G.discardCount[p] = %d, expected = %d ", G.discardCount[p], deckCount-2); 
#endif
                    asserttrue(G.discardCount[p] == deckCount-2, &globalFailFlag, 1); // check if discard count is correct
                }
                else if (discardCount == 1 && deckCount >= 1) {
#if (NOISY_TEST == 1)
                    printf("G.discardCount[p] = %d, expected = %d ", G.discardCount[p], deckCount-1);
#endif
                    asserttrue(G.discardCount[p] == deckCount-1, &globalFailFlag, 1); // check if discard count is correct
                }
                else if (discardCount == 2 && deckCount == 0) {
#if (NOISY_TEST == 1)
                    printf("G.discardCount[p] = %d, expected = %d ", G.discardCount[p], 0);
#endif
                    asserttrue(G.discardCount[p] == 0, &globalFailFlag, 1); // check if discard count is correct
                }
                else if (discardCount == 0 && deckCount == 1) {
#if (NOISY_TEST == 1)
                    printf("G.discardCount[p] = %d, expected = %d ", G.discardCount[p], 0);
#endif
                    asserttrue(G.discardCount[p] == 0, &globalFailFlag, 1); // check if discard count is correct
                }
                else if (discardCount == 1 && deckCount == 0) {
#if (NOISY_TEST == 1)
                    printf("G.discardCount[p] = %d, expected = %d ", G.discardCount[p], 0);
#endif
                    asserttrue(G.discardCount[p] == 0, &globalFailFlag, 1); // check if discard count is correct
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
                        asserttrue(G.discardCount[r] == 0, &globalFailFlag, 1); // check if all other players' discards are empty
                }
#if (NOISY_TEST == 1)
                        printf("G.playedCardCount = %d, expected = %d ", G.playedCardCount, 1);
#endif
                        asserttrue(G.playedCardCount == 1, &globalFailFlag, 1); // check if played card count is correct
#if (NOISY_TEST == 1)
                        printf("G.playedCards[0] = %d, expected = %d ", G.playedCards[0], adventurer);
#endif
                        asserttrue(G.playedCards[0] == adventurer, &globalFailFlag, 1); // check if played card array contains adventurer
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

/* -----------------------------------------------------------------------
 * Unit test for playCouncilRoom function in dominion.c
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
    printf ("TESTING playCouncilRoom():\n");

    for (p = 0; p < numPlayer; p++) // test for each player
    {
        for (deckCount = 0; deckCount <= 4; deckCount++) // test for different deck counts
        {
            for (discardCount = 0; discardCount <= 4; discardCount++) // test for different discard counts
            {
                for (handPos = 0; handPos < handCount; handPos++) // test council_room at each hand position
                {
#if (NOISY_TEST == 1)
                printf("Test player %d playing a Council Room card with deck = %d and discard = %d.\n", p, deckCount, discardCount);
#endif
                memset(&G, 23, sizeof(struct gameState));   // clear the game state
                r = initializeGame(numPlayer, k, seed, &G); // initialize a new game
                for (s = 0; s < numPlayer; s++) { // empty all hands, decks, and discard piles to check that the other player's states will not be changed
                    G.handCount[s] = 0;
                    G.discardCount[s] = 0;
                    G.deckCount[s] = 1; // needed to test that each other player draws a card
                    G.deck[s][0] = estate; // used to check for card type after council_room is played
                }
                for (s = 0; s <= treasure_map; s++) // empty card supplies
                    G.supplyCount[s] = 0;
                G.handCount[p] = handCount; // initialize player's hand, deck, and discard
                memcpy(G.hand[p], coppers, sizeof(int) * handCount);
                G.hand[p][handPos] = council_room;
                G.discardCount[p] = discardCount;
                memcpy(G.discard[p], silvers, sizeof(int) * discardCount);
                G.deckCount[p] = deckCount;
                memcpy(G.deck[p], golds, sizeof(int) * deckCount);

                G.playedCardCount = 0; // initialize played card count to zero
                G.playedCards[0] = copper; // temp placeholder

                G.numBuys = 1; // set number of buys to one

                r = playCouncil_Room(&G, handPos, p); // Call function under test
#if (NOISY_TEST == 1)
                printf("playCouncilRoom return value = %d, expected = %d ", r, 0);
#endif
                asserttrue(r == 0, &globalFailFlag, 1); // check if playCouncilRoom return value is correct
#if (NOISY_TEST == 1)
                printf("G.numBuys = %d, expected = %d ", G.numBuys, 2);
#endif
                asserttrue(G.numBuys == 2, &globalFailFlag, 1); // check if numBuys value is correct
                for (s = 0; s < G.handCount[p]; s++) { // check if council_room is removed from player's hand
#if (NOISY_TEST == 1)
                    printf("G.hand[p][s] = %d, expected != %d ", G.hand[p][s], council_room);
#endif
                    asserttrue(G.hand[p][s] != council_room, &globalFailFlag, 1);
                }
                if (deckCount >= 4) { // different tests depending on the size of the deck and discard
                    goldCardCount = 0; // count number of gold cards in player's hand since deck is full of gold cards to check drawing of cards to hand
                    for (s = 0; s < handCount+3; s++) {
                        if (G.hand[p][s] == gold)
                            goldCardCount++;
                    }
#if (NOISY_TEST == 1)
                    printf("goldCardCount = %d, expected = %d ", goldCardCount, 4);
#endif
                    asserttrue(goldCardCount == 4, &globalFailFlag, 1); // check if the player's hand has four gold cards
#if (NOISY_TEST == 1)
                    printf("G.handCount[p] = %d, expected = %d ", G.handCount[p], handCount-1+4);
#endif
                    asserttrue(G.handCount[p] == handCount-1+4, &globalFailFlag, 1); // check if hand count is correct
#if (NOISY_TEST == 1)
                    printf("G.deckCount[p] = %d, expected = %d ", G.deckCount[p], deckCount-4);
#endif
                    asserttrue(G.deckCount[p] == deckCount-4, &globalFailFlag, 1); // check if deck count is correct
#if (NOISY_TEST == 1)
                    printf("G.discardCount[p] = %d, expected = %d ", G.discardCount[p], discardCount);
#endif
                    asserttrue(G.discardCount[p] == discardCount, &globalFailFlag, 1); // check if discard count is correct
                    for (s = 0; s < deckCount-4; s++) {
#if (NOISY_TEST == 1)
                        printf("G.deck[p][s] = %d, expected = %d ", G.deck[p][s], gold);
#endif
                        asserttrue(G.deck[p][s] == gold, &globalFailFlag, 1); // check if deck is unchanged
                    }
                    for (s = 0; s < discardCount; s++) {
#if (NOISY_TEST == 1)
                        printf("G.discard[p][s] = %d, expected = %d ", G.discard[p][s], silver);
#endif
                        asserttrue(G.discard[p][s] == silver, &globalFailFlag, 1); // check if discard is unchanged
                    }
                }
                else if (deckCount + discardCount >= 4) { // test when shuffled discard pile becomes new deck when original deck is emptied
                    goldCardCount = 0;
                    silverCardCount = 0; // count number of gold and silver cards in player's hand to check if four cards are correctly drawn
                    for (s = 0; s < handCount+3; s++) {
                        if (G.hand[p][s] == gold)
                            goldCardCount++;
                        if (G.hand[p][s] == silver)
                            silverCardCount++;
                    }
#if (NOISY_TEST == 1)
                    printf("goldCardCount = %d, expected = %d ", goldCardCount, deckCount);
#endif
                    asserttrue(goldCardCount == deckCount, &globalFailFlag, 1); // check if the gold card count is correct (entire deck since deck count was less than four)
#if (NOISY_TEST == 1)
                    printf("silverCardCount = %d, expected = %d ", silverCardCount, 4 - deckCount);
#endif
                    asserttrue(silverCardCount == 4 - deckCount, &globalFailFlag, 1); // check if the silver card count is correct (four minus deck count since only draw from new deck or old discard pile when original deck is emptied)
#if (NOISY_TEST == 1)
                    printf("G.handCount[p] = %d, expected = %d ", G.handCount[p], handCount-1+4);
#endif
                    asserttrue(G.handCount[p] == handCount-1+4, &globalFailFlag, 1); // check if hand count is correct
#if (NOISY_TEST == 1)
                    printf("G.deckCount[p] = %d, expected = %d ", G.deckCount[p], deckCount+discardCount-4);
#endif
                    asserttrue(G.deckCount[p] == deckCount+discardCount-4, &globalFailFlag, 1); // check if deck count is correct
#if (NOISY_TEST == 1)
                    printf("G.discardCount[p] = %d, expected = %d ", G.discardCount[p], 0);
#endif
                    asserttrue(G.discardCount[p] == 0, &globalFailFlag, 1); // check if discard is empty in this case
                    for (s = 0; s < deckCount+discardCount-4; s++) {
#if (NOISY_TEST == 1)
                        printf("G.deck[p][s] = %d, expected = %d ", G.deck[p][s], silver);
#endif
                        asserttrue(G.deck[p][s] == silver, &globalFailFlag, 1); // check if deck was the previously the discard pile due to emptying of original deck
                    }
                }
                else { // test when less than four cards are available to be drawn
#if (NOISY_TEST == 1)
                    printf("G.handCount[p] = %d, expected = %d ", G.handCount[p], handCount-1+deckCount+discardCount);
#endif
                    asserttrue(G.handCount[p] == handCount-1+deckCount+discardCount, &globalFailFlag, 1); // check if hand count is correct
                }
                for (r = 0; r <= treasure_map; r++) {
#if (NOISY_TEST == 1)
                    printf("G.supplyCount[r] = %d, expected = %d ", G.supplyCount[r], 0);
#endif
                    asserttrue(G.supplyCount[r] == 0, &globalFailFlag, 1); // check if supply count is unchanged
                }
                for (r = 0; r < numPlayer; r++) {
#if (NOISY_TEST == 1)
                    if (r != p)
                        printf("G.handCount[r] = %d, expected = %d ", G.handCount[r], 1);
#endif
                    if (r != p)
                        asserttrue(G.handCount[r] == 1, &globalFailFlag, 1); // check if all other players' hands have one card
#if (NOISY_TEST == 1)
                    if (r != p)
                        printf("G.hand[r][0] = %d, expected = %d ", G.hand[r][0], estate);
#endif
                    if (r != p)
                        asserttrue(G.hand[r][0] == estate, &globalFailFlag, 1); // check if all other players' hands contain the estate card which must have come from their respective decks
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
                        asserttrue(G.playedCardCount == 1, &globalFailFlag, 1); // check if played card count is one
#if (NOISY_TEST == 1)
                        printf("G.playedCards[0] = %d, expected = %d ", G.playedCards[0], council_room);
#endif
                        asserttrue(G.playedCards[0] == council_room, &globalFailFlag, 1); // check if played card array contains council_room
                }
            }
        }
    }
    printf("******* playCouncilRoom function CARD TEST #3 EXECUTION IS COMPLETE *******\n");
    printf("RESULT: ");
    if (globalFailFlag == 0)
        printf("ALL TESTS PASSED\n");
    else
        printf("AT LEAST ONE TEST FAILED\n");

    return 0;
}

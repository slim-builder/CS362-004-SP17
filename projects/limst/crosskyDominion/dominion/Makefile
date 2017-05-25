CFLAGS = -Wall -fpic -coverage -lm

rngs.o: rngs.h rngs.c
	gcc -c rngs.c -g  $(CFLAGS)

dominion.o: dominion.h dominion.c rngs.o
	gcc -c dominion.c -g  $(CFLAGS)

playdom: dominion.o playdom.c
	gcc -o playdom playdom.c -g dominion.o rngs.o $(CFLAGS)

testDrawCard: testDrawCard.c dominion.o rngs.o
	gcc -o testDrawCard -g  testDrawCard.c dominion.o rngs.o $(CFLAGS)

badTestDrawCard: badTestDrawCard.c dominion.o rngs.o
	gcc -o badTestDrawCard -g  badTestDrawCard.c dominion.o rngs.o $(CFLAGS)

testBuyCard: testDrawCard.c dominion.o rngs.o
	gcc -o testDrawCard -g  testDrawCard.c dominion.o rngs.o $(CFLAGS)

testAll: dominion.o testSuite.c
	gcc -o testSuite testSuite.c -g  dominion.o rngs.o $(CFLAGS)

interface.o: interface.h interface.c
	gcc -c interface.c -g  $(CFLAGS)

runtests: testDrawCard 
	./testDrawCard &> unittestresult.out
	gcov dominion.c >> unittestresult.out
	cat dominion.c.gcov >> unittestresult.out

cardtest1.out: cardtest1.c dominion.c rngs.c
	echo “Results of running cardtest1.out:” > cardtest1.out
	gcc -o cardtest1 cardtest1.c dominion.c rngs.c $(CFLAGS)
	cardtest1 >> cardtest1.out
	gcov -b -f dominion.c >> cardtest1.out

cardtest2.out: cardtest2.c dominion.c rngs.c
	echo “Results of running cardtest2.out:” > cardtest2.out
	gcc -o cardtest2 cardtest2.c dominion.c rngs.c $(CFLAGS)
	cardtest2 >> cardtest2.out
	gcov -b -f dominion.c >> cardtest2.out

unittestresults.out: unittest1.c unittest2.c unittest3.c unittest4.c dominion.o rngs.o 
	echo "Result of running Dominion unit tests:" > unittestresults.out
	
	echo -e "\nunittest1.c:" >> unittestresults.out
	gcc -o unittest1 -g unittest1.c dominion.c rngs.c $(CFLAGS)
	unittest1 >> unittestresults.out
	gcov -b -f dominion.c >> unittestresults.out
	
	echo -e "\nunittest2.c:" >> unittestresults.out
	gcc -o unittest2 dominion.c rngs.c unittest2.c $(CFLAGS)
	unittest2 >> unittestresults.out
	gcov -b -f dominion.c >> unittestresults.out
	
	echo -e "\nunittest3.c:" >> unittestresults.out
	gcc -o unittest3 dominion.c rngs.c unittest3.c $(CFLAGS)
	unittest3 >> unittestresults.out
	gcov -b -f dominion.c >> unittestresults.out
	
	echo -e "\nunittest4.c:" >> unittestresults.out
	gcc -o unittest4 dominion.c rngs.c unittest4.c $(CFLAGS)
	unittest4 >> unittestresults.out
	gcov -b -f dominion.c >> unittestresults.out
	
	echo -e "\ncardtest1.c:" >> unittestresults.out
	gcc -o cardtest1 -g cardtest1.c dominion.c rngs.c $(CFLAGS)
	cardtest1 >> unittestresults.out
	gcov -b -f dominion.c >> unittestresults.out
	
	echo -e "\ncardtest2.c:" >> unittestresults.out   
	gcc -o cardtest2 -g cardtest2.c dominion.c rngs.c $(CFLAGS)
	cardtest2 >> unittestresults.out
	gcov -b -f dominion.c >> unittestresults.out
	
	echo -e "\ncardtest3.c:" >> unittestresults.out
	gcc -o cardtest3 -g cardtest3.c dominion.c rngs.c $(CFLAGS)
	cardtest3 >> unittestresults.out
	gcov -b -f dominion.c >> unittestresults.out
	
	echo -e "\ncardtest4.c:" >> unittestresults.out
	gcc -o cardtest4 -g cardtest4.c dominion.c rngs.c $(CFLAGS)
	cardtest4 >> unittestresults.out
	gcov -b -f dominion.c >> unittestresults.out

randomtestcard1.out: randomtestcard1.c dominion.c rngs.c
	echo “Results of running randomtestcard1.out:” > randomtestcard1.out
	gcc -o randomtestcard1 randomtestcard1.c dominion.c rngs.c $(CFLAGS)
	randomtestcard1 >> randomtestcard1.out
	gcov -b -f dominion.c >> randomtestcard1.out

randomtestcard2.out: randomtestcard2.c dominion.c rngs.c
	echo “Results of running randomtestcard2.out:” > randomtestcard2.out
	gcc -o randomtestcard2 randomtestcard2.c dominion.c rngs.c $(CFLAGS)
	randomtestcard2 >> randomtestcard2.out
	gcov -b -f dominion.c >> randomtestcard2.out
	
randomtestadventurer.out: randomtestadventurer.c dominion.c rngs.c
	echo “Results of running randomtestadventurer.out:” > randomtestadventurer.out
	gcc -o randomtestadventurer randomtestadventurer.c dominion.c rngs.c $(CFLAGS)
	randomtestadventurer >> randomtestadventurer.out
	gcov -b -f dominion.c >> randomtestadventurer.out 
	

player: player.c interface.o
	gcc -o player player.c -g  dominion.o rngs.o interface.o $(CFLAGS)

unittest: dominion.o rngs.o interface.o
	gcc -o unittest1 unittest1.c -g dominion.o rngs.o interface.o $(CFLAGS)
	gcc -o unittest2 unittest2.c -g dominion.o rngs.o interface.o $(CFLAGS)
	gcc -o unittest3 unittest3.c -g dominion.o rngs.o interface.o $(CFLAGS)
	gcc -o unittest4 unittest4.c -g dominion.o rngs.o interface.o $(CFLAGS)
	gcc -o cardtest1 cardtest1.c -g dominion.o rngs.o interface.o $(CFLAGS)
	gcc -o cardtest2 cardtest2.c -g dominion.o rngs.o interface.o $(CFLAGS)
	gcc -o cardtest3 cardtest3.c -g dominion.o rngs.o interface.o $(CFLAGS)
	gcc -o cardtest4 cardtest4.c -g dominion.o rngs.o interface.o $(CFLAGS)

randomtest: randomtestadventurer.c dominion.o rngs.o
	gcc -o randomtestadventurer -g randomtestadventurer.c dominion.o rngs.o $(CFLAGS)
	gcc -o randomtestcard1 -g randomtestcard1.c dominion.o rngs.o $(CFLAGS)
	gcc -o randomtestcard2 -g randomtestcard2.c dominion.o rngs.o $(CFLAGS)

all: randomtestcard1.out randomtestcard2.out randomtestadventurer.out

clean:
	rm -f *.o playdom player cardtest1 cardtest2 cardtest3 cardtest4 unittest1 unittest2 unittest3 unittest4 randomtestcard1 randomtestcard2 randomtestadventurer *.gcov *.gcda *.gcno *.so *.out

/* CS 362 Spring 2017
 * Steven Lim
 * 4/30/2017
 * Random Testing Quiz 2
 
I developed my random tester by first looking at the nature
of the conditions required to print the error message and
end execution of the testme program.  My initial strategy for
building a random tester was to choose a random input character
and a random input string of length five where each input character
would have an ASCII value in the range of 0 to 127, inclusive.

After running the resulting random test program, I found that the
program took too long to reach the condition for printing the error
message.

I narrowed down the range of ASCII values so that the random input
character was in the range of 32 to 126, inclusive.  This is because
I noticed that all of the conditions needed for transitioning from
state 0 to state 9 involved printable characters and the range of
ASCII values of printable characters is 32 to 126, inclusive.

I also narrowed down the range of ASCII values so that all the characters
of the random input string ranged from the lowercase letter
'e' to the lowercase letter 't'.  The corresponding ASCII values are
101 to 116, inclusive.  The reason is because the condition for printing
the error message required the string 'reset' and every character in the
string 'reset' has an ASCII value in the range of 101 to 116, inclusive.

The resuling random test program usually finds the condition to print
the error message within a few minutes.

*/

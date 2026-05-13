# Nicoleta Zmeu, 33531 - Operating Systems Project 1
# CSC1021/simpleshell

# use files form src/ and compile to bin/

simpleshell: src/simpleshell.c src/utility.c 
	gcc -Wall src/simpleshell.c src/utility.c -o bin/simpleshell



#I confirm that this work is entirely my own and that I have read and understood the DCU Academic Integrity Policy. 

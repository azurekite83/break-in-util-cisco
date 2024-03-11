compile: main.c
	gcc -g -Wall main.c -l:libserialport.a -o main

game: game.o cardtype.o getcard.o hold_inquire.o flop_inquire.o turn_inquire.o river_inquire.o  
	g++ -o game game.o cardtype.o getcard.o hold_inquire.o flop_inquire.o turn_inquire.o river_inquire.o
clean:
	rm -f game game.o cardtype.o getcard.ohold_inquire.o flop_inquire.o turn_inquire.o river_inquire.o

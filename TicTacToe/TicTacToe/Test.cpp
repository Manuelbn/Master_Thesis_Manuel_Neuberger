#include "stdafx.h"

#include "Test.h"

 bool Test::symmetrieHashTestOnSquare()
{
	/*bool testValue = true;
	board b =  board(4,4,4);
	b.isSqaure = true;
	b.setMark(0, 0,tile::Mark::X);
	b.setMark(0, 1, tile::Mark::X);
	b.setMark(1, 2, tile::Mark::X);
	b.printBoard();
	board c = board(4, 4, 4);
	c.isSqaure = true;
	c.setMark(0, 0, tile::Mark::X);
	c.setMark(1, 0, tile::Mark::X);
	c.setMark(2, 1, tile::Mark::X);
	c.printBoard();
	board d = board(4, 4, 4);
	d.isSqaure = true;
	d.setMark(0, 3, tile::Mark::X);
	d.setMark(1, 3, tile::Mark::X);
	d.setMark(2, 2, tile::Mark::X);
	d.printBoard();
	board e = board(4, 4, 4);
	e.isSqaure=true;
	e.setMark(3, 3, tile::Mark::X);
	e.setMark(3, 2, tile::Mark::X);
	e.setMark(2, 1, tile::Mark::X);
	e.printBoard();
	cout << b.computeBoardKey() << endl;
	cout << c.computeBoardKey() << endl;
	cout << d.computeBoardKey() << endl;
	cout << e.computeBoardKey() << endl;

	return (b.computeBoardKey()==c.computeBoardKey()&&b.computeBoardKey()==d.computeBoardKey()&&b.computeBoardKey()==d.computeBoardKey());*/
	 return true;
}

 bool Test::hashMethodTest()
 {
	 /*bool testValue = true;
	 board b = board(5, 5, 5);
	 b.isSqaure = true;
	 b.setMark(0, 0, tile::Mark::X);
	 b.setMark(0, 1, tile::Mark::X);
	 b.printBoard();
	 board c = board(5, 5, 5);
	 c.isSqaure = true;
	 c.setMark(0, 0, tile::Mark::X);
	 c.setMark(1, 0, tile::Mark::X);
	 c.printBoard();
	 board d = board(5, 5, 5);
	 d.isSqaure = true;
	 d.setMark(0, 4, tile::Mark::X);
	 d.setMark(1, 4, tile::Mark::X);
	 d.printBoard();
	 board e = board(5, 5, 5);
	 e.isSqaure = true;
	 e.setMark(4, 4, tile::Mark::X);
	 e.setMark(4, 3, tile::Mark::X);
	 e.printBoard();
	 cout << b.computeBoardKey() << endl;
	 cout << c.computeBoardKey() << endl;
	 cout << d.computeBoardKey() << endl;
	 cout << e.computeBoardKey() << endl;

	 return (b.computeBoardKey() == c.computeBoardKey() && b.computeBoardKey() == d.computeBoardKey() && b.computeBoardKey() == d.computeBoardKey());
 */
	 return true;
 }

 bool Test::symmetrieHashTestOnNotSquare()
 {
	/* bool testValue = true;
	 board b = board(3, 2, 3);
	 b.setMark(0, 0, tile::Mark::X);
	 b.printBoard();
	 board c = board(3, 2, 3);
	 c.setMark(2, 0, tile::Mark::X);
	 c.printBoard();
	 board d = board(3, 2, 3);
	 d.setMark(0, 1, tile::Mark::X);
	 d.printBoard();
	 board e = board(3, 2, 3);
	 e.setMark(2, 1, tile::Mark::X);
	 e.printBoard();
	 cout << b.computeBoardKey() << endl;
	 cout << c.computeBoardKey() << endl;
	 cout << d.computeBoardKey() << endl;
	 cout << e.computeBoardKey() << endl;

	 return (b.computeBoardKey() == c.computeBoardKey() && b.computeBoardKey() == d.computeBoardKey() && b.computeBoardKey() == d.computeBoardKey());
	*/
	 return true;
 }

 bool Test::tableTest()
 {
	 TranspositionTable t =  TranspositionTable(100);

	/* t.insert(4,true,2,4, );
	 t.insert(5, true, 2, 5, );
	 t.insert(3, true, 2, 3, );
	 t.insert(4, true, 2, 4, );
	 t.insert(4, true, 2, 4, );
	 t.insert(4, true, 2, 5, );
	 t.insert(4, true, 2, 4, );*/
	 return false;
 }

 void Test::heuristicTest()
 {
	 //board b = board(3, 3, 3);
	 //cout << "board" << endl;
	 //b.printBoard();
	 //cout << "heuristik" << endl;
	 //b.winningSetHeuristik.printHeuristicField();
	 //int m=1;
	 //int n=1;

	 //b.setMark(m,n,tile::O);
	 //b.updateHeuristicField(m,n,tile::Mark::O, false, 0);/*
	 //b.setMark(1, n, tile::O);
	 //b.updateHeuristicField(1, n, false);*/
	 //cout << "board" << endl;
	 //b.printBoard();
	 //cout << "heuristik" << endl;
	 //b.winningSetHeuristik.printHeuristicField();
	 
 }


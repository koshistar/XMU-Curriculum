// Fig. 24.13: TicTacToeServer.java
// This class maintains a game of Tic-Tac-Toe for two clients.
import java.awt.BorderLayout;
import java.net.ServerSocket;
import java.net.Socket;
import java.io.IOException;
import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.PreparedStatement;
import java.sql.SQLException;
import java.util.Formatter;
import java.util.Scanner;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;
import java.util.concurrent.locks.Condition;
import javax.swing.JFrame;
import javax.swing.JTextArea;
import javax.swing.SwingUtilities;

public class TicTacToeServer extends JFrame 
{
   private String[] board = new String[ 9 ]; // tic-tac-toe board
   private JTextArea outputArea; // for outputting moves
   private Player[] players; // array of Players
   private ServerSocket server; // server socket to connect with clients
   private int currentPlayer; // keeps track of player with current move
   private final static int PLAYER_X = 0; // constant for first player
   private final static int PLAYER_O = 1; // constant for second player
   private final static String[] MARKS = { "X", "O" }; // array of marks
   private ExecutorService runGame; // will run players
   private Lock gameLock; // to lock game for synchronization
   private Condition otherPlayerConnected; // to wait for other player
   private Condition otherPlayerTurn; // to wait for other player's turn

   private int winner=-1;

   private Connection connection;
   // set up tic-tac-toe server and GUI that displays messages
   public TicTacToeServer()
   {
      super( "Tic-Tac-Toe Server" ); // set title of window

      // create ExecutorService with a thread for each player
      runGame = Executors.newFixedThreadPool( 2 );
      gameLock = new ReentrantLock(); // create lock for game

      // condition variable for both players being connected
      otherPlayerConnected = gameLock.newCondition();

      // condition variable for the other player's turn
      otherPlayerTurn = gameLock.newCondition();      

      for ( int i = 0; i < 9; i++ )
         board[ i ] = new String( "" ); // create tic-tac-toe board
      players = new Player[ 2 ]; // create array of players
      currentPlayer = PLAYER_X; // set current player to first player
 
      try
      {
         server = new ServerSocket( 12345, 2 ); // set up ServerSocket
      } // end try
      catch ( IOException ioException ) 
      {
         ioException.printStackTrace();
         System.exit( 1 );
      } // end catch

      outputArea = new JTextArea(); // create JTextArea for output
      add( outputArea, BorderLayout.CENTER );
      outputArea.setText( "Server awaiting connections\n" );

      setSize( 300, 300 ); // set size of window
      setVisible( true ); // show window

      try {
         // 初始化数据库连接
         Class.forName("com.mysql.cj.jdbc.Driver");
         connection= DriverManager.getConnection(
                 "jdbc:mysql://localhost:3306/mysql",
                 "root",
                 "0d000721");
      } catch (Exception e) {
         e.printStackTrace();
      }
   } // end TicTacToeServer constructor

   private void saveGameResult(String winnerMark)
   {
      String boardState=String.join("",board);
      String sql="INSERT INTO tic_tac_toe_results "+"(winner_mark,board_state) VALUES (?,?)";
       try {
           PreparedStatement statement=connection.prepareStatement(sql);
           statement.setString(1,winnerMark);
           statement.setString(2,boardState);
           statement.executeUpdate();
           statement.close();
           displayMessage("\nSave result.");
       } catch (SQLException e) {
           throw new RuntimeException(e);
       }
   }
   // wait for two connections so game can be played
   public void execute()
   {
      // wait for each client to connect
      for ( int i = 0; i < players.length; i++ ) 
      {
         try // wait for connection, create Player, start runnable
         {
            players[ i ] = new Player( server.accept(), i );
            runGame.execute( players[ i ] ); // execute player runnable
         } // end try
         catch ( IOException ioException ) 
         {
            ioException.printStackTrace();
            System.exit( 1 );
         } // end catch
      } // end for

      gameLock.lock(); // lock game to signal player X's thread

      try
      {
         players[ PLAYER_X ].setSuspended( false ); // resume player X
         otherPlayerConnected.signal(); // wake up player X's thread
      } // end try
      finally
      {
         gameLock.unlock(); // unlock game after signalling player X
      } // end finally
   } // end method execute
   
   // display message in outputArea
   private void displayMessage( final String messageToDisplay )
   {
      // display message from event-dispatch thread of execution
      SwingUtilities.invokeLater(
         new Runnable() 
         {
            public void run() // updates outputArea
            {
               outputArea.append( messageToDisplay ); // add message
            } // end  method run
         } // end inner class
      ); // end call to SwingUtilities.invokeLater
   } // end method displayMessage

   // determine if move is valid
   public boolean validateAndMove( int location, int player )
   {
      // while not current player, must wait for turn
      while ( player != currentPlayer ) 
      {
         gameLock.lock(); // lock game to wait for other player to go

         try 
         {
            otherPlayerTurn.await(); // wait for player's turn
         } // end try
         catch ( InterruptedException exception )
         {
            exception.printStackTrace();
         } // end catch
         finally
         {
            gameLock.unlock(); // unlock game after waiting
         } // end finally
      } // end while

      // if location not occupied, make move
      if ( !isOccupied( location ) )
      {
         board[ location ] = MARKS[ currentPlayer ]; // set move on board
         currentPlayer = ( currentPlayer + 1 ) % 2; // change player

         // let new current player know that move occurred
         players[ currentPlayer ].otherPlayerMoved( location );

         gameLock.lock(); // lock game to signal other player to go

         try 
         {
            otherPlayerTurn.signal(); // signal other player to continue
         } // end try
         finally
         {
            gameLock.unlock(); // unlock game after signaling
         } // end finally

         return true; // notify player that move was valid
      } // end if
      else // move was not valid
         return false; // notify player that move was invalid
   } // end method validateAndMove

   // determine whether location is occupied
   public boolean isOccupied( int location )
   {
      if ( board[ location ].equals( MARKS[ PLAYER_X ] ) || 
         board [ location ].equals( MARKS[ PLAYER_O ] ) )
         return true; // location is occupied
      else
         return false; // location is not occupied
   } // end method isOccupied

   // place code in this method to determine whether game over 
   public boolean isGameOver()
   {
      //TODO:补充判断游戏结束部分代码
      winner=checkWinner();
      //检查棋盘是否已满
      boolean isFull=true;
      for(String cell:board)
         if (cell.isEmpty())
            isFull=false;
      if(isFull||winner!=-1)
      {
         if(winner!=-1)
         {
            saveGameResult(winner==0?"X":"O");
         }
         else
         {
            saveGameResult("D");//平局
         }
         return true;
      }
      return false;// this is left as an exercise
   } // end method isGameOver

   private int checkWinner()
   {
      //检查行
      for(int i=0;i<9;i+=3)
      {
         if(!board[i].isEmpty()&&board[i].equals(board[i+1])&&board[i].equals(board[i+2]))
            return board[i]=="X"?0:1;
      }
      //检查列
      for(int i=0;i<3;i++)
      {
         if(!board[i].isEmpty()&&board[i].equals(board[i+3])&&board[i].equals(board[i+6]))
            return board[i]=="X"?0:1;
      }
      //检查对角线
      if(!board[0].isEmpty()&&board[0].equals(board[4])&&board[0].equals(board[8]))
         return board[0]=="X"?0:1;
      if(!board[2].isEmpty()&&board[2].equals(board[4])&&board[2].equals(board[6]))
         return board[2]=="X"?0:1;
      return -1;
   }
   // private inner class Player manages each Player as a runnable
   private class Player implements Runnable 
   {
      private Socket connection; // connection to client
      private Scanner input; // input from client
      private Formatter output; // output to client
      private int playerNumber; // tracks which player this is
      private String mark; // mark for this player
      private boolean suspended = true; // whether thread is suspended

      // set up Player thread
      public Player( Socket socket, int number )
      {
         playerNumber = number; // store this player's number
         mark = MARKS[ playerNumber ]; // specify player's mark
         connection = socket; // store socket for client
         
         try // obtain streams from Socket
         {
            input = new Scanner( connection.getInputStream() );
            output = new Formatter( connection.getOutputStream() );
         } // end try
         catch ( IOException ioException ) 
         {
            ioException.printStackTrace();
            System.exit( 1 );
         } // end catch
      } // end Player constructor

      // send message that other player moved
      public void otherPlayerMoved( int location )
      {
         output.format( "Opponent moved\n" );
         output.format( "%d\n", location ); // send location of move
         output.flush(); // flush output
      } // end method otherPlayerMoved

      public void winnerOutput()
      {
         output.format("YOU WIN!\nGAME OVER\n");
         output.flush();
      }
      public void loserOutput()
      {
         output.format("YOU LOSE!\nGAME OVER\n");
         output.flush();
      }
      public void setLoser()
      {
         winner=(playerNumber+1)%2;
      }

      // control thread's execution
      public void run()
      {
         // send client its mark (X or O), process messages from client
         try 
         {
            displayMessage( "Player " + mark + " connected\n" );
            output.format( "%s\n", mark ); // send player's mark
            output.flush(); // flush output

            // if player X, wait for another player to arrive
            if ( playerNumber == PLAYER_X ) 
            {
               output.format( "%s\n%s", "Player X connected",
                  "Waiting for another player\n" );
               output.flush(); // flush output

               gameLock.lock(); // lock game to  wait for second player

               try 
               {
                  while( suspended )
                  {
                     otherPlayerConnected.await(); // wait for player O
                  } // end while
               } // end try 
               catch ( InterruptedException exception ) 
               {
                  exception.printStackTrace();
               } // end catch
               finally
               {
                  gameLock.unlock(); // unlock game after second player
               } // end finally

               // send message that other player connected
               output.format( "Other player connected. Your move.\n" );
               output.flush(); // flush output
            } // end if
            else
            {
               output.format( "Player O connected, please wait\n" );
               output.flush(); // flush output
            } // end else

            // while game not over
            while ( true )
            {
               int location = 0; // initialize move location

               if ( input.hasNext() )
                  location = input.nextInt(); // get move location

               if(!isGameOver()) {               // check for valid move
                  if (validateAndMove(location, playerNumber)) {
                     displayMessage("\nlocation: " + location);
                     output.format("Valid move.\n");
                     output.flush(); // flush output
                  } // end if
                  else // move was invalid
                  {
                     output.format( "Invalid move, try again\n" );
                     output.flush(); // flush output
                  } // end else
               }
               if(isGameOver())
               {
                  if(winner==playerNumber) {
                     winnerOutput();
                     players[currentPlayer].loserOutput();
                     players[currentPlayer].setLoser();
                  }
                  break;
               }
            } // end while
         } // end try
         finally
         {
            try
            {
               connection.close(); // close connection to client
            } // end try
            catch ( IOException ioException ) 
            {
               ioException.printStackTrace();
               System.exit( 1 );
            } // end catch
         } // end finally
      } // end method run

      // set whether or not thread is suspended
      public void setSuspended( boolean status )
      {
         suspended = status; // set value of suspended
      } // end method setSuspended

   } // end class Player
} // end class TicTacToeServer
/**************************************************************************
 * (C) Copyright 1992-2005 by Deitel & Associates, Inc. and               *
 * Pearson Education, Inc. All Rights Reserved.                           *
 *                                                                        *
 * DISCLAIMER: The authors and publisher of this book have used their     *
 * best efforts in preparing the book. These efforts include the          *
 * development, research, and testing of the theories and programs        *
 * to determine their effectiveness. The authors and publisher make       *
 * no warranty of any kind, expressed or implied, with regard to these    *
 * programs or to the documentation contained in these books. The authors *
 * and publisher shall not be liable in any event for incidental or       *
 * consequential damages in connection with, or arising out of, the       *
 * furnishing, performance, or use of these programs.                     *
 *************************************************************************/

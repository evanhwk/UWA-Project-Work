import java.util.*;
/**
 * A class implementing an object which represents an agent's view of the current game state of the trick-playing card game, Moss Side Whist.
 * @author Evan Huang 20916873
 * @version 2.0
 */
public class WhistStateCheat implements WState
{
    private static final int PLAYERS = 3;                            // # of players
    private static final int DISCARD_CARDS = 4;                      // # of cards discarded
    private static final int MAX_CARDS = 16;                         // Maximum # of cards that can be held by a player (after discard stage)
    private static final int THIS = 0;                                   // This player -> 1st index
    private static final int SUITS = 4;                              // # of suits
    private static final int SUIT_CARDS = 13;                        // # of cards in each suit
    private static final int LEFT = 1;                                   // Left player -> 2nd index
    private static final int RIGHT = 2;                                  // Right player -> 3rd index
    
    private int leaderPlayer, currPlayer,                                // Leading player for this round (of 16 tricks),  The player who is next to make a move
                tricksPlayed, trickCount;                                // Number of tricks played thus far, number of cards played in the current trick.
                    
    private HashMap<String, Integer> names;                              // Map names of agents to positions in this game state.
    private int[] scoreboard;                                            // Scoreboard 
    private Card[][] tricks;                                             // Array to store trick information
    private ArrayList<Card>[] hands;                                     // Hands for each player (opponent hands are simulated).
    private ArrayList<Card> discarded;                                   // Discarded cards
    
    private boolean determinization;                                     // True if this WhistState simulates opponents hands. 
    private boolean debug;                                               // True if debugging
    
    private Random rand;                                                 // Random for dealing
    
    /**
     * WhistState constructor
     * 
     * @param agentThis Name of this agent.
     * @param agentLeft Name of the left agent.
     * @param agentRight Name of the right agent.
     */
    public WhistStateCheat(String curr, String left, String right)
    {
        this.names = new HashMap<>(PLAYERS);                // init map of strings -> index
        names.put(curr, THIS);                            
        names.put(left, LEFT);
        names.put(right, RIGHT);         
        //this.leaderPlayer = 0;
        //this.currPlayer = 0;
        //this.tricksPlayed = 0;
        //this.trickCount = 0;
        this.scoreboard = new int[]{0,0,0};                     // init and set scoreboard to 0,0,0
        this.tricks = new Card[MAX_CARDS][PLAYERS];     // init tricks as a matrix of size 16 (number of tricks) * 3 (cards played at each trick)
        this.hands = new ArrayList[PLAYERS];                // init hands array with size 3, the number of players.
        this.discarded = new ArrayList<>(DISCARD_CARDS);    // init arraylist of discarded cards (with init size 4).
        this.determinization = false;
        this.debug = false;
        this.rand = new Random();
    }
    
    /** 
     * Clone Constructor for WhistState
     * 
     * @param N HashMap of name, index pairs.
     * @param L Index reprepresenting who made the first move in the current round (0 = This agent, 1 = Left Agent, 2 = Right Agent).
     * @param C index representing which player makes the next move (0 = This agent, 1 = Left Agent, 2 = Right Agent).
     * @param sc An array containing the scores of each player.
     * @param tC Trick Counting variable.
     * @param tP How many tricks have been played.
     * @param tA A matrix representing the tricks being played and that have already been played.
     * @param h An array of ArrayLists representing the hands of the current player and the opponents.
     */
    private WhistStateCheat(HashMap<String,Integer> N, int L, int C, int[] sc, int tC, int tP, Card[][] tA, ArrayList<Card>[] h, ArrayList<Card> discard, boolean d)
    {
        this.names = new HashMap<>(N);                          // init
        this.leaderPlayer = L;                                  // Copy primitive types
        this.currPlayer = C;
        this.tricksPlayed = tP;                                    
        this.trickCount = tC;
        this.scoreboard = new int[PLAYERS];                    // init and set scoreboard to 0,0,0
        for (int i=0; i<PLAYERS;i++)
            scoreboard[i] = sc[i];
        this.tricks = new Card[MAX_CARDS][PLAYERS];        // init
        for (int i=0; i <= tP; i++)
            for (int j=0; j < PLAYERS; j++)
                    tricks[i][j] = tA[i][j];                       // Copy all elements of tA -> tricks
       
        this.hands = new ArrayList[PLAYERS];
        for (int i=0; i<PLAYERS; i++) {                         // For every hand
            hands[i] = new ArrayList<Card>();
            if (h[i].isEmpty()) continue;
            for (Card c : h[i])
                hands[i].add(c);                                    // Copy all elements of each hand h[] into hands[]
        }
        this.determinization = d;
        this.discarded = new ArrayList<>(DISCARD_CARDS);
        for (Card c : discard)
            this.discarded.add(c);
        this.rand = new Random();
        this.debug = false;
    }
    
    /**
     * Method to initialise OR reset variables after being called during dealing stage of the current round. 
     * 
     * @param hand List representing the hand of the current player.
     * @param order Index of the current player's position relative to the leader. (0 = Leader, 1 = Left, 2 = Right).
     */
    public void setupDeal(List<Card> hand, int order) throws IllegalArgumentException
    {
        if ( (hand.size() != MAX_CARDS && order != 0) || (hand.size() != (DISCARD_CARDS+MAX_CARDS) && order == 0)) {
            throw new IllegalStateException("Illegal State setupDeal() - size of hand: "+hand.size()+" order = "+order);
        }
        if (order >= PLAYERS) {
            throw new IllegalArgumentException("Illegal Argument setupDeal() - order = "+order);
        }
        else {
            int leader = (PLAYERS - order) % PLAYERS;   // 3 - Order % 3
            // order=0-> leader = (0) we start the trick, 
            // order=1-> leader = (2) right player starts trick, 
            // order=2-> leader = (1) left player starts trick
            leaderPlayer = leader;                              // SET leaderPlayer to current leader.
            currPlayer = leader;                                // SET currPlayer to current leader. (We begin with the leader)
            tricksPlayed = 0;                                   // RESET tricksPlayed to 0 every time new deal is setup.
            trickCount = 0;                                     // RESET trickCount - reset trick count to 0 every time a new deal is setup. - not actually necessary (var should already be reset) but good to ensure state validity.
        
            // Store our own hand into the array
            hands[THIS] = new ArrayList<Card>(hand);            // Store current agent's hand as an arrayList into first index of hands array.
            hands[LEFT] = new ArrayList<Card>(MAX_CARDS);
            hands[RIGHT] = new ArrayList<Card>(MAX_CARDS);
            discarded.clear();                                  // RESET list of discarded cards.
        }
    }
    
    /** Method to set up opponents hands (cheating agent)
     * @param left the left opponents hand
     * @param right the right opponents hand
     */
    public void setupOpponents (List<Card> left, List<Card> right) {
        hands[LEFT] = new ArrayList<Card>(left);
        hands[RIGHT] = new ArrayList<Card>(right);
    }
    public WState cloneAndRandomize() { return null; }
    
    /**
     * Clone and randomize the current game simulation, changing the hands of the two simulated players, keeping other parameters constant.
     * 
     * @return The new randomized gamestate.
     */
    public WState clone() {
        // Clones the current state
        WState returnState = new WhistStateCheat(names, leaderPlayer, currPlayer, scoreboard, trickCount, tricksPlayed, tricks, hands, discarded, true);
        return returnState;
    }
    
    /** 
     * Discard cards for the given player.
     * 
     * @param player the player who is discarding cards.
     * @param index Index of the card to be discarded.
     */
    public void discard(int player, int index) 
    {
        if (player == 0) {
            discarded.add(hands[player].get(index));
        }
        hands[player].remove(index);
    }
    
    /** 
     * Discard cards for the given player.
     * 
     * @param player the player who is discarding cards.
     * @param c the card to be discarded.
     */
    public void discard(int player, Card c) 
    {
        if (player == 0) {
            discarded.add(c);
        }
        hands[player].remove(c);
    }
    
    /**
     * Simulate playing a card, given the current game state.
     * 
     * @param move The card to play.
     */
    public void doMove(Card move) throws IllegalStateException
    {
        if (tricksPlayed == MAX_CARDS) throw new IllegalStateException ("Illegal State: doMove() - tricksPlayed > 16");
        if (determinization && !isMoveValid(move)) throw new IllegalStateException ("Illegal State: doMove() invalid move attempted by "+currPlayer+" : "+move.toString());
            //System.err.println("doMove() invalid move attempted by "+currPlayer+" : "+move.toString());
        hands[currPlayer].remove(move);                                                         // Remove this Card from the player's hand.
        tricks[tricksPlayed][trickCount] = move;                                                // Simulate the move
        
        if (!endCheck())                                                                       // Check if the trick/round has concluded as a result of this move.
            currPlayer = (currPlayer+1)%PLAYERS;                                               // Update current player to the next player. 
        trickCount = (trickCount+1)%PLAYERS;
    }
    /**
     * Checks if this game has either finished playing a trick, or finished an entire round. If the last card of the trick has been played, updates scoreboard and trick matrix (and associated variables). 
     * If the last trick has concluded, update the scoreboard and clear the entire trick state matrix.
     * 
     * @return True if this was the end of the trick, false otherwise.
     */
    private boolean endCheck()
    {
        if (trickCount == PLAYERS-1) {                                                  // At the end of the trick
            int winner = getWinner(tricks[tricksPlayed][0], tricks[tricksPlayed][1], tricks[tricksPlayed][2]);       // Find winner
            scoreboard[winner]++;                                                          // Update winner's score
            currPlayer = winner;                                                           // Update currentPlayer to winner, who will lead next trick
            tricksPlayed++;                                                                // Move to next Card[3] array for the next trick.
            if (isTerminal()) {                                                            // At the end of the round (16 tricks played)
                scoreboard[leaderPlayer] = scoreboard[leaderPlayer]-8;                     // Winner score - 8
                int player1 = (leaderPlayer+1) % PLAYERS;
                scoreboard[player1] = scoreboard[player1]-4;                               // Rest score - 4
                int player2 = (leaderPlayer+2) % PLAYERS;
                scoreboard[player2] = scoreboard[player2]-4;
                    
                leaderPlayer = (leaderPlayer+1) % PLAYERS;                                // Player to left of leader leads.
                tricks = new Card[MAX_CARDS][PLAYERS];                                    // Clear tricks array
            }
            return true;
        }
        return false;
    }
    
    /**
     * Find the winner of the current trick.
     * <ul>
     * <li> [Adapted from MossSideWhist.java, author Tim French] </li>
     * </ul>
     * @param lead The first card in the trick.
     * @param next The second card in the trick.
     * @param last The third card in the trick.
     * @return integer representing the winning player from the perspective of this agent.
     */
    private int getWinner(Card lead, Card next, Card last) throws IllegalStateException
    {
        if (trickCount != PLAYERS-1) throw new IllegalStateException("Illegal State: getWinner() called before all players have played");
        int first = (currPlayer+1)%PLAYERS;
        int second = (first+1)%PLAYERS;
        int third = (second+1)%PLAYERS;;
        if(lead.compareTo(next)<0 || !followsSuit(lead, next)){     //first beats second
            if(lead.compareTo(last)<0 || !followsSuit(lead, last)){     //first beats third
                return first;                                            // first player (to play trick) is winner
            }
            else{//third beats first
                return third;                                                   // 3rd player (to play trick) is the winner
            }
        }
        else{//second beats first
            if(next.compareTo(last)<0 || ! followsSuit(lead, last)){            //second beats thirs                          
                return second;                                                  // 2nd player (to play trick) is the winner
            }
            else{//third beats second
                return third;                                                  // 3rd player (to play trick) is the winner                 
            }
        }
    }
        
    /** Compare the suits of two cards. 
     * <ul>
     * <li> [Copied from MossSideWhist.java, author Tim French] </li>
     * </ul>
     * @param c1 Card 1
     * @param c2 Card 2
     * @return True if Card 2 follows Card 1, OR if Card 2's suit is SPADES. Else return false. 
     */
    private boolean followsSuit(Card c1, Card c2){
        return c2.suit==c1.suit || c2.suit==Suit.SPADES;
    }
   
    /**
     * Run termination evaluation of the current (terminal) game state.
     * 
     * @return 1 if this agent wins this simulation, else returns 0.
     */
    public int getResult() throws IllegalStateException 
    {
        if (!isTerminal()) throw new IllegalStateException("Illegal state: getResult() run in non-terminal state");
        for (int i=1; i<PLAYERS;i++) {
            if (scoreboard[i] >= scoreboard[0]) {
                return 0;
            }
        }
        return 1;
    }
    
    /**
     * Using the default (rollout) policy, choose the next move to play.
     */
    public Card getSimulationMove() throws IllegalStateException
    {
        ArrayList<Card> moves = getValidMoves();
        if (moves.isEmpty()) throw new IllegalStateException("Illegal State: getSimulationMove() Move set is empty");
        Collections.sort(moves);
        int index = 0;
        // RANDOM POLICY
        index = rand.nextInt(moves.size());
        
        return moves.get(index);
    }
    
    /**
     * A method which given the current game state, returns a set of valid moves according to the rules of moss side whist.
     * (i.e. Can only play cards which are in your hand, must play card of same suit, etc.)
     * 
     * @return An arrayList of moves which are valid, given the current game state.
     */
    public ArrayList<Card> getValidMoves() throws IllegalStateException
    {
        ArrayList<Card> validMoves = new ArrayList<>(); 
        for (Card move : hands[currPlayer]) {
            if (isMoveValid(move)) {
                validMoves.add(move);
                if (debug) System.out.println(move);
            }
        }
        if (validMoves.isEmpty()) throw new IllegalStateException("Illegal State: getValidMoves() returned empty collection"); 
        return validMoves;
        // return SHOULD be sorted: S, D, C, H
    }
    
    /** 
     * Checks validity of the inputed move.  
     * <ul>
     * <li> [Adapted from MossSideWhist.java, author Tim French] </li>
     * </ul>
     * 
     * @param move The move to be analysed.
     * @return True if this is a valid move, false if invalid.
     */
    private boolean isMoveValid(Card move) {
        if (hands[currPlayer].contains(move)) {
            if (trickCount == 0) {
                return true;    // Leader playing the first card of the trick (with a card that is in his hand). Valid.
            }
            else {
                if (move.suit != tricks[tricksPlayed][0].suit) {
                    for (Card c: hands[currPlayer]) {
                        if (c.suit == tricks[tricksPlayed][0].suit)
                            return false;
                        // Played card didn't match suit, cards of matching suit found. Not Valid.
                    }
                    return true;    // Card didn't match suit, but no cards of matching suit found. Valid.
                }
                return true;        // Card is in hand, matches suit of current trick. Valid.
            }
        }
        return false;           // Card is not in hand. Invalid. 
    }
        
    /**
     * Checks if the simulated game is in a terminal state.
     * 
     * @return true if this game state is terminal, else false.
     */
    public boolean isTerminal() {
        return (tricksPlayed == MAX_CARDS);
    }
    
    /**
     * Validate our own scorekeeping with actual information from the game scoreboard.
     * 
     * @param sc the actual scoreboard.
     */
    public void validateScores(Map<String, Integer> sc) throws IllegalStateException
    {
        Set<String> players = sc.keySet();
        for (String p : players) {
            if ( scoreboard[names.get(p)] != sc.get(p) )
                throw new IllegalStateException("Mismatch found for "+p+" : "+scoreboard[names.get(p)] +" != "+sc.get(p));
        }
    }
    
    public void validateWinner(String winner)
    {
        if (names.get(winner) != currPlayer)
            System.err.println("Wrong leading player");
    }
}
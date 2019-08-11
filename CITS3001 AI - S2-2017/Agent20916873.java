import java.util.*;
/**
 * A class implementing the Information Sets Monte Carlo Search based AI for the trick-playing card game Moss Side Whist. Additionallly incorporates a 2ES pruning strategy, with 
 * @author Evan Huang 20916873
 * @version 5.6
 */
public class Agent20916873 implements MSWAgent {
    private String name;                                                          // Name of this agent
    private static final int LEADER = 0;
    private static final int LEFT = 1;
    private static final int RIGHT = 2;
    
    private List<Card> hand;                                                      // This agent's hand.
    private int order;                                                            // Initial playing order
    private WhistState currState;                                                 // WhistState object representing this agent's view of the game world.
    
    //private int iterationCount;                                                 // Counts the number of MC Search() iterations before hitting computational limit.
    private boolean ESEflag;                                                       // Flag to run the discarded card evaluation.
    private boolean DLSflag;                                                       // Flag to run the lowest suits & ranks discard policy.
    
    private Random rand;                                                          // Random object used to generate names, uniform distributions, etc.
    /**
    * Constructor for agent (with default naming parameters).
    */
    public Agent20916873() 
    {
        this.rand = new Random();
        this.name = "Agent20916873";
        this.ESEflag = true;
        this.DLSflag = true;
    }
    /**
    * Constructor for named agent.
    * @param name The name of this agent;
    */
    public Agent20916873(String name) 
    {
        this.rand = new Random();
        this.name = name;
    }
    
    /**
    * Constructor for named agent.
    * @param name The name of this agent.
    */
    public Agent20916873(String name, boolean dls, boolean ese) 
    {
        this.rand = new Random();
        this.name = name;
        this.DLSflag = dls;
        this.ESEflag = ese;
    }
    
    /**
    * Tells the agent the names of the competing agents, and their relative position.
    * 
    * @param agentLeft The name of the left agent.
    * @param agentRight The name of the right agent.
    * */
    public void setup(String agentLeft, String agentRight)
    {
        this.currState = new WhistState(this.name, agentLeft, agentRight, ESEflag);
    }
    
    /**
    * Starts the round with a deal of the cards.
    * The agent is told the cards they have (16 cards, or 20 if they are the leader)
    * and the order they are playing.
    * 
    * @param hand This agent's hand.
    * @param order The order they are playing (0 for the leader, 1 for the left of the leader, and 2 for the right of the leader).
    * */
    public void seeHand(List<Card> hand, int order) 
    {
        currState.setupDeal(hand, order);
        this.hand  = hand;
        this.order = order;
    }
    // We are a good, law abiding and peaceful AI and don't cheat :)
    public void seeOpponents(List<Card> left, List<Card> right) {}
    
    /**
    * This method will be called on the leader agent, after the deal.
    * If the agent is not the leader, it is sufficient to return an empty array.
    * 
    */
    public Card[] discard()
    {
        if (DLSflag) {
            if (this.order==0)  {
                // DISCARD LOWEST SUIT (DLS)
                HashMap<Suit, ArrayList<Card>> suits = new HashMap<Suit, ArrayList<Card>>();
                suits.put(Suit.SPADES, new ArrayList<Card>());
                suits.put(Suit.DIAMONDS, new ArrayList<Card>());
                suits.put(Suit.CLUBS, new ArrayList<Card>());
                suits.put(Suit.HEARTS, new ArrayList<Card>());
                for (Card c : hand) {
                    if (c.rank >= 11) continue;
                    switch (c.suit) {
                        case SPADES: suits.get(Suit.SPADES).add(c); break;
                        case DIAMONDS: suits.get(Suit.DIAMONDS).add(c); break;
                        case CLUBS: suits.get(Suit.CLUBS).add(c); break;
                        case HEARTS: suits.get(Suit.HEARTS).add(c); break;
                    }
                }
                
                PriorityQueue<SuitPQ> lowSuit = new PriorityQueue<SuitPQ>(4);
                Set<Suit> suitsSet = suits.keySet();
                for (Suit s : suitsSet) {
                    if (s == Suit.SPADES) continue;
                    int size = suits.get(s).size();
                    lowSuit.add(new SuitPQ(s, size));
                }
                lowSuit.add(new SuitPQ(Suit.SPADES, 99999));
                
                Suit match = lowSuit.peek().getSuit();
                ArrayList<Card> discardOrder = new ArrayList<Card>(); 
                while (discardOrder.size() < 4) {
                    match = lowSuit.poll().getSuit();
                    Collections.reverse(suits.get(match));
                    discardOrder.addAll(suits.get(match));
                }
                
                Card[] discard = new Card[4];
                for (int i=0; i<4; i++) {
                    discard[i] = discardOrder.remove(0);
                    currState.discard(0, discard[i]);
                }
                return discard;
            }
            else return new Card[0];
        }
        else {
            if (this.order==0)  {
                // RANDOM DISCARD POLICY
                Card[] discard = new Card[4];
                for (int i = 0; i<4;i++) {
                    int discardIndex = rand.nextInt(20-i);
                    //System.out.println("Discarded: "+ hand.get(discardIndex).toString());
                    discard[i] = hand.remove(discardIndex);
                    currState.discard(0, discardIndex);                         // UPDATE game state
                }
                return discard;
            }
            else return new Card[0];
        }
    }
    
    /**
    * Agent returns the card they wish to play.
    * A 200 ms timelimit is given for this method.
    * 
    * @return the Card this agent wishes to play.
    */
    public Card playCard()
    {
        Card play = Search(190);    //10 ms buffer
        return play;
    }
    
    /**
    * Sees an Agent play a card.
    * A 50 ms timelimit is given to this function.
    * @param card The Card played.
    * @param agent The name of the agent who played the card.
    * */
    public void seeCard(Card card, String agent)
    {
        //System.out.println("Agent "+agent+" playing "+card.toString());
        currState.doMove(card);
    }
    
    /**
    * See the result of the trick. 
    * A 50 ms timelimit is given to this method.
    * This method will be called on each agent at the end of each trick.
    * @param winner The player who played the winning card.
    * */
    public void seeResult(String winner)
    {
    }
    
    /**
    * See the score for each player.
    * A 50 ms timelimit is givien to this method
    * @param scoreboard, a Map from agent names to their score.
    **/
    public void seeScore(Map<String, Integer> scoreboard)
    {
    }
    
    /**
    * Returns the Agents name.
    * A 10ms timelimit is given here.
    * This method will only be called once.
    * */
    public String sayName()
    {
        return this.name;
    }
    
    /**
    * Run the Single Observer Information Sets MC Tree Search algorithm.
    * <ul>
    * <li> Start by generating the start of the MC Tree Search </li>
    * <li> SELECT (go down the tree) the best node UNTIL (1) a node with unvisited children OR (2) a terminal node </li>
    * <li> EXPAND a partially expanded node, adding one or more child nodes to expand the tree </li>
    * <li> From a child node, ROLLOUT the default/rollout policy until we reach an endgame state. </li>
    * <li> BACKPROPOGATE the result at this endgame state up the tree, from child to parent, to the root. </li>
    * </ul>
    * [Adapted following pseudocode of ISMCTS framework from Di Palma (2014)]
    * 
    * @param time Time (in ms) the search is allowed to run for. 
    * @return the recommended card to be played.
    * @see <a href="Monte Carlo Tree Search Algorithms applied to the card game Scopone">http://teaching.csse.uwa.edu.au/units/CITS3001/project/2017/paper1.pdf</a>
    */ 
    private Card Search(long time)
    {
        long start = System.currentTimeMillis();
        long end = start;
        //iterationCount = 0;
        WhistNode rootNode = new WhistNode(null, null);                           // Initialise the tree with a root node          
        while ((end-start) < time) {                                              // While we are within the time limit execute this.
          WhistNode currNode = rootNode;
          WhistState currRandState = currState.cloneAndRandomize();             // Choose a new determinization d.
          /* SELECT
           * Starting from root node, recursively selects most urgent node, until a node is reached that represents:
           *   (A) A node that is not fully expanded yet (in the current determinization)
           *   (B) A terminal node                                                                                  */ 
          while (!currNode.hasMovesToTry(currRandState) && currNode.hasChildren()) { // while !(A) && !(B)
              currNode = currNode.selectChild();                   // use IS-UCT algorithm to select next node to select [in WhistNode.selectChild()].
              currRandState.doMove(currNode.lastPlay);                          // make (simulated) move
          }
          /* EXPAND / Tree Policy
           * Once we reach a partially expanded node, add one or more child nodes and expand the tree               */ 
          if (currNode.hasMovesToTry(currRandState)) {      // if (A)
              Card move = currNode.selectUntriedMove(currRandState);            // choose which untried move to expand the tree to.
              currRandState.doMove(move);                                       // make (simulated) move
              currNode = currNode.addChild(move);                               // add a new node to the tree
          }
          /* ROLLOUT / Default Policy
           * Run a simulation from the new nodes according to the default policy to produce an outcome (or reward). */
          while (!currRandState.isTerminal()) {                                 // LOOP until terminal state
              currRandState.doMove(currRandState.getSimulationMove());          // make (simulated) move using the default/rollout policy.
          }
          int finalReward = currRandState.getResult();
          /* BACKPROPOGATE
           * Rewards are backpropogated through the previously selected nodes to update their statistics.           */
          while (currNode != null) {
              currNode.update(finalReward);                                     // N(n) <- N(n)+1, Q(n) <- Q(n)+delta
              currNode = currNode.parent;                                       // n <- parent of n
          }
          end =  System.currentTimeMillis();
          //iterationCount++;
        }
        return rootNode.getBestMove();
    }
    /**
     * A class representing the nodes of the tree that is built whilst conducting an Information Set Monte Carlo Tree Search, for the trick-playing card game Moss Side Whist.
     * @author Evan Huang
     * @version 6.0
     */
    public class WhistNode implements WNode
    {
        public WhistNode parent;
        public HashMap<Card, WhistNode> children;
        
        private int currPlayer;                                             // The player who is making the next move at this node
        private Card lastPlay;                                               // The last card played.
        private int reward;                                                 // Q [pseudocode]
        private int numVisits;                                              // N [pseudocode]
        private int numSeen;                                                // N' [pseudocode]
        
        private final double C = 1.0;                                       // c = 1 -> c = 1/sqrt(2) for UCT Algorithm. 
        /**
         * Constructor for WhistNode.
         *
         * @param c The last card played (that led to this node). 
         * @param p The parent node of this node.
         */
        public WhistNode(Card c, WhistNode p)
        {
            this.parent = p;
            this.children = new HashMap<>(); 
            this.lastPlay = c;
            this.reward = 0;
            this.numVisits = 0;
            this.numSeen = 0;
        }
        
        /** 
         * Update score during back propogating step.
         * <ul>
         * <li>Called from the BACKPROPOGATE stage of the ISMCTS algorithm. </li>
         * </ul> 
         * @param r Result to set this node to.
         */
        public void update(int r) 
        {
            reward += r;
            numVisits += 1;
        }
        
        /**
         * Determine if the current node has child nodes.
         * 
         * @return True if this node has no child nodes, false otherwise.
         */
        public boolean hasChildren()
        {
            return (children.isEmpty());
        }
    
        /**
         * Check if the current node has not been fully expanded, given a determinization d.
         *
         * @param d WhistState representing a random determinization of the game state.
         * @return True if there are still moves in this determinization that have not been explored. False otherwise.
         */
        public boolean hasMovesToTry(WState d)
        {
            // Given the current state, get all the valid moves
            ArrayList<Card> moves = d.getValidMoves();
            // Look through all the elements of this set of valid moves
            for (Card m : moves) {
                // If there is a valid move that is not a child of this node
                if (!children.containsKey(m)) return true;
            }
            return false;
        }
        
        /**
         * Selects which node to move to. 
         * <ul>
         * <li>Called from the SELECT stage of the ISMCTS algorithm. </li>
         * <li>Implement Tree Policy (SO-ISUCT) [Single Observer Information Set Upper Confidence Bound for Trees] to determine which node to select next. </li>
         * </ul>
         */
        public WhistNode selectChild()
        {
            // for all n' of n
            for (WhistNode wN : children.values()) {
                //N'(n) <- N'(n') + 1
                wN.numSeen++;
            }
            double bestQ = -999999;
            ArrayList<WhistNode> wNlist = new ArrayList<>();
            double currQ;
            // BestIsuctChild(n, d, c)
            // for all n' of n
            for (WhistNode wN : children.values()) {
                // IS-UCT Equation - Q(n')/N(n') + C * sqrt(2 ln N'(n')/N(n'))
                currQ = wN.reward/wN.numVisits + wN.C * Math.sqrt( (2*Math.log(wN.numSeen)) / wN.numVisits );
                if (currQ == bestQ) {
                    wNlist.add(wN);
                }
                else if (currQ > bestQ) {
                    wNlist.clear();
                    bestQ = currQ;
                    wNlist.add(wN);
                }
            }
            System.out.println(wNlist.size());
            int i = new Random().nextInt(wNlist.size());
            
            return wNlist.get(i);
        }
        
        /**
         * Suggest an unexplored play to expand the tree to, given a random determinization d. 
         * <ul>
         * <li>Called from the EXPAND stage of the ISMCTS Algorithm.</li>
         * <li>Selects using a RANDOM policy</li>
         * </ul>
         * 
         * @param d A random determinization of the game state.
         * @return The recommended card to play.
         */
        public Card selectUntriedMove(WState d) throws IllegalStateException
        {
            // Given the current state, get all the valid moves
            ArrayList<Card> moves = d.getValidMoves();
            // Look through all the elements of this set of valid moves
            for (Card m : moves) {
                // If there is a valid move that is not a child of this node
                if (!children.containsKey(m)) 
                    return m;
            }
            throw new IllegalStateException("Illegal State in selectUntriedMove() : No untried move");
        }
        
        /**
         * Add new children to tree.
         * 
         * @param play The card that was played.
         * @return The new child node.
         */
        public WhistNode addChild(Card play) 
        {
            WhistNode child = new WhistNode(play, this);
            this.children.put(play,child);
            return child;
        }
        
        /**
         * Suggest what the next card the agent should play next. 
         * <ul>
         * <li>Called AFTER the search has concluded</li>
         * <li>Selects the node with the highest reward, i.e. a MAX policy.</li>
         * </ul>
         */
        public Card getBestMove() 
        {
            int bestReward = Integer.MIN_VALUE;
            WhistNode bestNode = null;
            int cmp;
            // Look through all child nodes of this node
            for (WhistNode wN : children.values()) {
                cmp = wN.reward;
                if (cmp > bestReward) {
                    bestReward = cmp;
                    bestNode = wN;
                }
            }
            // return the action that led to this child node
            return bestNode.lastPlay;
        }
        
        /**
         * Accessor method that returns  the card (move) that led to this node
         * 
         * @return the move that led to this node
         */
        public Card getLastPlay()
        {
            return this.lastPlay;
        }
        
        /**
         * Accessor method that returns parent of this node
         * 
         * @return parent of this node
         */
        public WhistNode getParent() 
        {
            return this.parent;
        }
    }
    
    /**
     * A class implementing an object which represents an agent's view of the current game state of the trick-playing card game, Moss Side Whist.
     * Incorporates the Exhausted Suits Evaluator. (ESE).
     * @author Evan Huang 20916873
     * @version 6.0
     */
    public class WhistState implements WState
    {
        private static final int PLAYERS = 3;                                // # of players
        private static final int DISCARD_CARDS = 4;                          // # of cards discarded
        private static final int MAX_CARDS = 16;                             // Maximum # of cards that can be held by a player (after discard stage)
        private static final int THIS = 0;                                   // This player -> 1st index
        private static final int SUITS = 4;                                  // # of suits
        private static final int SUIT_CARDS = 13;                            // # of cards in each suit
        private static final int LEFT = 1;                                   // Left player -> 2nd index
        private static final int RIGHT = 2;                                  // Right player -> 3rd index
        
        private int leaderPlayer, currPlayer,                                // Leading player for this round (of 16 tricks),  The player who is next to make a move
                    tricksPlayed, trickCount;                                // Number of tricks played thus far, number of cards played in the current trick.
                        
        private int[] scoreboard;                                            // Scoreboard 
        private Card[][] tricks;                                             // Array to store trick information
        private ArrayList<Card>[] hands;                                     // Hands for each player (opponent hands are simulated).
        private ArrayList<Card> discarded;                                   // Discarded cards
        
        private boolean determinization;                                     // True if this WhistState simulates opponents hands.
        private boolean ESEflag;                                             // True if we are running a Exhausted Suit Evaluation (ESE) pruning strategy.
        private int suspectSuit;                                             // Index of suit that leader is suspected of discarding. 0 - Spades, 1 - Diamonds, 2 - Clubs, 3 - Hearts.
        private boolean[][] emptySuits;                                      // Store empty suit information in a matrix, Format: [playerID][SuitID]
        
        private Random rand;                                                 // Random for dealing
        
        /** Overloaded constructor (default)
         * @param agentThis Name of this agent.
         * @param agentLeft Name of the left agent.
         * @param agentRight Name of the right agent.
         */
        public WhistState(String curr, String left, String right) { this(curr,left,right,true); }
        /**
         * WhistState constructor which allows DSE to be set on/off
         * 
         * @param agentThis Name of this agent.
         * @param agentLeft Name of the left agent.
         * @param agentRight Name of the right agent.
         */
        public WhistState(String curr, String left, String right, boolean ese)
        {
            //this.leaderPlayer = 0;
            //this.currPlayer = 0;
            //this.tricksPlayed = 0;
            //this.trickCount = 0;
            this.suspectSuit = -99999;
            this.emptySuits = new boolean[PLAYERS][SUITS];          // init empty suit information matrix of size 3 (num players) * 4 (num suits)
            this.scoreboard = new int[]{0,0,0};                     // init and set scoreboard to 0,0,0
            this.tricks = new Card[MAX_CARDS][PLAYERS];     // init tricks as a matrix of size 16 (number of tricks) * 3 (cards played at each trick)
            this.hands = new ArrayList[PLAYERS];                // init hands array with size 3, the number of players.
            this.discarded = new ArrayList<>(DISCARD_CARDS);    // init arraylist of discarded cards (with init size 4).
            this.determinization = false;                           // current gamestate reflects actual game state that player can observe
            this.ESEflag = ese;
            this.rand = new Random();
        }
        
        /** 
         * Clone Constructor for WhistState
         * 
         * @param L Index reprepresenting who made the first move in the current round (0 = This agent, 1 = Left Agent, 2 = Right Agent).
         * @param C index representing which player makes the next move (0 = This agent, 1 = Left Agent, 2 = Right Agent).
         * @param sc An array containing the scores of each player.
         * @param tC Trick Counting variable.
         * @param tP How many tricks have been played.
         * @param tA A matrix representing the tricks being played and that have already been played.
         * @param h An array of ArrayLists representing the hands of the current player and the opponents.
         * @param discard An ArrayList representing any cards discarded by the current player
         * @param d A boolean which is true if this WhistState object represents a real state, false if representing a determinized, simulated state. 
         * @param empty Empty Suit matrix, recording any suits which a player has none of.
         * @param ese Flag for empty suit evaluation
         */
        private WhistState(int L, int C, int[] sc, int tC, int tP, Card[][] tA, ArrayList<Card>[] h, ArrayList<Card> discard, boolean d, boolean ese, int sus, boolean[][] empty)
        {
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
            this.discarded = new ArrayList<>(DISCARD_CARDS);
            for (Card c : discard)
                this.discarded.add(c);
            this.determinization = d;
            this.rand = new Random();
            this.ESEflag = ese;
            this.suspectSuit = sus;
            this.emptySuits = new boolean[PLAYERS][SUITS];
            for (int i=0; i<PLAYERS;i++)
                for (int j=0; j<SUITS;j++)
                    emptySuits[i][j] = empty[i][j];
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
                emptySuits = new boolean[PLAYERS][SUITS];           // RESET emptySuits matrix.
            }
        }
        
        /**
         * Clone and randomize the current game simulation, changing the hands of the two simulated players, keeping other parameters constant.
         * 
         * @return The new randomized gamestate.
         */
        public WhistState cloneAndRandomize() 
        {
            // Clones the current state with the same variables, EXCEPT for determinization which is set to true.
            WhistState returnState = new WhistState(leaderPlayer, currPlayer, scoreboard, trickCount, tricksPlayed, tricks, hands, discarded, true, ESEflag, suspectSuit, emptySuits);
            // Run the method to randomizeOpponents.
            returnState.randomizeOpponents();
            
            return returnState;
        }
        
        /**
         * Randomize opponents in the current game state (or simulation). Redistribute the cards of the left and right simulated players. Takes into account exhausted/empty suit data and accounts for this distribution, using the exhausted suit evalauting
         * information to inform the distribution of opponents hands in the simulation.
         */
        private void randomizeOpponents() throws IllegalStateException {
            ArrayList<Card> deck = new ArrayList<Card>(Arrays.asList(Card.values()));
            int deckSize;
            // Remove cards in agent's hand
            for (Card c: hands[THIS]) deck.remove(c);
            // Remove already played cards from the possible deck
            for (int i=0; i<tricksPlayed; i++)
                for (int j=0; j<PLAYERS; j++)
                    deck.remove(tricks[i][j]);
            for (int i=0; i<=2; i++)
                deck.remove(tricks[tricksPlayed][i]);
            deckSize = deck.size();
            // Remove any previously discarded card
            if (!discarded.isEmpty())
                for (Card c: discarded)
                    deck.remove(c);
            deckSize = deckSize-4;                                                  // Update deckSize counter, regardless of whether we have discarded 4 cards or not.
            
            int[] sizes = new int[PLAYERS];
            sizes[0] = deckSize;
            sizes[LEFT] = (deckSize / 2) + (deckSize % 2);                          // If there are an odd number of cards, the left hand will always have more cards than the right hand.
            sizes[RIGHT] = deckSize / 2;
            
            ArrayList<Card> emptySuitDeck = new ArrayList<Card>();
            int discardCount = 0;
            //----------------------------------------------------------------------   
            // Exhausted Suit Evaluator (DSE)
            //----------------------------------------------------------------------   
            if (ESEflag) {
                for (int i=0; i<SUITS-1; i++) {
                    int current = i;
                    if (suspectSuit >= 0)
                        current = suspectSuit+i%SUITS;
                    if (emptySuits[0][current]) {
                        Suit check = Suit.SPADES;
                        switch (current) {
                            case 0 : check = Suit.SPADES; break;
                            case 1 : check = Suit.DIAMONDS; break;
                            case 2 : check = Suit.CLUBS; break;
                            case 3 : check = Suit.HEARTS; break;
                        }
                        for (Card c : deck) {                                                                           // Loop over the entire deck of possibilities.
                            if (c.suit == check) {
                                emptySuitDeck.add(c);
                            }
                        }
                        if (emptySuitDeck.isEmpty()) continue;                                                          // Continue to the next suit if the (remaining) deck has no cards of this suit.
                        
                        final int DISCARD = 3;
                        int addToPlay = -1;
                        if (emptySuits[LEFT][current] && !emptySuits[RIGHT][current]) addToPlay = RIGHT;
                        else if (!emptySuits[LEFT][current] && emptySuits[RIGHT][current]) addToPlay = LEFT;
                        else if (emptySuits[LEFT][current] && emptySuits[RIGHT][current]) addToPlay = DISCARD;
                        
                        if (leaderPlayer != THIS && discardCount != DISCARD_CARDS) {                                   // Skip this if we are the leading player, (we already know the discarded set with 100% certainty).
                            // If we were the leading player (discarded 4 cards), we know for certain that when an opponent has none of a suit in their hand, that the rest of the cards of that suit remaining in the pool must belong
                            // to the other opponent. 
                        
                            // If we are not the leading player, there is a level of uncertainty related to discards, as the leading player might have discarded between 0-4 of the cards of this suit. 
                            // Uses probability values generated by analysis of card hand distributions.
                            // Only fully resolved when both opponents exhaust a suit. 
                            //          DLS [Observed]                 |      Random 
                            // 0 hands | 0.85933                        |       0.0423 
                            // 1 hands | 0.04                            |       0.30121 
                            // 2 hands | 0.00254                        |       0.52381 
                            // 3 hands | 0.04                           |       0.1325 
                            // 4 hands | 0                             |       1.0E-4                 
                            // Values still not fully finetuned yet but works with some level of accuracy
                            
                            double four_L = 0.85933;
                            double three_L = four_L+0.04;
                            double two_L = three_L+0.0025;
                            double one_L = two_L+0.04;
                        
                            double four_N = 0.0423;
                            double three_N = four_N + 0.30121;
                            double two_N = three_N + 0.52381;
                            double one_N = two_N + 0.1325;
                            double eval = rand.nextDouble();
                            int remove = 0;
                            
                            if (current != 0 && current == suspectSuit && emptySuits[leaderPlayer][current]) {                          // Run with probability distribution L if this is the suspected (non-trump) suit that the leader has discarded from. 
                                if (eval < four_L && discardCount < DISCARD_CARDS)             remove = DISCARD_CARDS;                  // remove 4 with probability 0.85933   
                                else if (eval < three_L && discardCount < DISCARD_CARDS-1)     remove = DISCARD_CARDS-1;                // remove 3 with probability 0.04
                                else if (eval < two_L && discardCount < DISCARD_CARDS-2)       remove = DISCARD_CARDS-2;                // remove 2 with probability 0.0025
                                else if (eval < one_L && discardCount < DISCARD_CARDS-3)       remove = DISCARD_CARDS-3;                // remove 1 with probability 0.04
                                else remove = 0;                                                                                        // remove 0 with probability 0
                                if (remove < 0) remove = 0;
                            } else {
                                if (eval < four_N  && discardCount < DISCARD_CARDS)             remove = DISCARD_CARDS;                   // remove 4 with probability 0.0423
                                else if (eval < three_N && discardCount < DISCARD_CARDS-1)     remove = DISCARD_CARDS-1;                 // remove 3 with probability 0.30121
                                else if (eval < two_N && discardCount < DISCARD_CARDS-2)       remove = DISCARD_CARDS-2;                 // remove 2 with probability 0.52381 
                                else if (eval < one_N && discardCount < DISCARD_CARDS-3)       remove = DISCARD_CARDS-3;                 // remove 1 with probability 0.1325
                                else remove = 0;                                                                                         // remove 0 with probability 1.0E-4   
                                if (remove < 0) remove = 0;
                            }
                            discardCount += remove;                                                                                      // Add to discordCounter (to prevent overdiscarding).
                            // Lower bound of remove value - Must discard enough cards to fit into a valid hand.
                            int elementsToAdd = emptySuitDeck.size() - remove;
                            if (addToPlay != DISCARD) {
                                 remove = elementsToAdd - sizes[addToPlay];
                            }
                            else if (addToPlay == DISCARD) {
                                discarded.addAll(emptySuitDeck);
                                if ( discarded.size() > DISCARD_CARDS) throw new IllegalStateException("Illegal State: randomizeOpponents() - Discarded collection has more than"+DISCARD_CARDS+"elements");
                            }
                            
                            double P = 1.1;
                            int index = -1;
                            int emptySize = emptySuitDeck.size();
                            // DISCARD LOOP
                            for (int j=0; j<remove;j++) {                                                                                // From the remaining cards of the empty suit, select (using log distribution) 
                                if (!emptySuitDeck.isEmpty()) {                                                                          // if this deck is not empty.;
                                    index = (int) Math.floor(Math.log((rand.nextDouble()*(Math.pow(P, emptySuitDeck.size())-1.0))+1.0) / Math.log(P));   // log function temporarily adapted from [https://stackoverflow.com/questions/30720103/generate-random-numbers-with-logarithmic-distribution-and-custom-slope]
                                    //index = rand.nextInt(emptySize-j);                                                                                                       // WIP
                                    emptySuitDeck.remove(index);
                                }
                            }
                        }
                        if (addToPlay != DISCARD) {
                             hands[addToPlay].addAll(emptySuitDeck); 
                             sizes[addToPlay] = sizes[addToPlay] - emptySuitDeck.size();
                        }
                        deck.removeAll(emptySuitDeck);                                                                      // Remove the empty suit cards from the deck
                            /*System.out.print("Empty Suit :");
                            for (Card c : emptySuitDeck) System.out.print(c+", ");
                            System.out.println();*/
                        emptySuitDeck.clear();
                    }
                }
            }
            //----------------------------------------------------------------------   
            int index = 0;
            // Distribute the remaining cards to both players.
            for (int i=0; i<sizes[LEFT]; i++) {
                index = rand.nextInt(deck.size());
                hands[LEFT].add(deck.remove(index));
            }
            for (int i=0; i<sizes[RIGHT];i++) {
                index = rand.nextInt(deck.size());
                hands[RIGHT].add(deck.remove(index));
            }
           
            Collections.sort(hands[LEFT]);
            Collections.sort(hands[RIGHT]);
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
            
            if (determinization || currPlayer == 0)
                hands[currPlayer].remove(move);                                                     // Remove this Card from the player's hand.
            
            tricks[tricksPlayed][trickCount] = move;                                                // Simulate the move
            
            // do further analysis - any opponent modelling/analysis goes here
            //---------------------------------------
            if (ESEflag)                                                                                // only run if the ESE flag is set to true.
                //if (!determinization)                                                                 // Only run this evaluation for real game states. Disabling this means that function also acts as a move pruner for MCTS. 
                    emptySuitEval(move);                                                                // Evaluate if the last player played a card out of suit (ruff/sluff) which can only occur iff they have no cards of that suit.
            //---------------------------------------
            
            if (!endCheck())                                                                      // Check if the trick/round has concluded as a result of this move.
                currPlayer = (currPlayer+1)%PLAYERS;                                                // Update current player to the next player. 
            trickCount = (trickCount+1)%PLAYERS;                                                   // Add +1 to trickCount variable to move the tricks array window to the next element.
        }
        
        /**
         * Runs the first part of the ESE (Exhausted Suit Evaluation) pruning strategy. When this method is called (by doMove) Examines the last card played and compares it to the first card. 
         * If the cards differ in suits, set the appropriate vaue in the boolean matrix emptySuits to record that the current player has run out of cards with the same suit as the leader.
         * 
         * @param latest The last card that was played
         */
        private void emptySuitEval(Card latest)
        {
            if ( trickCount == 0 || currPlayer == 0) return;                                                 // Skip over entire evaluation if we are beginning a trick or if current player is this agent.
            Card first = tricks[tricksPlayed][0];
            if ( first.suit != latest.suit) {                                                                // If the player has played a card out of suit...
                int emptySuit = -1;
                switch (first.suit) {
                    case SPADES : emptySuit = 0; break;
                    case DIAMONDS : emptySuit = 1; break;
                    case CLUBS : emptySuit = 2; break;
                    case HEARTS : emptySuit = 3; break;
                }
                if (leaderPlayer != 0) {
                    for (int i=1; i<SUITS; i++) {
                        if (emptySuits[leaderPlayer][i] != false) break;
                        if (i==SUITS) suspectSuit = emptySuit;
                    }
                }
                emptySuits[0][emptySuit] = true;
                emptySuits[currPlayer][emptySuit] = true;
            }
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
        public boolean isTerminal() 
        {
            return (tricksPlayed == MAX_CARDS);
        }
    }
    
    private class SuitPQ implements Comparable<SuitPQ>
    {
        private Suit suit;
        private int priority;
        
        public SuitPQ(Suit s, int p) 
        {
            this.suit = s;
            this.priority = p;
        }
        @Override
        public int compareTo(SuitPQ sq)
        {
            return priority - sq.getPriority();
        }
        
        public int getPriority() { return this.priority;}
        public Suit getSuit() { return this.suit;}
    }
    
    /**
      * An interface for tree nodes for the tree that is built whilst conducting an Information Set Monte Carlo Tree Search, for the trick-playing card game Moss Side Whist.
      * @author Evan Huang
      */
    public interface WNode {
        /** 
         * Update score during back propogating step.
         * <ul>
         * <li>Called from the BACKPROPOGATE stage of the ISMCTS algorithm. </li>
         * </ul> 
         * @param r Result to set this node to.
         */
        public void update(int r);
        
        /**
         * Determine if the current node has child nodes.
         * 
         * @return True if this node has no child nodes, false otherwise.
         */
        public boolean hasChildren();
        
        /**
         * Check if the current node has not been fully expanded, given a determinization d.
         *
         * @param d WState representing a random determinization of the game state.
         * @return True if there are still moves in this determinization that have not been explored. False otherwise.
         */
        public boolean hasMovesToTry(WState d);
        
        /**
         * Selects which node to move to. 
         * <ul>
         * <li>Called from the SELECT stage of the ISMCTS algorithm. </li>
         * <li>Implement Tree Policy (SO-ISUCT) [Single Observer Information Set Upper Confidence Bound for Trees] to determine which node to select next. </li>
         * </ul>
         */
        public WNode selectChild();
        
        /**
         * Suggest an unexplored play to expand the tree to, given a random determinization d. 
         * <ul>
         * <li>Called from the EXPAND stage of the ISMCTS Algorithm.</li>
         * <li>Selects using a RANDOM policy</li>
         * </ul>
         * 
         * @param d A random determinization of the game state.
         * @return The recommended card to play.
         */
        public Card selectUntriedMove(WState d) throws IllegalStateException;
        
        /**
         * Add new children to tree.
         * 
         * @param play The card that was played.
         * @return The new child node.
         */
        public WNode addChild(Card play);
        
        /**
         * Suggest what the next card the agent should play next. 
         * <ul>
         * <li>Called AFTER the search has concluded</li>
         * <li>Selects the node with the highest reward, i.e. a MAX policy.</li>
         * </ul>
         */
        public Card getBestMove();
        
        /**
         * Accessor method that returns  the card (move) that led to this node
         * 
         * @return the move that led to this node
         */
        public Card getLastPlay();
        
        /**
         * Accessor method that returns parent of this node
         * 
         * @return parent of this node
         */
        public WNode getParent();
    }
    
    public interface WState {
         /**
         * Method to initialise OR reset variables after being called during dealing stage of the current round. 
         * 
         * @param hand List representing the hand of the current player.
         * @param order Index of the current player's position relative to the leader. (0 = Leader, 1 = Left, 2 = Right).
         */
        public void setupDeal(List<Card> hand, int order) throws IllegalArgumentException;
        
        /**
         * Clone and randomize the current game simulation, changing the hands of the two simulated players, keeping other parameters constant.
         * 
         * @return The new randomized gamestate.
         */
        public WState cloneAndRandomize();
        
        /** 
         * Discard cards for the given player.
         * 
         * @param player the player who is discarding cards.
         * @param index Index of the card to be discarded.
         */
        public void discard(int player, int index);
        
        /** 
         * Discard cards for the given player.
         * 
         * @param player the player who is discarding cards.
         * @param c the card to be discarded.
         */
        public void discard(int player, Card c);
       
        /**
         * Simulate playing a card, given the current game state.
         * 
         * @param move The card to play.
         */
        public void doMove(Card move) throws IllegalStateException;
        
         /**
         * Run termination evaluation of the current (terminal) game state.
         * 
         * @return 1 if this agent wins this simulation, else returns 0.
         */
        public int getResult() throws IllegalStateException;
        
        /**
         * Using the default (rollout) policy, choose the next move to play.
         */
        public Card getSimulationMove() throws IllegalStateException;
        
        /**
         * A method which given the current game state, returns a set of valid moves according to the rules of moss side whist.
         * (i.e. Can only play cards which are in your hand, must play card of same suit, etc.)
         * 
         * @return An arrayList of moves which are valid, given the current game state.
         */
        public ArrayList<Card> getValidMoves() throws IllegalStateException;
        
        /**
         * Checks if the simulated game is in a terminal state.
         * 
         * @return true if this game state is terminal, else false.
         */
        public boolean isTerminal();
    }
}
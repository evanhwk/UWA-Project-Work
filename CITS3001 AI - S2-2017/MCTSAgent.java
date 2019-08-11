import java.util.*;
/**
 * A class implementing the Information Sets Monte Carlo Search based AI for the trick-playing card game Moss Side Whist.
 * @author Evan Huang 20916873
 * @version 3.0
 */
public class MCTSAgent implements MSWAgent {
    private String name;                                                          // Name of this agent
    private static final int LEADER = 0;
    private static final int LEFT = 1;
    private static final int RIGHT = 2;
    private static final int SEARCH_TIME = 5;
    
    private List<Card> hand;                                                      // This agent's hand.
    private int order;                                                            // Initial playing order
    private WhistStateCheat rootState;                                                 // WhistState object representing this agent's view of the game world.
    
    //private int iterationCount;                                                 // Counts the number of MC Search() iterations before hitting computational limit.
    boolean DLSflag;                                                                // Whether to run random or discard lowest suit discard.
    
    private Random rand;                                                          // Random object used to generate names, uniform distributions, etc.
    /**
    * Constructor for agent (with default naming parameters).
    */
    public MCTSAgent() 
    {
        this.rand = new Random();
        this.name = "AgentMCTS_IS ";
        for (int i = 0; i<3; i++)
            name+=(char)('A'+rand.nextInt(26));
        this.DLSflag = true;
    }
    
    /**
    * Constructor for named agent.
    * @param name The name of this agent.
    */
    public MCTSAgent(String name) 
    {
        this.rand = new Random();
        this.name = name;
        this.DLSflag = true;
    }
    
    /**
    * Tells the agent the names of the competing agents, and their relative position.
    * 
    * @param agentLeft The name of the left agent.
    * @param agentRight The name of the right agent.
    * */
    public void setup(String agentLeft, String agentRight)
    {
        this.rootState = new WhistStateCheat(this.name, agentLeft, agentRight);
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
        rootState.setupDeal(hand, order);
        this.hand  = hand;
        this.order = order;
    }
    
    public void seeOpponents(List<Card> left, List<Card> right)
    {
        rootState.setupOpponents(left, right);
    }
    
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
                    rootState.discard(0, discard[i]);
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
                    rootState.discard(0, discardIndex);                         // UPDATE game state
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
        Card play = Search(SEARCH_TIME);
        //System.out.println(iterationCount);
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
        rootState.doMove(card);
    }
    
    /**
    * See the result of the trick. 
    * A 50 ms timelimit is given to this method.
    * This method will be called on each agent at the end of each trick.
    * @param winner The player who played the winning card.
    * */
    public void seeResult(String winner)
    {
        //Function unused
        rootState.validateWinner(winner);
    }
    
    /**
    * See the score for each player.
    * A 50 ms timelimit is givien to this method
    * @param scoreboard, a Map from agent names to their score.
    **/
    public void seeScore(Map<String, Integer> scoreboard)
    {
        rootState.validateScores(scoreboard);
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
    * Run the MC Tree Search algorithm.
    * <ul>
    * <li> Start by generating the start of the MC Tree Search </li>
    * <li> SELECT (go down the tree) the best node UNTIL (1) a node with unvisited children OR (2) a terminal node </li>
    * <li> EXPAND a partially expanded node, adding one or more child nodes to expand the tree </li>
    * <li> From a child node, ROLLOUT the default/rollout policy until we reach an endgame state. </li>
    * <li> BACKPROPOGATE the result at this endgame state up the tree, from child to parent, to the root. </li>
    * </ul>
    * [Adapted following pseudocode of ISMCTS framework from Lanzi (2014)]
    * 
    * @param time Time (in ms) the search is allowed to run for. 
    * @return the recommended card to be played.
    * @see <a href="Monte Carlo Tree Search Algorithms applied to the card game Scopone">http://teaching.csse.uwa.edu.au/units/CITS3001/project/2017/paper1.pdf</a>
    */ 
    private Card Search(long time)
    {
        //iterationCount = 0;
        long start = System.currentTimeMillis();
        long end = start;
        WNode rootNode = new WhistNodeCheat(null, null);                           // Initialise the tree with a root node          
        while ((end-start) < time) {                                              // While we are within the time limit execute this.
        //for (int i=0; i<repeat; i++) {
          WNode currNode = rootNode;
          WState currState = rootState.clone();
          /* SELECT
           * Starting from root node, recursively selects most urgent node, until a node is reached that represents:
           *   (A) A node that is not fully expanded yet
           *   (B) A terminal node                                                                                  */ 
          while (!currNode.hasMovesToTry(currState) && currNode.hasChildren()) { // while !(A) && !(B)
              currNode = currNode.selectChild();                   // use UCT algorithm to select next node to select [in WhistNode.selectChild()].
              currState.doMove(currNode.getLastPlay());                              // make (simulated) move
          }
          /* EXPAND / Tree Policy
           * Once we reach a partially expanded node, add one or more child nodes and expand the tree               */ 
          if (currNode.hasMovesToTry(currState)) {      // if (A)
              Card move = currNode.selectUntriedMove(currState);            // choose which untried move to expand the tree to.
              currState.doMove(move);                                       // make (simulated) move
              currNode = currNode.addChild(move);                               // add a new node to the tree
          }
          /* ROLLOUT / Default Policy
           * Run a simulation from the new nodes according to the default policy to produce an outcome (or reward). */
          while (!currState.isTerminal()) {                                 // LOOP until terminal state
              currState.doMove(currState.getSimulationMove());          // make (simulated) move using the default/rollout policy.
          }
          int finalReward = currState.getResult();
          /* BACKPROPOGATE
           * Rewards are backpropogated through the previously selected nodes to update their statistics.           */
          while (currNode != null) {
              currNode.update(finalReward);                                     // N(n) <- N(n)+1, Q(n) <- Q(n)+delta
              currNode = currNode.getParent();                                       // n <- parent of n
          }
          end =  System.currentTimeMillis();
          //iterationCount++;
        }
        return rootNode.getBestMove();
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
}
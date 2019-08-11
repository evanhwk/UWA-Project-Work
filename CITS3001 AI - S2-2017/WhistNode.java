import java.util.*;
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
     * <li>Selects higher valued cards first, and trump suit first.</li>
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

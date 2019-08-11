import java.util.*;
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
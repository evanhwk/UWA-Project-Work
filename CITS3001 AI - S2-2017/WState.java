import java.util.*;

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
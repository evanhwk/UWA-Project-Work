import java.util.*;
import java.io.*;
/**
 * A class to implement a human player for the trick-playing card game Moss Side Whist.
 * @author Evan Huang
 * @version 1.0
 */
public class HumanAgent implements MSWAgent{
    private String name;                                                          // Name of this agent 
    private String nameL;                                                         // Name of left agent 
    private String nameR;                                                         // Name of right agent
    private static final int LEADER = 0;
    private static final int LEFT = 1;
    private static final int RIGHT = 2;
    
    private List<Card> hand;                                                      // This agent's hand.
    private int order;                                                            // Initial playing order
    private Scanner keyboard;                                                     // Scanner for keyboard input
    private PrintStream display;                                                  // Printstream to display output to user
    
    private Random rand = new Random();
    
    /**
     * Constructor for the HumanAgent Class
     */
    public HumanAgent() 
    {
        this.keyboard = new Scanner(System.in);
        this.display = System.out;
        this.name = "HumanAgent";
        for (int i = 0; i<3; i++)
            name+=(char)('A'+rand.nextInt(26));
    }
    
    /**
     * Tells the agent the names of the competing agents, and their relative position.
     */
    public void setup(String agentLeft, String agentRight)
    {
        display.println("Your left opponent is: "+agentLeft+" Your Right opponent is: "+agentRight);
    }
    
    /**
     * Starts the round with a deal of the cards.
     * The agent is told the cards they have (16 cards, or 20 if they are the leader)
     * and the order they are playing (0 for the leader, 1 for the left of the leader, and 2 for the right of the leader).
     */
    public void seeHand(List<Card> hand, int order) 
    {
        switch (order) {
            case LEADER: display.println("You are the leader"); break;
            case LEFT: display.println("You are left of the leader"); break;
            case RIGHT: display.println("You are right of the leader"); break;
        }
        
        int i = 0;
        for (Card c: hand) {
            display.print(c.toString());
            if (++i<16) System.out.print(",");
            else display.println();
        }
        
        this.hand  = hand;
        this.order = order;
    }
    
    public void seeOpponents(List<Card> left, List<Card> right) {}
    /**
     * This method will be called on the leader agent, after the deal.
     * If the agent is not the leader, it is sufficient to return an empty array.
     */
    public Card[] discard()
    {
        if (this.order!=0) return new Card[0];
        else {
            Card[] discard = new Card[4];
            display.println("Select 4 Cards to Discard");
            int i=0;
            for (Card c : hand) {
                System.out.print("["+i+"|");
                System.out.print(c+"] ");
                i++;
            }
            System.out.println();
            for(int j= 0; j<4;j++) {
                int d = keyboard.nextInt();
                while (d >= hand.size()) {
                    System.out.println("ERROR: Out of array index. Try again.");
                    d = keyboard.nextInt();
                }
                display.println("Discarding "+hand.get(d)+" ...");
                discard[j] = hand.get(d);
                hand.remove(d);
                i=0;
                for (Card c : hand) {
                    System.out.print("["+i+"|");
                    System.out.print(c+"] ");
                    i++;
                }
                display.println();
            }
            return discard;
        }
    }
    
    /**
     * Agent returns the card they wish to play.
     * A 200 ms timelimit is given for this method
     * @return the Card they wish to play.
     */
    public Card playCard()
    {
        int i=0;
        for (Card c : hand) {
            System.out.print("["+i+"|");
            System.out.print(c+"] ");
            i++;
        }
        display.println();
        display.println("Choose a card to play");
        int p = keyboard.nextInt();
        while (p >= hand.size()) {
            display.println("ERROR: Out of array index. Try again");
            p = keyboard.nextInt();
        }
        Card play = hand.get(p);
        hand.remove(p);
        return play;
    }
    
    /**
     * Sees an Agent play a card.
     * A 50 ms timelimit is given to this function.
     * @param card, the Card played.
     * @param agent, the name of the agent who played the card.
     */
    public void seeCard(Card card, String agent)
    {
        display.println("Agent "+agent+" playing "+card.toString());
    }
    
    /**
    * See the result of the trick. 
    * A 50 ms timelimit is given to this method.
    * This method will be called on each agent at the end of each trick.
    * @param winner, the player who played the winning card.
    * */
    public void seeResult(String winner)
    {
        display.println("Agent "+winner+" won this trick");
    }
    
    /**
    * See the score for each player.
    * A 50 ms timelimit is givien to this method
    * @param scoreboard, a Map from agent names to their score.
    **/
    public void seeScore(Map<String, Integer> scoreboard)
    {
        display.println("Current Scores");
        display.println(name+": "+scoreboard.get(name));
        display.println(nameL+": "+scoreboard.get(nameL));
        display.println(nameR+": "+scoreboard.get(nameR));
    }
    
    /**
    * Returns the Agents name.
    * A 10ms timelimit is given here.
    * This method will only be called once.
    * */
    public String sayName()
    {
        String name= "";
        for(int i = 0; i<6; i++)
            name+=(char)('A'+rand.nextInt(26));
        return name;
    }

}
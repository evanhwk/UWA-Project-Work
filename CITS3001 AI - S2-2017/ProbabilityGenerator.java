import java.util.*;
/**
 * A class to test and generate probabities of certain card-hands. (Mainly to avoid doing really horrible probability calculations)
 *
 * @author Evan Huang 20916873
 * @version 1.2
 */
public class ProbabilityGenerator
{
    private ArrayList<Card> leader;
    private ArrayList<Card> left;
    private ArrayList<Card> right;
    private Random rand;
    private Card[] discarded;
    
    int[] ranks;
    int four;
    int three;
    int two;
    int one;
    int zero;
    int dealCounter;
    int emptyLeader;
    /**
     * Probability Generator Constructor
     */
    public ProbabilityGenerator()
    {
        rand = new Random();
        ranks = new int[15];
    }

    /**
     * Create a new deal
     */
    private void deal(int policy)
    {
        leader = new ArrayList<>();
        left = new ArrayList<>();
        right = new ArrayList<>();
        
        ArrayList<Card> deck = new ArrayList<Card>(Arrays.asList(Card.values()));
        for(int i = 0; i<4; i++)
        leader.add(deck.remove(rand.nextInt(deck.size())));
        for(int i = 0; i<16; i++) {
            left.add(deck.remove(rand.nextInt(deck.size())));
            right.add(deck.remove(rand.nextInt(deck.size())));
            leader.add(deck.remove(rand.nextInt(deck.size())));
        }
        Collections.sort(leader);
        Collections.sort(left);
        Collections.sort(right);
        discarded = discard(policy);
        analyse();
        dealCounter++;
    }
    
    private Card[] discard(int policy)
    {
        if (policy==1) {
            // RANDOM DISCARD POLICY
            Card[] discard = new Card[4];
            for (int i = 0; i<4;i++) {
                 int discardIndex = rand.nextInt(20-i);
                 discard[i] = leader.remove(discardIndex);
            }
            return discard;
        }
        else if (policy==2) {
            // SUIT BREAK DISCARD 
            HashMap<Suit, ArrayList<Card>> suits = new HashMap<Suit, ArrayList<Card>>();
            suits.put(Suit.SPADES, new ArrayList<Card>());
            suits.put(Suit.DIAMONDS, new ArrayList<Card>());
            suits.put(Suit.CLUBS, new ArrayList<Card>());
            suits.put(Suit.HEARTS, new ArrayList<Card>());
            for (Card c : leader) {
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
            lowSuit.add(new SuitPQ(Suit.SPADES, 100));
            
            Suit match = lowSuit.peek().getSuit();
            ArrayList<Card> discardOrder = new ArrayList<Card>(); 
            emptyLeader--;
            while (discardOrder.size() < 4) {
                match = lowSuit.poll().getSuit();
                Collections.reverse(suits.get(match));
                discardOrder.addAll(suits.get(match));
                emptyLeader++;
            }
            
            Card[] discard = new Card[4];
            for (int i=0; i<4; i++) {
                discard[i] = discardOrder.remove(0);
                leader.remove(discard[i]);
            }
            return discard;
        }
        else if (policy==3) {
            // SUIT BREAK DISCARD + AVOID HIGH
            HashMap<Suit, ArrayList<Card>> suits = new HashMap<Suit, ArrayList<Card>>();
            suits.put(Suit.SPADES, new ArrayList<Card>());
            suits.put(Suit.DIAMONDS, new ArrayList<Card>());
            suits.put(Suit.CLUBS, new ArrayList<Card>());
            suits.put(Suit.HEARTS, new ArrayList<Card>());
            for (Card c : leader) {
                if (c.rank >= 12) continue;
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
            lowSuit.add(new SuitPQ(Suit.SPADES, 100));
            
            Suit match = lowSuit.peek().getSuit();
            ArrayList<Card> discardOrder = new ArrayList<Card>(); 
            emptyLeader--;
            while (discardOrder.size() < 4) {
                match = lowSuit.poll().getSuit();
                Collections.reverse(suits.get(match));
                discardOrder.addAll(suits.get(match));
                emptyLeader++;
            }
            
            Card[] discard = new Card[4];
            for (int i=0; i<4; i++) {
                discard[i] = discardOrder.remove(0);
                leader.remove(discard[i]);
            }
            return discard;
        }
        else return new Card[0];
    }
    
    private void analyse()
    {
        int spades[] = new int[3];
        int diamonds[] = new int[3];
        int hearts[] = new int[3];
        int clubs[] = new int[3];
        
        for (Card c : leader) {
            switch (c.suit) {
                case SPADES: spades[0]++; break;
                case HEARTS: hearts[0]++; break;
                case CLUBS: clubs[0]++; break;
                case DIAMONDS: diamonds[0]++; break;
            }
        }
        
        for (Card c : left) {
            switch (c.suit) {
                case SPADES: spades[1]++; break;
                case HEARTS: hearts[1]++; break;
                case CLUBS: clubs[1]++; break;
                case DIAMONDS: diamonds[1]++; break;
            }
        }
        
        for (Card c : right) {
            switch (c.suit) {
                case SPADES: spades[2]++; break;
                case HEARTS: hearts[2]++; break;
                case CLUBS: clubs[2]++; break;
                case DIAMONDS: diamonds[2]++; break;
            }
        }
        int lowest = 13;
        for (int c : clubs) {
            if (c<lowest) lowest = c;
        }
        for (int d : diamonds) { 
            if (d<lowest) lowest = d;
        }
        for (int h : hearts) { 
            if (h<lowest) lowest = h;
        }
        for (int s : spades) { 
            if (s<lowest) lowest = s;
        }
        
        switch (lowest) {
            case 0 : zero++; break;
            case 1 : one++; break;
            case 2 : two++; break;
            case 3 : three++; break;
            case 4 : four++; break;
            default : break;
        }
        
        for (Card c : discarded) {
            int i = c.rank;
            ranks[i]++;
            ranks[0]++;
        }
    }
    
    public static void main(String[] args){
        ProbabilityGenerator PG = new ProbabilityGenerator();
        int zero = 0;
            int one = 0;
            int two = 0;
            int three =0;
            int four = 0;
            int five = 0;
            int test = 0;
        for (int i=0; i < 1000; i++) {
            PG.deal(1);
            int max = 13;
            double p = 1.1;
            int idx = (int) Math.floor(Math.log((Math.random()*(Math.pow(p, max)-1.0))+1.0) / Math.log(p)); 
            
            if (idx == 0) zero++;
            if (idx == 1) one++;
            if (idx == 2) two++;
            if (idx == 3) three++;
            if (idx == 4) four++;
            if (idx == 5) five++;
            if (idx == 12) test++;
        }
        
        int total = zero+one+two+three+four+test+five;
            System.out.println(zero);
            System.out.println((double)zero/total);
            System.out.println(one);
            System.out.println((double)one/total);
            System.out.println(two);
            System.out.println((double)two/total);
            System.out.println(three);
            System.out.println((double)three/total);
            System.out.println(four);
            System.out.println((double)four/total);
            System.out.println(five);
            System.out.println((double)five/total);
            System.out.println(test);
            System.out.println((double)test/total);
            System.out.println(total);
            
        System.out.println("0 Hands "+PG.zero+" Prob: "+(double)PG.zero/PG.dealCounter);
        System.out.println("1 Hands "+PG.one+" Prob: "+(double)PG.one/PG.dealCounter);
        System.out.println("2 Hands "+PG.two+" Prob: "+(double)PG.two/PG.dealCounter);
        System.out.println("3 Hands "+PG.three+" Prob: "+(double)PG.three/PG.dealCounter);
        System.out.println("4 Hands "+PG.four+" Prob: "+(double)PG.four/PG.dealCounter);
        System.out.println("Leader has created "+PG.emptyLeader+" new empty suits");
        System.out.println("Total Deals "+PG.dealCounter);
        
        System.out.println("A : "+PG.ranks[14]+" Prob: "+(double)PG.ranks[14]/PG.ranks[0]);
        System.out.println("K : "+PG.ranks[13]+" Prob: "+(double)PG.ranks[13]/PG.ranks[0]);
        System.out.println("Q : "+PG.ranks[12]+" Prob: "+(double)PG.ranks[12]/PG.ranks[0]);
        System.out.println("J : "+PG.ranks[11]+" Prob: "+(double)PG.ranks[11]/PG.ranks[0]);
        System.out.println("T : "+PG.ranks[10]+" Prob: "+(double)PG.ranks[10]/PG.ranks[0]);
        System.out.println("9 : "+PG.ranks[9]+" Prob: "+(double)PG.ranks[9]/PG.ranks[0]);
        System.out.println("8 : "+PG.ranks[8]+" Prob: "+(double)PG.ranks[8]/PG.ranks[0]);
        System.out.println("7 : "+PG.ranks[7]+" Prob: "+(double)PG.ranks[7]/PG.ranks[0]);
        System.out.println("6 : "+PG.ranks[6]+" Prob: "+(double)PG.ranks[6]/PG.ranks[0]);
        System.out.println("5 : "+PG.ranks[5]+" Prob: "+(double)PG.ranks[5]/PG.ranks[0]);
        System.out.println("4 : "+PG.ranks[4]+" Prob: "+(double)PG.ranks[4]/PG.ranks[0]);
        System.out.println("3 : "+PG.ranks[3]+" Prob: "+(double)PG.ranks[3]/PG.ranks[0]);
        System.out.println("2 : "+PG.ranks[2]+" Prob: "+(double)PG.ranks[2]/PG.ranks[0]);
        

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

import java.util.List;
import java.util.Map;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.Arrays;
import java.util.ArrayList;
import java.util.PriorityQueue;
import java.util.Stack;
import java.util.Set;

/**
*   CITS2200 Algorithms Project
*   @author Evan Huang 20916873
*/

public class CITS2200Project { //implements CITS2200Project 
    //private  //<DEBUG>
    CITS2200Graph Graph;

    public CITS2200Project()
    {
        Graph = new CITS2200Graph();
    }

    /**
     * Adds an edge to the Wikipedia page graph. If the pages do not
     * already exist in the graph, they will be added to the graph.
     * 
     * @param urlFrom the URL which has a link to urlTo.
     * @param urlTo the URL which urlFrom has a link to.
     */
    public void addEdge(String urlFrom, String urlTo)
    {
        Graph.addEdge(urlFrom, urlTo);
    }

    /**
     * Finds the shoretst path in number of links between two pages.
     * 
     * @param urlFrom the URL where the path should start.
     * @param urlTo the URL where the path should end.
     * @return the length of the shortest path in number of links followed. -1 if no path.
     */
    public int getShortestPath(String urlFrom, String urlTo) 
    {
        return Dijkstra_SinglePair(urlFrom, urlTo);
    }

    /**
     * Finds all the centers of the page graph.
     * 
     * @return a string array containing all the URL strings that correspond to pages that are centers.
     */
    public String[] getCenters() 
    {
        int gSize = Graph.getSize();
        int minDist = Integer.MAX_VALUE;
        int minLinks = gSize/2+1;
        int currLinks, currMaxDist;
        Map<Integer, Integer> dist;
        ArrayList<String> pageCenterNames = new ArrayList<String>();
        
        for (int i = 0; i < gSize; i++)                             // For every page in the graph
        {
            dist = Dijkstra_SingleSource(i);                        // Solve single source problem for this page
            currMaxDist = 0;
            currLinks = dist.size();
            if (currLinks < minLinks) continue;
            for (Map.Entry<Integer, Integer> entry: dist.entrySet())
            {
                int currDist = entry.getValue();
                if (currMaxDist < currDist)
                    currMaxDist = currDist;
            }
            
            if (currMaxDist <= minDist && currMaxDist!= 0) 
            {
                if (currMaxDist < minDist)
                {
                    //System.out.println("ArrayList Cleared");  // <DEBUG>
                    pageCenterNames.clear();
                }
                minDist = currMaxDist;
                String url = Graph.getPageString(i);
                pageCenterNames.add(url);
                //System.out.print("["+url+"] | ");           // <DEBUG>
                //System.out.print("Links: "+currLinks+" | ");
                //System.out.println("minDist: "+currMaxDist);
            }
        }
        return pageCenterNames.toArray(new String[0]);
    }

    /**
     * Finds all the strongly connected components of the page graph.
     * Every strongly connected component can be represented as an array 
     * containing the page URLs in the component. The return value is thus an array
     * of strongly connected components. The order of elements in these arrays
     * does not matter. Any output that contains all the strongly connected
     * components is considered correct.
     * 
     * @return an array containing every strongly connected component.
     */
    public String[][] getStronglyConnectedComponents() 
    {
        Gabow G = new Gabow(Graph);
        
        return G.GabowRun();
    }

    /**
     * Finds a Hamiltonian path in the page graph. There may be many
     * possible Hamiltonian paths. Any of these paths is a correct output.
     * This method should never be called on a graph with more than 20
     * vertices. If there is no Hamiltonian path, this method will
     * return an empty array. The output array should contain the URLs of pages
     * in a Hamiltonian path. The order matters, as the elements of the
     * array represent this path in sequence. So the element [0] is the start
     * of the path, and [1] is the next page, and so on.
     * 
     * @return a Hamiltonian path of the page graph.
     */
    public String[] getHamiltonianPath() throws IllegalArgumentException
    {
        if (Graph.getSize() > 20) throw new IllegalArgumentException("Graph has too many verticies.");
        String[] result = BHK_HamiltonPath();
        return result;
    }
    
    /**
    *  Runs Dijkstra's algorithm on a directed, unweighted graph to find the shortest distance from the source wiki pages to the specified source wiki page to solve the "single pair" shortest path pproblem
    *  @param urlFrom the originating wiki page
    *  @param urlTo the destination wiki page
    *  @return an integer value of the shortest number of links from the source wikipage to the specified wikipage.
    */
    private int Dijkstra_SinglePair(String urlFrom, String urlTo) throws NullPointerException
    { 
        if (Graph == null) throw new NullPointerException("Graph is null.");
        //if (!Graph.containsKey(urlFrom)) throw new NullPointerException("Graph does not contain page url: "+source);
        int gSize = Graph.getSize();
        int[] dist = new int[gSize];
        int[] prev = new int[gSize];
        int from = Graph.getIndex(urlFrom);
        int to = Graph.getIndex(urlTo);
        PriorityQueue<WikiPage> Q = new PriorityQueue<WikiPage>();
        
        //for (int i = 0; i < gSize; i++) dist[i] = Integer.MAX_VALUE;
        Arrays.fill(dist, Integer.MAX_VALUE);
        
        // Initialise urlFrom at top of Q
        Q.offer(new WikiPage(from,0));
        dist[from] = 0;
        prev[from] = -1;
        
        int origin, dFromStart;
        while (!Q.isEmpty())
        {
            WikiPage originP = Q.poll();
            origin = originP.getIndex();
            //int priority = originP.getPriority();
            //System.out.println("Popped Priority Queue Item: ["+origin+","+priority+"]"); // <DEBUG>
            LinkedList<Integer> links = Graph.getEdge(origin);
            if (links == null) continue;
            for (int link : links)
            {
                dFromStart = dist[origin] + 1;  // Unweighted graph
                if (link == to)
                {
                    return dFromStart;
                }
                if (dFromStart < dist[link])
                {
                    Q.offer(new WikiPage(link,dFromStart));      // Add new item to queue
                    //System.out.println("New Priority Queue Item: ["+link+","+dFromStart+"]"); // <DEBUG> 
                    dist[link] = dFromStart;
                }
            }
        }
        //System.out.println(dist); // <DEBUG>
        return -1;
    }
    
    /**
    *  Runs Dijkstra's algorithm on a directed, unweighted graph to find the distances to all wiki pages from the specified source wiki page to solve the single source shortest path problem
    *  @param source the originating wiki page
    *  @return a map listing the distance to each vertex in the single source shortest path problem, or -1 if the vertex is not reachable from the source.
    */
    private HashMap<Integer, Integer> Dijkstra_SingleSource(int from) throws NullPointerException
    {
        if (Graph == null) throw new NullPointerException("Graph is null.");
        //if (!Graph.containsKey(source)) throw new NullPointerException("Graph does not contain page url: "+source);
        int gSize = Graph.getSize();
        
        HashMap<Integer, Integer> dist = new HashMap<Integer, Integer>();
        //HashMap<Integer, Integer> prev = new HashMap<Integer, Integer>();
        PriorityQueue<WikiPage> Q = new PriorityQueue<WikiPage>();

        // Initialise urlFrom at top of Q
        Q.offer(new WikiPage(from,0));
        dist.put(from,0);
        //prev.put(from,from);
        
        int origin, dFromStart;
        while (!Q.isEmpty())
        {
            WikiPage originP = Q.poll();
            origin = originP.getIndex();
            //int priority = originP.getPriority();
            //System.out.println("Popped Priority Queue Item: ["+origin+","+priority+"]"); // <DEBUG>
            LinkedList<Integer> links = Graph.getEdge(origin);
            if (links == null) continue;
            for (int link : links)
            {
                dFromStart = dist.get(origin) + 1;  // Unweighted graph
                if (!dist.containsKey(link) || dFromStart < dist.get(link))
                {
                    Q.offer(new WikiPage(link,dFromStart));      // Add new item to queue
                    //System.out.println("New Priority Queue Item: ["+Graph.getPageString(link)+","+dFromStart+"]"); // <DEBUG>
                    dist.put(link,dFromStart);
                    //prev.put(link,origin);
                }
            }
        }
        //System.out.println(dist); // <DEBUG>
        //System.out.println(prev);
        return dist;
    }

    /** 
     * Runs a variant of the Bellman/Held-Karp Subset Dynamic Programming algorithm to find the hamiltonian path that goes through every wiki page without visiting one page more than once.
     * 
     * @return a string array representing the verticies travelled in the order of the hamiltonian path
     */
    private String[] BHK_HamiltonPath() {
        int n = Graph.getSize();
        boolean[][] path = new boolean[n][1<<n];
        for (boolean[] a : path)
            Arrays.fill(a, false);
        for (int i = 0; i < n; i++)
            path[i][1 << i] = true;
            
        /*
        for (int subMask = 0; subMask < (1<<n); subMask++)          // for 0 to 2^n
        {
            for (int v = 0; v < n; v++)                           // for each vertex
            {
                if ( (subMask & 1<<v) != 0)                       // 2^n | n =/= 0
                {
                    for (int u = 0; u < n; u++)                   // check for all possible neighbours
                    {
                        if ( u!= v && (Graph.queryEdge(u,v) && path[u][subMask ^ (1<<v)]))            // subMask XOR 2^v - Examine previous related subsets
                        {
                            path[v][subMask] = true;
                            break;
                        }
                    }
                }
            }
        }                                                 // After this point, all subsets are solved
        */
        boolean hFound = false;
        for (int subMask = 0; subMask < (1<<n); subMask++)          // for 0 to 2^n
        {
            for (int u = 0; u < n; u++)                             // Loop through every vertex
            {
                LinkedList<Integer> edges = Graph.getEdge(u);
                for (int v : edges)                                 // For every degree/edge of this vertex
                {
                    if ((subMask & 1<<v) != 0 && path[u][subMask ^ (1<<v)]) // subMask XOR 2^v - Examine previous related subsets
                    {
                        path[v][subMask] = true;
                        if (subMask >= (1 << n) - 1) hFound = true;
                        break;
                    }
                }
                if (hFound == true) break;
            }
        }                                                           // After this point, all subsets are solved
        /* <DEBUG>
        for (int i = 0; i < n; i++) {
           for (int j = 0; j < (1 << n); j++) {
                if (path[i][j]) {
                    System.out.print("T");
                }
                else {
                    System.out.print("0");
                }
                for (int k = 0; k <= n; k++) {
                    if (j == ((1 << k) -1))
                    System.out.print("|");
                }
           }
           System.out.println();
        }
        */
       
        // Recreate path going backwards from the final destination vertex
        int cur = (1 << n) - 1;
        String[] hamilton = new String[n];
        for (int add = n - 1; add >= 0; add--) {
            int i = -1;                                                                // <DEBUG>
            for (int x = 0; x < n; x++) {
                if ( (cur & 1 << x) != 0 && path[x][cur])
                {
                     i = x;
                     break;
                }
            }
            if (i == -1) { 
                return new String[0];
            }
            hamilton[add] = Graph.getPageString(i);
            cur = cur ^ (1 << i);
        }
        return hamilton;
    }

    /** Class for implementation of the Gabow Algorithm for Path-based Depth-First Search algorithm utilising two stacks. */
    private class Gabow
    {
        private CITS2200Graph Graph;
        private int gSize;
        private int current;
        private int preCount;
        private int[] preOrder;
        private boolean[] visited;
        private boolean[] added;
        
        ArrayList<ArrayList<Integer>> strongComponentGroups;
        Stack<Integer> st1;
        Stack<Integer> st2;
        
        public Gabow(CITS2200Graph g)
        {
            Graph = g;
            gSize = Graph.getSize();
            
            preOrder = new int[gSize];
            visited = new boolean[gSize];
            added = new boolean[gSize];
            strongComponentGroups = new ArrayList<>();
            st1 = new Stack<Integer>();
            st2 = new Stack<Integer>();
        }
        
        /** Run the Gabow Algorithm 
         * @return A string 2D Array of strongly connected components */
        public String[][] GabowRun()                                                
        {
            for (int p = 0; p < gSize; p++)                                         // O(N)
            {
                if (!visited[p]) dfs(p);
            }
            
            int scgSize = strongComponentGroups.size();
            String[][] scgArray = new String[scgSize][];
            for (int i = 0; i < scgSize; i++)
            {
                 ArrayList<Integer> group = strongComponentGroups.get(i);
                 int groupSize = group.size();
                 String[] row = new String[groupSize];
                 for (int j = 0; j < groupSize; j++)
                 {
                    int id = group.get(j);
                    row[j] = Graph.getPageString(id);
                 }
                 scgArray[i] = row;
            }
            return scgArray;
        }
        
        /** Recursive method implementing DFS component of Gabow Algorithm */
        public void dfs(int u)                                                      // O(E)
        {
            visited[u] = true;
            preOrder[u] = preCount++;
            st1.push(u);
            st2.push(u);
            
            LinkedList<Integer> edges = Graph.getEdge(u);
            for (int v : edges)
            {
                if (!visited[v])
                    dfs(v);
                else if (!added[v]) {
                    // Pop every item off Stack 2 until v is at the top of the stack
                    while (preOrder[st2.peek()] > preOrder[v]) 
                        st2.pop();
                }
            }
            
            // If found component u
            if (st2.peek() == u)
            {
                st2.pop();
                ArrayList<Integer> list = new ArrayList<Integer>();
                // Pop every object off stack 2 between v -> u
                int v;
                do 
                {
                    v = st1.pop();
                    list.add(v);
                    added[v] = true;
                } 
                while (v != u);
                strongComponentGroups.add(list);       
            }
        }
        
    }
    
    private class WikiPage implements Comparable<WikiPage>
    {
        private int index;
        private int priority;

        public WikiPage(int i, int p)
        {
            this.index = i;
            this.priority = p;
        }
        
        public int getIndex() { return this.index; }
        public int getPriority() { return this.priority;}
        
        @Override
        public int compareTo(WikiPage p)
        {
            return this.priority - p.priority;
        }
    }
}

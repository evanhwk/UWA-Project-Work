import java.util.Arrays;
import java.util.ArrayList;
import java.util.List;
import java.util.LinkedList;
import java.util.Map;
import java.util.HashMap;
import java.util.PriorityQueue;
import java.util.Stack;
import java.util.Set;

/**
*   CITS2200 Algorithms Project
*   @author Evan Huang 20916873
*   @version 3.0.1
*/

public class MyCITS2200Project implements CITS2200Project
{
    /** Graph data structure */
    private WikiGraph Graph;
    
    /** Initialise the project */
    public MyCITS2200Project()
    {
        Graph = new WikiGraph();
    }
    
    /**
     * Initialise a random graph for testing
     * 
     * @param V The number of verticies (pages) in the grapph
     * @param E the number of edges (links) in the graph
     */
    public void testProject(int V, int E)
    {
        Graph.randomGraph(V,E);
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
     * Prints the wikipedia page graph to the console log.
     */
    public void printGraph()
    {
        Graph.printGraph();
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
        //return Dijkstra_getCenters();
        return FloydWarshall_getCenters();
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
        String[] result = new String[0];
        if (Graph.getSize() > 20) {
                System.err.println("Graph has too many verticies.");
        }
        else 
        {
            result = BHK_HamiltonPath();
        }
        return result;
    }
    
    /**
    *  Runs Dijkstra's algorithm on a directed, unweighted graph to find the shortest distance from the source wiki pages to the specified source wiki page to solve the "single pair" shortest path problem
    *  Solves Problem 1.
    *  @param urlFrom the originating wiki page
    *  @param urlTo the destination wiki page
    *  @return an integer value of the shortest number of edges from the source wikipage to the specified wikipage.
    */
    private int Dijkstra_SinglePair(String urlFrom, String urlTo)
    {
        if (Graph == null) {
            System.err.println("Graph is null.");
            return -1;
        }
        int from = Graph.getIndex(urlFrom);                                       // O(V)
        int to = Graph.getIndex(urlTo);                                           // O(V)
        if (from == -1) {                                                         // O(1)
            System.err.println("Graph does not contain page url: "+urlFrom);
            return -1; 
        }
        if (to == -1) {                                                           // O(1)
            System.err.println("Graph does not contain page url: "+urlTo);
            return -1; 
        }
        int gSize = Graph.getSize();                                              // O(1)
        int[] dist = new int[gSize];                                              // SysCall
        Arrays.fill(dist, Integer.MAX_VALUE);                                     // O(V)
        PriorityQueue<WikiPage> Q = new PriorityQueue<WikiPage>();                // SysCall
        
        // Initialise urlFrom at top of Q
        Q.offer(new WikiPage(from,0));                                            // O(1)
        dist[from] = 0;                                                           // O(1) + SysCall
        
        int origin, dFromStart;
        LinkedList<Integer> edges = new LinkedList<Integer>();                    // SysCall
        int Qpoll = 0, Qoffer = 0;                      // <DEBUG>
        while (!Q.isEmpty())                                                                    // O(V)
        {
            Qpoll++;                                    // <DEBUG>
            WikiPage originP = Q.poll();                                                        // O(log V)
            origin = originP.getIndex();                                          // O(1)
            edges = Graph.getEdge(origin);                                        // O(1)* 
            for (int v : edges)                                                                 // O(E)
            {
                dFromStart = dist[origin] + 1;  // Unweighted graph               // O(1)
                if (v == to)
                {
                    //System.out.print("poll O(V): "+Qpoll+" | ");  // <DEBUG>
                    //System.out.print("offer O(E): "+Qoffer+" | ");
                    //System.out.println("total: "+(Qpoll+Qoffer));
                    return dFromStart;
                }
                if (dFromStart < dist[v])
                {   // Add new item to queue 
                    Qoffer++;                                      // <DEBUG>
                    Q.offer(new WikiPage(v,dFromStart));                                        // O(log V)
                    dist[v] = dFromStart;                                         // O(1)
                }
            }
        }
        //System.out.print("poll O(V): "+Qpoll+" | ");  // <DEBUG>
        //System.out.print("offer O(E): "+Qoffer+" | ");
        //System.out.println("total: "+(Qpoll+Qoffer));
        return -1;
    }
    
    /** 
     * Runs a variant of the Bellman/Held-Karp Subset Dynamic Programming algorithm to find the hamiltonian path that 
     * goes through every wiki page without visiting one page more than once.
     * Solves in Problem 2.
     * @return a string array representing the verticies travelled in the order of the hamiltonian path
     * @exception if Graph is null
     */
    private String[] BHK_HamiltonPath() throws NullPointerException
    {
        if (Graph == null) throw new NullPointerException("Graph is null.");
        int n = Graph.getSize();
        boolean[][] path = new boolean[n][1<<n];
        for (boolean[] a : path)
            Arrays.fill(a, false);
        for (int i = 0; i < n; i++)
            path[i][1 << i] = true;
        boolean hFound = false;
        for (int subMask = 0; subMask < (1<<n); subMask++)          // for 0 to 2^n
        {
            for (int u = 0; u < n; u++)                             // Loop through every vertex
            {
                LinkedList<Integer> edges = Graph.getEdge(u);
                for (int v : edges)                                 // For every degree/edge of this vertex
                {
                    if ((subMask & 1<<v) != 0 && path[u][subMask ^ (1<<v)]) // subMask XOR 2^v - Examine all subsets (u->{0} u->{1} ... u->{0, 1 ...} | v ∉ S)
                    {
                        path[v][subMask] = true;
                        if (subMask >= (1 << n) - 1) hFound = true;
                        break;
                    }
                }
                if (hFound == true) break;
            }
        }                                                           // After this point, all subsets are solved
        
        // Recreate path going backwards from the final destination vertex
        int cur = (1 << n) - 1;
        String[] result = new String[n];
        for (int add = n - 1; add >= 0; add--)                      // Loop which runs after all verticies have been examined, adds item to the result array
        {                   
            int i = -1;
            for (int v = 0; v < n; v++) {
                if ( (cur & 1 << v) != 0 && path[v][cur])
                {
                     i = v;
                     break;
                }
            }
            if (i == -1) { 
                return new String[0];
            }
            result[add] = Graph.getPageString(i);
            cur = cur ^ (1 << i);                               // Examine subsets without {i, ... } on next iteration
        }
        return result;
    }

    /** Method to repeatedly call Dijkstra's Single Source Algorithm for every page to solve the all pairs shortest distance problem - then uses this data to find the jordan centers of the graph.
     * Solves Problem 4.
     * @return The page urls of the graph centers.
     */
    private String[] Dijkstra_getCenters()
    {
        int gSize = Graph.getSize();
        int minDist = Integer.MAX_VALUE/2+1;
        int minEdges = gSize/2+1;
        int currEdges, currMaxDist;
        HashMap<Integer, Integer> dist;
        ArrayList<String> pageCenterNames = new ArrayList<String>();
        
        for (int i = 0; i < gSize; i++)                             // For every page in the graph
        {
            dist = Dijkstra_SingleSource(i);                        // Solve single source problem for this page
            
            currMaxDist = 0;
            currEdges = dist.size();
            if (currEdges < minEdges) continue;
            for (Map.Entry<Integer, Integer> entry: dist.entrySet())
            {
                int currDist = entry.getValue();
                if (currDist > currMaxDist) 
                    currMaxDist = currDist;
            }
            if (currMaxDist <= minDist && currMaxDist!= 0) 
            {
                if (currMaxDist < minDist)
                {
                    pageCenterNames.clear();
                }
                minDist = currMaxDist;
                String url = Graph.getPageString(i);
                pageCenterNames.add(url);
            }
        }
        return pageCenterNames.toArray(new String[0]);
    }
    
    /**
    *  Runs Dijkstra's algorithm on a directed, unweighted graph to find the distances to all wiki pages from 
    *  the specified source wiki page to solve the single source shortest path problem
    *  Solves (in part) Problem 4.
    *  @param from the originating wiki page
    *  @return a map listing the distance to each vertex in the single source shortest path problem, or -1 if the vertex is not reachable from the source.
    */
    private HashMap<Integer, Integer> Dijkstra_SingleSource(int from)
    {
        int gSize = Graph.getSize();                                           // O(1)
        HashMap<Integer, Integer> dist = new HashMap<Integer, Integer>();      // SysCall
        PriorityQueue<WikiPage> Q = new PriorityQueue<WikiPage>();             // SysCall

        // Initialise urlFrom at top of Q
        Q.offer(new WikiPage(from,0));                                         // O(1) + SysCall
        dist.put(from,0);                                                      // O(1)
        
        int origin, dFromStart;
        LinkedList<Integer> edges = new LinkedList<Integer>();                   // SysCall
        //int Qpoll = 0, Qoffer = 0;                      // <DEBUG>
        while (!Q.isEmpty())                                                                    // O(V)
        {
            //Qpoll++;                                    // <DEBUG>
            WikiPage originP = Q.poll();                                                        // O(log V)
            origin = originP.getIndex();                                       // O(1)
            edges = Graph.getEdge(origin);                                     // O(1)*
            for (int v : edges)                                                                 // O(E)
            {
                dFromStart = dist.get(origin) + 1;                             // O(1)
                if (!dist.containsKey(v) || dFromStart < dist.get(v))          // O(1)
                {
                    //Qoffer++;                           // <DEBUG>
                    Q.offer(new WikiPage(v,dFromStart));      // Add new item to queue          // O(log V)
                    dist.put(v,dFromStart);                                    // O(1)
                }
            }
        }
        //System.out.print("poll O(V): "+Qpoll+" | ");                   // <DEBUG>
        //System.out.println("offer O(E): "+Qoffer);
        return dist;
    }

    /**
     * Runs the Floyd-Warshall algorithm to find the all paths shortest problem and uses this data to then find the graph centers
     * Solves problem 4
     * @return The page urls of the graph centers.
     */
    private String[] FloydWarshall_getCenters()
    {
        int gSize = Graph.getSize();
        int dist[][] = new int[gSize][gSize];
        LinkedList<Integer> edges = new LinkedList<Integer>();
    
        int minDist = Integer.MAX_VALUE/2+1;
        int minEdges = gSize/2;
        int currEdges, currMaxDist;
        ArrayList<String> pageCenterNames = new ArrayList<String>();

        //int count = 0;                                            // <DEBUG>
        for (int i = 0; i < gSize; i++)
        {
            for (int j = 0; j < gSize; j++) 
            {
                if (i != j)
                    dist[i][j] = Integer.MAX_VALUE/2;
                else
                    dist[i][j] = 0;
                //count++;                                          // <DEBUG>
            }
        }
        for (int u = 0; u < gSize; u++)
        {
            edges = Graph.getEdge(u);
            for (int v : edges)
                dist[u][v] = 1; 
            //count++;                                              // <DEBUG>
        }

        for (int k = 1; k < gSize; k++)                                 // O(V)
            for (int i = 1; i < gSize; i++)                             // O(V)
                for (int j = 1; j < gSize; j++)                         // O(V)
                    if ( (dist[i][k] + dist[k][j]) < dist[i][j]) {
                        dist[i][j] = dist[i][k] + dist[k][j];
                        //count++;                                         // <DEBUG>
                    }
        //System.out.println(count+" assignments"); // <DEBUG>
        
        for (int i = 0; i < gSize; i++)
        {
            currEdges = 0;
            currMaxDist = 0;
            for (int j = 0; j < gSize; j++) 
            {
                int currDist = dist[i][j];
                if (currDist != Integer.MAX_VALUE/2 && currDist != 0) 
                {
                    currEdges++;
                    if (currDist > currMaxDist)
                    {
                        currMaxDist = currDist;
                    }
                }
            }
            if (currEdges < minEdges) continue;
            
            if (currMaxDist <= minDist && currMaxDist!= 0) 
            {   
                if (currMaxDist < minDist)
                {
                    pageCenterNames.clear();
                }
                minDist = currMaxDist;
                String url = Graph.getPageString(i);
                pageCenterNames.add(url);
            }
        }
        return pageCenterNames.toArray(new String[0]);
    }

    /** Class for implementation of the Gabow Algorithm for Path-based Depth-First Search algorithm utilising two stacks. 
     * Solves Problem 3
     */
    private class Gabow
    {
        private WikiGraph Graph;
        private int gSize;
        private int current;
        private int searchCount;
        private int[] search;
        private boolean[] visited;
        private boolean[] added;
        
        ArrayList<ArrayList<Integer>> strongComponentGroups;
        Stack<Integer> upper;
        Stack<Integer> lower;
        
        public Gabow(WikiGraph g)
        {
            Graph = g;
            gSize = Graph.getSize();
            
            search = new int[gSize];
            visited = new boolean[gSize];
            added = new boolean[gSize];
            strongComponentGroups = new ArrayList<>();
            upper = new Stack<Integer>();
            lower = new Stack<Integer>();
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
        public void dfs(int u)                                                      
        {
            visited[u] = true;
            search[u] = searchCount++;
            upper.push(u);
            lower.push(u);
            
            LinkedList<Integer> edges = Graph.getEdge(u);
            
            for (int v : edges)                                                         // O(E)
            {
                if (!visited[v])
                    dfs(v);
                else if (!added[v]) 
                {
                    while (search[lower.peek()] > search[v]) 
                        lower.pop();
                }
            }
            
            // If found component u
            if (lower.peek() == u)
            {
                lower.pop();
                ArrayList<Integer> list = new ArrayList<Integer>();
                // Pop every object off stack between v -> u
                int v;
                do 
                {
                    v = upper.pop();
                    list.add(v);
                    added[v] = true;
                } 
                while (v != u);
                strongComponentGroups.add(list);       
            }
        }
        
    }
    
    /** A class to implement the adjacency linked list Graph data structure used to store wikipedia page link data */
    private class WikiGraph 
    {

        /** Use ArrayList to store adjacency lists (1 linked list for each wiki page)*/
        ArrayList<LinkedList<Integer>> adjLists;
        /** ArrayList of page url names */
        private ArrayList<String> pageUrls;
        /** Number of edges (links) in the graph */
        private int numEdges;
        
        public WikiGraph()
        {
            adjLists = new ArrayList<LinkedList<Integer>>();
            pageUrls = new ArrayList<String>();
        }

        /** Add wiki page to the graph 
         * 
         * @param s The string of the page to be added
         * @return The index value of the added page
         */
        private int addPages(String s)
        {
            adjLists.add(new LinkedList<Integer>());
            pageUrls.add(s);

            return pageUrls.size() - 1;
        }
 
        /** Adds edge (link) in the adjacency list from the origin wiki page to the destination page 
          * 
          * @param from String of the originating page
          * @param to String of the destination page
          * @exception IllegalValue if the string is null
          */
        public void addEdge(String urlFrom, String urlTo) throws IllegalArgumentException
        {
            if (urlFrom == null || urlTo == null) throw new IllegalArgumentException("urlFrom/urlTo is null.");
        
            int from = getIndex(urlFrom);
            int to = getIndex(urlTo);
            if (from == -1)
                from = addPages(urlFrom);
            if (to == -1)
                to = addPages(urlTo);
            LinkedList<Integer> Edge = getEdge(from);
            Edge.add(to);
        }
        
        /**
        * Initialise a random graph for testing
        * 
        * @param V The number of verticies (pages) in the grapph
        * @param E the number of edges (links) in the graph
        */
        public void randomGraph(int V, int E)
        {
            double rand, rand2;
            for (int i = 0; i <= E; i++) {
                rand = Math.random();
                rand2 = Math.random();
                int u = (int) (rand * V);
                int v = (int) (rand2 * V);
                addEdge(Integer.toString(u), Integer.toString(v));
            }
        }
 
        /** Returns the adjacency List containing the pages linked by the origin wiki page
         * 
         * @param s The string of the origin wiki page
         * @return Linked list of all the edges from the origin wiki page
         */     
        public LinkedList<Integer> getEdge(String s)
        {
            return adjLists.get(getIndex(s));
        }
    
        /** Returns the adjacency List containing the pages linked by the origin wiki page
         * 
         * @param i The key value (index) of the origin wiki page
         * @return Linked list of all the edges from the origin wiki page
         */     
        public LinkedList<Integer> getEdge(int i)
        {
            return adjLists.get(i);
        }
    
        /** Find how many wiki pages there are in the graph
         * 
         * @return the number of pages in the graph
         */
        public int getSize()
        {
             return adjLists.size();
        }
    
        /** Get the index of the specified wiki page url, if it exists in the graph
         * 
         * @return the index value of the page, if it exists in the graph, else -1.
         */
        public int getIndex(String s)
        {
            return pageUrls.lastIndexOf(s);
        }
    
        /** Given an index value, return the string of the wiki page url at the referenced index key value of the graph.
         * 
         * @return the string of the indexed wiki page.
         */
        public String getPageString(int i)
        {
            return pageUrls.get(i);
        }
    
        /** Prints the graph to the console (for debugging purposes)
         * 
         */
        public void printGraph()
        {
            int numEdgesTotal = 0, numEdgesPage = 0;
            int numPages = adjLists.size();
            for (int i = 0 ; i < numPages; i++)
            {
                LinkedList<Integer> edgeList = getEdge(i);
                numEdgesPage = edgeList.size();
                numEdgesTotal += numEdgesPage;
                System.out.print("<"+getPageString(i)+">"+"[Edges: "+numEdgesPage+"]-> { ");
                for (int e : edgeList) {
                    System.out.print(getPageString(e)+" ");
                }
                System.out.println("}");
            }
            double density = ( (double) numEdgesTotal/ (double) (numPages*numPages));
            System.out.println("Number of pages: "+numPages);
            System.out.println("Number of edges: "+numEdgesTotal);
            System.out.println("Density: "+density);
        }
    }
    
    /** A class to implement the WikiPage list element for use with the java.util.PriorityQueue min heap */
    private class WikiPage implements Comparable<WikiPage>
    {
        /** Wiki page index (key value in Graph) */
        private int index;
        /** Priority of this element */
        private int priority;
        
        
        /** Create a new wikipage list element 
          * 
          * @param i the vertex key value/index of the page
          * @param p the priority of the item ( used to sort items in the java.util.priorityQueue heap)
          */
        public WikiPage(int i, int p)
        {
            this.index = i;
            this.priority = p;
        }
        /** Accessor method which returns page index (key value in Graph)
         * 
         * @return the vertex key value/index of the page
         */
        public int getIndex() { return this.index; }
        
        /** Accessor method which returns priority (Used for debugging)
         * 
         * @return the priority of the wikipage list element
         */
        public int getPriority() { return this.priority;}
        
        @Override
        public int compareTo(WikiPage p)
        {
            return this.priority - p.priority;
        }
    }
}

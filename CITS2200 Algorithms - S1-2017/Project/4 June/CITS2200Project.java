import java.util.List;
import java.util.Map;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.ArrayList;
import java.util.PriorityQueue;
import java.util.Stack;
import java.util.Set;

/**
*   CITS2200 Algorithms Project
*   @author Evan Huang 20916873
*/

public class CITS2200Project { //implements CITS2200Project 
    //private CITS2200Graph Graph;
    public CITS2200Graph Graph; // <DEBUG>

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
        int numCenters = 0, maxLinks = 1, minDist = Integer.MAX_VALUE;
        int numPages = Graph.getNumPages();
        Set<String> pageUrls = Graph.getPageUrls();
        ArrayList<String> pageCenterNames = new ArrayList<String>();
        
        for ( String url : pageUrls)                                // For every page in the graph
        {
            Map<String,Integer> dist = Dijkstra_SingleSource(url);   // Solve single source problem for this page
            if (dist.size() < maxLinks) continue;                        // If the number of pages that can be reached by this page is lower than current maximum links, move on to next page
            maxLinks = dist.size();
            
            int sumDist = 0;
            for (Map.Entry<String, Integer> entry: dist.entrySet())
            {
                sumDist += entry.getValue();
            }
            if (sumDist <= minDist) 
            {
                if (sumDist == minDist)
                {
                    minDist = sumDist;
                    pageCenterNames.clear();
                    System.out.println("new lowest distance - arraylist cleared");  // <DEBUG>
                }
                pageCenterNames.add(url);
                
                System.out.print("["+url+"] | ");
                System.out.print("Links: "+maxLinks+" | ");
                System.out.println("SumDistance: "+sumDist);
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
        String[][] array = new String[4][4];
        
        return array;
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
    public String[] getHamiltonianPath()
    {
        if (Graph.getNumPages() > 20) return null;
        String[] array = new String[4];
        return array;
    }
    
    private void Gabow() 
    {
        int pageCount;          // page count
        int preCount;   // pre-ordeer number counteer
        Set<String> pages = Graph.getPageUrls();
        int size = Graph.getNumPages();
        int[] preorder = new int[size];
        
        //boolean[] visited;  // check if page has been visited
        ArrayList<String> visited = new ArrayList<String>();
        boolean[] chk = new boolean[size];      // check strong component containing v;
        
        List<Integer>[] graph; // Store given graph
        
        ArrayList<List<Integer>> sccComp = new ArrayList<>();
        Stack<Integer> stack1 = new Stack<Integer>();
        Stack<Integer> stack2 = new Stack<Integer>();
        
        for (String p : pages)
        {
            //if (
        }
        
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
        //if (!Graph.containsKey(source)) throw new NullPointerException("Graph does not contain page url: "+source);
        
        Map<String, Integer> dist = new HashMap<String, Integer>();
        //Map<String, String> prev = new HashMap<String, String>();
        PriorityQueue<Page> Q = new PriorityQueue<Page>();

        // Initialise urlFrom at top of Q
        Q.offer(new Page(urlFrom,0));
        dist.put(urlFrom,0);
        //prev.put(urlFrom,urlTo);

        while (!Q.isEmpty())
        {
            Page originP = Q.poll();
            String origin = originP.getName();
            int priority = originP.getPriority();
            //System.out.println("Popped Priority Queue Item: ["+origin+","+priority+"]"); // <DEBUG>
            LinkedList<String> links = Graph.getEdge(origin);
            if (links == null) continue;
            for (String link : links)
            {
                int dFromStart = 0;
                dFromStart = dist.get(origin) + 1;  // Unweighted graph
                if (link.compareTo(urlTo)==0) 
                {
                    return dFromStart;
                }
                if (!dist.containsKey(link) || dFromStart < dist.get(link))
                {
                    Q.offer(new Page(link,dFromStart));      // Add new item to queue
                    //System.out.println("New Priority Queue Item: ["+link+","+dFromStart+"]"); // <DEBUG> 
                    dist.put(link,dFromStart);
                }
            }
        }
        //System.out.println(dist); // <DEBUG>
        //System.out.println(prev);
        return -1;
    }
    
    /**
    *  Runs Dijkstra's algorithm on a directed, unweighted graph to find the distances to all wiki pages from the specified source wiki page to solvee the single source shortest path problem
    *  @param source the originating wiki page
    *  @return a map listing the distance to each vertex in the single source shortest path problem, or -1 if the vertex is not reachable from the source.
    */
    private Map<String, Integer> Dijkstra_SingleSource(String urlFrom) throws NullPointerException
    {
        if (Graph == null) throw new NullPointerException("Graph is null.");
        //if (!Graph.containsKey(source)) throw new NullPointerException("Graph does not contain page url: "+source);
        
        Map<String, Integer> dist = new HashMap<String, Integer>();
        //Map<String, String> prev = new HashMap<String, String>();
        PriorityQueue<Page> Q = new PriorityQueue<Page>();

        // Initialise urlFrom at top of Q
        Q.offer(new Page(urlFrom,0));
        dist.put(urlFrom,0);
        //prev.put(urlFrom,urlTo);

        while (!Q.isEmpty())
        {
            Page originP = Q.poll();
            String origin = originP.getName();
            int priority = originP.getPriority();
            //System.out.println("Popped Priority Queue Item: ["+origin+","+priority+"]"); // <DEBUG>
            LinkedList<String> links = Graph.getEdge(origin);
            if (links == null) continue;
            for (String link : links)
            {
                int dFromStart = 0;
                dFromStart = dist.get(origin) + 1;  // Unweighted graph
                if (!dist.containsKey(link) || dFromStart < dist.get(link))
                {
                    Q.offer(new Page(link,dFromStart));      // Add new item to queue
                    //System.out.println("New Priority Queue Item: ["+link+","+dFromStart+"]"); // <DEBUG> 
                    //prev.put(link,origin);
                    dist.put(link,dFromStart);
                }
            }
        }
        //System.out.println(dist); // <DEBUG>
        //System.out.println(prev);
        return dist;
    }
    
    // PAGE CLASS (for PQueue) //
    
    private class Page implements Comparable<Page>
    {
        private String name;
        private int priority;

        public Page(String s, int p)
        {
            this.name = s;
            this.priority = p;
        }
        
        public String getName() { return this.name; }
        public int getPriority() { return this.priority; }
        
        @Override
        public int compareTo(Page p)
        {
            return this.priority - p.priority;
        }
    }
}

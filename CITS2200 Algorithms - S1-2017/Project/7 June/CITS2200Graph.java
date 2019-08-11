import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.ArrayList;
 
public class CITS2200Graph 
{

    /** Use HashMap collection to store adjacency lists (1 for each wiki page)*/
    private HashMap<Integer, LinkedList<Integer>> adjLists;
    /** List of page url names */
    private ArrayList<String> pageUrls;

    public CITS2200Graph()
    {
        adjLists = new HashMap<Integer, LinkedList<Integer>>();
        pageUrls = new ArrayList<String>();
    }

    /** Add wiki page to the graph 
     * 
     * @param s The string of the page to be added 
     */
    private int addPages(String s)
    {
        adjLists.put(pageUrls.size(), new LinkedList<Integer>());
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
 
    /** Returns the adjacency List containing the pages linked by the origin wiki page
     * 
     * @param s The string of the origin wiki page
     * @return Linked list of all the links from the origin wiki page
     */     
    public LinkedList<Integer> getEdge(String s)
    {
        return adjLists.get(getIndex(s));
    }
    
    /** Returns the adjacency List containing the pages linked by the origin wiki page
     * 
     * @param i The key value (index) of the origin wiki page
     * @return Linked list of all the links from the origin wiki page
     */     
    public LinkedList<Integer> getEdge(int i)
    {
        return adjLists.get(i);
    }
    
    /** Query graph to determine if link exists from a source page to a destination wiki page
     * 
     * @param from index of the originating wiki page
     * @param to index of the destination wiki page 
     */
    public boolean queryEdge(int from, int to)
    {
        LinkedList<Integer> edges = getEdge(from);
        return edges.contains(to);
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
    
    /** Print all the adjacency Lists for the pages linked by the origin wiki page
     * 
     */
    public void printLists()
    {
        int numEdgesTotal = 0, numEdgesPage = 0;
        int numPages = adjLists.size();
        for (Map.Entry<Integer, LinkedList<Integer>> entry: adjLists.entrySet())
        {
            int pageNum = entry.getKey();
            LinkedList<Integer> edgeList = entry.getValue();
            numEdgesPage = edgeList.size();
            numEdgesTotal += numEdgesPage;
            System.out.print("<"+getPageString(pageNum)+">"+"[Links: "+numEdgesPage+"]-> { ");
            for (int e : edgeList)
                System.out.print(getPageString(e)+" ");
            System.out.println("}");
        }
        double density = ( (double) numEdgesTotal/ (double) (numPages*numPages));
        System.out.println("Number of pages: "+numPages);
        System.out.println("Number of edges: "+numEdgesTotal);
        System.out.println("Density: "+density);
    }
}

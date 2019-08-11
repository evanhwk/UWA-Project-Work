import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Set;
 
public class CITS2200Graph 
{

    /** Use HashMap collection to store adjacency lists (1 for each wiki page)*/
    private HashMap<String, LinkedList<String>> adjLists;
    /** List of page url names */
    //private LinkedList<String> pageUrls;

    /** Init the map */
    public CITS2200Graph()
    {
        adjLists = new HashMap<String, LinkedList<String>>();
        //pageUrls = new LinkedList<String>();
    }

    /** Add wiki page to the graph 
    * @param s The string of the page to be added */
    private void addPages(String s)
    {
        adjLists.put(s, new LinkedList<String>());
        //pageUrls.add(s);
    }
 
    /** Adds edge (link) in the adjacency list from the origin wiki page to the destination page 
    * @param from String of the originating page
    * @param to String of the destination page */
    public void addEdge(String from, String to)
    {
        if (!adjLists.containsKey(from))
            addPages(from);
        LinkedList<String> Edge = adjLists.get(from);
        Edge.add(to);
    }
 
    /** Returns the adjacency List containing the pages linked by the origin wiki page
     * @param source The string of the origin wiki page
     * @return Adjacency list of the origin wiki page to be returned */     
    public LinkedList<String> getEdge(String s)
    {
        return adjLists.get(s);
    }

    /** Find how many wiki pages there are in the graph
     * @return the number of pages in the graph
     */
    public int getSize()
    {
        return adjLists.size();
    }

    public Set<String> getPageUrls()
    {
        //return pageUrls;
        return adjLists.keySet();
    }
    
    /** Print all the adjacency Lists for the pages linked by the origin wiki page
     */
    public void printLists()
    {
        int numEdgesTotal = 0, numEdgesPage = 0;
        int numPages = adjLists.size();
        for (Map.Entry<String, LinkedList<String>> entry: adjLists.entrySet())
        {
            String page = entry.getKey();
            LinkedList<String> edgeList = entry.getValue();
            numEdgesPage = edgeList.size();
            numEdgesTotal += numEdgesPage;
            System.out.print("["+numEdgesPage+"]"+page+"->"+edgeList);
            System.out.println();
        }
        double density = ( (double) numEdgesTotal/ (double) (numPages*numPages));
        System.out.println("Number of pages: "+numPages);
        System.out.println("Number of edges: "+numEdgesTotal);
        System.out.println("Density: "+density);
        
        /*
        for (String key: adjLists.keySet()) 
        {
            System.out.print(key+"->");
            //System.out.println("value :" + adjLists.get(key));
            List<String> edgeList = adjLists.get(key);
            for (int j = 1 ; j <= edgeList.size(); j++) 
            {
                System.out.print(edgeList.get(j-1));
                if (j != edgeList.size())
                {
                    System.out.print("->");
                }    
            }
            System.out.println();
        }
        System.out.println();
        */
       
       /*
        List<String>  = new LinkedList<String>();
        for (Map.Entry<String, List<String>> entry: adjLists.entrySet())
        { 
            String pN = entry.getKey();
            pageUrls.add(pN);
        }
        */
    }
}
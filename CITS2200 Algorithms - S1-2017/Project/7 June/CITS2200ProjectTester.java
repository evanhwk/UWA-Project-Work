import java.io.*;
import java.util.*;

public class CITS2200ProjectTester {
    
    public static void loadGraph(CITS2200Project project, String path) {
        // The graph is in the following format:
        // Every pair of consecutive lines represent a directed edge.
        // The edge goes from the URL in the first line to the URL in the second line.
        try {
            BufferedReader reader = new BufferedReader(new FileReader(path));
            while (reader.ready()) {
                String from = reader.readLine();
                String to = reader.readLine();
                //System.out.println("Adding edge from " + from + " to " + to);
                project.addEdge(from, to);
            }
        } catch (Exception e) {
            System.out.println("There was a problem:");
            System.out.println(e.toString());
        }
        project.Graph.printLists();
    }

    public static void main(String[] args) {
        // Change this to be the path to the graph file.
        String pathToGraphFile = "example_graph.txt";
        String pathToGraphFile2 = "example_graph4.txt";
        String pathToGraphFile3 = "example-graph3.txt";
        String pathToGraphFile5 = "example_graph5.txt";
        String pathToGraphFile6 = "example_graph6.txt";
        // Create an instance of your implementation.
        CITS2200Project proj = new CITS2200Project();
        CITS2200Project proj2 = new CITS2200Project();
        CITS2200Project proj3 = new CITS2200Project();
        CITS2200Project proj4 = new CITS2200Project();
        CITS2200Project proj5 = new CITS2200Project();
        // Load the graph into the project.
        loadGraph(proj, pathToGraphFile);
        loadGraph(proj2, pathToGraphFile2);
        loadGraph(proj3, pathToGraphFile3);
        loadGraph(proj4, pathToGraphFile5);
        loadGraph(proj5, pathToGraphFile6);

        // Write your own tests!
        
        //System.out.println("SHORTEST PATH :"+proj.getShortestPath("/wiki/Flow_network","/wiki/Out-of-kilter_algorithm"));
        //System.out.println(proj.getShortestPath("321","550"));
        //System.out.println("Shortest path: "+proj.getShortestPath("4","50"));
        /*
        String[] ResultArray = proj2.getCenters();
        System.out.println("GET CENTER");
        for (String s: ResultArray)
            System.out.print(s+" ");
        System.out.println();
        
        ResultArray = proj.getCenters();
        System.out.println("GET CENTER");
        for (String s: ResultArray)
            System.out.print(s+" ");
        System.out.println(); 
        
        ResultArray = proj3.getCenters();
        System.out.println("GET CENTER");
        for (String s: ResultArray)
            System.out.print(s+" ");
        System.out.println();   
        
        String[][] Result = proj.getStronglyConnectedComponents();
        int size = Result.length;
        for (int i = 0; i < Result.length; i++)
        {
            System.out.println("Group "+i);
            for (String s : Result[i])
                System.out.print("["+s+"]");
            System.out.println();
        }
        
        String[][] Result2 = proj3.getStronglyConnectedComponents();
        size = Result2.length;
        for (int i = 0; i < Result2.length; i++)
        {
            System.out.println("Group "+i);
            for (String s : Result2[i])
                System.out.print("["+s+"]");
            System.out.println();
        }
        */
        String[] hamilton = proj4.getHamiltonianPath();
        int size = hamilton.length;
        System.out.println("HAMILTON PATH ["+size+"]");
        for (int i = 0; i < hamilton.length; i++)
        {
            System.out.println("H["+i+"]"+hamilton[i]);
        }
        
        String[] hamilton2 = proj5.getHamiltonianPath();
        int size2 = hamilton2.length;
        System.out.println("HAMILTON PATH ["+size2+"]");
        for (int i = 0; i < hamilton2.length; i++)
        {
            System.out.println("H["+i+"]"+hamilton2[i]);
        }
    }
}

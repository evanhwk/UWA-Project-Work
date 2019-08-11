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
                System.out.println("Adding edge from " + from + " to " + to);
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
        //String pathToGraphFile = "example_graph.txt";
        // Create an instance of your implementation.
        CITS2200Project proj = new CITS2200Project();
        CITS2200Project proj2 = new CITS2200Project();
        // Load the graph into the project.
        loadGraph(proj, pathToGraphFile);
        loadGraph(proj2, pathToGraphFile2);

        // Write your own tests!
        System.out.println(proj.getShortestPath("/wiki/Flow_network","/wiki/Out-of-kilter_algorithm"));
        //System.out.println(proj.getShortestPath("321","550"));
        //System.out.println("Shortest path: "+proj.getShortestPath("4","50"));
        String[] ResultArray = proj2.getCenters();
        for (String s: ResultArray)
            System.out.println(s);
            
        ArrayList<String> hello = new ArrayList<String>();
        hello.add("hi");
        System.out.println(hello.contains("hi"));
    }
}

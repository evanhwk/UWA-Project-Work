import java.io.*;
import java.util.*;
import java.sql.Timestamp;

public class CITS2200ProjectTester {
    
    public static void loadGraph(MyCITS2200Project project, String path) {
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
        //project.printGraph();
    }

    public static void main(String[] args) {
        // Change this to be the path to the graph file.
        String Test = "TEST1.txt";
        String Test2 = "TEST2.txt";
        String pathToGraphFile5 = "example_graph5.txt";
        String pathToGraphFile6 = "example_graph6.txt";
        
        // Create an instance of your implementation.
        MyCITS2200Project proj = new MyCITS2200Project();
        MyCITS2200Project proj2 = new MyCITS2200Project();
        MyCITS2200Project proj3 = new MyCITS2200Project();
        MyCITS2200Project projH1 = new MyCITS2200Project();
        MyCITS2200Project projH2 = new MyCITS2200Project();
        
        MyCITS2200Project rand10 = new MyCITS2200Project();
        MyCITS2200Project rand20 = new MyCITS2200Project();
        MyCITS2200Project rand50 = new MyCITS2200Project();
        MyCITS2200Project rand100 = new MyCITS2200Project();
        MyCITS2200Project rand1000 = new MyCITS2200Project();
        
        // Load the graph into the project.
        loadGraph(proj, Test);
        loadGraph(proj2, Test2);
        loadGraph(projH1, pathToGraphFile5);
        loadGraph(projH2, pathToGraphFile6);
        
        rand10.testProject(10, (int) (0.85*10*10));
        rand20.testProject(20, (int) (0.85*20*20));
        rand50.testProject(50, (int) (0.85*50*50));
        rand100.testProject(100, (int) (0.85*100*100));
        rand1000.testProject(1000, (int) (0.85*1000*1000));
        
        // Write your own tests!
        String[] ResultArray = new String[0];
        int size = 0;
        long startTime, endTime;
        
        System.out.println("-------------------------------------------------------------------------------");
        System.out.println("getShortesPath");
        System.out.println("-------------------------------------------------------------------------------");
        startTime = System.currentTimeMillis();
        System.out.println("Test1 : </wiki/Flow_network -> /wiki/Out-of-kilter_algorithm> : "+proj.getShortestPath("/wiki/Flow_network","/wiki/Out-of-kilter_algorithm"));
        endTime = System.currentTimeMillis();
        System.out.println((endTime-startTime));
        System.out.println("-------------------------------------------------------------------------------");
        startTime = System.currentTimeMillis();
        System.out.println("Test2 : <Floyd%27s_cycle-finding_algorithm -> Greedy_Algorithm> : "+proj2.getShortestPath("Floyd%27s_cycle-finding_algorithm","Greedy_Algorithm"));
        endTime = System.currentTimeMillis();
        System.out.println((endTime-startTime));
        System.out.println("-------------------------------------------------------------------------------");
        startTime = System.currentTimeMillis();
        System.out.println("Rand10 : <0 -> 4> : "+rand10.getShortestPath("0","4"));
        endTime = System.currentTimeMillis();
        System.out.println((endTime-startTime));
        System.out.println("-------------------------------------------------------------------------------");
        startTime = System.currentTimeMillis();
        System.out.println("Rand20 : <0 -> 9> : "+rand20.getShortestPath("0","9"));
        endTime = System.currentTimeMillis();
        System.out.println((endTime-startTime));
        System.out.println("-------------------------------------------------------------------------------");
        startTime = System.currentTimeMillis();
        System.out.println("Rand50 : <0 -> 24> : "+rand50.getShortestPath("0","24"));
        endTime = System.currentTimeMillis();
        System.out.println((endTime-startTime));
        System.out.println("-------------------------------------------------------------------------------");
        startTime = System.currentTimeMillis();
        System.out.println("Rand100 : <0 -> 50> : "+rand100.getShortestPath("0","50"));
        endTime = System.currentTimeMillis();
        System.out.println((endTime-startTime));
        System.out.println("-------------------------------------------------------------------------------");
        startTime = System.currentTimeMillis();
        System.out.println("Rand1000 : <0 -> 500> : "+rand1000.getShortestPath("0","500"));
        endTime = System.currentTimeMillis();
        System.out.println((endTime-startTime));
        System.out.println("-------------------------------------------------------------------------------");
        startTime = System.currentTimeMillis();
        System.out.println("getCenters");
        endTime = System.currentTimeMillis();
        System.out.println((endTime-startTime));
        System.out.println("-------------------------------------------------------------------------------");
        startTime = System.currentTimeMillis();
        ResultArray = proj.getCenters();
        endTime = System.currentTimeMillis();
        System.out.println((endTime-startTime));
        System.out.println("TEST1");
        for (String s: ResultArray)
            System.out.print(s+" ");
        System.out.println();
        System.out.println("-------------------------------------------------------------------------------");
        startTime = System.currentTimeMillis();
        ResultArray = proj2.getCenters();
        endTime = System.currentTimeMillis();
        System.out.println((endTime-startTime));
        System.out.println("TEST2");
        for (String s: ResultArray)
            System.out.print(s+" ");
        System.out.println();
        System.out.println("-------------------------------------------------------------------------------");
        startTime = System.currentTimeMillis();
        //ResultArray = rand10.getCenters();
        endTime = System.currentTimeMillis();
        System.out.println((endTime-startTime));
        System.out.println("Rand10");
        for (String s: ResultArray)
            System.out.print(s+" ");
        System.out.println();
        System.out.println("-------------------------------------------------------------------------------");
        startTime = System.currentTimeMillis();
        //ResultArray = rand20.getCenters();
        endTime = System.currentTimeMillis();
        System.out.println((endTime-startTime));
        System.out.println("Rand20");
        for (String s: ResultArray)
            System.out.print(s+" ");
        System.out.println();
        System.out.println((endTime-startTime));
        System.out.println("-------------------------------------------------------------------------------");
        startTime = System.currentTimeMillis();
        //ResultArray = rand50.getCenters();
        endTime = System.currentTimeMillis();
        System.out.println((endTime-startTime));
        System.out.println("Rand50");
        for (String s: ResultArray)
            System.out.print(s+" ");
        System.out.println();
        System.out.println((endTime-startTime));
        System.out.println("-------------------------------------------------------------------------------");
        startTime = System.currentTimeMillis();
        //ResultArray = rand100.getCenters();
        endTime = System.currentTimeMillis();
        System.out.println((endTime-startTime));
        System.out.println("Rand100");
        for (String s: ResultArray)
            System.out.print(s+" ");
        System.out.println();
        System.out.println((endTime-startTime));
        System.out.println("-------------------------------------------------------------------------------");
        startTime = System.currentTimeMillis();
        //ResultArray = rand1000.getCenters();
        endTime = System.currentTimeMillis();
        System.out.println((endTime-startTime));
        System.out.println("Rand1000");
        for (String s: ResultArray)
            System.out.print(s+" ");
        System.out.println();
        System.out.println((endTime-startTime));
        System.out.println("-------------------------------------------------------------------------------");
        
        startTime = System.currentTimeMillis();
        String[][] Result = proj2.getStronglyConnectedComponents();
        endTime = System.currentTimeMillis();
        System.out.println((endTime-startTime));
        size = Result.length;
        System.out.println("Project 2");
        for (int i = 0; i < Result.length; i++)
        {
            System.out.print("<"+i+"> ");
            for (String s : Result[i])
                System.out.print("["+s+"]");
            System.out.println();
        }
        
        startTime = System.currentTimeMillis();
        String[] hamilton = projH1.getHamiltonianPath();
        endTime = System.currentTimeMillis();
        System.out.println((endTime-startTime));
        
        size = hamilton.length;
        System.out.println("HAMILTON PATH ["+size+"]");
        for (int i = 0; i < hamilton.length; i++)
        {
            System.out.print(hamilton[i]+" ");
        }
        System.out.println();
        
        startTime = System.currentTimeMillis();
        String[] hamilton2 = projH2.getHamiltonianPath();
        endTime = System.currentTimeMillis();
        System.out.println((endTime-startTime));
        
        size = hamilton2.length;
        System.out.println("HAMILTON PATH ["+size+"]");
        for (int i = 0; i < hamilton2.length; i++)
        {
            System.out.print(hamilton2[i]+" ");
        }
        System.out.println();
    }
}

import java.io.*;
import java.net.*;
import javax.net.ssl.*;
import java.security.*;

import com.sun.net.httpserver.*;
import com.sun.net.*;

import java.util.Arrays;
import java.util.ArrayList;
import java.util.BitSet;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Random;
import java.util.Scanner;
import java.util.Timer;
import java.util.TimerTask;
import java.util.UUID;

import java.lang.StringBuilder;
import java.lang.Math;
import java.lang.Runtime;

/**
 * Class to implement Testing-Server of CITS3002 Quiz
 */
public class TestingServer {

    private static final boolean DEBUG = false;
    private static final boolean DEMO = true;           // For project demonstration
    /** Port to connect to QuestionServer */
    private static final int QPORT = 48009;
    /** Port to serve HTTP Server */
    private static final int HPORT = 8080;
    // Keystore variables for HTTPS Server
    private static final String KEYSTORE_LOC = "store"+File.separator+"keystore.jks";
    private static final String KEYSTORE_TYPE = "JKS";
    private static final String KEYSTORE_PWD = "as2@2qte";
    // Authentication password database for HTTPS Server
    private static final String PASSWORDS_LOC = "store"+File.separator+"passwords.txt";
    // Total number of questions to request
    private static final byte TOTAL_QUESTIONS = 10;
    private static final byte TOTAL_MCQ = 9;

    // Truststore for QuestionClient 
    private static final String TRUSTSTORE_LOC = "store"+File.separator+"truststore.jks";
    private static final String TRUSTSTORE_PWD = "as2@2qte";
    // Authentication details for Question Server
    private static final String QSERVER_USR = "admin";
    private static final String QSERVER_PWD = "password";

    private IPBuckets buckets;
    private HttpServer server;
    private HttpsServer https_server;
    private int http_port;

    private QuestionClient question_clt;
    private String ques_addr;
    private int ques_port;

    /** Constructor for TestingServer */
    private TestingServer(boolean tls, int hport, String qserver, int qport) throws IOException {

        this.buckets = new IPBuckets();
        this.http_port = hport;
        this.ques_addr = qserver;
        this.ques_port = qport;
        if (tls) 
            start_secure_server();
        else 
            start_server();
    }

    /** Basic HTTP (No SSL) Implementation - Backup method */
    private void start_server() throws IOException {

        InetSocketAddress address = new InetSocketAddress(http_port);                                       // Open Socket on specified port
        this.question_clt = new QuestionClient(this.ques_addr, this.ques_port, false);                   // Open Communication with Question-Server
        // Launch HTTPServer
        try {
            this.server = HttpServer.create(address, 0);
            this.server.createContext("/", new THandler(this.buckets, this.question_clt));
            this.server.setExecutor(null); // default executor
            System.out.println("Starting Unsecure Server - http://localhost:"+http_port);
            this.server.start();
        } catch (IOException except) {
            except.printStackTrace();
            stop_server();
            System.exit(1);
        }
    }

    /** TLS Implementation
     * Based upon: 
     * https://stackoverflow.com/questions/2308479/simple-java-https-server
     * Oracle javadocs - https://docs.oracle.com/javase/8/docs/jre/api/net/httpserver/spec/com/sun/net/httpserver/package-summary.html
     */
    private void start_secure_server() throws IOException {

        InetSocketAddress address = new InetSocketAddress(http_port);                                       // Open Socket on specified port
        this.question_clt = new QuestionClient(this.ques_addr, this.ques_port, true);                   // Open Communication with Question-Server

        // Launch the HTTPS server
        try {
            this.https_server = HttpsServer.create(address, 0);
            SSLContext sslContext = SSLContext.getInstance("TLSv1.2");
            char[] pwd = KEYSTORE_PWD.toCharArray();
            KeyStore keystore = KeyStore.getInstance(KEYSTORE_TYPE);                                        // Initialise the keystore       
            FileInputStream pfx_stream = new FileInputStream(KEYSTORE_LOC);   
            keystore.load(pfx_stream, pwd);                                                                 
            KeyManagerFactory key_manager_factory = KeyManagerFactory.getInstance("SunX509");               // Setup KeyManagerFactory - decides which key material to use.
            key_manager_factory.init(keystore, pwd);
            TrustManagerFactory trust_manager_factory = TrustManagerFactory.getInstance("SunX509");         // Setup TrustManagerFactory - decides whether to allow connections.
            trust_manager_factory.init(keystore);

            // HTTPS Config
            sslContext.init(key_manager_factory.getKeyManagers(), trust_manager_factory.getTrustManagers(), null);
            https_server.setHttpsConfigurator( 
                new HttpsConfigurator(sslContext) {
                    public void configure (HttpsParameters params) {
                        try {
                            // initialise the SSL context
                            SSLContext ssl_context = SSLContext.getDefault();
                            SSLEngine ssl_engine = ssl_context.createSSLEngine();
                            // params
                            params.setNeedClientAuth(true);
                            params.setCipherSuites(ssl_engine.getEnabledCipherSuites() );
                            params.setProtocols(ssl_engine.getEnabledProtocols() );
                            // get the default parameters
                            SSLParameters defaultSSLParameters = ssl_context.getDefaultSSLParameters();
                            params.setSSLParameters(defaultSSLParameters);
                        }
                        catch (NoSuchAlgorithmException except)
                        {
                            System.out.println("Err: " + except);
                            except.printStackTrace();
                            System.exit(1);
                        }
                    }
                } 
            );
            https_server.createContext("/", new THandler(this.buckets, this.question_clt));
            https_server.setExecutor(null);         // default executor

            System.out.println("Starting Secure Server - https://localhost:"+this.http_port);
            https_server.start();                                                                       // Start Server
        }
        catch (BindException except) {
            System.out.println("Err: Port already in use.");
            System.out.println("Run > sudo kill $(sudo lsof -t -i:"+this.http_port+")");
            System.exit(1);
        }
        catch (Exception except) {
            except.printStackTrace();
            stop_secure_server();
            System.exit(1);
        }
    }

    private void stop_server() throws IOException { this.server.stop(0); this.question_clt.stop(); }
    private void stop_secure_server() throws IOException { this.https_server.stop(0); this.question_clt.stop(); }

    /** 
     * Rudimentary Leaky Bucket Implementation.
     * Will not stop sophisticated flooding attacks but at least prevents clients originating from single IP from overwhelming server
     */
    private class IPBuckets {

        /** Bucket capacity - Number of connections */
        private static final short BUCKET_CAPACITY = 40;
        /** Maximum leak rate */
        private static final short MAX = 350;
        /** Minimum leak rate */
        private static final short MIN = 300;

        private Map<InetAddress,Short> ip_buckets;
        private Random random;
        private Timer timer;
        private int runtime;

        /** Constructor for IPBuckets */
        private IPBuckets() {
            this.ip_buckets = new HashMap<>();
            this.random = new Random();
            this.runtime = random.nextInt(MAX-MIN+1) + MIN;
            this.timer = new Timer();

            this.timer.scheduleAtFixedRate(new TimerTask() {
                @Override
                public void run() {
                    leak();
                }
            }, 0, runtime);
        }

        /** Create new bucket for this ip */
        private void add_ip(InetAddress ip) {
            short tokens = (short) 1;
            if (ip_buckets.containsKey(ip)) {
               tokens = (short) (ip_buckets.get(ip)+1);
            }
            this.ip_buckets.put(ip, tokens);
        }

        /** Check if this bucket exists */
        private boolean has_ip(InetAddress ip) {
            return ip_buckets.containsKey(ip);
        }

        /** Check if this IP's bucket is full */
        private boolean bucket_full(InetAddress ip) {
            if (ip_buckets.containsKey(ip)) {
                if (ip_buckets.get(ip) >= BUCKET_CAPACITY)
                    return true;
            }
            return false;
        }

        /** Leak 1 token from all buckets. */
        private void leak() {

            Iterator<Entry<InetAddress, Short>> bucket_iterator = ip_buckets.entrySet().iterator();
            while (bucket_iterator.hasNext()) {
                Entry<InetAddress, Short> bucket = bucket_iterator.next();
                InetAddress ip = bucket.getKey();
                short level = bucket.getValue();
                if (--level > 0) {
                    this.ip_buckets.put(ip, level);
                }
                else {
                    bucket_iterator.remove();
                }
            }
            if ( (bucket_iterator.hasNext() && DEBUG) || (bucket_iterator.hasNext() && DEMO) ) print();
        }

        /** Print all buckets (for debugging) */
        private void print() {
            System.out.println("Buckets:");
            for (Entry<InetAddress,Short> keyVal : ip_buckets.entrySet()) {
                InetAddress inet = keyVal.getKey();
                byte[] byte_list = inet.getAddress();
                System.out.print("+ ");
                for (byte b: byte_list) {
                    System.out.print(b);
                }
                System.out.print(": "+keyVal.getValue());
                System.out.print("\n");

            }
        }
    }

    /**
     * HttpHandler implementation for Testing-Server
     */
    private class THandler implements HttpHandler {

        /** Defining undefined HTML Error Response 429: Too Many Requests */
        private static final int HTTP_TOO_MANY = 429;

        private List<SessionHolder> session_list;
        private Map<String, String> user_pass;
        private IPBuckets buckets;
        private String[] html;
        private QuestionClient client;
        private PasswordAuthentication passAuth;

        /** Constructor for HttpHandler */
        protected THandler(IPBuckets buckets, QuestionClient client) throws IOException {
            this.session_list = new ArrayList<>();
            this.user_pass = new HashMap<>();
            load_passwords(PASSWORDS_LOC);
            this.buckets = buckets;
            this.html = new String[5];
            this.html[0] = load_html("html"+File.separator+"index.html");
            this.html[1] = load_html("html"+File.separator+"quiz.html");
            this.html[2] = load_html("html"+File.separator+"mcq.html");
            this.html[3] = load_html("html"+File.separator+"prog.html");
            this.html[4] =  load_html("html"+File.separator+"summary.html");
            this.client = client;
            this.passAuth = new PasswordAuthentication();
        }

        /** Load password.txt to create username/password list */
        private String load_passwords(String filepath) throws IOException, FileNotFoundException {
            try(BufferedReader buff_read = new BufferedReader(new FileReader(filepath))) {
                StringBuilder string_build = new StringBuilder();
                String line = buff_read.readLine();

                while (line != null) {
                    String[] password_pair = line.split(":");
                    if (password_pair.length == 2) {
                        this.user_pass.put(password_pair[0], password_pair[1]);
                    }
                    line = buff_read.readLine();
                }
                return string_build.toString();
            }
        }

        /** Load all *.html files */
        private String load_html(String filepath) throws IOException, FileNotFoundException {
            try(BufferedReader buff_read = new BufferedReader(new FileReader(filepath))) {
                StringBuilder string_build = new StringBuilder();
                String line = buff_read.readLine();

                while (line != null) {
                    string_build.append(line);
                    string_build.append(System.lineSeparator());
                    line = buff_read.readLine();
                }
                return string_build.toString();
            }
        }

        @Override
        /** Overwritten method for handling http input/response */
        public void handle(HttpExchange http_exchange) throws IOException {

            // get HTTP POST
            InputStreamReader input_stream = new InputStreamReader(http_exchange.getRequestBody(), "utf-8");
            BufferedReader buff_read = new BufferedReader(input_stream);
            String http_post = buff_read.readLine();
            if (DEBUG) System.out.println("POST in: "+http_post);

            // Response
            InetAddress remote_ip = http_exchange.getRemoteAddress().getAddress();
            OutputStream outstream = http_exchange.getResponseBody();

            // Rate-limiting check
            if (!buckets.bucket_full(remote_ip)) {
                buckets.add_ip(remote_ip);
                if (DEBUG || DEMO) buckets.print();
            }
            else {
                if (DEBUG || DEMO) System.out.println("Bucket Overflow");
                String response = "<h1>429: Too Many Requests</h1>";
                http_exchange.sendResponseHeaders(HTTP_TOO_MANY, response.length());
                outstream.write(response.getBytes());
            }

            // Handle POST requests 
            if (http_post == null)
                respond_login(http_exchange);
            else if (http_post != null)
                handle_post(http_exchange, http_post);

            outstream.close();
        }

        /** respond with login page when null POST request is detected */
        private void respond_login(HttpExchange http_exchange) throws IOException {
            OutputStream outstream = http_exchange.getResponseBody();

            http_exchange.sendResponseHeaders(HttpURLConnection.HTTP_OK, this.html[0].length());
            outstream.write(this.html[0].getBytes());
        }

        /**
         * Handles HTTP POST requests
         * i.e. Logins (New Session/Existing Session), Quiz Start, Quiz Question (MCQ), Quiz Question (Program) 
         * Based on:
         * Oracle Java documentation
         * https://www.studytonight.com/servlet/hidden-form-field.php
         * https://www.codeproject.com/tips/1040097/create-simple-http-server-in-java
         */
        private void handle_post(HttpExchange http_exchange, String query) throws IOException {

            Map<String, String> params = new HashMap<>();
            OutputStream outstream = http_exchange.getResponseBody();

            String login_html = this.html[0];

            String pairs[] = query.split("[&]");

            for (String pair: pairs) {
                String key_val[] = pair.split("[=]");
                String key = null, value = null;
                if (key_val.length > 0)
                    key = URLDecoder.decode(key_val[0], System.getProperty("file.encoding"));
                if (key_val.length > 1)
                    value = URLDecoder.decode(key_val[1], System.getProperty("file.encoding"));
                
                params.put(key, value);
            }

            // A: Login - Generate Session - [*login*, user, pwd]
            if (params.containsKey("login") && params.containsKey("user") && params.containsKey("pwd")) {
                case_login(http_exchange, params);
            }
            // B: Already Logged in - [*session*, ... ]
            else if (params.containsKey("session")) {

                String session_id = params.get("session");
                SessionHolder session = get_curr_session(session_id); 
                // If bad session, respond with bad request
                if (session == null) {
                    String response = "<h1>400: Bad Request</h1>";
                    http_exchange.sendResponseHeaders(HttpURLConnection.HTTP_BAD_REQUEST, response.getBytes().length);
                    outstream.write(response.getBytes());
                }
                // CASE B1: Generate Quiz Questions - [*generate*, session]
                if (params.containsKey("generate")) {
                    case_generate_quiz(http_exchange, session);
                }
                // CASE B2a: Select Next Question - [*next*, session]
                else if (params.containsKey("next")) {
                    // Move Session to Next/Previous Question
                    case_next_question(http_exchange, session);
                }
                // Case B2b: Select Previous Question - [*prev*, session]
                else if (params.containsKey("prev")) {
                    case_prev_question(http_exchange, session);
                }
                // CASE B3: Submit this Question for Marking - [*mark*, mcq, session]
                else if (params.containsKey("mark")) {
                    case_mark_question(http_exchange, session, params);
                }

            }
            // CASE C: Invalid Request
            else {
                http_exchange.sendResponseHeaders(HttpURLConnection.HTTP_OK, login_html.getBytes().length);
                outstream.write(login_html.getBytes());
            }
        }
 
        /** Case A - Handle Logins */ 
        private void case_login(HttpExchange http_exchange, Map<String, String> params) throws IOException {
            OutputStream outstream = http_exchange.getResponseBody();
            String login_html = this.html[0];
            String entry_html = this.html[1];
            String end_html = this.html[4];

            String username = params.get("user");
            String password = params.get("pwd");

            // Password Authentication
            if (authenticateLogin(username, password)) {

                if (DEBUG || DEMO) System.out.println("Authenticated!");
                String session_id = null;
                SessionHolder current_session = null;

                // Loop through active sessions - if session already exists, use it.
                for (SessionHolder session: this.session_list) {
                    if (username.equals(session.get_username())) {
                        session_id = session.get_session();
                        current_session = session;
                        break;
                    }
                }
                // Create new sessions if does not exist
                if (current_session == null) {
                    session_id = UUID.randomUUID().toString();      // Only 121 bits of randomness - We'll consider this good enough - Collision likelihood >1:billion
                    current_session = new SessionHolder(this.client, username, session_id);
                    // New Session -> session_list
                    this.session_list.add(current_session);
                }

                if (DEBUG) current_session.print_session();

                if (!current_session.has_quiz_started()) {
                    String session_out = String.format(entry_html, session_id);
                    http_exchange.sendResponseHeaders(HttpURLConnection.HTTP_OK, session_out.getBytes().length);
                    outstream.write(session_out.getBytes());
                }
                else if (current_session.has_completed()) {
                    String session_out = String.format(end_html, current_session.get_total_marks());
                    http_exchange.sendResponseHeaders(HttpURLConnection.HTTP_OK, session_out.getBytes().length);
                    outstream.write(session_out.getBytes());
                }
                else {
                    respond_question(http_exchange, current_session);            // Respond with current question HTML
                }
            }
            else {
                if (DEBUG) System.out.println("Authentication Failed.");
                String response = login_html.concat(" Authentication Failed");
                http_exchange.sendResponseHeaders(HttpURLConnection.HTTP_OK, response.getBytes().length);
                outstream.write(response.getBytes());
            }
        }

        /** Authenticate plaintext Username/Password - True if authenticates, false otherwise. */
        @Deprecated
        private boolean old_authenticateLogin(String username, String password) {
            if (username != null && password != null)
                if (this.user_pass.containsKey(username))
                    if (password.equals(this.user_pass.get(username)))
                        return true;
            return false;
        }

        /** Authenticate hashed Username/Password - True if authenticates, false otherwise. */
        private boolean authenticateLogin(String username, String password) {

            Iterator<Entry<String, String>> user_pass_it = this.user_pass.entrySet().iterator();
            while (user_pass_it.hasNext()) {
                Entry<String, String> entry = user_pass_it.next();
                String user_hash = entry.getKey();

                if (this.passAuth.authenticate(username.toCharArray(), user_hash)) {
                    String pwd_hash = entry.getValue();
                    if (this.passAuth.authenticate(password.toCharArray(), pwd_hash)) {
                        return true;
                    }
                    else continue;
                }
                else continue;
            }
            return false;
        }

        /** Using session identification, retrieve the current session */
        private SessionHolder get_curr_session(String session_id) {

            // Loop through active sessions - if session already exists, use it.
            for (SessionHolder session: this.session_list) {
                //if (!session.has_completed() && session_id.equals(session.get_session())) {
                if (session_id.equals(session.get_session())) {
                    return session;
                }
            }
            return null;
        }

        /** Case B1 - Prompt to Generate questions from question-server, respond with first MCQ question */
        private void case_generate_quiz(HttpExchange http_exchange, SessionHolder session) throws IOException {
            session.generate_questions();                        // Generate Questions (From Testing-Server)
            respond_question(http_exchange, session);            // Respond with Question HTML
        }

        /** Case B2a - Go to next MCQ question */
        private void case_next_question(HttpExchange http_exchange, SessionHolder session) throws IOException {
            byte q_number = session.get_curr_question_number();
            if (q_number+1 < TOTAL_QUESTIONS)                       // Prevent overflow
                session.set_curr_question((++q_number));            // move to next question
            respond_question(http_exchange, session);               // Respond with Question HTML
        }

        /** Case B2b - Go to previous MCQ question */
        private void case_prev_question(HttpExchange http_exchange, SessionHolder session) throws IOException {
            byte q_number = session.get_curr_question_number();
            if (q_number > 0)                                      // Prevent underflow
                session.set_curr_question((--q_number));           // move to next question
            respond_question(http_exchange, session);              // Respond with Question HTML
        }

        /** Case B3 - Submit Question for marking */
        private void case_mark_question(HttpExchange http_exchange, SessionHolder session,  Map<String, String> params) throws IOException {

            String end_html = this.html[4];

            if (params.containsKey("mcq")) {

                String answer = params.get("mcq");
                session.submit_question(answer);                   // Submit question for marking                
            }
            else if (params.containsKey("code")) {

                String answer = params.get("code");
                if (answer != null && answer.length() > 0) {
                    // Run python subprocess to process user-written code
                    ProcessBuilder process_b = new ProcessBuilder("python", "-c", answer);
                    Process process = process_b.start();
                    
                    BufferedReader in = new BufferedReader(new InputStreamReader(process.getInputStream()));
                    answer = in.readLine();
                    System.out.println("Processed Answer: "+answer);
                    session.submit_question(answer);                   // Submit question for marking
                }
            }
            // Sesion not completed - respond with question
            if (!session.has_completed()) {
                respond_question(http_exchange, session);              // Respond with Question HTML
            }
            // Session completed - respond with summary page
            else {
                session.dump_session();
                String session_end = String.format(end_html, session.get_total_marks());
                http_exchange.sendResponseHeaders(HttpURLConnection.HTTP_OK, session_end.getBytes().length);
                OutputStream outstream = http_exchange.getResponseBody();
                outstream.write(session_end.getBytes());
            }
        }

        /** Respond to HTTP POST with filled question html */
        private void respond_question(HttpExchange http_exchange, SessionHolder session) throws IOException {
            OutputStream outstream = http_exchange.getResponseBody();

            String mcq_html = this.html[2];
            String prog_html = this.html[3];

            Question question = session.get_curr_question();                        // Question Object

            String session_id = session.get_session();                              // Session ID
            byte q_number = (byte) (session.get_curr_question_number() + 1);        // Question Number

            String q_string = question.get_question();                              // Question String
            byte mark = question.get_mark();                                        // Marks for this question
            byte attempt_count = question.get_attempt_count();                      // Attempt count

            short total_marks = session.get_total_marks();                          // Total marks (all questions)
            String session_out = null;
            if (question.get_question_type() == QType.MCQ) {

                String prev_btn = "<input name='prev' value='previous' type='submit'>";
                String next_btn = "<input name='next' value='next' type='submit'>";
                String submit_btn = "<input name='mark' value='submit' type='submit'>";

                if (q_number == 1) prev_btn = "";                                   // Don't draw prev_btn for first question
                if (q_number == TOTAL_QUESTIONS) next_btn = "";                     // Don't draw next_btn for last question
                if (attempt_count == 0 || mark > 0) submit_btn = "";                // Don't draw submit_btn if attempts exhausted OR marked

                String[] mcq_options = question.get_mcq_options();                  // MCQ Strings
                session_out = String.format(mcq_html, q_number, q_string, session_id, 
                                                mcq_options[0], mcq_options[1], mcq_options[2], mcq_options[3], 
                                                prev_btn, next_btn, submit_btn,
                                                attempt_count, mark, total_marks);
                //(mcq_html, question number, the question, session, A, B, C, D, prev button, next button, submit button, attempt count, mark, total marks);
            }
            else {

                String prev_btn = "<input name='prev' value='previous' type='submit'>";
                String next_btn = "<input name='next' value='next' type='submit'>";
                String submit_btn = "<input name='mark' value='submit' type='submit'>";

                if (q_number == 1) prev_btn = "";                                   // Don't draw prev_btn for first question
                if (q_number == TOTAL_QUESTIONS) next_btn = "";                     // Don't draw next_btn for last question
                if (attempt_count == 0 || mark > 0) submit_btn = "";                // Don't draw submit_btn if attempts exhausted OR marked

                session_out = String.format(prog_html, q_number, q_string, session_id, 
                                                prev_btn, next_btn, submit_btn,
                                                attempt_count, mark, total_marks);
                //(prog_html, question number, the question, session, prev button, next button, submit button, attempt count, mark, total marks);
            }

            if (DEBUG) session.print_session();
            http_exchange.sendResponseHeaders(HttpURLConnection.HTTP_OK, session_out.getBytes().length);
            outstream.write(session_out.getBytes());
        }
    }

    /**
    * Class for managing Question->Testing Server Connection
    * Loosely Based on: http://www.oracle.com/technetwork/java/socket-140484.html
    */
    private class QuestionClient {

        private Socket socket;
        private SSLSocket s_socket;
        private OutputStream outstream;
        private InputStream instream;

        private PrintWriter out;
        private BufferedReader in;

        private final static String EOM = "#^";
        private final static String SEP = "!_!";

        /* Note: Protocol Reference
                          Sends                              Receives
        INIT_SESS "IS|session#|Q_limit"           ->     "OK"/"ERR"
        GET_Q     "QG|session#|Q#"                ->     "question|a|b|c|d"/"question"/"ERR"
        MARK_Q    "QM|session#|Question#|Ans#"    ->     "true"/"false"/"ERR"
        */

        private QuestionClient(String remotehost, int qport, boolean secure) throws IOException, UnknownHostException {
            try {
                if (secure)
                    this.connect_secure(remotehost, qport);
                else
                    this.connect_unsecure(remotehost, qport);
            } 
            catch (SSLException except) {
                System.out.println("Err: Question Server is not secured with TLS");
                System.exit(1);

            }

            System.out.println("Now connected to Question-Server - " + remotehost + ":" + qport);
            
            String response = this.in.readLine();
            if (DEMO) System.out.println("TS: "+response);
        }

        /** Connect unsecure socket - Partly deprecated backup method */
        private void connect_unsecure(String remotehost, int qport) throws IOException {
            this.socket = new Socket(remotehost, qport);

            this.outstream = this.socket.getOutputStream();           // Write to output stream
            this.instream = this.socket.getInputStream();             // Read from input stream
            this.out = new PrintWriter(outstream, true);
            this.in = new BufferedReader(new InputStreamReader(instream));
        }

        /** Connect securely with TLS */
        private void connect_secure(String remotehost, int qport) throws IOException {
            SSLSocketFactory factory = (SSLSocketFactory)SSLSocketFactory.getDefault();
            this.s_socket = (SSLSocket)factory.createSocket(remotehost, qport);
            this.s_socket.startHandshake();

            this.outstream = this.s_socket.getOutputStream();           // Write to output stream
            this.instream = this.s_socket.getInputStream();             // Read from input stream
            this.out = new PrintWriter(outstream, true);
            this.in = new BufferedReader(new InputStreamReader(instream));
        }


        /** Method for INIT_SESS "IS|session#|Questions" -> "true/false" */
        private void create_session(String session) throws IOException {
            
            this.out.println( ("\t\t\tIS"+SEP+session+SEP+TOTAL_QUESTIONS+SEP+QSERVER_USR+SEP+QSERVER_PWD+EOM+"\t\t\t"));
            
            String response = this.in.readLine().replace(SEP, "|");
            if (DEMO) {
                System.out.println("TS: IS"+"|"+session+"|"+TOTAL_QUESTIONS);                                    // Send INIT_SESS Request to Question-Server -> Expect acknowledgement
                System.out.println("QS: "+response);
            }


        }

        /** Method for GET_Q "QG|session#|Q#" -> "MCQ|question|a|b|c|d or PROG|question" */ 
        private Question fetch_question(String session, byte qnum) throws IOException {

            this.out.println(("\t\t\tQG"+SEP+session+SEP+qnum)+EOM+"\t\t\t");                                    // Send GET_Q Request to Question-Server -> Expect reply back with question
            String response = this.in.readLine();                                           // get response

            if (DEMO) {
                System.out.println("TS: QG"+"|"+session+"|"+qnum);
                System.out.println("QS: "+response.replace(SEP, "|"));
            }

            String[] question_data;
            if (response != null) {
                question_data = response.split(SEP);
            }
            else {
                // Revert to default
                if (qnum < TOTAL_MCQ) {
                    question_data = new String[] {"%question%", "%a%", "%b%", "%c%", "%d%"};    // Default values
                }
                else {
                    question_data = new String[] {"%question%"};
                }
            }

            Question question = new Question(question_data);

            return question;
        }

        /** Method for MARK_Q "QM|session#|Question#|Ans#" -> "true/false" */
        private boolean mark_question(String session, byte qnum, String answer) throws IOException {

            this.out.println("\t\t\tQM"+SEP+session+SEP+qnum+SEP+answer+EOM+"\t\t\t");                        // Send MARK_Q Request to Question-Server -> Expect true/false response.
            String response = this.in.readLine();                                          // get response

            if (DEMO) {
                System.out.println("TS: QM"+"|"+session+"|"+qnum);
                System.out.println("QS: "+response.replace(SEP, "|"));
            }

            if (response != null && response != "ERR") {
                return Boolean.valueOf(response);
            }
            else return false;
        }

        /** Return array of questions generated by Question-Server. (Send INIT_SESS, then retrieve all questions with GET_Q requests) */
        private Question[] generate_quiz(String session) throws IOException {
            Question[] questions = new Question[TOTAL_QUESTIONS];

            this.create_session(session);                          // Tell [Question-Server] to create a new session object
            for (byte i=0; i<TOTAL_QUESTIONS; i++) {          // For each question
                questions[i] = fetch_question(session, i);      // Ask [Question-Server] for question data. 
            }

            return questions;
        }

        /** Close socket and out/in streams*/
        private void stop() throws IOException {
            this.instream.close();
            this.outstream.close();
            if (this.socket != null)
                this.socket.close();
            if (this.s_socket != null)
                this.s_socket.close();
        }

    }

    /** Enum for Question Type */
    private enum QType {
        MCQ, PROG
    }

    /** Container class for Questions */
    private class Question {

        private static final byte ATTEMPT_LIMIT = 3;
        private static final byte FULL_MARKS = 3;
        private static final byte MCQ_OPTIONS = 4;

        private QType type;
        private String question;
        private byte attempt_count;
        private byte mark;
        private String[] mcq_options;       // MCQ only
        private String response;

        /** General Constructor for both MCQ and programming questions */
        private Question(String[] options) {
            // check null
            if (options != null) {
                //  [<question>, <a>, <b>, <c>, <d>]
                byte len = MCQ_OPTIONS+1;
                if (options.length == len) {
                    this.type = QType.MCQ;
                    this.mcq_options = Arrays.copyOfRange(options, 1, len);
                }
                // [<question>]
                else this.type = QType.PROG;

                this.question = options[0];
                this.attempt_count = ATTEMPT_LIMIT;
                this.mark = 0;
            }
        }

        // Set Methods
        /** Set question response */
        private void set_response(String response) { this.response = response; }

        /** Method called on failed attempt. Reduce attempts left by one. Return true if attempts left is 0. */
        private boolean attempt_failed() {
            this.attempt_count = (byte) Math.max(0, this.attempt_count-1);
            return (this.attempt_count == 0);
        }

        /**  Method called on successful attempt. Set marks to attempts left. */
        private void attempt_succeeded() { 
            this.mark = this.attempt_count;
            this.attempt_count = 0;
        }

        // Get Methods
        private QType get_question_type() { return this.type; }
        private String get_question() { return this.question; }
        private String[] get_mcq_options() { return this.mcq_options; }
        private byte get_attempt_count() { return this.attempt_count; }
        private byte get_mark() { return this.mark; }
        private String get_response(String response) { return this.response; }
    }

    /** Class for managing Sessions */
    private class SessionHolder {

        private BitSet status;
        private boolean quiz_started;
        private String username;
        private String session;
        private short total_marks;
        private byte question_number;
        private Question[] questions;

        private QuestionClient client;

        /** Constructor */
        private SessionHolder(QuestionClient client, String username, String session) {
            this.status = new BitSet(TOTAL_QUESTIONS);      // Create bitset of 10 bits
            this.status.set(0, TOTAL_QUESTIONS);            // Set all bits to 1
            this.quiz_started = false;
            this.username = username;
            this.session = session;
            this.total_marks = 0;
            this.question_number = 0;
            this.client = client;
        }
        /** Method called to generate quiz with 10 questions */
        private void generate_questions() throws IOException {
            this.quiz_started = true;
            this.questions = this.client.generate_quiz(this.session);
        }

        /** Submit question data using this object's QuestionClient to Question-Server, get response */
        private void submit_question(String answer) throws IOException {

            Question question = questions[this.question_number];
            question.set_response(answer);
            boolean correct = this.client.mark_question(this.session, this.question_number, answer);

            if (correct) {
                question.attempt_succeeded();
                this.total_marks = (short) (this.total_marks + question.get_mark());
                this.status.clear(this.question_number);                // Set bitset bit to 0 for this question
                if (DEMO || DEBUG) System.out.println("Status: "+this.status);
            }
            else {
                if (question.attempt_failed()) {
                    // if attempt_failed returns true, no attempts left.
                    this.status.clear(this.question_number);            // Set bitset bit to 0 for this question
                    if (DEMO || DEBUG) System.out.println("Status: "+this.status);
                }
            }
 
        }

        /** Print session to console for debugging */
        private void print_session() {
            // For debugging
            System.out.println("-------------------------------------------");
            System.out.println("SESSION INFO");
            System.out.println("Complete?: "+this.status);
            System.out.println("Quiz Started? "+this.quiz_started);
            System.out.println("Username: "+this.username);
            System.out.println("Session: "+this.session);
            System.out.println("Current Question: "+this.question_number);
            System.out.println("Marks: "+this.total_marks);
            System.out.println("-------------------------------------------");
        }

        /** Dump session to log/results file */
        private void dump_session() {

            if (has_completed()) {
                String result = this.username+" : "+this.total_marks;
                //System.out.println(result);
            }
        }
        // Get methods
        private boolean has_completed() { return (this.status.isEmpty()); }
        private boolean has_quiz_started() {return this.quiz_started; }
        private String get_username() { return this.username; }
        private String get_session() { return this.session; }
        private Question get_curr_question() { return this.questions[this.question_number]; }
        private byte get_curr_question_number() { return this.question_number; }
        private short get_total_marks() {return this.total_marks; }
        // Set methods
        
        private void set_curr_question(byte num) {
            if ((0 <= num) && (num < this.questions.length)) {
                this.question_number = num;
            }        
        }
    }

    public static void main(String[] args) throws IOException {

        boolean tls = true;
        int hport = HPORT;
        String qserver = "localhost";
        int qport = QPORT;
        
        // Usage Example: java QuestionServer qserver=10.0.0.9 ques=48009 tls=true http=443
        for (String arg: args) {
            String[] pairs = arg.split("=");
            switch(pairs[0]) {
                case "qserver": qserver = pairs[1]; break;
                case "tls": tls = Boolean.parseBoolean(pairs[1]); break;
                case "httpport": hport = Integer.parseInt(pairs[1]); break;
                case "qport": qport = Integer.parseInt(pairs[1]); break;
                default: break;
            }
        }

        try {

            System.setProperty("javax.net.ssl.trustStore", TRUSTSTORE_LOC);
            System.setProperty("javax.net.ssl.trustStorePassword", TRUSTSTORE_PWD);

            TestingServer testing_srv = new TestingServer(tls, hport, qserver, qport);        // Testing-Server part of project
            System.out.println("Type kill to terminate server");
            // Terminate server + socket connections when kill is inputed into console
            Scanner scanner = new Scanner(System.in);
            String input = "";
            while (!input.equals("kill")) {
                input = scanner.next();
                System.out.println(input);
            }
            if (tls) 
                testing_srv.stop_secure_server();
            else 
                testing_srv.stop_server();

            System.exit(0);
        }
        catch(ConnectException except) {
            System.out.println("Err: No connection to question server");
            System.exit(1);
        }
    }
}

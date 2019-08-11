/** Load password.txt to create username/password list */
import java.io.*;
import java.util.*;

public class PasswordGenerator {

    /** Constructor for PasswordGenerator*/
    public PasswordGenerator(String password_loc, String hash_loc) throws IOException {
        convert_passwords(password_loc, hash_loc);
    }

    private void convert_passwords(String inpath, String outpath) throws IOException, FileNotFoundException {

        try(BufferedReader buff_read = new BufferedReader(new FileReader(inpath))) {
            try (PrintWriter writer = new PrintWriter(outpath)) {

                StringBuilder string_build = new StringBuilder();
                String line = buff_read.readLine();
                PasswordAuthentication passAuth = new PasswordAuthentication();

                while (line != null) {

                    String[] password_pair = line.split(":");
                    if (password_pair.length == 2) {
                        String usr_hash = passAuth.hash(password_pair[0].toCharArray());
                        String pwd_hash = passAuth.hash(password_pair[1].toCharArray());
                        writer.println(usr_hash+":"+pwd_hash);
                    }
                    line = buff_read.readLine();
                }
            }
        }
    }

    public static void main(String[] args) throws IOException {

        if (args.length == 2) {
            String pwd = args[0];
            String out = args[1];
            PasswordGenerator pgen = new PasswordGenerator(pwd, out);
        } 
        else {
            System.out.println("Usage: java PasswordGenerator [password.txt] [hash.txt]");
        }
    }
}
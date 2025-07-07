// Fig. 16.17: WordTypeCount.java
// Program counts the number of occurrences of each word in a String.
import java.util.Map;
import java.util.HashMap;
import java.util.Set;
import java.util.TreeSet;
import java.util.Scanner;

public class WordTypeCount {
    public static void main(String[] args) {
        // create HashMap to store String keys and Integer values
        Map<String, Integer> myMap = new HashMap<>();
        Map<Character,Integer> myCharMap=new HashMap<>();

        createMap(myMap); // create map based on user input
        stringMapToCharMap(myMap,myCharMap);
        displayMap(myCharMap); // display map content
    }

    // create map from user input
    private static void createMap(Map<String, Integer> map) {
        Scanner scanner = new Scanner(System.in); // create scanner
        System.out.println("Enter a string:"); // prompt for user input
        String input = scanner.nextLine();

        // tokenize the input
        String[] tokens = input.split(" ");

        // processing input text
        for (String token : tokens) {
            String word = token.toLowerCase(); // get lowercase word

            // if the map contains the word
            if (map.containsKey(word)) { // is word in map?
                int count = map.get(word); // get current count
                map.put(word, count + 1); // increment count
            }
            else {
                map.put(word, 1); // add new word with a count of 1 to map
            }
        }
    }

    private static void stringMapToCharMap(Map<String,Integer> map,Map<Character,Integer> charMap)
    {
        for(Map.Entry<String,Integer> entry:map.entrySet())
        {
            String str=entry.getKey();
            for(int i=0;i<str.length();i++)
            {
                if(str.charAt(i)>='a'&&str.charAt(i)<='z')
                    charMap.put(str.charAt(i),charMap.getOrDefault(str.charAt(i),0)+entry.getValue());
            }
        }
    }
    // display map content
    private static void displayMap(Map<Character, Integer> map) {
        Set<Character> keys = map.keySet(); // get keys

        // sort keys
        TreeSet<Character> sortedKeys = new TreeSet<>(keys);

        System.out.printf("%nMap contains:%nKey\t\tValue%n");

        // generate output for each key in map
        for (Character key : sortedKeys) {
            System.out.printf("%-10s%10s%n", key, map.get(key));
        }

        System.out.printf(
                "%nsize: %d%nisEmpty: %b%n", map.size(), map.isEmpty());
    }
} 
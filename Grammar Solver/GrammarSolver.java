import java.util.*;
/** This is the class for the GrammarMain program.
 *
 *  @author Kun Ki Lee @ CS145
 *  @version 5/4/2018
 */
public class GrammarSolver
{
   private Map<String, List<String>> myMap;
   private List<String> listw;   
   public GrammarSolver(List <String> rules)
   {
      if(rules.isEmpty() == true || rules == null) throw new IllegalArgumentException();
      myMap = new TreeMap<String, List<String>>();
      for(int i = 0; i<rules.size();i++)
      {
         
         String line = rules.get(i); //to get each line in the List
         String[] part = line.split("::=");
         String[] part2 = part[1].split("[|]");
         String nterminal = part[0];//the non terminal Strings which will be the key for the Map
         for(String x : myMap.keySet())
         {
            if(x.equals(nterminal)) throw new IllegalArgumentException();//if key already exists in the map
         }
         //add the values into a set
         listw = new ArrayList<String>();
         for(int j = 0; j < part2.length;j++)
         {
            listw.add(part2[j]);
         }
         myMap.put(nterminal, listw);
      }
   }
 
   /** This method is to check if the symbol is a non-terminal in the grammar or not.
    *  @param Symbols is the string to check on whether it is a key in the map or not as the key is a non-terminal in the grammar.
    *  @return true if the string is indeed a non-terminal in the grammar.
    *  @return false if the string is not a non-terminal in the grammar.
    */
   public boolean contains(String Symbols)
   {
      if(Symbols == null || Symbols.length() == 0) throw new IllegalArgumentException();
      for(String w : myMap.keySet())
      {
         if(Symbols.equals(w) == true)
         {
            return true;
         }
      }
      return false;
   }
   
   /** This method returns the non-terminal symbols of your grammar which is in the keySet of the map.
    *  @return myMap.keySet() is the set of keys of the map.
    */       
   public Set<String> getSymbols()
   {
      return myMap.keySet();
   }
   
   /** This method is used to randomize the string to be taken from the given Symbol.
    *  @param Symbol is the non-terminal in the grammar which string would be chosen from to print randomly.
    *  @return answer returns the String
    */
   public String generate(String Symbol)
   {
      String answer = "";
      if(Symbol== null||Symbol.length() == 0) throw new IllegalArgumentException();
      for(String w : myMap.keySet())
      {
         if(w.equals(Symbol))
         {
            //use recursion if the one of the values is also a key in the map.
            Random rand = new Random();
            int randnum = rand.nextInt((myMap.get(Symbol)).size());

            String chosenw = myMap.get(Symbol).get(randnum);
            String[] choiceArray = chosenw.split("[ \t]+");
            for(int i = 0; i<choiceArray.length ; i++)
            {
               if(myMap.containsKey(choiceArray[i]))
               {
                 answer += generate(choiceArray[i]);
               }
               else
               {
                 answer += choiceArray[i] + " ";
               }
              
            }
          }

       }
         return answer;
   }
}
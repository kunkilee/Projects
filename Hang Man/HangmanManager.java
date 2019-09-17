import java.util.*;

/** This is the class for the HangmanMain program.
 *
 *  @author Kun Ki Lee @ CS145
 *  @version 4/29/2018
 */
public class HangmanManager
{
   private char cguess;
   private int wlength;
   private int chance;
   private Set<String> wordset;
   private SortedSet<Character> gletters;
   private Map<String, Set<String>> wordmap;
   private String bestPattern;
   private String initialP;
   private String savedPattern;
   
   /** This is the constructor for this class,it passes a dictionary 
    *  of words, a target word length and the maximum number of wrong guesses
    *  the player is allowed to make.
    *  @param List<String> dictionary is the list of words from the dictionary
    *  @param length is the amount of word length the user wants to guess.
    *  @param max is the maximum number of tries the user would like. 
    */
   public HangmanManager(List<String> dictionary, int length, int max)
   {
      wlength = length;
      chance = max;
      savedPattern = "";
      wordset = new TreeSet<String>();
      gletters = new TreeSet<Character>();
      if(length < 1 || max < 0) throw new IllegalArgumentException();
      
      for(int i = 0; i<wlength ; i++)
      {
         savedPattern += '-';
      }

      for(String x : dictionary)
      {
         if(x.length() == length)
         {
            wordset.add(x);
         }
      }
      
      bestPattern = savedPattern;
   }
   
   /** This method gets the current set of words being considered by the HangmanManager.
    *  @return wordset is the current set of words being considered.
    */
   public Set<String> words()
   {
      return wordset;
   }
   
   /** This method is to find out the amount of chance the player has left.
    *  @return chance is the amount of chance left.
    */
   public int guessesLeft()
   {
      return chance;
   }
   
   /** This method is to find the current set of letters that have been guessed.
    *  @return gletters returns the current set of letters the user has guessed.
    */
   public SortedSet<Character> guesses()
   {
      return gletters;
   }
   
   /** This method returns the current pattern to be displayed for the hangman game 
    *  including guesses that have been made.
    *  @return spacing is the pattern to be displayed in the game.
    */
   public String pattern()
   {
      String x ="";
      String spacing = "";
      
      if(wordset.isEmpty()) throw new IllegalStateException();
      for(int i = 0; i < bestPattern.length();i++)
      {
         if (savedPattern.charAt(i) != '-')
         {
            x = x + savedPattern.charAt(i);
         }
         else if (bestPattern.charAt(i) == cguess)
         {
            x = x + cguess;
         }
         else
         {
            x = x + savedPattern.charAt(i);
         }
      }
      for(int i = 0; i< x.length()-1; i++)
      {
         spacing += x.charAt(i) + " ";
      }
      spacing += x.charAt(x.length()-1); 
      savedPattern = x;
      return spacing;      
   }
   
   /** This method records the next guess made by the user.
    *  @param guess is the character guessed by the user.
    *  @return occurence returns the number of occurence of the guessed letter in the new pattern.
    */
   public int record (char guess)
   {
      if(wordset.isEmpty() == false && gletters.contains(guess)) throw new IllegalArgumentException();
      cguess = guess;
      gletters.add(guess);  
      wordmap = new TreeMap<String, Set<String>>(); 
      if(chance < 1 || wordset.isEmpty()) throw new IllegalStateException();
      
      for(String w : wordset)
      {
         String currentG = "";
         for(int i = 0; i < w.length() ; i++)
         {
            if(w.charAt(i)==guess)
            {
               currentG += guess;
            }
            else
            {
               currentG += "-";
            }            
         }
         
            if(wordmap.containsKey(currentG))
            {
               wordmap.get(currentG).add(w);
            }
            else
            {
               Set<String> temp = new TreeSet<String>();
               temp.add(w);
               wordmap.put(currentG, temp);
            }
      }
      
      int biggestkey = 0; //to compare
      String biggestpattern = "";
      for(String pattern : wordmap.keySet())
      {
         if(wordmap.get(pattern).size() > biggestkey)
         {
            biggestkey = wordmap.get(pattern).size();
            biggestpattern = pattern;
         }
      }
      bestPattern = biggestpattern;
      wordset.retainAll(wordmap.get(bestPattern));
      
      int occurence = numOccurence(bestPattern, guess);
      return occurence;       
   }
   
   /** This method counts the number the number of occurence of the guessed letter in the new pattern.
    *  @param word is the word considered in the game.
    *  @param guessC is the character guessed by the user.
    *  @return counter returns the amount of occurence.
    */
   public int numOccurence(String word, char guessC)
   {
      int counter = 0;
      for(int i = 0; i<word.length(); i++)
      {
         if(word.charAt(i) == guessC)
         {
            counter++;
         }
      }
      
      if(counter == 0)
      {
         chance = chance - 1;
      }
      wordmap.clear();
      return counter;
   }
}
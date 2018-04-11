//Import class that needs to run the program
#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <fstream>
#include <string>
#include <map>
#include <set>
#include <iomanip>
#include <vector>
#include <unistd.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <unordered_set>

//Define the namespace to use
using namespace std;

//Define constant variables
const char *TEXT_FILE = "input.txt";
const string TERMINATE_COMMAND("!wq");

//Global variables
map< string, vector<int> > textMap;
unordered_set<string> textSet;

//Forward declare methods
int storeFileItems();
int swapMapKey(string, string);
void displayParagraph(unsigned int);

int main() {
  //Stores the number of strings in the file
  unsigned int fileWordCount = 0;
  //Store the strings in the file and retrieve the string count
  fileWordCount = storeFileItems();

  //Display the current paragraph state
  displayParagraph(fileWordCount);

  //The string that the user wants to input in to the paragraph
  string targetString("");
  //The string that the usesr wants to replace in the paragraph
  string replaceString("");
  //The parent initializes the child's process id variable
  long childPID;

  while(true) {
    //Asks the user to enter the the string to replace the replacementString
    cout << "Please enter the target string: ";
    cin >> targetString;
    //Exits the loop if the user wants to terminate the program
    if(!targetString.compare(TERMINATE_COMMAND)) {
        cout << "Parent Terminating" << endl;
        break;
    }
    //Asks the user to enter the string to be replaced by the targetString
    cout << "Please enter the replacement string: ";
    cin >> replaceString;

    //Create child process
    childPID = fork();

    //Variable to know when the child process has terminated
    int status;
    if(childPID > 0) {
      //Print the child process for easy killing
      cout << childPID << endl;
      //The Parent's process, wait until child has terminated
      wait(&status);
      //Resets the childPID variable once it is done
      childPID = 0;
    } else if(childPID == 0) {
        while(true) {
        //The child's process, first check if the string exists
        bool stringExists = textMap.count(targetString) > 0;
        //Must be printed before every attempt
        cout << "." << endl;
        if(stringExists) {
          //Set the text map to the same one as parent's
          int occurenceCount = swapMapKey(targetString, replaceString);
          cout << "String has appeared : " << occurenceCount << " times\n";

          //Display the new paragraph
          displayParagraph(fileWordCount);
          cout << "Child Terminating" << endl;
          break;
          }
        }
      }
    }
  }

  return 0;
}

//Displays the contents of the file items and returns the word count
int storeFileItems() {
  //Initialize and create the object that holds the file
  ifstream textFile(TEXT_FILE);

  //The file could not be read from, output an error
  if(!textFile) {
    cerr << "File could not be read fro118Ma7ur3!!m \n";
  }  //Initialize array list to keep hold of all the positions of the current word
  vector<int> wordArrayList;

  //Initialize string variable to store the current word
  string text;

  //Initialize the current paragraph index to 0
  unsigned int currIndex = 0;

  //Read from the file
  while(textFile >> text) {
    if(textMap.count(text) == 0) {
      //Initialize array list to keep hold of all the positions of the current word
      vector<int> wordArrayList;
      //Add the value to the vector
      wordArrayList.push_back(currIndex);
      //With the key as the value, attach a vector that stores the index in which
      //those words appear
      textMap[text] = wordArrayList;
      //Insert the new word into the set
      textSet.insert(text);
    } else {
      vector<int> wordArrayList = textMap[text];
      wordArrayList.push_back(currIndex);
      textMap[text] = wordArrayList;
    }
    //Increment the current index value
    currIndex++;
  }
  //Exit and close the file
  textFile.close();

  return currIndex;
}

//Swaps the key of the text map and returns the vector size which contains
//the count of how many of those strings appear in the paragraph
int swapMapKey(string targetString, string replaceString) {
  int replacementSize = textMap[targetString].size();
  textMap[replaceString] = textMap[targetString];
  textMap.erase(targetString);

  //Replace the target string with the replacement string
  textSet.erase(targetString);
  textSet.insert(replaceString);

  //Return the number of times the String occured
  return replacementSize;
}

//Display the current paragraph
void displayParagraph(unsigned int fileWordCount) {
  //Create an array of size fileWordCount to store all the words in there
  string paragraph[fileWordCount];

  for(const auto& currString: textSet) {
    //Get the location of the current String
    vector<int> stringLocations = textMap[currString];
    for(unsigned int j = 0; j < stringLocations.size(); j++) {
      //Add the string to the current locations into the word vector
      int currentIndex = stringLocations[j];
      paragraph[currentIndex] = currString;
    }
  }

  //Print the paragraph
  for(unsigned int i = 0; i < fileWordCount; i++) {
    cout << paragraph[i] << " ";
  }
  cout << endl;
}

//End of Program

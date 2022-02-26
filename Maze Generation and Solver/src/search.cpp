/* (Harry) Junhao Wang
 * CS106B Programming Abstraction
 * Section Leader: Eric Bear
 * This file is the Search Engine section of assignment 2.
 * This file attempts to create a console prompt that functions
 * as a search engine. Some functionalities of helper functions, such as
 * trimming the tokens in a query, reading a database file,
 * generating an inversed index database, and finding matching results
 * according to the query, are used to create the search engine.
 * The last section includes provided tests and student tests for all the funcitons.
 */
#include "testing/SimpleTest.h"
#include "map.h"
#include "set.h"
#include <string>
#include <iostream>
#include "filelib.h"
#include <fstream>
#include "simpio.h"
#include "strlib.h"
#include "search.h"
#include <ctype.h>

using namespace std;



/*
 * This function trims all the punctation characters from both
 * the beginning and end of the string token. Non letter tokens
 * will be discarded by returning an empty string.
 * @param token: A string that is the body text of a webpage.
 * @return: A string that has all punctuations trimmed
 *          expect for the punctuations within a token.
 *          If the argument does not contain at least one letter,
 *          then an empty string will be returned.
 * Precondition: The string must not contain any spaces.
 */
string cleanToken(string token) {

    /* Preliminary check on whether
     * the token has at least one letter.
     */
    bool containLetter = false;
    for (char c : token){
        if (isalpha(c)){
            containLetter = true;
            break;
        }
    }
    if (!containLetter){
        return "";
    }


    /* Removes all punctation from the
     * beginning and end of token.
     */
    /* From the beginning of token */
    int tokenLength = token.length(); // token.length() changes if a character is removed.
                                      // therefore, a fixed variable tokenLength is declared to avoid bug.
    for (int i = 0; i <= tokenLength; i++){
        if (ispunct(token[0])){
            token = token.substr(1);
        } else {
            break;
        }

    }
    /* From the end of token */
    for (int i = token.length() - 1; i >= 0; i--) { // As its the last character might be deleted, token.length()
        if (ispunct(token[token.length() - 1])){    // should be used instead of declaring a fixed variable.
            token = token.substr(0, token.length() - 1);
        } else {
            break;
        }
    }

    token = toLowerCase(token);

    return token;

}

/*
 * This funciton reads a text file and produces a map that has
 * the URLs as the keys and the unique words contained on that page as
 * the values respectively.
 * @param dbfile: A text file that contains the URL on every odd lines
 *                (1st, 3rd, 5th etc.) and the content of the corresponding
 *                URL on every even lines (2nd, 4th, etc.). For instance,
 *                the content of the URL on the first line is on the second line,
 *                and the content of the URL on the third line is on the fourth
 *                lines.
 * @return: A map that has the keys as the URLs, and the value corresponding to each key
 *          as the unique words of that URL's content.
 * Precondition: dbfile must be in the correct format as described above.
 */
Map<string, Set<string>> readDocs(string dbfile) {

    Map<string, Set<string>> docs;

    ifstream in;

    if (!openFile(in, dbfile))
        error("Cannot open file named " + dbfile);

    Vector<string> lines;
    readEntireFile(in, lines);

    if (lines.size() % 2 != 0){
        error("file not in correct format (file must have even number of lines)");
    }

    // The first for loop increments by 2 from 0 to the second last
    // line so that line i is always on the line with the URL, while
    // line i+1 is always on the line with the content.
    // (So lines[i] is the URL, and lines[i+1] is the content of the URL)
    for (int i = 0; i < lines.size() - 1; i += 2){
        for (string word : stringSplit(lines[i+1], " ")){ // Delimit the line with the content(lines[i+1])
            string cleanedWord = cleanToken(word);        // And clean every word using cleanToken
            if(cleanedWord != ""){
                docs[lines[i]].add(cleanedWord);          // With URL as the key (lines[i]), add all cleanedWord
            }                                             // to the value associated with that key in the Map, docs.
        }
    }

    return docs;

}

/*
 * This function takes a map and inverse index it. It essentially
 * makes the words as the keys and the URL pages that contains
 * the word as the values.
 * @param docs: A map that has the URLs as the keys, and the set
 *              of unique words that appears on each corresponding
 *              URL's page as the value.
 * @return: The inverse indexed map of docs. Essentailly, the words
 *          are the keys and the set of URLs that contains the word
 *          are the values.
 */
Map<string, Set<string>> buildIndex(Map<string, Set<string>>& docs) {

    Map<string, Set<string>> index;

    for (string key : docs){
        for (string word : docs[key]){
            // Essentially, the keys of values become values of
            // index and the values of docs become keys of index.
            index[word].add(key);
        }
    }

    return index;

}

/*
 * This function gives the URLs that contains words from a
 * search result. Essentially, this function allows users to
 * query the database and find matching URLs.
 * @param index: An inverse indexed map, with every unique words as a key
 *               and the URL's page that contain the word as the value.
 * @param query: A string that is essentaily the search input. Words are
 *               separated by a space along with either '+', '-', or nothing
 *               before each word. (Expect for the first word). '+' gives
 *               URL's that contains both words, '-' gives URL's that
 *               does not contain the word, and nothing just gives
 *               more results containing that word.
 * @return: A set of string that contains the URLs according to the query.
 */
Set<string> findQueryMatches(Map<string, Set<string>>& index, string query) {

    /* Creates a vector that has all the tokens in
     * query cleaned/trimmed using cleanToken.
     */
    Vector<string> cleanQuery = stringSplit(query, " ");
    for (string& term : cleanQuery){
        if(term[0] == '+' || term[0] == '-'){
            // Avoid trimming the '+' and '-' in query
            term = term[0] + cleanToken(term.substr(1));
        } else {
            term = cleanToken(term);
        }
    }

    Set<string> result;
    // For all the cleaned tokens (cleanQuery[i])
    // Find the matches in the database accordingly.
    for (int i = 0; i < cleanQuery.size(); i++){
        char firstChar = (cleanQuery.get(i))[0];
        if (firstChar == '+'){
            // substr(1) is used to drop the sign.
            result = result * index[cleanQuery[i].substr(1)];
        } else if (firstChar == '-'){
            // substr(1) is used to drop the sign.
            result = result - index[cleanQuery[i].substr(1)];
        } else {
            result = result + index[cleanQuery[i]];
        }
    }

    return result;
}

/*
 * This is a console function that takes a input of a database file of
 * URLs and acts as a simplistic seach engine. The function displays
 * the total amount of pages and unique terms, along with the number of
 * matching results and the results.
 * @param dbfile: A text file that contains the URL on every odd lines
 *                (1st, 3rd, 5th etc.) and the content of the corresponding
 *                URL on every even lines (2nd, 4th, etc.). For instance,
 *                the content of the URL on the first line is on the second line,
 *                and the content of the URL on the third line is on the fourth
 *                lines.
 */
void searchEngine(string dbfile) {
    cout << "Stand by while building index..." << endl;

    Map<string, Set<string>> docs = readDocs(dbfile);
    Map<string, Set<string>> index = buildIndex(docs);

    cout << "Indexed " << docs.size() << " pages containing "
         << index.size() << " unique terms." << endl << endl;

    while(true){
        string query = getLine("Enter query sentence (RETURN/ENTER to quit): ");
        if (query == ""){
            break;
        }

        Set<string> result = findQueryMatches(index, query);
        cout << "Found " << result.size() << " matching pages" << endl;
        cout << result << endl << endl;
    }

    cout << endl << "All done!" << endl;
}


/* * * * * * Test Cases * * * * * */

PROVIDED_TEST("cleanToken on tokens with no punctuation") {
    EXPECT_EQUAL(cleanToken("hello"), "hello");
    EXPECT_EQUAL(cleanToken("WORLD"), "world");
}

PROVIDED_TEST("cleanToken on tokens with some punctuation at beginning and end") {
    EXPECT_EQUAL(cleanToken("/hello/"), "hello");
    EXPECT_EQUAL(cleanToken("~woRLD!"), "world");
}

PROVIDED_TEST("cleanToken on non-word tokens"){
    EXPECT_EQUAL(cleanToken("106"), "");
    EXPECT_EQUAL(cleanToken("~!106!!!"), "");
}

PROVIDED_TEST("readDocs from tiny.txt, contains 4 documents") {
    Map<string, Set<string>> docs = readDocs("res/tiny.txt");
    EXPECT_EQUAL(docs.size(), 4);
}

PROVIDED_TEST("readDocs from tiny.txt, suess has 5 unique words and includes lowercase fish") {
    Map<string, Set<string>> docs = readDocs("res/tiny.txt");
    Set<string> seuss = docs["www.dr.seuss.net"];
    EXPECT_EQUAL(seuss.size(), 5);
    EXPECT(seuss.contains("fish"));
    EXPECT(!seuss.contains("Fish"));
}

PROVIDED_TEST("buildIndex from tiny.txt, 20 unique tokens overall") {
    Map<string, Set<string>> docs = readDocs("res/tiny.txt");
    Map<string, Set<string>> index = buildIndex(docs);
    EXPECT_EQUAL(index.size(), 20);
    EXPECT(index.containsKey("fish"));
    EXPECT(!index.containsKey("@"));
}

PROVIDED_TEST("findQueryMatches from tiny.txt, single word query") {
    Map<string, Set<string>> docs = readDocs("res/tiny.txt");
    Map<string, Set<string>> index = buildIndex(docs);
    Set<string> matchesRed = findQueryMatches(index, "red");
    EXPECT_EQUAL(matchesRed.size(), 2);
    EXPECT(matchesRed.contains("www.dr.seuss.net"));
    Set<string> matchesHippo = findQueryMatches(index, "hippo");
    EXPECT(matchesHippo.isEmpty());
}

PROVIDED_TEST("findQueryMatches from tiny.txt, compound queries") {
    Map<string, Set<string>> docs = readDocs("res/tiny.txt");
    Map<string, Set<string>> index = buildIndex(docs);
    Set<string> matchesRedOrFish = findQueryMatches(index, "red fish");
    EXPECT_EQUAL(matchesRedOrFish.size(), 3);
    Set<string> matchesRedAndFish = findQueryMatches(index, "red +fish");
    EXPECT_EQUAL(matchesRedAndFish.size(), 1);
    Set<string> matchesRedWithoutFish = findQueryMatches(index, "red -fish");
    EXPECT_EQUAL(matchesRedWithoutFish.size(), 1);
}


/* * * * * STUDENT TEST * * * * */

STUDENT_TEST("cleanToken with different words/nonwords"){
    EXPECT_EQUAL(cleanToken(".Bye."), "bye");
    EXPECT_EQUAL(cleanToken("HELLO"), "hello");
    EXPECT_EQUAL(cleanToken(".I'M,"), "i'm");
    EXPECT_EQUAL(cleanToken("...I..."), "i");
    EXPECT_EQUAL(cleanToken("123853.!"), "");
    EXPECT_EQUAL(cleanToken("(x"), "x");
    EXPECT_EQUAL(cleanToken("(!(x"), "x");
    EXPECT_EQUAL(cleanToken("x......."), "x");
    EXPECT_EQUAL(cleanToken(".........x"), "x");
    EXPECT_EQUAL(cleanToken(""), "");
}

STUDENT_TEST("Tests for readDocs"){
    Map<string, Set<string>> map = readDocs("res/tiny.txt");
    Set<string> set1 = map["www.shoppinglist.com"];
    Set<string> ans = {"eggs", "milk", "fish", "bread", "cheese"};

    EXPECT_EQUAL(set1, ans);

    set1 = map["www.rainbow.org"];
    ans = {"red", "green", "orange", "yellow", "blue", "indigo", "violet"};

    EXPECT_EQUAL(set1, ans);

    set1 = map["www.dr.seuss.net"];
    ans = {"one", "fish", "two", "red", "blue"};

    EXPECT_EQUAL(set1, ans);

    set1 = map["www.bigbadwolf.com"];
    ans = {"i'm", "not", "trying", "to", "eat", "you"};

    EXPECT_EQUAL(set1, ans);

}

STUDENT_TEST("Tests for buildIndex"){
    Map<string, Set<string>> map = readDocs("res/tiny.txt");
    Map<string, Set<string>> map1 = buildIndex(map);

    Set<string> set1 = map1["blue"];
    Set<string> ans = {"www.dr.seuss.net", "www.rainbow.org"};
    EXPECT_EQUAL(set1, ans);

    set1 = map1["bread"];
    ans = {"www.shoppinglist.com"};
    EXPECT_EQUAL(set1, ans);
}

STUDENT_TEST("Query tests for findQueryMatches with non-clean tokens"){
    Map<string, Set<string>> docs = readDocs("res/tiny.txt");
    Map<string, Set<string>> index = buildIndex(docs);
    Set<string> matchesRedOrBlue = findQueryMatches(index, "!RED, ,blue");
    EXPECT_EQUAL(matchesRedOrBlue.size(), 2);
    Set<string> matchesRedAndBlue = findQueryMatches(index, "!red. +,blue 543241");
    EXPECT_EQUAL(matchesRedAndBlue.size(), 2);
    Set<string> matchesRedWithoutBlue = findQueryMatches(index, "Red. -blue.");
    EXPECT_EQUAL(matchesRedWithoutBlue.size(), 0);
}

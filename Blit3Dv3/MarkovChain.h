#pragma once

/*
	Class to build Markov Chains for word generation. It can create a Markov probability
	mapping for a file and then generate new words based on the mappings.
*/
#include<fstream>
#include<unordered_map>
#include<string>
#include<random>

class MarkovChain
{
private:
	// Frequency distribution list of starting letters for words
	std::unordered_map<char, int> firstLetterMap;

	// Mapping of letter pairs: probability that a given first letter is followed by any certian second letter
	std::unordered_map<char, std::unordered_map<char, int>> letterMap;

	std::string randomLetters = "abcdefghijklmnopqrstuvwxyz";
	std::mt19937 rng;

public:
	/**
	* Initializes the rng
	*/
	MarkovChain() {
		std::random_device rd;
		rng.seed(rd());
	}

	/**
	* Build Markov probablilities from a text file of words
	* @param String - Path of the file
	* @result True if the build was succesful
	*/
	bool ParseFile(std::string filename){
		// Read file in
		std::ifstream inputFile(filename);
		std::string inputString;
		if (inputFile.is_open()){
			while (!inputFile.eof()){
				inputFile >> inputString;
				ParseString(inputString);
			}
			inputFile.close();
			return true;
		}
		// It couldn't open file, so return error
		return false;
	}
	
	/**
	* Add a word to the mapping
	* @param String - a word of the list
	*/
	void ParseString(std::string aString) {
		// Increment first letter frequency list
		auto search = this->firstLetterMap.find(aString[0]);
		if (search != this->firstLetterMap.end()) search->second++;
		else this->firstLetterMap[aString[0]] = 1;

		// Now add letter pairs
		for (unsigned int i = 0; i < aString.size() - 1; ++i)
			AddLetterPair(aString[i], aString[i + 1]);
	}

	/**
	* Add a letter pair to the mapping
	* @param char FirstLetter, char SecondLetter
	*/
	void AddLetterPair(char firstLetter, char secondLetter) {
		// Look up firstLetter in the map
		auto search = this->letterMap.find(firstLetter);
		if (search != this->letterMap.end())
		{
			// FirstLetter is in the map, now see if it has secondLetter on it's list of following letters
			auto innerSearch = search->second.find(secondLetter);
			if (innerSearch != search->second.end())
			{
				// SecondLetter already exists, so increment it's count
				innerSearch->second++;
			}
			else
			{
				// We've never seen secondLetter follow firstLetter before, so add it with a count of 1
				search->second[secondLetter] = 1;
			}
		}
		else
		{
			// FirstLetter isn't in the list of first letters, so add it and secondLetter to the maps
			std::unordered_map<char, int> m;
			m[secondLetter] = 1;
			this->letterMap[firstLetter] = m;
		}
	}
	/**
	* Generate the next letter in the chain (to follow firstLetter)
	* @param char FirstLetter
	* @result char NextLetter
	*/
	char GenerateNextLetter(char firstLetter) {
		char nextLetter = 'a';
		// Look up first in the map
		auto search = this->letterMap.find(firstLetter);
		if (search != this->letterMap.end())
		{
			// Loop over all of the following letters and add up their counts
			int total = 0;
			for (auto letter : search->second)
			{
				total += letter.second;
			}

			// Roll a random number based off the total
			std::uniform_int_distribution<int> dist(1, total);
			int randRoll = dist(rng);

			// Find the letter that matches this roll
			int totalSoFar = 0;
			for (auto letter : search->second)
			{
				totalSoFar += letter.second;
				if (totalSoFar >= randRoll)
				{
					nextLetter = letter.first;
					break;
				}
			}

		}
		else
		{
			// We didn't find this letter as a letter that has anything following it, ever.
			// er....what now, roll a random letter? Probably best, or just end here?
			std::uniform_int_distribution<int> dist26(0, 25);

			nextLetter = this->randomLetters[dist26(rng)];
		}

		return nextLetter;
	}

	/**
	* Generate a word of a certian length 
	* @param Int Lenght - Lenght of the word to generate
	* @resutl String - Word generated
	*/
	std::string GenerateChain(int length){
		std::string chain;
		// First, pick a first letter.
		char firstLetter = 'A';
		// Look it up in a frequency list of first letters...
		// Loop over all of the first letters and add up their counts
		int total = 0;
		for (auto letter : firstLetterMap)
		{
			total += letter.second;
		}

		// Roll a random number based off the total
		std::uniform_int_distribution<int> dist(1, total);
		int randRoll = dist(rng);

		// Find the letter that matches this roll
		int totalSoFar = 0;
		for (auto letter : firstLetterMap)
		{
			totalSoFar += letter.second;
			if (totalSoFar >= randRoll)
			{
				firstLetter = letter.first;
				break;
			}
		}
		// Add the chosen first letter to our string
		chain += firstLetter;

		// Generate the rest of the chain based on the Markov Chain
		for (int i = 0; i < length - 1; ++i)
			chain += GenerateNextLetter(chain[i]);
		return chain;
	}

	std::string toCapitalCase(std::string str) {
		std::transform(str.begin(), str.end(), str.begin(), ::tolower);
		str[0] = toupper(str[0]);
		return str;
	}
};

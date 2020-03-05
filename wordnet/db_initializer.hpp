#ifndef DB_INITIALIZER_HPP
#define DB_INITIALIZER_HPP

#include <vector>
#include <string>
#include <sstream>
#include <map>
#include <set>
#include <iostream>
using std::cout;
using std::endl;

#include <SQLiteCpp/SQLiteCpp.h>

void add_hyponym(const std::string& start_id, std::map<std::string, std::set<std::string>>& hypernyms, std::map<std::string, std::set<std::string>>& hyponyms, std::string id = "")
{
	for (auto i: hypernyms[(id=="") ? start_id : id])
	{
		hyponyms[i].insert(start_id);
		add_hyponym(start_id, hypernyms, hyponyms, i);
	}
}

void db_initializer(SQLite::Database& db)
{			
	std::string entity_id;
	
	SQLite::Transaction transaction(db);
	db.exec("CREATE TABLE entity_id (ID CHARACTER(10) PRIMARY KEY)");
	db.exec("CREATE TABLE semfields (word_id CHARACTER(10), semfield TEXT)");
	db.exec("CREATE TABLE hyponyms_count (word_id CHARACTER(10), count INT)");
	
	std::vector<std::string> words;
	
	SQLite::Statement query1(db, "SELECT lemma, synset FROM nouns");
	while (query1.executeStep())
	{
		const char* synset_id = query1.getColumn(1);
		
		if (std::string(query1.getColumn(0)) == "entity")
		{
			SQLite::Statement q (db, "INSERT INTO entity_id VALUES (?)");
			q.bind(1, synset_id, 10);
			entity_id = std::string(query1.getColumn(1));
			q.exec();
		}
				
		words.push_back(synset_id);
	}
	
	//Hyponyms map
	std::map<std::string, std::set<std::string>> hyponyms;
	//Reading hypernyms
	std::map<std::string, std::set<std::string>> hypernyms;
	SQLite::Statement query2(db, "SELECT * FROM hypernyms");
	
	while (query2.executeStep())
	{
		const char* syn1 = query2.getColumn(0);
		const char* syn2 = query2.getColumn(1);
				
		hypernyms[syn1].insert(syn2);
	}
	
	for (auto& hyper: hypernyms)
	{
		for (auto h: hyper.second)
		{
			add_hyponym(hyper.first, hypernyms, hyponyms);
		}
	}
					
	//Adding entity as hypernym of every node without hypernyms
	SQLite::Statement query6 (db, "INSERT INTO hypernyms VALUES(?, ?)");
	for (auto& w: words)
	{
		if (hypernyms[w].size() == 0)
		{
			hypernyms[w].insert(entity_id);
			hyponyms[entity_id].insert(w);
			
			query6.reset();
			query6.bind(1, w);
			query6.bind(2, entity_id);
			query6.exec();
		}
	}
	
	//Writing hyponyms count
	SQLite::Statement query3 (db, "INSERT INTO hyponyms_count VALUES(?, ?)");
	for (auto& hyp: hyponyms)
	{
		query3.reset();
		query3.bind(1, hyp.first);
		query3.bind(2, (int) hyp.second.size());
		query3.exec();
	}
	
	//Writing semfields
	SQLite::Statement query4(db, "SELECT * FROM semfield");
	SQLite::Statement query5(db, "INSERT INTO semfields VALUES(?, ?)");
	while (query4.executeStep())
	{
		const char*  		field1 		 = query4.getColumn(0);
		std::string  		field2 		   (query4.getColumn(1));
		std::stringstream 	field_stream   (field2);
		std::string 		field;
		
		while (field_stream >> field)
		{
			query5.reset();
			query5.bind(1, field1);
			query5.bind(2, field);
			query5.exec();
		}
	}
	
	transaction.commit();
}

#endif

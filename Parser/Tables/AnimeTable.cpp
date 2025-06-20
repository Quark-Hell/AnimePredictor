#include "AnimeTable.h"
#include <iostream>
#include <filesystem>

std::vector<AnimeTable> AnimeTable::ParseCSV(const std::string& filename) {
    std::ifstream file(filename);
    std::vector<AnimeTable> result;

    if (!file.is_open()) {
        std::cerr << "Cannot open file: " << filename << std::endl;
        return result;
    }

    std::string line;

    //skip first line
    std::getline(file, line);

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string field;

        std::getline(ss, field, ',');
        int id = std::stoi(field);

        std::string name;
        if (ss.peek() == '"') {
            ss.get();
            std::getline(ss, name, '"');
            ss.get();
        } else {
            std::getline(ss, name, ',');
        }

        std::string genres;
        char next = ss.peek();
        if (next == '"') {
            ss.get();
            std::getline(ss, genres, '"');
            ss.get();
        } else {
            std::getline(ss, genres, ',');
        }

        std::getline(ss, field, ',');
        std::string type = field;

        std::getline(ss, field, ',');
        int episodes = -1;
        try {
            episodes = std::stoi(field);
        } catch (const std::exception& e) {
            episodes = -1;
        }

        std::getline(ss, field, ',');
        float rating = -1;
        try {
            rating = std::stof(field);
        } catch (const std::exception& e) {
            rating = -1;
        }

        std::getline(ss, field);
        int members = std::stoi(field);

        result.emplace_back(id, name, genres, type, episodes, rating, members);
    }

    return result;
}
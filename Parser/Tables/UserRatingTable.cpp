#include "UserRatingTable.h"
#include <iostream>
#include <filesystem>

std::vector<UserRatingTable> UserRatingTable::ParseCSV(const std::string& filename) {
    std::ifstream file(filename);
    std::vector<UserRatingTable> result;

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
        int user_id = std::stoi(field);

        std::getline(ss, field, ',');
        int anime_id = std::stoi(field);

        std::getline(ss, field);
        int rating = std::stoi(field);

        result.emplace_back(user_id, anime_id, rating);
    }

    return result;
}
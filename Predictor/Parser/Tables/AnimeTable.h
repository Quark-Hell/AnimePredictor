#ifndef ANIMEPREDICTOR_ANIMETABLE_H
#define ANIMEPREDICTOR_ANIMETABLE_H

#include <fstream>
#include <sstream>
#include <vector>
#include <string>

class AnimeTable {
public:
    float anime_id;
    std::string name;
    std::vector<std::string> genres;
    std::string type;
    float episodes;
    float rating;
    float members;

    AnimeTable(float id, const std::string& name, const std::string& genreStr,
               const std::string& type, float episodes, float rating, float members)
            : anime_id(id), name(name), type(type), episodes(episodes), rating(rating), members(members)
    {
        std::stringstream ss(genreStr);
        std::string genre;
        while (std::getline(ss, genre, ',')) {
            genres.push_back(genre);
        }
    }

    static std::vector<AnimeTable> ParseCSV(const std::string& filename);
};


#endif //ANIMEPREDICTOR_ANIMETABLE_H

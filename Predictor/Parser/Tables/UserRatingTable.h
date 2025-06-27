#ifndef ANIMEPREDICTOR_USERRATINGTABLE_H
#define ANIMEPREDICTOR_USERRATINGTABLE_H

#include <fstream>
#include <sstream>
#include <vector>
#include <string>

class UserRatingTable {
public:
    float user_id;
    float anime_id;
    float rating;

    UserRatingTable(float userId, float animeId, float rating)
            : user_id(userId), anime_id(animeId), rating(rating) {}

    static std::vector<UserRatingTable> ParseCSV(const std::string& filename);
};


#endif //ANIMEPREDICTOR_USERRATINGTABLE_H

#ifndef ANIMEPREDICTOR_PARSER_H
#define ANIMEPREDICTOR_PARSER_H

#include "Tables/AnimeTable.h"
#include "Tables/UserRatingTable.h"

class Parser {
private:
    std::vector<AnimeTable> animeTable;
    std::vector<UserRatingTable> userRatingTable;

public:
    static Parser& GetInstance();

    Parser(const Parser&) = default;
    Parser& operator=(const Parser&) = delete;
    Parser(Parser&&) = delete;
    Parser& operator=(Parser&&) = delete;

    void ImportTables(const std::string& animePath, const std::string& ratingPath);
    void ExportTables(const std::string& animePath, const std::string& ratingPath);

    ~Parser() = default;

private:
    Parser() = default;
};

#endif //ANIMEPREDICTOR_PARSER_H

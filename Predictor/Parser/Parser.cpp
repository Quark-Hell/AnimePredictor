#include "Parser.h"

#include "Exporter.h"

#include <iostream>
#include <filesystem>

Parser& Parser::GetInstance() {
    static Parser instance;
    return instance;
}

void Parser::ImportTables(const std::string& animePath, const std::string& ratingPath) {
    const std::filesystem::path animeTableSource = std::filesystem::current_path() / animePath;
    const std::filesystem::path userRatingTableSource = std::filesystem::current_path() / ratingPath;

    animeTable = AnimeTable::ParseCSV(animeTableSource);
    userRatingTable = UserRatingTable::ParseCSV(userRatingTableSource);

    std::cout << "Imported " << userRatingTable.size() << " ratings and " << animeTable.size() << " anime." << std::endl;
}

void Parser::ExportTables(const std::string& animePath, const std::string& ratingPath) {
    const std::filesystem::path animeTableDest = std::filesystem::current_path() / animePath;
    const std::filesystem::path userRatingTableDest = std::filesystem::current_path() / ratingPath;

    Exporter::ExportFilteredDataToCSV(
            animeTableDest,
            userRatingTableDest,
            animeTable,
            userRatingTable,
            10,
            100);
}

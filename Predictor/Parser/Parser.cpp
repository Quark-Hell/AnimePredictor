#include "Parser.h"

#include "Exporter.h"
#include "Logger/LogManager.h"

#include <filesystem>

Parser& Parser::GetInstance() {
    static Parser instance;
    return instance;
}

void Parser::ImportTables(const std::string& animePath, const std::string& ratingPath) {
    static std::string bufferName = "Training";

    const std::filesystem::path animeTableSource = std::filesystem::current_path() / animePath;
    const std::filesystem::path userRatingTableSource = std::filesystem::current_path() / ratingPath;

    animeTable = AnimeTable::ParseCSV(animeTableSource);
    userRatingTable = UserRatingTable::ParseCSV(userRatingTableSource);

    LogManager::LogCustom(
            false,
            bufferName,
            "Imported " + std::to_string(userRatingTable.size()) + " ratings and " + std::to_string(animeTable.size()) + " anime.");
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

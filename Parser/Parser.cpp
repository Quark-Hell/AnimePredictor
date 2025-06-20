#include <iostream>
#include <filesystem>

//#include "tensorflow/c/c_api.h"
#include "Tables/AnimeTable.h"
#include "Tables/UserRatingTable.h"

#include "Exporter.h"

std::vector<AnimeTable> animeTable;
std::vector<UserRatingTable> userRatingTable;

void ImportTables() {
    const std::filesystem::path animeTableSource = std::filesystem::current_path() / "data" / "anime.csv";
    const std::filesystem::path userRatingTableSource = std::filesystem::current_path() / "data" / "rating.csv";

    animeTable = AnimeTable::ParseCSV(animeTableSource);
    userRatingTable = UserRatingTable::ParseCSV(userRatingTableSource);

    std::cout << "Imported " << userRatingTable.size() << " ratings and " << animeTable.size() << " anime." << std::endl;
}

void ExportTables() {
    const std::filesystem::path animeTableDest = std::filesystem::current_path() / "out" / "anime.csv";
    const std::filesystem::path userRatingTableDest = std::filesystem::current_path() / "out" / "rating.csv";

    Exporter::ExportFilteredDataToCSV(
            animeTableDest,
            userRatingTableDest,
            animeTable,
            userRatingTable,
            10);
}


int main() {
    ImportTables();
    ExportTables();

    return 0;
}

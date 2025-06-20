#ifndef ANIMEPREDICTOR_EXPORTER_H
#define ANIMEPREDICTOR_EXPORTER_H

#include "Tables/AnimeTable.h"
#include "Tables/UserRatingTable.h"
#include <unordered_map>

class Exporter {
public:
    Exporter() = delete;
    ~Exporter() = delete;
    Exporter(const Exporter&) = delete;
    Exporter(Exporter&&) = delete;
    Exporter& operator=(const Exporter&) = delete;
    Exporter& operator=(Exporter&&) = delete;

    static void ExportFilteredDataToCSV(const std::string& animePath,
                                        const std::string& ratingPath,
                                        const std::vector<AnimeTable>& animeData,
                                        const std::vector<UserRatingTable>& ratingData,
                                        int tolerance);

private:
    // Counting the number of ratings for each anime
    static std::unordered_map<int, int> GetRatingCount(const std::vector<UserRatingTable>& ratingData);

    static void ExportFilteredAnime(
            const std::string& animePath,
            const std::vector<AnimeTable>& animeData,
            const std::unordered_map<int, bool>& validAnimeIds);

    static void ExportFilteredRating(
            const std::string& ratingPath,
            const std::vector<UserRatingTable>& ratingData,
            const std::unordered_map<int, bool>& validAnimeIds);
};


#endif //ANIMEPREDICTOR_EXPORTER_H

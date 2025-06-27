#ifndef ANIMEPREDICTOR_EXPORTER_H
#define ANIMEPREDICTOR_EXPORTER_H

#include "Tables/AnimeTable.h"
#include "Tables/UserRatingTable.h"

#include <unordered_map>
#include <unordered_set>

class FilterStats {
public:
    size_t initial_anime_count = 0;
    size_t initial_rating_count = 0;
    size_t removed_invalid_ratings = 0;
    size_t removed_nan_values = 0;
    size_t removed_low_rating_count = 0;
    size_t removed_inactive_users = 0;
    size_t final_anime_count = 0;
    size_t final_rating_count = 0;

    void print() const;
};

class Exporter {
public:
    Exporter() = delete;
    ~Exporter() = delete;
    Exporter(const Exporter&) = delete;
    Exporter(Exporter&&) = delete;
    Exporter& operator=(const Exporter&) = delete;
    Exporter& operator=(Exporter&&) = delete;

public:
    /**
     * Основная функция экспорта с полной фильтрацией данных
     * @param animePath - путь для сохранения отфильтрованных данных аниме
     * @param ratingPath - путь для сохранения отфильтрованных рейтингов
     * @param animeData - исходные данные аниме
     * @param ratingData - исходные данные рейтингов
     * @param minAnimeRatings - минимальное количество рейтингов для аниме (по умолчанию 10)
     * @param minUserRatings - минимальное количество рейтингов от пользователя (по умолчанию 5)
     */
    static void ExportFilteredDataToCSV(const std::string& animePath,
                                        const std::string& ratingPath,
                                        const std::vector<AnimeTable>& animeData,
                                        const std::vector<UserRatingTable>& ratingData,
                                        int minAnimeRatings = 10,
                                        int minUserRatings = 5);

private:
    static std::unordered_map<int, int> GetAnimeRatingCount(const std::vector<UserRatingTable>& ratingData);
    static std::unordered_map<int, int> GetUserRatingCount(const std::vector<UserRatingTable>& ratingData);

    static bool IsValidAnime(const AnimeTable& anime);
    static bool IsValidRating(const UserRatingTable& rating);

    static std::vector<AnimeTable> FilterAnimeData(const std::vector<AnimeTable>& animeData, FilterStats& stats);
    static std::vector<UserRatingTable> FilterRatingData(const std::vector<UserRatingTable>& ratingData, FilterStats& stats);

    static std::pair<std::unordered_set<int>, std::unordered_set<int>>
    GetValidIds(const std::vector<AnimeTable>& animeData,
                const std::vector<UserRatingTable>& ratingData,
                int minAnimeRatings,
                int minUserRatings,
                FilterStats& stats);


    static void ExportFilteredAnime(const std::string& animePath,
                                    const std::vector<AnimeTable>& animeData,
                                    const std::unordered_set<int>& validAnimeIds,
                                    FilterStats& stats);

    static void ExportFilteredRating(const std::string& ratingPath,
                                     const std::vector<UserRatingTable>& ratingData,
                                     const std::unordered_set<int>& validAnimeIds,
                                     const std::unordered_set<int>& validUserIds,
                                     FilterStats& stats);
};


#endif //ANIMEPREDICTOR_EXPORTER_H

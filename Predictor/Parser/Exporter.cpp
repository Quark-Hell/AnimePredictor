#include "Exporter.h"

#include "Logger/LogManager.h"

#include <filesystem>
#include <cmath>

void FilterStats::print() const {
    static std::string bufferName = "Training";

    LogManager::LogCustom(false, bufferName, "=== DETAILED FILTER STATISTICS ===");
    LogManager::LogCustom(false, bufferName, "Initial anime entries: " + std::to_string(initial_anime_count));
    LogManager::LogCustom(false, bufferName, "Initial rating entries: " + std::to_string(initial_rating_count));
    LogManager::LogCustom(false, bufferName, "Removed invalid ratings (-1): " + std::to_string(removed_invalid_ratings));
    LogManager::LogCustom(false, bufferName, "Removed NaN/invalid values: " + std::to_string(removed_nan_values));
    LogManager::LogCustom(false, bufferName, "Removed anime with low rating count: " + std::to_string(removed_low_rating_count));
    LogManager::LogCustom(false, bufferName, "Removed ratings from inactive users: " + std::to_string(removed_inactive_users));
    LogManager::LogCustom(false, bufferName, "Final anime entries: " + std::to_string(final_anime_count));
    LogManager::LogCustom(false, bufferName, "Final rating entries: " + std::to_string(final_rating_count));

    double percent_removed = (1.0 - static_cast<double>(final_rating_count) / initial_rating_count) * 100.0;
    std::string str = std::to_string(percent_removed);

    LogManager::LogCustom(false, bufferName, "Data reduction: " + str + "%");
    LogManager::LogCustom(false, bufferName, "===================================");
}

std::unordered_map<int, int> Exporter::GetAnimeRatingCount(const std::vector<UserRatingTable>& ratingData) {
    std::unordered_map<int, int> ratingCounts;
    for (const auto& r : ratingData) {
        if (r.rating != -1 && !std::isnan(r.rating) && r.rating >= 1 && r.rating <= 10) {
            ++ratingCounts[static_cast<int>(r.anime_id)];
        }
    }
    return ratingCounts;
}

std::unordered_map<int, int> Exporter::GetUserRatingCount(const std::vector<UserRatingTable>& ratingData) {
    std::unordered_map<int, int> userCounts;
    for (const auto& r : ratingData) {
        if (r.rating != -1 && !std::isnan(r.rating) && r.rating >= 1 && r.rating <= 10) {
            ++userCounts[static_cast<int>(r.user_id)];
        }
    }
    return userCounts;
}

bool Exporter::IsValidAnime(const AnimeTable& anime) {
    // Проверяем на NaN и валидные значения
    if (std::isnan(anime.anime_id) || std::isnan(anime.rating) ||
        std::isnan(anime.episodes) || std::isnan(anime.members)) {
        return false;
    }

    // Проверяем разумные диапазоны значений
    if (anime.anime_id <= 0 || anime.rating < 0 || anime.rating > 10 ||
        anime.episodes < 0 || anime.members < 0) {
        return false;
    }

    // Проверяем, что название не пустое
    if (anime.name.empty()) {
        return false;
    }

    // Проверяем, что тип указан
    if (anime.type.empty()) {
        return false;
    }

    return true;
}

bool Exporter::IsValidRating(const UserRatingTable& rating) {
    // Проверяем на NaN и валидные значения
    if (std::isnan(rating.user_id) || std::isnan(rating.anime_id) || std::isnan(rating.rating)) {
        return false;
    }

    // Проверяем диапазоны
    if (rating.user_id <= 0 || rating.anime_id <= 0 ||
        rating.rating < 1 || rating.rating > 10) {
        return false;
    }

    return true;
}

std::vector<AnimeTable> Exporter::FilterAnimeData(const std::vector<AnimeTable>& animeData, FilterStats& stats) {
    std::vector<AnimeTable> filteredAnime;
    stats.initial_anime_count = animeData.size();

    for (const auto& anime : animeData) {
        if (IsValidAnime(anime)) {
            filteredAnime.push_back(anime);
        } else {
            stats.removed_nan_values++;
        }
    }

    return filteredAnime;
}

std::vector<UserRatingTable> Exporter::FilterRatingData(const std::vector<UserRatingTable>& ratingData, FilterStats& stats) {
    std::vector<UserRatingTable> filteredRatings;
    stats.initial_rating_count = ratingData.size();

    for (const auto& rating : ratingData) {
        if (rating.rating == -1) {
            stats.removed_invalid_ratings++;
            continue;
        }

        if (IsValidRating(rating)) {
            filteredRatings.push_back(rating);
        } else {
            stats.removed_nan_values++;
        }
    }

    return filteredRatings;
}

std::pair<std::unordered_set<int>, std::unordered_set<int>>
Exporter::GetValidIds(const std::vector<AnimeTable>& animeData,
                      const std::vector<UserRatingTable>& ratingData,
                      int minAnimeRatings,
                      int minUserRatings,
                      FilterStats& stats) {

    // Получаем статистику рейтингов
    auto animeRatingCounts = GetAnimeRatingCount(ratingData);
    auto userRatingCounts = GetUserRatingCount(ratingData);

    // Определяем валидные anime_id
    std::unordered_set<int> validAnimeIds;
    for (const auto& anime : animeData) {
        int animeId = static_cast<int>(anime.anime_id);
        if (animeRatingCounts[animeId] >= minAnimeRatings) {
            validAnimeIds.insert(animeId);
        } else {
            stats.removed_low_rating_count++;
        }
    }

    // Определяем валидные user_id
    std::unordered_set<int> validUserIds;
    for (const auto& pair : userRatingCounts) {
        if (pair.second >= minUserRatings) {
            validUserIds.insert(pair.first);
        }
    }

    // Подсчитываем удаленные рейтинги от неактивных пользователей
    for (const auto& rating : ratingData) {
        int userId = static_cast<int>(rating.user_id);
        if (validUserIds.find(userId) == validUserIds.end()) {
            stats.removed_inactive_users++;
        }
    }

    return {validAnimeIds, validUserIds};
}

void Exporter::ExportFilteredDataToCSV(const std::string& animePath,
                                       const std::string& ratingPath,
                                       const std::vector<AnimeTable>& animeData,
                                       const std::vector<UserRatingTable>& ratingData,
                                       int minAnimeRatings,
                                       int minUserRatings) {

    static std::string bufferName = "Training";
    FilterStats stats;

    LogManager::LogCustom(false, bufferName, "Starting comprehensive data filtering...");

    // === Создание директорий, если не существуют ===
    for (const auto& path : {animePath, ratingPath}) {
        std::filesystem::path dir = std::filesystem::path(path).parent_path();
        if (!dir.empty() && !std::filesystem::exists(dir)) {
            std::filesystem::create_directories(dir);
        }
    }

    // Шаг 1: Фильтрация базовых данных (удаление NaN, невалидных значений)
    LogManager::LogCustom(false, bufferName, "Step 1: Filtering invalid data...");
    auto filteredAnime = FilterAnimeData(animeData, stats);
    auto filteredRatings = FilterRatingData(ratingData, stats);

    // Шаг 2: Получение валидных ID на основе минимальных требований
    LogManager::LogCustom(false, bufferName, "Step 2: Applying minimum requirements filter...");
    auto [validAnimeIds, validUserIds] = GetValidIds(filteredAnime, filteredRatings,
                                                     minAnimeRatings, minUserRatings, stats);

    LogManager::LogCustom(false, bufferName, "Valid anime IDs: " + std::to_string(validAnimeIds.size()));
    LogManager::LogCustom(false, bufferName, "Valid user IDs: " + std::to_string(validUserIds.size()));

    // Шаг 3: Экспорт отфильтрованных данных
    LogManager::LogCustom(false, bufferName, "Step 3: Exporting filtered data...");
    ExportFilteredAnime(animePath, filteredAnime, validAnimeIds, stats);
    ExportFilteredRating(ratingPath, filteredRatings, validAnimeIds, validUserIds, stats);

    // Вывод детальной статистики
    stats.print();
}

void Exporter::ExportFilteredAnime(const std::string& animePath,
                                   const std::vector<AnimeTable>& animeData,
                                   const std::unordered_set<int>& validAnimeIds,
                                   FilterStats& stats) {

    static std::string bufferName = "Training";

    std::ofstream animeFile(animePath);
    if (!animeFile.is_open()) {
        LogManager::LogCritical("Cannot open file for writing: " + animePath + " " + __LOGERROR__);
    }

    animeFile << "anime_id,name,genres,type,episodes,rating,members\n";

    for (const auto& anime : animeData) {
        int animeId = static_cast<int>(anime.anime_id);

        if (validAnimeIds.find(animeId) != validAnimeIds.end()) {
            // Создаем строку жанров
            std::ostringstream genresStream;
            for (size_t i = 0; i < anime.genres.size(); ++i) {
                genresStream << anime.genres[i];
                if (i + 1 < anime.genres.size()) genresStream << '|';
            }

            // Экранируем название для CSV
            std::string safeName = anime.name;
            if (safeName.find(',') != std::string::npos || safeName.find('"') != std::string::npos) {
                std::ostringstream escaped;
                escaped << '"';
                for (char c : safeName) {
                    if (c == '"') escaped << "\"\"";
                    else escaped << c;
                }
                escaped << '"';
                safeName = escaped.str();
            }

            animeFile << static_cast<int>(anime.anime_id) << ','
                      << safeName << ','
                      << genresStream.str() << ','
                      << anime.type << ','
                      << static_cast<int>(anime.episodes) << ','
                      << std::fixed << std::setprecision(2) << anime.rating << ','
                      << static_cast<int>(anime.members) << '\n';

            stats.final_anime_count++;
        }
    }

    animeFile.close();
    LogManager::LogCustom(false, bufferName, "Exported " + std::to_string(stats.final_anime_count) + " anime entries to " + animePath);
}

void Exporter::ExportFilteredRating(const std::string& ratingPath,
                                    const std::vector<UserRatingTable>& ratingData,
                                    const std::unordered_set<int>& validAnimeIds,
                                    const std::unordered_set<int>& validUserIds,
                                    FilterStats& stats) {

    static std::string bufferName = "Training";

    std::ofstream ratingFile(ratingPath);
    if (!ratingFile.is_open()) {
        LogManager::LogCritical("Cannot open file for writing: " + ratingPath + " " + __LOGERROR__);
    }

    ratingFile << "user_id,anime_id,rating\n";

    for (const auto& rating : ratingData) {
        // Пропускаем невалидные рейтинги
        if (rating.rating == -1 || !IsValidRating(rating)) {
            continue;
        }

        int userId = static_cast<int>(rating.user_id);
        int animeId = static_cast<int>(rating.anime_id);

        // Проверяем, что и пользователь и аниме валидны
        if (validUserIds.find(userId) != validUserIds.end() &&
            validAnimeIds.find(animeId) != validAnimeIds.end()) {

            ratingFile << userId << ',' << animeId << ','
                       << static_cast<int>(rating.rating) << '\n';
            stats.final_rating_count++;
        }
    }

    ratingFile.close();
    LogManager::LogCustom(false,bufferName,"Exported " + std::to_string(stats.final_rating_count) + " rating entries to " + ratingPath);
}
#include "Exporter.h"
#include <filesystem>
#include <iostream>

std::unordered_map<int, int> Exporter::GetRatingCount(const std::vector<UserRatingTable>& ratingData) {
     std::unordered_map<int, int> ratingCounts;
     for (const auto& r : ratingData) {
         if (r.rating != -1)
             ++ratingCounts[r.anime_id];
     }

    return ratingCounts;
}

void Exporter::ExportFilteredDataToCSV( const std::string& animePath,
                                        const std::string& ratingPath,
                                        const std::vector<AnimeTable>& animeData,
                                        const std::vector<UserRatingTable>& ratingData,
                                        int tolerance) {
    std::cout << "\n" << "Export Statistics:" << "\n";

    namespace fs = std::filesystem;

    std::unordered_map<int, int> ratingCounts = GetRatingCount(ratingData);

    std::unordered_map<int, bool> validAnimeIds;
    for (const auto& pair : ratingCounts) {
        if (pair.second >= tolerance) {
            validAnimeIds[pair.first] = true;
        }
    }

    // === Создание директорий, если не существуют ===
    for (const auto& path : {animePath, ratingPath}) {
        fs::path dir = fs::path(path).parent_path();
        if (!dir.empty() && !fs::exists(dir)) {
            fs::create_directories(dir);
        }
    }

    ExportFilteredAnime(animePath, animeData, validAnimeIds);
    std::cout << "\n";
    ExportFilteredRating(ratingPath, ratingData, validAnimeIds);
    std::cout << "\n";
}

void Exporter::ExportFilteredAnime(
        const std::string& animePath,
        const std::vector<AnimeTable>& animeData,
        const std::unordered_map<int, bool>& validAnimeIds) {

    size_t animeCount = 0;
    size_t removeCount = 0;

    std::ofstream animeFile(animePath);
    if (!animeFile.is_open()) {
        throw std::runtime_error("Cannot open file for writing: " + animePath);
    }

    animeFile << "anime_id,name,genres,type,episodes,rating,members\n";

    for (const auto &anime: animeData) {
        if (!validAnimeIds.count(anime.anime_id)) { removeCount++; continue; }

        std::ostringstream genresStream;
        for (size_t i = 0; i < anime.genres.size(); ++i) {
            genresStream << anime.genres[i];
            if (i + 1 < anime.genres.size()) genresStream << '|';
        }

        std::string safeName = anime.name;
        if (safeName.find(',') != std::string::npos || safeName.find('"') != std::string::npos) {
            std::ostringstream escaped;
            escaped << '"';
            for (char c: safeName) {
                if (c == '"') escaped << "\"\"";
                else escaped << c;
            }
            escaped << '"';
            safeName = escaped.str();
        }

        animeFile << anime.anime_id << ','
                  << safeName << ','
                  << genresStream.str() << ','
                  << anime.type << ','
                  << anime.episodes << ','
                  << anime.rating << ','
                  << anime.members << '\n';

        ++animeCount;
    }

    animeFile.close();
    std::cout << "Exported " << animeCount << " anime entries to " << animePath << "\n";
    std::cout << "Removed " << removeCount << " lines" << "\n";
}

void Exporter::ExportFilteredRating(
        const std::string& ratingPath,
        const std::vector<UserRatingTable>& ratingData,
        const std::unordered_map<int, bool>& validAnimeIds) {

    size_t ratingCount = 0;
    size_t removeCount = 0;

    std::ofstream ratingFile(ratingPath);
    if (!ratingFile.is_open()) {
        throw std::runtime_error("Cannot open file for writing: " + ratingPath);
    }

    ratingFile << "user_id,anime_id,rating\n";

    for (const auto &row: ratingData) {
        if (row.rating == -1) { removeCount++; continue; }
        if (!validAnimeIds.count(row.anime_id)) { removeCount++; continue; }

        ratingFile << row.user_id << ',' << row.anime_id << ',' << row.rating << '\n';
        ++ratingCount;
    }

    ratingFile.close();
    std::cout << "Exported " << ratingCount << " rating entries to " << ratingPath << "\n";
    std::cout << "Removed " << removeCount << " lines" << "\n";
}
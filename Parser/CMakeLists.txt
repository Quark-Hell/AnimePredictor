set(CMAKE_SOURCE_DIR ${CMAKE_SOURCE_DIR}/Parser)

# Создаем исполняемый файл
add_executable(
        Parser

        Parser.cpp
        Exporter.cpp

        Tables/AnimeTable.cpp
        Tables/UserRatingTable.cpp)

# Указываем путь к исходному файлу

set(DATA_SOURCE "${CMAKE_SOURCE_DIR}/data")
set(DATA_DESTINATION "$<TARGET_FILE_DIR:Parser>")
message("${CMAKE_CURRENT_BINARY_DIR}")

add_custom_command(TARGET Parser POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${DATA_DESTINATION}"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${DATA_SOURCE}" "${DATA_DESTINATION}/data"
)
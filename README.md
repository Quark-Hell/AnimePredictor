# Anime Rating Predictor

**AnimePredictor** — это кроссплатформенное приложение с графическим интерфейсом на базе **Dear ImGui**, которое позволяет загружать модель машинного обучения и предсказывать рейтинг аниме на основе типа, жанров и количества эпизодов.

Модель обучается в контейнере **Docker** и экспортируется в формате TensorFlow Lite для использования в C++ приложении.

## Возможности

- Графический интерфейс (Dear ImGui)
- Поддержка обучения модели внутри Docker
- Предсказание рейтинга аниме по жанрам, типу и числу эпизодов
- Справка по жанрам и типам
- Использование TensorFlow Lite в C++
- Встроенное логирование

## Статистика обучения

- MAE (Mean Absolute Error): 0.9056
- RMSE (Root Mean Square Error): 1.1944
- R² (Coefficient of Determination): 0.4185
- Среднее остатков: -0.0481
- Стандартное отклонение остатков: 1.1934

Точность по диапазонам рейтингов:
- Рейтинг 1-4: MAE = 3.6458 (записей: 13275)
- Рейтинг 4-6: MAE = 1.9525 (записей: 63458)
- Рейтинг 6-8: MAE = 0.7893 (записей: 324079)
- Рейтинг 8-10: MAE = 0.6369 (записей: 429593)
  
Статистика предсказаний:
- Минимум предсказания: 1.06
- Максимум предсказания: 25.94
- Среднее предсказание: 7.74
- Стандартное отклонение предсказаний: 0.90


## Требования

### Необходимое ПО

| Зависимость | Назначение |
|-------------|------------|
| `C++20`     | Язык проекта |
| `CMake`     | Сборка проекта |
| `Docker`    | Обучение модели |

### Автоматически подтягиваемые зависимости

#### Для C++ приложения

- [Dear ImGui](https://github.com/ocornut/imgui)
- [GLFW](https://github.com/glfw/glfw)
- [stb_image](https://github.com/nothings/stb)
- [TensorFlow Lite](https://github.com/tensorflow/tensorflow)
- [nlohmann/json](https://github.com/nlohmann/json)
- [OpenGL](https://www.opengl.org/)
- [Flatbuffers](https://github.com/google/flatbuffers)

#### Для обучения в Docker:

- `python`
- `numpy`
- `pandas`
- `scikit-learn`
- `tensorflow`

## Сборка

```bash
git clone https://github.com/yourusername/AnimePredictor.git
cd AnimePredictor
mkdir build && cd build
cmake ..
cmake --build .


import pandas as pd
import numpy as np
import tensorflow as tf
from sklearn.model_selection import train_test_split
from sklearn.preprocessing import MultiLabelBinarizer, LabelEncoder, StandardScaler
from sklearn.metrics import mean_absolute_error, mean_squared_error, r2_score
import pickle
import json
import warnings
from collections import Counter

warnings.filterwarnings('ignore')

# Установка seed для воспроизводимости
tf.random.set_seed(42)
np.random.seed(42)

def load_and_preprocess_data():
    """Загружает и предобрабатывает данные"""
    # Загрузка данных
    anime_df = pd.read_csv('anime.csv')
    rating_df = pd.read_csv('rating.csv')

    # Объединяем данные по anime_id
    merged_df = rating_df.merge(anime_df, on='anime_id', how='inner')

    print(f"Общее количество записей после объединения: {len(merged_df)}")
    print(f"Уникальных аниме: {merged_df['anime_id'].nunique()}")
    print(f"Уникальных пользователей: {merged_df['user_id'].nunique()}")

    return merged_df


def create_features(df):
    """Создает признаки для модели (исправленная версия)"""
    features_df = df.copy()

    # Сначала фильтруем данные с валидными рейтингами
    features_df = features_df[features_df['rating_x'].notna()]
    features_df = features_df.reset_index(drop=True)  # Сбрасываем индексы

    print(f"Данных после фильтрации по рейтингам: {len(features_df)}")

    # 1. Обработка жанров (One-Hot Encoding)
    genres_list = []
    for idx, genres in enumerate(features_df['genres'].fillna('')):
        if genres and genres != 'Unknown' and str(genres) != 'nan':
            genres_list.append([g.strip() for g in genres.split('|')])
        else:
            genres_list.append(['Unknown'])  # Добавляем категорию для неизвестных жанров

    # Создаем бинарное представление жанров
    mlb = MultiLabelBinarizer()
    genres_encoded = mlb.fit_transform(genres_list)
    genres_df = pd.DataFrame(genres_encoded, columns=[f'genre_{genre}' for genre in mlb.classes_])

    print(f"Найдено жанров: {len(mlb.classes_)}")
    print(f"Примеры жанров: {list(mlb.classes_)[:10]}")

    # 2. Обработка типа аниме (Label Encoding)
    le_type = LabelEncoder()
    features_df['type_filled'] = features_df['type'].fillna('Unknown')
    features_df['type_encoded'] = le_type.fit_transform(features_df['type_filled'])

    print(f"Найдено типов аниме: {len(le_type.classes_)}")
    print(f"Типы аниме: {list(le_type.classes_)}")

    # 3. Числовые признаки
    features_df['episodes'] = pd.to_numeric(features_df['episodes'], errors='coerce').fillna(0)

    # Логарифмируем количество эпизодов для лучшего распределения
    features_df['episodes_log'] = np.log1p(features_df['episodes'])

    # 4. Статистики по пользователям и аниме
    user_stats = features_df.groupby('user_id')['rating_x'].agg(['mean', 'std', 'count']).reset_index()
    user_stats.columns = ['user_id', 'user_avg_rating', 'user_rating_std', 'user_rating_count']
    user_stats['user_rating_std'] = user_stats['user_rating_std'].fillna(0)

    anime_stats = features_df.groupby('anime_id')['rating_x'].agg(['mean', 'std', 'count']).reset_index()
    anime_stats.columns = ['anime_id', 'anime_avg_user_rating', 'anime_user_rating_std', 'anime_user_rating_count']
    anime_stats['anime_user_rating_std'] = anime_stats['anime_user_rating_std'].fillna(0)

    # Объединяем статистики
    features_df = features_df.merge(user_stats, on='user_id', how='left')
    features_df = features_df.merge(anime_stats, on='anime_id', how='left')

    # Убеждаемся, что все индексы совпадают
    assert len(features_df) == len(genres_df), f"Несовпадение индексов: {len(features_df)} vs {len(genres_df)}"

    # Объединяем все признаки
    final_features = pd.concat([
        features_df[['type_encoded', 'episodes', 'episodes_log', 'user_avg_rating', 'user_rating_std', 'user_rating_count',
                     'anime_user_rating_std', 'anime_user_rating_count']].reset_index(drop=True),
        genres_df.reset_index(drop=True)
    ], axis=1)

    # Целевая переменная
    target = features_df['rating_x'].reset_index(drop=True)

    # Проверяем на NaN значения
    print(f"NaN в признаках: {final_features.isnull().sum().sum()}")
    print(f"NaN в целевой переменной: {target.isnull().sum()}")

    # Показываем важность признаков
    print(f"\nИтоговые признаки ({final_features.shape[1]} штук):")
    numeric_features = [col for col in final_features.columns if not col.startswith('genre_')]
    genre_features = [col for col in final_features.columns if col.startswith('genre_')]
    print(f"Числовые признаки ({len(numeric_features)}): {numeric_features}")
    print(f"Жанровые признаки ({len(genre_features)}): {genre_features[:10]}...")

    # Проверяем распределение признаков
    print(f"\nСтатистика по основным признакам:")
    print(f"Episodes - min: {final_features['episodes'].min()}, max: {final_features['episodes'].max()}, mean: {final_features['episodes'].mean():.2f}")
    print(f"Type encoded - уникальных значений: {final_features['type_encoded'].nunique()}")
    print(f"Среднее количество жанров на аниме: {final_features[genre_features].sum(axis=1).mean():.2f}")

    # Показываем статистику по типам аниме
    print("\nРаспределение по типам аниме:")
    type_counts = features_df['type_filled'].value_counts()
    print(type_counts.head(10))

    # Показываем топ жанров
    print("\nТоп-10 популярных жанров:")
    all_genres = []
    for genres in features_df['genres'].fillna(''):
        if genres and str(genres) != 'nan':
            all_genres.extend([g.strip() for g in genres.split('|')])
    genre_counts = Counter(all_genres)
    print(genre_counts.most_common(10))

    return final_features, target, mlb, le_type


def create_feature_only_model(num_features):
    """Создает улучшенную модель, использующую только признаки"""
    features_input = tf.keras.layers.Input(shape=(num_features,), name='features')

    # Первый блок
    x = tf.keras.layers.Dense(512, activation='relu')(features_input)
    x = tf.keras.layers.BatchNormalization()(x)
    x = tf.keras.layers.Dropout(0.3)(x)

    # Второй блок
    x = tf.keras.layers.Dense(256, activation='relu')(x)
    x = tf.keras.layers.BatchNormalization()(x)
    x = tf.keras.layers.Dropout(0.3)(x)

    # Третий блок с residual connection
    x = tf.keras.layers.Dense(128, activation='relu')(x)
    x = tf.keras.layers.BatchNormalization()(x)
    x = tf.keras.layers.Dropout(0.2)(x)

    # Добавляем пропуск соединения
    shortcut = tf.keras.layers.Dense(128, activation='relu')(features_input)
    x = tf.keras.layers.Add()([x, shortcut])
    x = tf.keras.layers.Activation('relu')(x)

    # Финальные слои
    x = tf.keras.layers.Dense(64, activation='relu')(x)
    x = tf.keras.layers.Dropout(0.2)(x)

    x = tf.keras.layers.Dense(32, activation='relu')(x)
    x = tf.keras.layers.Dropout(0.1)(x)

    output = tf.keras.layers.Dense(1, activation='linear')(x)

    model = tf.keras.Model(inputs=[features_input], outputs=output)
    return model


def train_feature_model(features_df, target):
    """Обучает модель без использования эмбеддингов идентификаторов"""
    # Используем только остальные признаки
    other_features = features_df.copy()

    # Нормализация признаков
    scaler = StandardScaler()
    other_features_scaled = scaler.fit_transform(other_features)

    # Разделение данных: обучение, валидация, тест (60/20/20)
    X_train, X_temp, y_train, y_temp = train_test_split(
        other_features_scaled, target, test_size=0.4, random_state=42, stratify=None
    )
    X_val, X_test, y_val, y_test = train_test_split(
        X_temp, y_temp, test_size=0.5, random_state=42, stratify=None
    )

    print(f"Размер обучающей выборки: {len(X_train)}")
    print(f"Размер валидационной выборки: {len(X_val)}")
    print(f"Размер тестовой выборки: {len(X_test)}")

    num_features = other_features_scaled.shape[1]

    model = create_feature_only_model(num_features)

    # Используем более агрессивный оптимизатор
    optimizer = tf.keras.optimizers.Adam(learning_rate=0.001, beta_1=0.9, beta_2=0.999)

    model.compile(
        optimizer=optimizer,
        loss='mse',
        metrics=['mae']
    )

    print("\nАрхитектура модели:")
    model.summary()

    # Callbacks
    callbacks = [
        tf.keras.callbacks.EarlyStopping(
            monitor='val_loss', patience=20, restore_best_weights=True, verbose=1
        ),
        tf.keras.callbacks.ReduceLROnPlateau(
            monitor='val_loss', factor=0.5, patience=10, min_lr=1e-7, verbose=1
        ),
        tf.keras.callbacks.ModelCheckpoint(
            'best_model.h5', monitor='val_loss', save_best_only=True, verbose=1
        )
    ]

    print("\nОбучение модели (без эмбеддингов)...")
    history = model.fit(
        X_train, y_train,
        validation_data=(X_val, y_val),
        epochs=10,  # Увеличиваем количество эпох
        batch_size=128,  # Увеличиваем batch size
        callbacks=callbacks,
        verbose=1
    )

    return model, (X_train, X_val, X_test), (y_train, y_val, y_test), scaler, history, other_features.columns.tolist()


def evaluate_model(model, X_test, y_test):
    """Оценивает качество модели"""
    y_pred = model.predict(X_test, verbose=0).flatten()

    mae = mean_absolute_error(y_test, y_pred)
    rmse = np.sqrt(mean_squared_error(y_test, y_pred))
    r2 = r2_score(y_test, y_pred)

    print("\n" + "=" * 50)
    print("ОЦЕНКА КАЧЕСТВА МОДЕЛИ")
    print("=" * 50)
    print(f"MAE (Mean Absolute Error): {mae:.4f}")
    print(f"RMSE (Root Mean Square Error): {rmse:.4f}")
    print(f"R² (Coefficient of Determination): {r2:.4f}")

    residuals = y_test - y_pred
    print(f"Среднее остатков: {np.mean(residuals):.4f}")
    print(f"Стандартное отклонение остатков: {np.std(residuals):.4f}")

    rating_ranges = [(1, 4), (4, 6), (6, 8), (8, 10)]
    print("\nТочность по диапазонам рейтингов:")
    for low, high in rating_ranges:
        mask = (y_test >= low) & (y_test < high)
        if mask.sum() > 0:
            range_mae = mean_absolute_error(y_test[mask], y_pred[mask])
            print(f"  Рейтинг {low}-{high}: MAE = {range_mae:.4f} (записей: {mask.sum()})")

    # Показываем статистику предсказаний
    print(f"\nСтатистика предсказаний:")
    print(f"Минимум предсказания: {y_pred.min():.2f}")
    print(f"Максимум предсказания: {y_pred.max():.2f}")
    print(f"Среднее предсказание: {y_pred.mean():.2f}")
    print(f"Стандартное отклонение предсказаний: {y_pred.std():.2f}")

    print("=" * 50)
    return y_pred


def save_model_and_metadata(model, scaler, mlb, le_type, feature_columns):
    """Сохраняет модель и метаданные для использования в C++"""
    import os
    os.makedirs('model_export', exist_ok=True)

    # Сохраняем модель в формате SavedModel
    model.save('model_export/saved_model')
    print("Модель сохранена в формате SavedModel")

    # Сохраняем модель в формате TensorFlow Lite
    converter = tf.lite.TFLiteConverter.from_saved_model('model_export/saved_model')
    converter.optimizations = [tf.lite.Optimize.DEFAULT]
    tflite_model = converter.convert()
    with open('model_export/model.tflite', 'wb') as f:
        f.write(tflite_model)
    print("Модель сохранена в формате TensorFlow Lite")

    # Сохраняем скейлер
    with open('model_export/scaler.pkl', 'wb') as f:
        pickle.dump(scaler, f)

    # Сохраняем бинаризатор жанров и энкодер типов
    with open('model_export/mlb.pkl', 'wb') as f:
        pickle.dump(mlb, f)
    with open('model_export/le_type.pkl', 'wb') as f:
        pickle.dump(le_type, f)

    # Сохраняем метаданные в JSON
    metadata = {
        'num_features': len(feature_columns),
        'feature_columns': feature_columns,
        'genre_classes': mlb.classes_.tolist(),
        'type_classes': le_type.classes_.tolist(),
        'scaler_mean': scaler.mean_.tolist(),
        'scaler_scale': scaler.scale_.tolist()
    }
    with open('model_export/metadata.json', 'w') as f:
        json.dump(metadata, f, indent=2)

    print("Метаданные сохранены в metadata.json")
    print("Все файлы сохранены в папке 'model_export/'")


def main():
    print("Загрузка и предобработка данных...")
    merged_df = load_and_preprocess_data()

    print("\nСоздание признаков (без user_id и anime_id)...")
    features, target, mlb, le_type = create_features(merged_df)

    print(f"Финальный размер датасета: {len(features)}")
    print(f"Количество признаков: {features.shape[1]}")

    print(f"\nРаспределение рейтингов:")
    print(f"Минимум: {target.min()}, Максимум: {target.max()}")
    print(f"Среднее: {target.mean():.2f}, Медиана: {target.median():.2f}")
    print(f"Стандартное отклонение: {target.std():.2f}")

    print("\n" + "=" * 60)
    print("ОБУЧЕНИЕ МОДЕЛИ (без эмбеддингов)")
    print("=" * 60)

    model, splits, targets_splits, scaler, history, feature_columns = train_feature_model(features, target)
    X_train, X_val, X_test = splits
    y_train, y_val, y_test = targets_splits

    y_pred = evaluate_model(model, X_test, y_test)

    print("\n" + "=" * 60)
    print("СОХРАНЕНИЕ МОДЕЛИ")
    print("=" * 60)
    save_model_and_metadata(model, scaler, mlb, le_type, feature_columns)

    print("\nМодель успешно обучена и сохранена!")

    return model


if __name__ == "__main__":
    main()

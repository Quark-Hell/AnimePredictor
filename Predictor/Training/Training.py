import pandas as pd
import numpy as np
import tensorflow as tf
from sklearn.model_selection import train_test_split
from sklearn.preprocessing import MultiLabelBinarizer, LabelEncoder, StandardScaler
from sklearn.metrics import mean_absolute_error, mean_squared_error, r2_score
import pickle
import json
import warnings

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
    """Создает признаки для модели (без user_id, anime_id, anime_rating и members)"""
    features_df = df.copy()

    # 1. Обработка жанров (One-Hot Encoding)
    genres_list = []
    for genres in features_df['genres'].fillna(''):
        if genres:
            genres_list.append([g.strip() for g in genres.split('|')])
        else:
            genres_list.append([])

    # Создаем бинарное представление жанров
    mlb = MultiLabelBinarizer()
    genres_encoded = mlb.fit_transform(genres_list)
    genres_df = pd.DataFrame(genres_encoded, columns=[f'genre_{genre}' for genre in mlb.classes_])

    # 2. Обработка типа аниме (Label Encoding)
    le_type = LabelEncoder()
    features_df['type_encoded'] = le_type.fit_transform(features_df['type'].fillna('Unknown'))

    # 3. Числовые признаки
    features_df['episodes'] = pd.to_numeric(features_df['episodes'], errors='coerce').fillna(0)

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

    final_features = pd.concat([
        features_df[['type_encoded', 'episodes', 'user_avg_rating', 'user_rating_std', 'user_rating_count',
                     'anime_user_rating_std', 'anime_user_rating_count']],
        genres_df
    ], axis=1)

    # Целевая переменная
    target = features_df['rating_x']

    return final_features, target, mlb, le_type


def create_feature_only_model(num_features):
    """Создает модель, использующую только признаки (без эмбеддингов)"""
    features_input = tf.keras.layers.Input(shape=(num_features,), name='features')

    x = tf.keras.layers.Dense(256, activation='relu')(features_input)
    x = tf.keras.layers.BatchNormalization()(x)
    x = tf.keras.layers.Dropout(0.3)(x)

    x = tf.keras.layers.Dense(128, activation='relu')(x)
    x = tf.keras.layers.BatchNormalization()(x)
    x = tf.keras.layers.Dropout(0.2)(x)

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
        other_features_scaled, target, test_size=0.4, random_state=42
    )
    X_val, X_test, y_val, y_test = train_test_split(
        X_temp, y_temp, test_size=0.5, random_state=42
    )

    print(f"Размер обучающей выборки: {len(X_train)}")
    print(f"Размер валидационной выборки: {len(X_val)}")
    print(f"Размер тестовой выборки: {len(X_test)}")

    num_features = other_features_scaled.shape[1]

    model = create_feature_only_model(num_features)
    model.compile(
        optimizer=tf.keras.optimizers.Adam(learning_rate=0.001),
        loss='mse',
        metrics=['mae']
    )

    print("\nАрхитектура модели (без эмбеддингов):")
    model.summary()

    # Callbacks
    callbacks = [
        tf.keras.callbacks.EarlyStopping(
            monitor='val_loss', patience=15, restore_best_weights=True
        ),
        tf.keras.callbacks.ReduceLROnPlateau(
            monitor='val_loss', factor=0.5, patience=7, min_lr=1e-6
        )
    ]

    print("\nОбучение модели (без эмбеддингов)...")
    history = model.fit(
        X_train, y_train,
        validation_data=(X_val, y_val),
        epochs=1,
        batch_size=64,
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
    print("ОЦЕНКА КАЧЕСТВА МОДЕЛИ (без эмбеддингов)")
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


def plot_training_history(history):
    """Отображает историю обучения"""
    import matplotlib.pyplot as plt

    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(12, 4))
    ax1.plot(history.history['loss'], label='Training Loss')
    ax1.plot(history.history['val_loss'], label='Validation Loss')
    ax1.set_title('Model Loss')
    ax1.set_xlabel('Epoch')
    ax1.set_ylabel('Loss')
    ax1.legend()

    ax2.plot(history.history['mae'], label='Training MAE')
    ax2.plot(history.history['val_mae'], label='Validation MAE')
    ax2.set_title('Model MAE')
    ax2.set_xlabel('Epoch')
    ax2.set_ylabel('MAE')
    ax2.legend()

    plt.tight_layout()
    plt.show()


def main():
    print("Загрузка и предобработка данных...")
    merged_df = load_and_preprocess_data()

    print("\nСоздание признаков (без user_id и anime_id)...")
    features, target, mlb, le_type = create_features(merged_df)

    # Удаление строк с NaN
    valid_indices = ~(features.isnull().any(axis=1) | target.isnull())
    features = features[valid_indices]
    target = target[valid_indices]

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
    # plot_training_history(history)  # опционально: показать графики истории обучения


if __name__ == "__main__":
    main()

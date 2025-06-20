import pandas as pd
import numpy as np
import tensorflow as tf
from tensorflow.keras import layers, models
from sklearn.preprocessing import MultiLabelBinarizer, StandardScaler, RobustScaler
from sklearn.model_selection import train_test_split
from tensorflow.keras import regularizers
import matplotlib.pyplot as plt
import argparse
import joblib
import os


# === Конфигурация через флаги ===
def parse_arguments():
    parser = argparse.ArgumentParser(description='Anime Rating Prediction Model')
    parser.add_argument('--mode', choices=['train', 'test', 'both'], default='both',
                        help='Режим работы: train (только обучение), test (только тестирование), both (обучение и тестирование)')
    parser.add_argument('--model-path', default='saved_model/anime_rating_model_improved',
                        help='Путь к сохраненной модели')
    parser.add_argument('--epochs', type=int, default=5,
                        help='Количество эпох для обучения')
    parser.add_argument('--batch-size', type=int, default=128,
                        help='Размер батча')
    parser.add_argument('--data-limit', type=int, default=5000000,
                        help='Лимит строк для загрузки из ratings.csv')
    parser.add_argument('--plot', action='store_true',
                        help='Показать графики обучения')
    parser.add_argument('--verbose', type=int, default=1,
                        help='Уровень детализации вывода (0, 1, 2)')

    return parser.parse_args()


# === Загрузка и объединение данных ===
def load_and_prepare_data(data_limit):
    print("=== Загрузка данных ===")
    anime_df = pd.read_csv("out/anime.csv")
    ratings_df = pd.read_csv("out/rating.csv").head(data_limit)

    merged_df = pd.merge(ratings_df, anime_df, on="anime_id")
    merged_df['genres'] = merged_df['genres'].fillna('').apply(lambda g: g.split('|'))

    print(f"Размер данных: {merged_df.shape}")
    print(f"Статистика целевой переменной (rating_x):")
    print(merged_df['rating_x'].describe())
    print(f"\nСтатистика числовых признаков:")
    print(merged_df[['episodes', 'rating_y', 'members']].describe())

    return merged_df


# === Преобразование признаков ===
def prepare_features(merged_df, mlb=None, scaler=None, is_training=True):
    print("=== Подготовка признаков ===")

    # Обработка жанров
    if is_training:
        mlb = MultiLabelBinarizer()
        genre_features = pd.DataFrame(mlb.fit_transform(merged_df['genres']), columns=mlb.classes_)
    else:
        if mlb is None:
            raise ValueError("MultiLabelBinarizer должен быть предоставлен для тестирования")
        genre_features = pd.DataFrame(mlb.transform(merged_df['genres']), columns=mlb.classes_)

    # Обработка типов
    type_dummies = pd.get_dummies(merged_df['type'].fillna('Unknown'))

    # Обработка числовых признаков
    numeric_cols = ['episodes', 'rating_y', 'members']
    numeric_data = merged_df[numeric_cols].fillna(0)

    if is_training:
        scaler = RobustScaler()
        numeric_features_scaled = pd.DataFrame(
            scaler.fit_transform(numeric_data),
            columns=[f"{col}_scaled" for col in numeric_cols]
        )
    else:
        if scaler is None:
            raise ValueError("Scaler должен быть предоставлен для тестирования")
        numeric_features_scaled = pd.DataFrame(
            scaler.transform(numeric_data),
            columns=[f"{col}_scaled" for col in numeric_cols]
        )

    # Объединяем признаки
    features = pd.concat([
        genre_features.reset_index(drop=True),
        type_dummies.reset_index(drop=True),
        numeric_features_scaled.reset_index(drop=True)
    ], axis=1)

    target = merged_df['rating_x'].reset_index(drop=True)

    print(f"Размер матрицы признаков: {features.shape}")
    print(f"Количество уникальных жанров: {len(mlb.classes_)}")

    return features, target, mlb, scaler


# === Создание модели ===
def create_model(input_dim, dropout_rate=0.3):
    model = models.Sequential([
        layers.Input(shape=(input_dim,)),

        # Первый блок
        layers.Dense(256, activation='relu',
                     kernel_regularizer=regularizers.l2(0.001),
                     kernel_initializer='he_normal'),
        layers.BatchNormalization(),
        layers.Dropout(dropout_rate),

        # Второй блок
        layers.Dense(128, activation='relu',
                     kernel_regularizer=regularizers.l2(0.001),
                     kernel_initializer='he_normal'),
        layers.BatchNormalization(),
        layers.Dropout(dropout_rate),

        # Третий блок
        layers.Dense(64, activation='relu',
                     kernel_regularizer=regularizers.l2(0.001),
                     kernel_initializer='he_normal'),
        layers.BatchNormalization(),
        layers.Dropout(dropout_rate / 2),

        # Четвертый блок
        layers.Dense(32, activation='relu',
                     kernel_initializer='he_normal'),
        layers.Dropout(dropout_rate / 2),

        # Выходной слой
        layers.Dense(1, kernel_initializer='normal')
    ])
    return model


# === Обучение модели ===
def train_model(X_train, y_train, epochs, batch_size, verbose):
    print("\n=== Создание и компиляция модели ===")
    model = create_model(X_train.shape[1])

    # Улучшенная компиляция с адаптивным learning rate
    initial_learning_rate = 0.001
    lr_schedule = tf.keras.optimizers.schedules.ExponentialDecay(
        initial_learning_rate,
        decay_steps=1000,
        decay_rate=0.9,
        staircase=True
    )

    optimizer = tf.keras.optimizers.Adam(learning_rate=lr_schedule)
    model.compile(
        optimizer=optimizer,
        loss='huber',  # Более устойчивая к выбросам функция потерь
        metrics=['mae', 'mse']
    )

    if verbose > 0:
        print(model.summary())

    # Колбэки для обучения
    callbacks = [
        tf.keras.callbacks.EarlyStopping(
            monitor='val_loss',
            patience=10,
            restore_best_weights=True,
            verbose=verbose
        ),
        tf.keras.callbacks.ReduceLROnPlateau(
            monitor='val_loss',
            factor=0.5,
            patience=5,
            min_lr=1e-7,
            verbose=verbose
        ),
        tf.keras.callbacks.ModelCheckpoint(
            'best_model.h5',
            save_best_only=True,
            monitor='val_loss',
            verbose=verbose
        )
    ]

    print("\n=== Начало обучения ===")
    history = model.fit(
        X_train, y_train,
        epochs=epochs,
        batch_size=batch_size,
        validation_split=0.15,
        callbacks=callbacks,
        verbose=verbose
    )

    return model, history


# === Загрузка сохраненной модели ===
def load_saved_model(model_path):
    print(f"\n=== Загрузка модели из {model_path} ===")
    if not os.path.exists(model_path):
        raise FileNotFoundError(f"Модель не найдена по пути: {model_path}")

    model = tf.keras.models.load_model(model_path)

    # Загрузка препроцессоров
    scaler_path = "saved_model/scaler.pkl"
    mlb_path = "saved_model/mlb.pkl"

    if not os.path.exists(scaler_path) or not os.path.exists(mlb_path):
        raise FileNotFoundError("Препроцессоры (scaler.pkl или mlb.pkl) не найдены")

    scaler = joblib.load(scaler_path)
    mlb = joblib.load(mlb_path)

    print("Модель и препроцессоры успешно загружены")
    return model, scaler, mlb


# === Тестирование модели ===
def test_model(model, X_test, y_test, verbose):
    print("\n=== Оценка модели ===")
    results = model.evaluate(X_test, y_test, verbose=0)
    print(f"Test loss: {results[0]:.4f}")
    print(f"MAE: {results[1]:.4f}")
    print(f"MSE: {results[2]:.4f}")

    # Анализ предсказаний
    predictions = model.predict(X_test, verbose=0)
    residuals = y_test - predictions.flatten()

    print(f"\n=== Анализ остатков ===")
    print(f"Среднее остатков: {np.mean(residuals):.4f}")
    print(f"Стандартное отклонение остатков: {np.std(residuals):.4f}")
    print(f"Медиана остатков: {np.median(residuals):.4f}")

    # Примеры предсказаний
    if verbose > 0:
        print("\n=== Примеры предсказаний ===")
        test_indices = np.random.choice(len(X_test), min(10, len(X_test)), replace=False)

        for i, idx in enumerate(test_indices):
            pred = predictions[idx][0]
            actual = y_test.iloc[idx] if hasattr(y_test, 'iloc') else y_test[idx]
            error = abs(pred - actual)
            print(f"[{i + 1}] Predicted: {pred:.2f}, Actual: {actual:.2f}, Error: {error:.2f}")

    return predictions


# === Сохранение модели ===
def save_model(model, scaler, mlb, model_path):
    print(f"\n=== Сохранение модели в {model_path} ===")
    os.makedirs(os.path.dirname(model_path), exist_ok=True)
    os.makedirs("saved_model", exist_ok=True)

    model.save(model_path)
    joblib.dump(scaler, "saved_model/scaler.pkl")
    joblib.dump(mlb, "saved_model/mlb.pkl")
    print("Модель и препроцессоры сохранены")


# === Визуализация обучения ===
def plot_training_history(history):
    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(12, 4))

    # Loss
    ax1.plot(history.history['loss'], label='Training Loss')
    ax1.plot(history.history['val_loss'], label='Validation Loss')
    ax1.set_title('Model Loss')
    ax1.set_xlabel('Epoch')
    ax1.set_ylabel('Loss')
    ax1.legend()

    # MAE
    ax2.plot(history.history['mae'], label='Training MAE')
    ax2.plot(history.history['val_mae'], label='Validation MAE')
    ax2.set_title('Model MAE')
    ax2.set_xlabel('Epoch')
    ax2.set_ylabel('MAE')
    ax2.legend()

    plt.tight_layout()
    plt.show()


# === Основная функция ===
def main():
    args = parse_arguments()

    print(f"=== Режим работы: {args.mode.upper()} ===")

    # Общие переменные
    model = None
    scaler = None
    mlb = None
    history = None

    if args.mode in ['train', 'both']:
        # === РЕЖИМ ОБУЧЕНИЯ ===
        print("\n🔥 ЗАПУСК РЕЖИМА ОБУЧЕНИЯ")

        # Загрузка и подготовка данных
        merged_df = load_and_prepare_data(args.data_limit)
        features, target, mlb, scaler = prepare_features(merged_df, is_training=True)

        # Деление выборки
        X_train, X_test, y_train, y_test = train_test_split(
            features, target, test_size=0.2, random_state=42,
            stratify=pd.cut(target, bins=5)
        )

        # Приведение к float32
        X_train = X_train.astype(np.float32)
        X_test = X_test.astype(np.float32)
        y_train = y_train.astype(np.float32)
        y_test = y_test.astype(np.float32)

        # Обучение
        model, history = train_model(X_train, y_train, args.epochs, args.batch_size, args.verbose)

        # Сохранение модели
        save_model(model, scaler, mlb, args.model_path)

        # Тестирование после обучения
        if args.mode == 'both':
            test_model(model, X_test, y_test, args.verbose)

        # Визуализация
        if args.plot and history:
            plot_training_history(history)

    elif args.mode == 'test':
        # === РЕЖИМ ТЕСТИРОВАНИЯ ===
        print("\n🧪 ЗАПУСК РЕЖИМА ТЕСТИРОВАНИЯ")

        # Загрузка сохраненной модели
        model, scaler, mlb = load_saved_model(args.model_path)

        # Загрузка и подготовка данных для тестирования
        merged_df = load_and_prepare_data(args.data_limit)
        features, target, _, _ = prepare_features(merged_df, mlb, scaler, is_training=False)

        # Для тестирования используем все данные или можно разделить
        X_test = features.astype(np.float32)
        y_test = target.astype(np.float32)

        # Тестирование
        test_model(model, X_test, y_test, args.verbose)

    print(f"\n✅ Выполнение в режиме '{args.mode}' завершено успешно!")


if __name__ == "__main__":
    main()
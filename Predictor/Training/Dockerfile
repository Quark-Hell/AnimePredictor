FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    unzip \
    python3-pip \
    python3-dev \
    nano

RUN pip install --no-cache-dir \
    numpy \
    pandas \
    scikit-learn \
    matplotlib \
    tensorflow==2.13.1

# Создаем рабочую директорию
WORKDIR /app

COPY Training.py .
COPY out .

# Стартовая команда
#CMD [ "bash" ]
CMD [ "bash", "-c", "python3 Training.py" ]


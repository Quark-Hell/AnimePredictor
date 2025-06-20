FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    wget \
    unzip \
    curl \
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

COPY /Training .

# Стартовая команда
CMD [ "bash" ]
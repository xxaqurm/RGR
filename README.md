## РГР

**Описание:**
Расчетно-графическая работа по программированию. Реализует три алгоритма шифрования/дешифрования файлов в виде динамической библиотеки (.so) с консольным приложением для выбора алгоритма и ввода данных.

**Алгоритмы:**
1. **Матричная шифровка**
2. **Шифр Виженера**
3. **Шифр Плейфера**

---

#### Установка и сборка.

**Linux (Debian, Ubuntu, Linux Mint):**
1. Установите необходимые инструменты:
```bash
sudo apt update
sudo apt install g++ cmake build-essential
```

2. Клонируйте репозиторий:
```bash
git clone https://github.com/xxaqurm/RGR
cd RGR
```

3. Соберите проект:
```bash
mkdir build && cd build
cmake ..
make
```

4. Запустите приложение:
```bash
cd bin
./comprehensive_data_protection_system
```

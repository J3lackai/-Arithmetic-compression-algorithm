using namespace std;
#include <iostream>
#include <map>
#include <vector>
#include <fstream>
#include <random>
struct characteristics
{ // создали структуру с полями freqCount, Probability, cumulProb чтобы не делать три хеш-таблицы,
    // а обойтись одной с этой структурой в виде значения (получаемого по ключу)
    size_t freqCount;
    size_t cumulativefreq;
    characteristics(size_t freq = 0)
    {
        freqCount = freq;
        cumulativefreq = 0;
    }
    characteristics(size_t freq, size_t cumulfreq)
    {
        freqCount = freq;
        cumulativefreq = cumulfreq;
    }
};
unsigned char *VecToStr(unsigned char *vec, size_t bits) // преобразование вектора в строку вида 011010101
{
    if (vec)
    {
        size_t bytes = ((bits - 1) / 8) + 1;                                             // Количество байт
        unsigned char *str = (unsigned char *)calloc(sizeof(unsigned char), (bits + 1)); // Выделение памяти для строки
        if (str)
        {                        // Если указатель не нулевой
            size_t strIndex = 0; // Индекс для строки
            for (size_t i = 0; i < bytes; i++)
            {                                // Проход по ячейкам
                unsigned char mask = 1 << 7; // Создаем маску, начиная с самого левого бита
                for (int j = 0; j < 8 && i * 8 + j < bits; j++)
                {
                    if ((vec[i] & mask) != 0)
                        str[strIndex] = '1';
                    else
                        str[strIndex] = '0';
                    mask = mask >> 1;
                    strIndex++;
                }
                mask = 1 << 7;
            }
            str[bits] = '\0'; // Добавляем завершающий символ нуля
            return str;
        }
    }
    return NULL;
}
vector<bool> encode(string &text, size_t &size, size_t &num_of_sym, map<char, characteristics> &statistics)
{
    vector<bool> encoded;
    size_t temp = 0;
    size_t left = 0;
    size_t bits_to_follow = 0;
    size_t right = 65535;
    size_t First_quarter = (right / 4) + 1;
    size_t Second_quarter = First_quarter * 2;
    size_t Third_quarter = First_quarter * 3;
    cout << left << endl;
    cout << right << endl;
    size = 0;
    for (size_t i = 0; text[i]; i++)
    {
        temp = left;
        left = left + (statistics[text[i]].cumulativefreq - statistics[text[i]].freqCount) * (right - left) / num_of_sym;
        cout << left << endl;
        right = temp + statistics[text[i]].cumulativefreq * (right - temp) / num_of_sym - 1;
        cout << right << endl;
        while (true)
        {
            if (right < Second_quarter)
            {
                encoded.push_back(0);
                size++;
                for (; bits_to_follow > 0; bits_to_follow--)
                    encoded.push_back(1);
            }
            else if (left >= Second_quarter)
            {
                encoded.push_back(1);
                size++;
                for (; bits_to_follow > 0; bits_to_follow--)
                    encoded.push_back(0);
                right -= Second_quarter;
                left -= Second_quarter;
            }
            else if ((left >= First_quarter) && (right < Third_quarter))
            {
                bits_to_follow++; // добавляем биты условно
                size++;
                left -= First_quarter;
                right -= First_quarter;
            }
            else
                break;
            right += right + 1;
            left += left;
        }
    }                    // в функцию передаём size как аргумент просто чтобы получить из функции размер закодированного текста
    bits_to_follow += 1; // Завершаем кодирование выводим биты определяющие четверть лежащую в текущем интервале
    size++;
    if (left < First_quarter)
    {
        encoded.push_back(0);
        size++;
        for (; bits_to_follow > 0; bits_to_follow--)
            encoded.push_back(1);
    }
    else
    {
        encoded.push_back(1);
        size++;
        for (; bits_to_follow > 0; bits_to_follow--)
            encoded.push_back(0);
    }
    cout << "encodedText: ";
    for (size_t i = 0; i < size; i++)
    {
        cout << encoded[i];
    }
    cout << endl;
    return encoded;
}
string decode(vector<bool> &encoded, size_t &encodedSize, size_t &num_of_sym, map<char, characteristics> &statistics)
{ // заменяем на книжный вариант4
    cout << "encodedSize: " << encodedSize;
    cout << "num_of_sym:  " << num_of_sym;
    string decodedText;
    size_t left = 0;
    size_t right = 65535;
    size_t temp = 0;
    size_t First_quarter = (right / 4) + 1;
    size_t Second_quarter = First_quarter * 2;
    size_t Third_quarter = First_quarter * 3;
    size_t freq = 0;
    size_t j = 16;
    cout << endl;
    if (encodedSize == 0)
    {
        cout << "ERROR OF ENCODING! \n";
    }
    unsigned short value = 0; // value = 4072; 12345 // value = 6912;
    size_t i = 0;
    for (; i < j && i < encodedSize; i++) // value = 4072; 12345
    {
        value = value * 2 + encoded[i];
    }
    if (i < j)
        value = value << (j - i);
    cout << "value: " << value << endl;

    for (size_t i = 0; i < num_of_sym; i++)
    {
        freq = ((value + 1 - left) * num_of_sym - 1) / (right - left + 1);
        cout << freq << " freq\n";
        char symbol;
        for (auto it : statistics) // ищем символ
        {
            if (it.second.cumulativefreq > freq)
            {
                symbol = it.first;
                // right = left + (it.second.cumulativeProb * (right - left + 1)) - 1;
                break;
            }
        }
        decodedText += symbol; // нашли символ

        temp = left;
        left = left + (statistics[symbol].cumulativefreq - statistics[symbol].freqCount) * (right - left) / num_of_sym;
        cout << left << endl;
        right = temp + statistics[symbol].cumulativefreq * (right - temp) / num_of_sym - 1;
        cout << right << endl;
        while (true)
        {
            if (right < Second_quarter)
            {
                // Пропускаем 0 в закодированной последовательности
            }
            else if (left >= Second_quarter)
            {
                value -= Second_quarter;
                left -= Second_quarter;
                right -= Second_quarter;
            }
            else if ((left >= First_quarter) && (right < Third_quarter))
            {
                value -= First_quarter;
                left -= First_quarter;
                right -= First_quarter;
            }
            else
                break;
            left += left;
            right += right + 1;
            value += value;
            if (j < encodedSize)
            {
                value += encoded[j];
                j++;
            }
        }
    }
    return decodedText;
}
int main()
{
    string text;
    cout << "text: ";
    cin >> text;
    // Создаём необходимые структуры
    map<char, characteristics> statistics; // size_t 2^32-1
    size_t size = 0;
    double_t sumrange = 0;
    size_t temp = 0;
    size_t tempSize = 0;
    double_t prob = 0;
    // Подсчет частоты встречаемости символов в тексте
    for (const char c : text)
    {
        statistics[c].freqCount++;
        size++;
    }
    for (auto i : statistics)
    {
        temp = statistics[i.first].freqCount;
        tempSize += temp;
        statistics[i.first] = characteristics(temp, tempSize);
        cout << i.first << ", " << statistics[i.first].freqCount << ", " << statistics[i.first].cumulativefreq << endl;
    }
    // size_t 2^32-1
    size_t num_of_sym = size;
    vector<bool> encodedText = encode(text, size, num_of_sym, statistics);
    string decodedText = decode(encodedText, size, num_of_sym, statistics);
    cout << decodedText;
    return 0;
}
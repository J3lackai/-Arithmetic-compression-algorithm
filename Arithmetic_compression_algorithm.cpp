using namespace std;
#include <iostream>
#include <map>
#include <vector>
#include <fstream>
#include <random>
#include <string>
struct characteristics
{ // создали структуру с полями freqCount, cumulfreq чтобы обойтись одной этой структурой получаемой по ключу
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
vector<unsigned char> VecToStr(vector<unsigned char> vec, size_t bits) // преобразование вектора в строку вида 011010101
{

    size_t bytes = ((bits - 1) / 8) + 1; // Количество байт
    vector<unsigned char> str;
    for (size_t i = 0; i < bits + 1; i++)
    {
        str.push_back(0);
    }                    // Выделение памяти для строки
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
vector<unsigned char> encode(string &text, size_t &size, size_t &num_of_sym, map<char, characteristics> &statistics)
{
    vector<unsigned char> encodedText;
    vector<unsigned char> mask;
    // unsigned char byte;
    for (size_t i = 0; i < 8; i++) // создаём маску для записи побитово в vector
    {
        mask.push_back(1 << (7 - i));
    }
    size_t temp = 0;
    size_t left = 0;
    size_t bits_to_follow = 0;
    size_t right = 65535;
    size_t First_quarter = (right / 4) + 1;
    size_t Second_quarter = First_quarter * 2;
    size_t Third_quarter = First_quarter * 3;
    size = 0; // в функцию передаём size как аргумент  чтобы получить из функции размер закодированного текста
    for (size_t i = 0; text[i]; i++)
    {
        temp = left;
        left = left + (statistics[text[i]].cumulativefreq - statistics[text[i]].freqCount) * (right - left) / num_of_sym;
        right = temp + statistics[text[i]].cumulativefreq * (right - temp) / num_of_sym - 1;
        while (true)
        {
            if (right < Second_quarter)
            {
                size++;
                if (size % 8 == 1) // увеличили размер и проверили не начался ли новый байт
                    encodedText.push_back(0);
                for (; bits_to_follow > 0; bits_to_follow--)
                {
                    size++;
                    if (size % 8 == 1)
                        encodedText.push_back(0);
                    encodedText[(size - 1) / 8] = encodedText[(size - 1) / 8] | mask[(size - 1) % 8];
                }
            }
            else if (left >= Second_quarter)
            {
                size++;
                if (size % 8 == 1)
                    encodedText.push_back(0);
                encodedText[(size - 1) / 8] = encodedText[(size - 1) / 8] | mask[(size - 1) % 8];
                for (; bits_to_follow > 0; bits_to_follow--)
                {
                    size++;
                    if (size % 8 == 1)
                        encodedText.push_back(0);
                }
                right -= Second_quarter;
                left -= Second_quarter;
            }
            else if ((left >= First_quarter) && (right < Third_quarter))
            {
                bits_to_follow++; // добавляем биты условно
                left -= First_quarter;
                right -= First_quarter;
            }
            else
                break;
            right += right + 1;
            left += left;
        }
    }
    bits_to_follow += 1; // Завершаем кодирование выводим биты определяющие четверть лежащую в текущем интервале
    if (left < First_quarter)
    {
        size++;
        if (size % 8 == 1)
            encodedText.push_back(0);
        for (; bits_to_follow > 0; bits_to_follow--)
        {
            size++;
            if (size % 8 == 1)
                encodedText.push_back(0);
            encodedText[(size - 1) / 8] = encodedText[(size - 1) / 8] | mask[(size - 1) % 8];
        }
    }
    else
    {
        size++;
        if (size % 8 == 1)
            encodedText.push_back(0);
        encodedText[(size - 1) / 8] = encodedText[(size - 1) / 8] | mask[(size - 1) % 8];
        for (; bits_to_follow > 0; bits_to_follow--)
        {
            size++;
            if (size % 8 == 1)
                encodedText.push_back(0);
        }
    }
    cout << "encodedText: ";
    vector<unsigned char> vec = VecToStr(encodedText, size);
    for (size_t i = 0; i < size; i++)
    {
        cout << vec[i];
    }
    cout << endl;
    return encodedText;
}
string decode(vector<unsigned char> &encodedText, size_t &encodedSize, size_t &num_of_sym, map<char, characteristics> &statistics)
{
    string decodedText = "";
    if (encodedSize == 0)
    {
        cout << "ERROR OF ENCODING! \n";
        return decodedText;
    }
    cout << "encodedText: ";
    /*   vector <unsigned char> vec = VecToStr(encodedText, encodedSize);
       for (size_t i = 0; i < encodedSize; i++)
       {
           cout << vec[i];
       }*/
    vector<unsigned char> mask;
    // unsigned char byte;
    for (size_t i = 0; i < 8; i++)
    {
        mask.push_back(1 << (7 - i));
    }
    size_t left = 0;
    size_t right = 65535;
    size_t temp = 0;
    size_t First_quarter = (right / 4) + 1;
    size_t Second_quarter = First_quarter * 2;
    size_t Third_quarter = First_quarter * 3;
    size_t freq = 0;
    size_t j = 16;
    cout << endl;

    size_t value = 0;
    value = encodedText[0] << 8; // Сдвигаем старший байт на 8 бит влево
    if (encodedSize > 8)
        value |= encodedText[1]; // Записываем младший байт

    cout << "value: " << value << endl;

    for (size_t i = 0; i < num_of_sym; i++)
    {
        freq = ((value + 1 - left) * num_of_sym - 1) / (right - left + 1);
        char symbol = ' ';
        for (auto it : statistics) // ищем символ
        {
            if (it.second.cumulativefreq > freq)
            {
                symbol = it.first;
                break;
            }
        }
        decodedText += symbol; // нашли символ, добавили к раскодированному тексту

        temp = left;
        left = left + (statistics[symbol].cumulativefreq - statistics[symbol].freqCount) * (right - left) / num_of_sym;
        right = temp + statistics[symbol].cumulativefreq * (right - temp) / num_of_sym - 1;
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
                if ((encodedText[j / 8] | mask[j % 8]) == encodedText[j / 8])
                    value |= 1;
                j++;
            }
        }
    }
    return decodedText;
}
int main()
{
    string line;
    bool flag = 0;
    cout << "Choose Decode or Encode (Enter 0 or 1): ";
    cin >> flag;
    if (flag)
    {
        ifstream in("text.txt");
        if (!in.is_open())
        {
            cout << "ERROR: file 'text.txt' not found create it \n";
            return 1;
        }
        ofstream write("encodedtext.bin", ios::binary | ios::out); // Работаем с бинарным файлом работа с которым быстрее
        string text;
        while (getline(in, line))
            text += line;
        if (text.length() == 0)
        {
            cout << "Error: text.txt is empty";
            exit(1);
        }
        // Создаём необходимые структуры
        map<char, characteristics> statistics;
        size_t size = 0;
        size_t temp = 0;
        size_t tempSize = 0;
        size_t num_of_sym = 0;
        // Подсчет частоты встречаемости символов в тексте
        for (const char c : text)
        {
            statistics[c].freqCount++;
            num_of_sym++;
        }
        for (auto i : statistics)
        {
            temp = statistics[i.first].freqCount;
            tempSize += temp;
            statistics[i.first] = characteristics(temp, tempSize);
            cout << i.first << ", " << statistics[i.first].freqCount << ", " << statistics[i.first].cumulativefreq << endl;
        }
        vector<unsigned char> encodedText = encode(text, size, num_of_sym, statistics);
        write << size << ' ' << num_of_sym << "\n";
        cout << "size: " << size;
        cout << "num_of_sym: " << num_of_sym;
        for (size_t i = 0; i < ((size - 1) / 8 + 1); i++) // записываем в файл закодированный текст вместе с таблицей частот
            write << encodedText[i];
        write << "\n\n";
        for (auto &pair : statistics)
        {
            write << pair.first << pair.second.freqCount << ' ';
        }
    }
    else
    {
        ifstream in("encodedtext.bin", ios::binary | ios::in);
        ofstream write("text.txt");
        if (!in.is_open())
        {
            cout << "ERROR: file 'encodedtext.txt' not found create it \n";
            return 1;
        }
        vector<unsigned char> encodedText;
        map<char, characteristics> statistics;
        size_t size = 0;
        size_t num_of_sym = 0;
        size_t sumfreq = 0;
        bool flag = false;
        char sym;
        size_t freq = 0;
        bool backspace = false;
        getline(in, line);
        size_t k = 0;
        for (; line[k] != ' ' && line[k]; k++) // побитово считываем длину кода и количество символов в тексте
        {
            size *= 10;
            size += line[k] - 48;
        }
        for (size_t n = k + 1; line[n]; n++)
        {
            num_of_sym *= 10;
            num_of_sym += line[n] - 48;
        }
        cout << "size: " << size;
        cout << "num_of_sym: " << num_of_sym;

        while (getline(in, line))
        {
            if (line.length() == 0) // отступ между деревом и кодом
                break;
            if (flag) // передали байт == \n
                encodedText.push_back(10);
            for (size_t i = 0; i < line.length(); i++)
                encodedText.push_back(line[i]);
            flag = true;
        }
        getline(in, line);
        for (size_t i = 0; line[i];)
        {
            sym = line[i];
            freq = 0;
            size_t j = i + 1;
            for (; line[j] != ' ' && line[j]; j++)
            {
                freq *= 10;
                freq += line[j] - 48;
            }
            i = j + 1;
            sumfreq += freq;
            statistics[sym] = characteristics(freq, sumfreq);
            cout << "sym: " << sym << " freq: " << freq << endl;
        }
        string decodedText = decode(encodedText, size, num_of_sym, statistics);
        write << decodedText;
        cout << decodedText;
        return 0;
    }
}
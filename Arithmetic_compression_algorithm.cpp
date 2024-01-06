using namespace std;
#include <iostream>
#include <stdio.h>
#include <unordered_map>
#include <map>
#include <vector>
#include <fstream>
#include <random>
struct characteristics
{ // создали структуру с полями freqCount, Probability, cumulProb чтобы не делать три хеш-таблицы,
    // а обойтись одной с этой структурой в виде значения (получаемого по ключу)
    size_t freqCount;
    double_t Probability;
    double_t cumulativeProb;
    characteristics(size_t freq = 0)
    {
        freqCount = freq;
        Probability = 0;
        cumulativeProb = 0;
    }
    characteristics(size_t freq, double_t prob, double_t cumul)
    {
        freqCount = freq;
        Probability = prob;
        cumulativeProb = cumul;
    }
};
vector<size_t> encode(string &text, size_t size, map<char, characteristics> &statistics)
{
    vector<size_t> encoded;
    size_t i = 0;
    size_t left = 0;
    size_t bits_to_follow = 0;
    size_t right = 4294967295;
    size_t First_quarter = (right / 4) + 1;
    size_t Second_quarter = First_quarter * 2;
    size_t Third_quarter = First_quarter * 3;
    cout << left << endl;
    cout << right << endl;
    i++;
    while (text[i - 1])
    {
        left = left + (statistics[text[i - 1]].cumulativeProb - statistics[text[i - 1]].Probability) * (right - left);
        cout << left << endl;
        right = left + statistics[text[i - 1]].cumulativeProb * (right - left);
        cout << right << endl;
        while (true)
        {
            if (right < Second_quarter)
            {
                encoded.push_back(0);
                for (; bits_to_follow > 0; bits_to_follow--)
                    encoded.push_back(1);
            }
            else if (left >= Second_quarter)
            {
                encoded.push_back(1);
                for (; bits_to_follow > 0; bits_to_follow--)
                    encoded.push_back(0);
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
        i++;
    }

    cout << ((left + right) / 2) << endl;
    for (size_t i = 0; i < encoded.size(); i++)
    {
        cout << encoded[i];
    }
    return encoded;
}
/*
string decode(vector<size_t> &encoded, size_t size, map<char, size_t> &freqCount, map<char, double_t> &Probability, map<char, double_t> &cumulativeProbability)
{
    double_t sumrange = 0;
    // Вычисление кумулятивных вероятностей
    for (auto i : freqCount)
    {
        sumrange += Probability[i.first];
        cumulativeProbability[i.first] = sumrange;
    }

    size_t len = freqCount.size();
    size_t i = 0;
    size_t left = 0;
    size_t right = 4294967295;
    size_t First_quarter = right / 4;
    size_t Second_quarter = First_quarter * 2;
    size_t Third_quarter = First_quarter * 3;
    string decoded = "";
    size_t code = 0;
    size_t bits_to_follow = 0;
    double_t value = encoded[i] / double_t(size);

    while (true)
    {
        code = (value - left) / double_t(right - left);
        auto it = std::find_if(cumulativeProbability.begin(), cumulativeProbability.end(), [=](std::pair<char, double_t> elem)
                               { return elem.second > code; });
        char c = it->first;
        decoded += c;

        left = left + (cumulativeProbability[c] - Probability[c]) * (right - left);
        right = left + cumulativeProbability[c] * (right - left);
        value = (value - left) / double_t(right - left);

        while (true)
        {
            if (right < 2147483647)
            {
                // Ничего не делаем
            }
            else if (left >= 2147483647)
            {
                right <<= 1;
                left <<= 1;
                value = (value << 1) - 1;
            }
            else if ((left >= First_quarter) && (right < Third_quarter))
            {
                bits_to_follow++;
                left -= First_quarter;
                right -= First_quarter;
                value = (value - First_quarter) / double_t(right - left);
            }
            else
                break;
            right += right + 1;
            left += left;
            value = (value << 1);
            if (i < encoded.size())
            {
                value += encoded[++i];
            }
        }

        if (decoded.size() == size)
        {
            break;
        }
    }

    return decoded;
}*/
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
        // теряем поле freqCount если будем заполнять его по конструктору с тремя переменными поэтому сохраним значение поля в переменной temp
        prob = double_t(i.second.freqCount) / size;
        sumrange += prob;
        statistics[i.first] = characteristics(temp, prob, sumrange);
        cout << i.first << ", " << statistics[i.first].freqCount << ", " << statistics[i.first].Probability << ", " << statistics[i.first].cumulativeProb << endl;
    }
    // size_t 2^32-1
    vector<size_t> code = encode(text, size, statistics);
    // vector<size_t> decodedtext = decode(text, size, freqCount, Probability, cumulativeProbability);
    return 0;
}
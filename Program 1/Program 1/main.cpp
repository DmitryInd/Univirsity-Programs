#include <iostream>
#include <string>
#include <vector>



int prefix_function(const std::string& pattern, std::vector<int>& prefix_value,
                    const char now, const int now_array_position,
                    int previous_array_position) { //array в названии - массив значений префикс функции
/*
Функция возращает значение префикс функции для некоторого элемента.
В функцию подаётся:
1) Паттерн
1) Обрабатываемый символ
2) Индекс, в который будет положено значение префикс функции
3) Где хранится значение префикс функции предыдущего символа
Это нужно так как:
1) Строка не хранится польностью, и индексы букв в массиве значений
префикс функции и в строке не синхронизованы
2) Чтобы была единая префикс функция и для паттерна, и для строки:
2.1) У паттерна значение префикс функции предыдущего символа хранится в предыдущей "ячейке"
2.2) У строки в той же
*/
	while (true) {
		if (previous_array_position < 0) {
			prefix_value[now_array_position] = -1;
			break;
		}
		previous_array_position = prefix_value[previous_array_position];
		if (pattern[previous_array_position + 1] == now) {
			prefix_value[now_array_position] = previous_array_position + 1;
			break;
		}
	}
	return prefix_value[now_array_position];
}

void search_pattern(const std::string& pattern, std::vector<int> &prefix_value) {
	//Объявление вспомогательных переменных
	char now;
	int i = 0;
	//Построение массива значений префик функции для паттерна
	for (; i < pattern.size(); i++)
		prefix_function(pattern, prefix_value, pattern[i], i, i - 1);
	prefix_value[pattern.size()] = -1;
	//Поиск вхождений паттерна с помощью префикс функций
	while (std::cin >> now) {
		if (prefix_function(pattern, prefix_value, now,
			pattern.size(), pattern.size()) == pattern.size() - 1) {
			int answer = i - 2 * pattern.size() + 1;
			std::cout << answer << " ";
		}
		i++;
	}
}

int main() {
	//Ускорение ввода
	std::ios::sync_with_stdio(false);
	std::cin.tie(nullptr); 
	//Объявление переменных
	std::string pattern;
	std::cin >> pattern;
	std::vector<int> prefix_value(pattern.size() + 1);

	search_pattern(pattern, prefix_value);

	return 0;
}

#include <iostream>
#include <vector>
#include <bitset>

constexpr size_t alphabet_size = 26;



std::string reverse_pre_function(std::vector<int>& prefix_value) {
	//Номер последнего символа последнего ненулевого префикса, совпадающего с суффиксом
	int last_success = -1;
	std::bitset<alphabet_size> used(1); //Множество запрещённых символов
	std::string answer = "a"; //Так как первое значение префикс функции всегда -1
	for (int i = 1; i < prefix_value.size(); i++) {
		if (prefix_value[i] == 0) {
			while (last_success > -1) {
				/*
				Символ нельзя использовать, иначе будет ненулевой суффикс, совпадающий с префиксом.
				За хранение его "номера - 1" и отвечает last_success
				*/
				used[answer[last_success + 1] - 'a'] = 1;
				//Ищем такие символы в подсуффиксах
				last_success = prefix_value[last_success] - 1;
			}
			//Ищем минимальный незапрещённый символ
			int j = 0;
			for (; j < used.size(); j++) {
				if (used[j] == 0)
					break;
			}
			//Добавляем символ в строку
			answer += ('a' + j);
			//Префикса нет, поэтому last_success сбрасывается
			last_success = -1;
			//Так как 'a' всегда первый символ, он всегда запрещён
			used.reset();
			used.set(0);
		}
		else {
			/*
			Чтобы правильно определить следующую букву,
			нужно запомнить положение предыдущей в префиксе
			*/
			last_success = prefix_value[i] - 1;
			answer += answer[last_success];
		}
	}

	return answer;
}


int main() {
	int a;
	std::vector<int> prefix_value;

	while (std::cin >> a)
		prefix_value.push_back(a);
	if (prefix_value.size() != 0) {
		std::string answer = reverse_pre_function(prefix_value);
		std::cout << answer;
	}

	return 0;
}

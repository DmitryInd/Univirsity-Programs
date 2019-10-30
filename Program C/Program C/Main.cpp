#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

constexpr int alphabet_size = 27;



//Группировка символов для экономии памяти
int get_index(char input) {
	return input == '#' ? 0 : input - ('a' - 1);
}


//Суффиксный массив
class suf_arr {
public:
	suf_arr(const std::string& input);

	//Возращает индекс начала суффикса, находящегося на данной позиции в лексикографическом порядке
	const int64_t& get_suf_num(int64_t num_str) const { return suf[num_str][2]; }
	//Возращает положение суффикса в суффиксном массиве
	const int64_t& get_num_suffix(int64_t num) const { return rev_suf[num]; }

private:
	//суффиксный массив {классы эквивалетности двух половин, из которых собрана строка; индекс начала строки}
	std::vector<std::vector<int64_t>> suf;
	std::vector<int64_t> rev_suf; //обратный суффиксный массив
	std::vector<int64_t> equivalence; //классы эквивалетности
	std::string copy; //введённая строка с '$' в конце

	void count_sort();
	void count_sort(const std::string& input);
};


//Построение суффиксного массива
suf_arr::suf_arr(const std::string& input) {
	suf = std::vector<std::vector<int64_t>>(input.size() + 1, std::vector<int64_t>(3, 0));
	rev_suf = std::vector<int64_t>(input.size() + 1);
	equivalence = std::vector<int64_t>(input.size() + 1, 0);

	count_sort(input);

	for (int64_t k = 1; k < input.size(); k = k << 1) {
		for (int64_t i = 0; i <= input.size(); i++) {
			int64_t index = suf[i][2] - k; //индекс левой половины новой подстроки
			if (index < 0) {
				index = suf.size() + index;
			}
			suf[i][0] = equivalence[index];
			suf[i][2] = index;
		}
		count_sort(); //Сортировка новых подстрок
	}
}


//Сортировка двойных подстрок
void suf_arr::count_sort() {
	//Новый суффиксный массив
	std::vector<std::vector<int64_t>> copy_suf(suf.size(), std::vector<int64_t>(3, 0));
	std::vector<int64_t> count(suf.size());
	for (int64_t i = 0; i < suf.size(); i++) {
		count[suf[i][0]]++;
	}

	for (int64_t i = 1; i < suf.size(); i++) {
		count[i] += count[i - 1];
	}

	for (int64_t i = suf.size() - 1; i >= 0; i--) {
		copy_suf[--count[suf[i][0]]] = suf[i];
		rev_suf[suf[i][2]] = count[suf[i][0]];
	}

	suf = copy_suf;
	suf[0][1] = 0;

	int64_t num_of_class = 1; //Крайний номер класса эквивалентности
	for (int64_t i = 1; i < suf.size(); i++) {
		if (copy_suf[i][0] == copy_suf[i - 1][0] && copy_suf[i][1] == copy_suf[i - 1][1]) {
			equivalence[suf[i][2]] = equivalence[suf[i - 1][2]];
		}
		else {
			equivalence[suf[i][2]] = num_of_class;
			num_of_class++;
		}

		suf[i][1] = equivalence[suf[i][2]];
	}
}


//Сортировка единичных подстрок
void suf_arr::count_sort(const std::string& input) {
	std::vector<int64_t> count(alphabet_size, 0);
	count[0] = 1;
	for (int64_t i = 0; i < input.size(); i++) {
		count[get_index(input[i])]++;
	}

	for (int64_t i = 1; i < alphabet_size; i++) {
		count[i] += count[i - 1];
	}

	for (int64_t i = input.size() - 1; i >= 0; i--) {
		suf[--count[get_index(input[i])]][2] = i;
		rev_suf[i] = count[get_index(input[i])];
	}
	suf[0][2] = input.size();
	rev_suf[input.size()] = 0;

	int64_t num_of_class = 1; //Крайний номер класса эквивалентности
	for (int64_t i = 1; i <= input.size(); i++) {
		if (input[suf[i][2]] == input[suf[i - 1][2]]) {
			equivalence[suf[i][2]] = equivalence[suf[i - 1][2]];
		}
		else {
			equivalence[suf[i][2]] = num_of_class;
			num_of_class++;
		}

		suf[i][1] = equivalence[suf[i][2]];
	}
}


//Поиск наибольшего совпадения
int64_t search_coincidences(int64_t first, int64_t second, const std::string& input, int64_t num_coincidence) {
	int64_t maximum = std::max(first, second);
	for (; maximum + num_coincidence < input.size(); num_coincidence++) {
		if (input[first + num_coincidence] != input[second + num_coincidence]) {
			break;
		}
	}

	return num_coincidence;
}


//Строит массив значений lcp
std::vector<int64_t> build_lcp(const std::string& input, const suf_arr& base) {
	int64_t num_coincidence = 0;
	std::vector<int64_t> lcp(input.size(), 0);
	for (int64_t i = 0; i < input.size(); i++) {
		int64_t suf_index = base.get_num_suffix(i) - 1;
		if (suf_index == 0) {
			continue;
		}
		int64_t first = base.get_suf_num(suf_index);
		num_coincidence = search_coincidences(first, i, input, num_coincidence);
		lcp[suf_index] = num_coincidence;
		if (num_coincidence > 0) {
			num_coincidence = num_coincidence - 1;
		}
	}

	return lcp;
}


//Проверяет, что подстроки взяты из одинаковых строк
bool one_str(const int64_t a, const int64_t b, const int64_t first_size) {
	bool answer = false;
	if (a < first_size && b < first_size) {
		answer = true;
	}
	else if (a > first_size && b > first_size) {
		answer = true;
	}

	return answer;
}

std::string k_str(const std::vector<int64_t> lcp, const suf_arr& base, const int64_t first_size, int64_t num, std::string summ_str) {
	int64_t min_common_len = 0; //Длина минимальной общей подстроки
	int64_t max_zero = 0;
	for (int64_t i = 2; i < lcp.size(); i++) {
		if (one_str(base.get_suf_num(i), base.get_suf_num(i + 1), first_size) || lcp[i] == 0) {
			min_common_len = std::min(min_common_len, lcp[i]);
			continue;
		}
		int64_t number_of_substr = std::max(max_zero, lcp[i] - min_common_len); //Кол-во новых общих подстрок
		if (num <= number_of_substr) {
			return summ_str.substr(base.get_suf_num(i), num + min_common_len); //Так как порядок связан с длинной
		}
		num -= number_of_substr;
		min_common_len = lcp[i];
	}
	return "-1";
}


int main() {
	std::string str_first, str_second;
	int64_t num = 0;
	std::cin >> str_first >> str_second >> num;
	str_first = str_first + "#" + str_second;

	suf_arr base(str_first);
	std::vector<int64_t> lcp = build_lcp(str_first, base);
	std::cout << k_str(lcp, base, str_first.size() - str_second.size() - 1, num, str_first);
	return 0;
}
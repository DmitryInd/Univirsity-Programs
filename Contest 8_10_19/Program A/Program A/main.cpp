#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

constexpr int alphabet_size = 26;



//Суффиксный массив
class suf_arr {
public:
	suf_arr(const std::string& input);

	//Возращает индекс начала суффикса, находящегося на данной позиции в лексикографическом порядке
	const int& get_suf_num(int num_str) const { return suf[num_str][2]; }
	//Возращает положение суффикса в суффиксном массиве
	const int& get_num_suffix(int num) const { return rev_suf[num]; }

private:
	//суффиксный массив {классы эквивалетности двух половин, из которых собрана строка; индекс начала строки}
	std::vector<std::vector<int>> suf;
	std::vector<int> rev_suf; //обратный суффиксный массив
	std::vector<int> equivalence; //классы эквивалетности
	std::string copy; //введённая строка с '$' в конце

	void count_sort();
	void count_sort(const std::string& input);
};


//Построение суффиксного массива
suf_arr::suf_arr(const std::string& input) {
	suf = std::vector<std::vector<int>>(input.size() + 1, std::vector<int>(3, 0));
	rev_suf = std::vector<int>(input.size() + 1);
	equivalence = std::vector<int>(input.size() + 1, 0);

	count_sort(input);

	for (int k = 1; k < input.size(); k = k << 1) {
		for (int i = 0; i <= input.size(); i++) {
			int index = suf[i][2] - k; //индекс левой половины новой подстроки
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
	std::vector<std::vector<int>> copy_suf(suf.size(), std::vector<int>(3, 0));
	std::vector<int> count(suf.size());
	for (int i = 0; i < suf.size(); i++) {
		count[suf[i][0]]++;
	}

	for (int i = 1; i < suf.size(); i++) {
		count[i] += count[i - 1];
	}

	for (int i = suf.size() - 1; i >= 0; i--) {
		copy_suf[--count[suf[i][0]]] = suf[i];
		rev_suf[suf[i][2]] = count[suf[i][0]];
	}

	suf = copy_suf;
	suf[0][1] = 0;

	int num_of_class = 1; //Крайний номер класса эквивалентности
	for (int i = 1; i < suf.size(); i++) {
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
void suf_arr::count_sort(const std::string &input) {
	std::vector<int> count(alphabet_size, 0);
	count[0] = 1;
	for (int i = 0; i < input.size(); i++) {
		count[input[i] - 'a']++;
	}

	for (int i = 1; i < alphabet_size; i++) {
		count[i] += count[i - 1];
	}

	for (int i = input.size() - 1; i >= 0; i--) {
		suf[--count[input[i] - 'a']][2] = i;
		rev_suf[i] = count[input[i] - 'a'];
	}
	suf[0][2] = input.size();
	rev_suf[input.size()] = 0;

	int num_of_class = 1; //Крайний номер класса эквивалентности
	for (int i = 1; i <= input.size(); i++) {
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
int search_coincidences(int first, int second, const std::string& input, int num_coincidence) {
	int maximum = std::max(first, second);
	for (; maximum + num_coincidence < input.size(); num_coincidence++) {
		if (input[first + num_coincidence] != input[second + num_coincidence]) {
			break;
		}
	}

	return num_coincidence;
}


//Строит массив значений lcp
std::vector<int> build_lcp(const std::string &input, const suf_arr &base) {
	int num_coincidence = 0;
	std::vector<int> lcp (input.size(), 0);
	for (int i = 0; i < input.size(); i++) {
		int suf_index = base.get_num_suffix(i) - 1;
		if (suf_index == 0) {
			continue;
		}
		int first = base.get_suf_num(suf_index);
		num_coincidence = search_coincidences(first, i, input, num_coincidence);
		lcp[suf_index] = num_coincidence;
		if (num_coincidence > 0) {
			num_coincidence = num_coincidence - 1;
		}
	}

	return lcp;
}


//Кол-во различных подстрок
int number_of_substr(const std::string& input, const suf_arr& base, const std::vector<int> &lcp) {
	int answer = 0;
	for (int i = 1; i <= input.size(); i++) {
		answer += input.size() - base.get_suf_num(i) - lcp[i - 1];
	}

	return answer;
}


int main() {
	std::string input;
	std::cin >> input;

	suf_arr base(input);
	std::vector<int> lcp = build_lcp(input, base);
	std::cout << number_of_substr(input, base, lcp);
	return 0;
}
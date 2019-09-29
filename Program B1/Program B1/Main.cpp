#include <iostream>
#include <vector>
#include <bitset>



void reverse_pre_function(std::vector<int>& prefix_value, std::string& answer) {
	int last_success = -1; //Номер последего символа последнего ненулевого преффикса
	char base_char; //Символ, который будет вводится при нулевом суффиксе
	std::bitset<30> used(1); //Множество запрещённых символов
	answer = 'a';
	for (int i = 1; i < prefix_value.size(); i++) {
		if (prefix_value[i] == 0) {
			while (last_success > -1) {
				used[answer[last_success + 1] - 'a'] = 1;
				last_success = prefix_value[last_success] - 1;
			}
			int j = 0;
			while (j < used.size()) {
				if (used[j] == 0)
					break;
				j++;
			}
			base_char = 'a' + j;
			answer += base_char;
			last_success = -1;
			used.reset();
			used.set(0);
		}
		else {
			last_success = prefix_value[i] - 1;
			answer += answer[last_success];
		}
	}
}


int main() {
	int a;
	std::vector<int> prefix_value;

	while (std::cin >> a)
		prefix_value.push_back(a);
	if (prefix_value.size() != 0) {
		std::string answer;
		reverse_pre_function(prefix_value, answer);
		std::cout << answer;
	}

	return 0;
}

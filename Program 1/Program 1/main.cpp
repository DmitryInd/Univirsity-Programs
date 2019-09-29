#include <iostream>
#include <string>
#include <vector>



int pre_function(const std::string& pattern, std::vector<int>& prefix_value,
					const char now, const int now_array_position,
					int previous_array_position) { //array - массив значений префикс функции
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


int main() {
	std::ios::sync_with_stdio(false);
	std::cin.tie(nullptr);

	std::string pattern;
	char now;
	int i = 0;

	std::cin >> pattern;
	std::vector<int> prefix_value(pattern.size() + 1);

	for (; i < pattern.size(); i++)
		pre_function(pattern, prefix_value, pattern[i], i, i - 1);
	prefix_value[pattern.size()] = -1;
	
	while (std::cin >> now) {
		if (pre_function(pattern, prefix_value, now,
						pattern.size(), pattern.size()) == pattern.size() - 1) {
			int answer = i - 2 * pattern.size() + 1;
			std::cout << answer << " ";
		}
		i++;
	}

	return 0;
}

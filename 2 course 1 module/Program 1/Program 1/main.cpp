#include <iostream>
#include <string.h>
#include <vector>

int pre_function(const std::string& p, int* pi, int i, char& now) {
	int k = p.size();
	if (i < k) {
		now = p[i];
		k = i;
	}
	int l = k < p.size() ? k - 1 : k;
	while (true) {
		if (l < 0) {
			pi[k] = -1;
			break;
		}
		l = pi[l];
		if (p[l + 1] == now) {
			pi[k] = l + 1;
			break;
		}
	}
	return pi[k];
}

int main() {
	std::string p;
	char now = ' ';
	std::cin >> p;
	int* pi = new int[p.size() + 1];
	int i = 0;
	for (i = 0; i < p.size(); i++)
		pre_function(p, pi, i, now);
	pi[p.size()] = -1;
	std::vector<int> answer;
	std::cin >> now;
	while (now != '\n') {
		if (pre_function(p, pi, i, now) == p.size() - 1)
			answer.push_back(i - 2 * p.size() + 1);
		now = char(std::cin.get());
		i++;
	}
	for (int os : answer)
		std::cout << os << " ";
	delete[] pi;
	return 0;
}
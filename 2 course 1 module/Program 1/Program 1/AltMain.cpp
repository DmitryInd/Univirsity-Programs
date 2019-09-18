/*#include <iostream>
#include <vector>
#include <string>

std::vector<int> PrefixFun(std::string& str) {
	std::vector<int> pfv(str.size());
	pfv[0] = 0;
	int k = 0;

	for (int i = 1; i < str.size(); ++i) {
		k = pfv[i - 1];
		while (k > 0 && str[i] != str[k])
			k = pfv[k - 1];
		if (str[i] == str[k])
			++k;
		pfv[i] = k;
	}

	return pfv;
}

int main() {
	std::string pat;
	char s;
	std::cin >> pat;// >> str;
	std::vector<int> ans;
	std::vector<int> pat_pfv = PrefixFun(pat);
	int pat_size = pat.size();
	int pfv = 0;
	int i = 0;
	while (std::cin >> s) {
		while (pfv > 0 && s != pat[pfv])
			pfv = pat_pfv[pfv - 1];
		if (s == pat[pfv])
			++pfv;
		if (pfv == pat_size)
			ans.push_back(i + 1 - pat_size);
		++i;
	}
	//std::vector<int> ans = Occurrences(str, pat);
	for (auto x : ans)
		std::cout << x << " ";
	return 0;
}*/
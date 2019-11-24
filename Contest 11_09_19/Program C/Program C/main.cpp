#include <iostream>
#include <string>
#include <queue>
#include <memory>
#include <utility>
#include <unordered_map>
#include <vector>



//Реализация бора
class pattern_tree {
public:
	pattern_tree();

	friend void search_pattern(const pattern_tree& bor, const std::string &input);
	friend std::vector<int> number_of_substr(const pattern_tree& bor, const std::string& input);

private:
	struct node; // forward-декларация для IntelliSense
	struct node {
		char last_char = '\0'; //Буква, по которой перешли в данный узел
		node *parent; //Указатель на родительский узел
		std::unordered_map<char, std::unique_ptr<node>> edges; //Хеш-таблица ребёр из узла (буква ребра, указатель на узёл)
		node *suffix_link; //Суффиксная ссылка из узла
		node *c_suffix_link; //Cжатая суффиксная ссылка из узла
		std::vector<int> terminal; //Является ли ссылка терминальной, если да, то где начинается в строке
	};

	std::unique_ptr<node> root; //Корневой узел
	int num_substr = 0; //Количество паттернов (подстрок, на которые делится строка с шаблонами)
	int size_str = 0; //Общий размер строки

	void build_tree();
	void build_links();
	void build_suffix_link(node *now);
	void build_compressed_suffix_link(node *now);
};


pattern_tree::pattern_tree() {
	root.reset(new node);
	build_tree();
	build_links();
}


//Строит базовое дерево бора
void pattern_tree::build_tree() {
	//Объявление переменных
	char pattern_char = ' '; //Сюда посимвольно считывается паттерн
	int length_substr = 0; //Длина подстроки
	node *now = root.get(); //Узел, в котором сейчас находимся
	//Считывание
	std::string input; //Строка для ввода
	std::cin >> input;
	for (int i = 0; i <= input.size(); i++) {
		pattern_char = input[i];
		if (pattern_char == '?' || i == input.size()) {
			if (length_substr != 0) {
				length_substr = 0;
				now->terminal.push_back(size_str); //Так как пришёл конец бесшаблонной подстроки, то узел терминальный
				now = root.get();
				num_substr++;
			}
			if (i == input.size())
				break;
		}
		else {
			if (now->edges.find(pattern_char) == now->edges.end()) {
				now->edges[pattern_char] = std::move(std::unique_ptr<node>(new node));
				now->edges[pattern_char]->parent = now;
				now->edges[pattern_char]->last_char = pattern_char;
			}
			now = now->edges[pattern_char].get();
			length_substr++;
		}
		size_str++;
	}
}


//Строит суффиксальные ссылки
void pattern_tree::build_links() {
	root->suffix_link = root.get();
	root->c_suffix_link = root.get(); //root будет аналогом пустой сокращённой ссылки
	std::queue<node *> qu; //Очередь обхода
	node *now; //Узел, в котором сейчас находимся
	qu.push(root.get());
	while (!qu.empty()) {
		now = qu.front();
		qu.pop();
		for (const auto& next : now->edges)
			qu.push(next.second.get());

		if (now == root.get())
			continue;

		build_suffix_link(now);
		build_compressed_suffix_link(now);
	}
}


//Строит суффиксную ссылку для узла now
void pattern_tree::build_suffix_link(node *now) {
	//Суффиксная ссылка родителя
	node *previous_parent = now->parent;
	while (true) {
		node* parent_suffix_link = previous_parent->suffix_link;
		/*Проверка на то, что мы не попали в бесконечный цикл
		(Иными словами, дошли до корневого узла)*/
		if (parent_suffix_link == previous_parent) {
			now->suffix_link = root.get();
			break;
		}
		
		//Итератор предполагаемой суффиксальной ссылки
		auto iterator = parent_suffix_link->edges.find(now->last_char);
		//Проверка сущестования суффиксальной ссылки
		if (iterator != parent_suffix_link->edges.end()) {
			now->suffix_link = (*iterator).second.get();
			break;
		}
		previous_parent = parent_suffix_link;
	}
}


//Строит сокращённую суффиксальную ссылку для узла now
void pattern_tree::build_compressed_suffix_link(node *now) {
	//Проверка на терминальность 
	if (now->suffix_link == root.get())
		now->c_suffix_link = root.get();
	else if (now->suffix_link->terminal.size() != 0)
		now->c_suffix_link = now->suffix_link;
	else now->c_suffix_link = now->suffix_link->c_suffix_link;
}


//Основная функция поиска паттерна с шаблонами
void search_pattern(const pattern_tree &bor, const std::string& input) {
	//Кол-во входящих подстрок паттерна, начинающегося с индекса i
	std::vector<int> num = number_of_substr(bor, input);

	for (int i = 0; i + bor.size_str <= num.size(); i++)
		if (num[i] == bor.num_substr)
			std::cout << i << " ";
}


//Функция, заполняющая вектор num
std::vector<int> number_of_substr(const pattern_tree& bor, const std::string& input) {
	std::vector<int> num;
	//char chr; //Символ, который мы считали
	int index = 0; //Индес символа, который мы считали
	pattern_tree::node *now = bor.root.get(); //Узел, в котором сейчас находимся
	for (char chr: input) {
		num.push_back(0);
		//Итератор, указывающий на узел, в который мы переходим
		auto iterator = now->edges.find(chr);
		while (now != bor.root.get()) {
			if (iterator == now->edges.end()) {
				now = now->suffix_link;
				iterator = now->edges.find(chr);
			}
			else break;
		}

		if (iterator != now->edges.end())
			now = (*iterator).second.get();

		//Узел, в котором сейчас находимся при проходе по сжатым циклам
		pattern_tree::node*c_now = now;
		while (c_now != bor.root.get()) {
			for (int i = 0; i < c_now->terminal.size(); i++)
				if (index - c_now->terminal[i] > -2)
					num[index - c_now->terminal[i] + 1]++;
			c_now = c_now->c_suffix_link;
		}
		index++;
	}

	return num;
}


int main() {
	//Ускорение ввода
	std::ios::sync_with_stdio(false);
	std::cin.tie(nullptr);
	//Построение бора
	pattern_tree bor;
	//Ввод
	std::string input;
	std::cin >> input;
	//Поиск вхождений паттерна и вывод индексов
	search_pattern(bor, input);
	return 0;
}

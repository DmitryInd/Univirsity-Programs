#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>

constexpr int ascii_size = 28;


class vertex {
public:
	std::vector<vertex*> get_children() const; //Список указателей на дочерние вершины
	std::pair<int, int> get_info(int str_size) const; //Информация о ребре, которое ведёт в вершину

private:
	int left_border = 0; //Левая граница ребра, который ведёт к данной вершине
	int right_border = 0; //Правая граница ребра, который ведёт к данной вершине

	 //Хеш-таблица с указателями на дочерние вершины, которые связаны с перым символом ребра
	std::vector<vertex*> children = std::vector<vertex*>(ascii_size, nullptr);
	//Указатель на родителя
	vertex* parent = this;
	//Суффиксная ссылка
	vertex* suf_ptr = this;

	friend class suf_tree;
};


//Список указателей на дочерние вершины
std::vector<vertex*> vertex::get_children() const
{
	std::vector<vertex*> values;
	for (int i = 0; i < ascii_size; i++) {
		if (children[i] != nullptr) {
			values.push_back(children[i]);
		}
	}

	return values;
}


//Информация о ребре, которое ведёт в вершину
std::pair<int, int> vertex::get_info(int str_size) const
{
	return { left_border, right_border == -1 ? str_size : right_border };
}


//Класс суффиксного дерева
class suf_tree {
public:
	suf_tree(const std::string& input);
	~suf_tree();
	void delete_dfs(const vertex* node);

	vertex* root;

private:
	std::string str; //Строка, на которой основано суффиксное дерево

	int get_right_border(int board, int now) { return board == -1 ? (now + 1) : board; }
	void suf_step(std::pair<vertex*, int>& now, int i); //Переход по суффиксной ссылке
	void new_edge(std::pair<vertex*, int>& now, int i); //Создание нового ответвления c одним символом
	int get_index(char input);
};


//Группировка символов для экономии памяти
int suf_tree::get_index(char input) {
	return input - (input <= '$' ? '#' : ('a' - 2));
}


suf_tree::~suf_tree()
{
	delete_dfs(root);
}

//Для поветочного удаления
void suf_tree::delete_dfs(const vertex* node) {
	std::vector<vertex*> values = node->children;
	for (int i = 0; i < values.size(); i++) {
		if (values[i] != nullptr) {
			delete_dfs(values[i]);
			delete values[i];
		}
	}
}

suf_tree::suf_tree(const std::string& input) {
	str = input;
	root = new vertex;
	std::pair<vertex*, int> now = { root, 0 }; //Вершина и индекс, на которых остановились
	int remainder = 0; //Количество оставшихся суффиксов, которым нужны листья

	for (int i = 0; i < str.size(); i++) {
		vertex* from = nullptr;
		remainder++;
		while (true) {
			if (now.second == get_right_border(now.first->right_border, i)) {
				from = nullptr;
				vertex* next = now.first->children[get_index(str[i])]; //Указаетель на следующую вершину
				if (next == nullptr) {
					remainder--;
					new_edge(now, i);
				}
				else {
					now = { next, next->left_border + 1 };
					break;
				}
			}
			else {
				if (str[now.second] == str[i]) {
					from = nullptr;
					now.second = now.second + 1;
					break;
				}
				else {
					vertex* left_node = new vertex;
					*left_node = *now.first;
					now.first->parent->children[get_index(str[now.first->left_border])] = left_node;
					now.first->left_border = now.second;
					now.first->parent = left_node;
					left_node->right_border = now.second;
					left_node->children = std::vector<vertex*>(ascii_size, nullptr);
					left_node->children[get_index(str[now.second])] = now.first;
					now.first = left_node;
					if (from != nullptr) {
						from->suf_ptr = now.first;
					}
					from = now.first;
					remainder--;
					new_edge(now, i);
				}
			}
			//Проверка на то, что остались суффиксы без листьев
			if (remainder == 0) {
				break;
			}
		}
	}
}


//Создание нового ответвления c одним символом
void suf_tree::new_edge(std::pair<vertex*, int>& now, int i)
{
	now.first->children[get_index(str[i])] = new vertex;
	vertex* next = now.first->children[get_index(str[i])];
	next->parent = now.first;
	next->left_border = i;
	next->right_border = -1;
	suf_step(now, i);
}


//Переход по суффиксной ссылке
void suf_tree::suf_step(std::pair<vertex*, int>& now, int i)
{
	if (now.first == root) {
		return;
	}
	int left_border = now.first->left_border;
	int length = now.second - now.first->left_border;
	std::pair<vertex*, int> to = { nullptr, 0 }; //Суффиксная ссылка и новое now
	if (now.first->parent == root && now.second - left_border <= 1) {
		to = { root, 0 };
		length--;
	}
	else {
		if (now.first->parent == root) {
			left_border++;
			length--;
		}
		//Переходим с помомощью суффиксной ссылки родителя
		to.first = now.first->parent->suf_ptr->children[get_index(str[left_border])];
		to.second = to.first->left_border + length;
	}

	//Поднимаемся по дереву до своего индекса (до суффиксной вершины)
	int new_length = get_right_border(to.first->right_border, i) - to.first->left_border;
	while (length > new_length) {
		left_border += new_length;
		length -= new_length;
		to.first = to.first->children[get_index(str[left_border])];
		to.second = to.first->left_border + length;
		new_length = get_right_border(to.first->right_border, i) - to.first->left_border;
	}

	now.first->suf_ptr = to.first;
	now = to;
}


int dfs(const vertex* node, int sum_size, int s_size, int& n, std::vector<std::vector<int>>& answer) {
	std::vector<vertex*> values = node->get_children();
	int start = n;
	answer.push_back(std::vector<int>(4, 0));
	for (int i = 0; i < values.size(); i++) {
		n++;
		int number = dfs(values[i], sum_size, s_size, n, answer);
		std::pair<int, int> borders = values[i]->get_info(sum_size);
		bool w = 0;
		if (borders.first < s_size) {
			borders.second = borders.second < s_size ? borders.second : s_size;
		}
		else {
			w = 1;
			borders.first -= s_size;
			borders.second -= s_size;
		}
		answer[number] = { start, w, borders.first, borders.second };
	}
	return start;
}

int main() {
	std::string s, t = "";
	std::cin >> s;
	std::cin >> t;
	s += t;
	suf_tree bor(s);

	int start = 0;
	std::vector<std::vector<int>> answer;
	dfs(bor.root, s.size(), s.size() - t.size(), start, answer);

	std::cout << answer.size() << std::endl;
	for (int i = 1; i < answer.size(); i++) {
		printf("%d%c%d%c%d%c%d\n", answer[i][0], ' ', answer[i][1], ' ', answer[i][2], ' ', answer[i][3]);
	}
	return 0;
}
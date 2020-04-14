#include<stdio.h>
#include<iostream>
#include<vector>
#include<array>
#include<cfloat>
#include<math.h>
#include<algorithm>

constexpr double base_angle = 0.000001;



//Точка(вектор) в пространстве
struct point {
	double x = DBL_MAX;
	double y = DBL_MAX;
	double z = DBL_MAX;

	void rotate(double angle);

	point& operator=(const point& other) = default;
	point operator-(const point& other) const
	{
		return point{ x - other.x, y - other.y, z - other.z };
	}
	point operator+(const point& other) const
	{
		return point{ x + other.x, y + other.y, z + other.z };
	}
};

struct node_hull {
	point coord;
	node_hull* prev = nullptr;
	node_hull* next = nullptr;

	node_hull& operator=(const node_hull& other) = default;
	int index = -1;

	void step();
};

//Попадание в оболочку и выход из неё
void node_hull::step() {
	if (prev->next == this) {
		prev->next = next;
		next->prev = prev;
	}
	else {
		next->prev = this;
		prev->next = this;
	}
}

/*Поворот радиус вектора точки на заданный угл вокруг всех осей для
выполнения условий корректной работы алгоритма Чана.
(Никакие три точки не должны лежать в одной вертикальной плоскости)*/
void point::rotate(double angle) {
	double x_next, y_next, z_next;
	//Поворот вокруг оси x
	z_next = z * cos(angle) + y * sin(angle);
	y_next = -z * sin(angle) + y * cos(angle);
	z = z_next;
	y = y_next;
	//Поворот вокруг оси y
	x_next = x * cos(angle) + z * sin(angle);
	z_next = -x * sin(angle) + z * cos(angle);
	x = x_next;
	z = z_next;

	//Поворот вокруг оси z
	x_next = x * cos(angle) + y * sin(angle);
	y_next = -x * sin(angle) + y * cos(angle);
	x = x_next;
	y = y_next;
}

//Компоратор для сортировки по оси x
struct compare_by_x {
	bool operator()(const node_hull& first, const node_hull& second) {
		return first.coord.x < second.coord.x;
	}
};

//Грань, состоящая из трёх точек
struct face {
	node_hull* first_p;
	node_hull* second_p;
	node_hull* third_p;

	void index_order();
};

//Выстраивание порядка точек по индексам
void face::index_order() {
	if (second_p->index < first_p->index && second_p->index < third_p->index) {
		node_hull* time_p = first_p;
		first_p = second_p;
		second_p = third_p;
		third_p = time_p;
	}
	else if (third_p->index < first_p->index && third_p->index < second_p->index) {
		node_hull* time_p = third_p;
		third_p = second_p;
		second_p = first_p;
		first_p = time_p;
	}
}

//Компоратор для сортировки в лексикографическом порядке
struct lexic_comparator {
	bool operator()(const face& first, const face& second) {
		if (first.first_p->index >= second.first_p->index) {
			if (second.first_p->index == first.first_p->index && first.second_p->index <= second.second_p->index) {
				if (second.second_p->index == first.second_p->index && first.third_p->index >= second.third_p->index) {
					return false;
				}
				return true;
			}
			return false;
		}
		return true;
	}
};


//Проверка на существование/фиктивность точки
bool is_point_exist(const node_hull* point_ptr) {
	return (point_ptr != nullptr && point_ptr->index != -1);
}

/*Векторное произведение двух векторов, определённых тремя точками, в проекции на плоскость xy
или знак векторного произведения в проекции Чана при t = -INF*/
double initial_vector_product(const node_hull* first_p, const node_hull* second_p, const node_hull* third_p) {
	if (!(is_point_exist(first_p) && is_point_exist(second_p) && is_point_exist(third_p))) {
		return 1;
	}
	point first_vector = second_p->coord - first_p->coord;
	point second_vector = third_p->coord - first_p->coord;
	return first_vector.x * second_vector.y - second_vector.x * first_vector.y;
}

/*Векторное произведение двух векторов в проекции на плоскость xy
или знак векторного произведения в проекции Чана при t = -INF*/
double initial_vector_product(const point* first_vector, const point* second_vector) {
	return first_vector->x * second_vector->y - second_vector->x * first_vector->y;
}

//Поиск момента, в который вторая точка попадёт в оболочку
double search_intersection_time(const node_hull* first_p, const node_hull* second_p, const node_hull* third_p) {
	if (!(is_point_exist(first_p) && is_point_exist(second_p) && is_point_exist(third_p))) {
		return DBL_MAX;
	}
	point first_vector = second_p->coord - first_p->coord;
	point second_vector = third_p->coord - first_p->coord;
	return (first_vector.x * second_vector.z - second_vector.x * first_vector.z)
            /initial_vector_product(&first_vector, &second_vector);
}

//Поиск нижней части оболочки
std::vector<node_hull*> find_down_hull(std::vector<node_hull> &vertices,  int size, node_hull* zero, int begin = 0) {
	if (size <= 1) {
		std::vector<node_hull*> entire_hull;
		entire_hull.push_back(zero);
		vertices[begin].next = zero;
		vertices[begin].prev = zero;
		return entire_hull;
	}  

	//Точки, состовляющие мост
	node_hull* u = &vertices[begin + size / 2 - 1];
	node_hull* v = &vertices[begin + size / 2];
	node_hull* midle = v;
	//Разбиение оболочки на две части
	std::vector<node_hull*> first_half_hull = find_down_hull(vertices, size / 2, zero, begin);
	std::vector<node_hull*> second_half_hull = find_down_hull(vertices, size - size / 2, zero, begin + size / 2);
	std::vector<node_hull*> entire_hull;

	//Поиск моста при t = -INF
	while (true) {
		if (initial_vector_product(u, v, v->next) < 0) {
			v = v->next;
		}
		else if (initial_vector_product(u->prev, u, v) < 0) {
			u = u->prev;
		}
		else {
			break;
		}
	}

	double first_time = -DBL_MAX;
	double second_time;
	std::array<double, 6> certain_time;
	//Слияние
	for (int i = 0, j = 0; ; first_time = second_time) {
		certain_time[0] = search_intersection_time(first_half_hull[i]->prev, first_half_hull[i], first_half_hull[i]->next);
		certain_time[1] = search_intersection_time(second_half_hull[j]->prev, second_half_hull[j], second_half_hull[j]->next);
		certain_time[2] = search_intersection_time(u, u->next, v);
		certain_time[3] = search_intersection_time(u->prev, u, v);
		certain_time[4] = search_intersection_time(u, v, v->next);
		certain_time[5] = search_intersection_time(u, v->prev, v);
		//Поиск ближайшего события
		second_time = DBL_MAX;
		int event_number = -1;
		for (int h = 0; h < 6; h++) {
			if (certain_time[h] > first_time && certain_time[h] < second_time) {
				event_number = h;
				second_time = certain_time[h];
			}
		}
		if (event_number == -1) {
			break;
		}
		else if (event_number == 0) {
			if (first_half_hull[i]->coord.x < u->coord.x) {
				entire_hull.push_back(first_half_hull[i]);
			}
			first_half_hull[i++]->step();
		}
		else if (event_number == 1) {
			if (second_half_hull[j]->coord.x > v->coord.x) {
				entire_hull.push_back(second_half_hull[j]);
			}
			second_half_hull[j++]->step();
		}
		else if (event_number == 2) {
			u = u->next;
			entire_hull.push_back(u);
		}
		else if (event_number == 3) {
			entire_hull.push_back(u);
			u = u->prev;
		}
		else if (event_number == 4) {
			entire_hull.push_back(v);
			v = v->next;
		}
		else if (event_number == 5) {
			v = v->prev;
			entire_hull.push_back(v);
		}
	}
	entire_hull.push_back(zero);

	//Создание моста и возврат к t = -INF с обновлением связей
	u->next = v;
	v->prev = u;
	for (int real_size = entire_hull.size() - 2; real_size >= 0; real_size--) {
		if (entire_hull[real_size]->coord.x <= u->coord.x || entire_hull[real_size]->coord.x >= v->coord.x) {
			entire_hull[real_size]->step();
			u = entire_hull[real_size] == u ? u->prev : u;
			v = entire_hull[real_size] == v ? v->next : v;
		} else {
			u->next = entire_hull[real_size];
			entire_hull[real_size]->prev = u;
			v->prev = entire_hull[real_size];
			entire_hull[real_size]->next = v;
			if (entire_hull[real_size]->coord.x < midle->coord.x) {
				u = entire_hull[real_size];
			}
			else {
				v = entire_hull[real_size];
			}
		}
	}
	return entire_hull;
}

//Поиск внешней оболочки
std::vector<face> find_convex_hull(std::vector<node_hull> &vertices, int size, node_hull* zero) {
	std::vector<face> answer;
	compare_by_x point_comp;
	std::sort(vertices.begin(), vertices.end(), point_comp);
	std::vector<node_hull*> half_hull = find_down_hull(vertices, size, zero);

	int k = 0;
	int answer_size = 0;
	while (half_hull[k] != zero) {
		if (half_hull[k]->prev->next != half_hull[k]) {
			answer.push_back(face{ half_hull[k]->prev, half_hull[k], half_hull[k]->next });
		}
		else {
			answer.push_back(face{ half_hull[k], half_hull[k]->prev, half_hull[k]->next });
		}
		half_hull[k++]->step();
		answer[answer_size].index_order();
		answer_size++;
	}

	for (int i = 0; i < vertices.size(); i++) {
		vertices[i].coord.z = -vertices[i].coord.z;
	}
	zero->prev = nullptr;
	zero->next = nullptr;

	half_hull = find_down_hull(vertices, size, zero);

	k = 0;
	while (half_hull[k] != zero) {
		if (half_hull[k]->prev->next == half_hull[k]) {
			answer.push_back(face{ half_hull[k]->prev, half_hull[k], half_hull[k]->next });
		}
		else {
			answer.push_back(face{ half_hull[k], half_hull[k]->prev, half_hull[k]->next });
		}
		half_hull[k++]->step();
		answer[answer_size].index_order();
		answer_size++;
	}
	zero->prev = nullptr;
	zero->next = nullptr;

	lexic_comparator face_comp;
	std::sort(answer.begin(), answer.end(), face_comp);
	return answer;
}

int main() {
	node_hull zero;
	int m = 0;
	std::cin >> m;
	for (int j = 0; j < m; j++) {
		int n = 0;
		std::cin >> n;
		std::vector<node_hull> vertices(n);
		for (int i = 0; i < n; i++) {
			std::cin >> vertices[i].coord.x >> vertices[i].coord.y >> vertices[i].coord.z;
			vertices[i].coord.rotate(base_angle);
			vertices[i].index = i;
		}

		std::vector<face> answer = find_convex_hull(vertices, n, &zero);
		std::cout << answer.size() << std::endl;
		for (int i = 0; i < answer.size(); i++) {
			std::cout << 3 << " " << answer[i].first_p->index << " " << answer[i].second_p->index << " " << answer[i].third_p->index << std::endl;
		}
	}

	return 0;
}

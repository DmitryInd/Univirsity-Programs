#include<stdio.h>
#include<vector>
#include<array>
#include<cfloat>
#include<math.h>
#include <algorithm>

constexpr double base_angle = 0.000001;



//Точка(вектор) в пространстве
struct point {
	double x = DBL_MAX;
	double y = DBL_MAX;
	double z = DBL_MAX;
	point* prev = nullptr;
	point* next = nullptr;

	int index = -1;

	point(double a, double b, double c) { x = a; y = b; z = c; }
	point() = default;
	void step();
	void rotate(double angle);

	bool operator<(const point& other) const { return x < other.x; };
	point& operator=(const point& other) = default;
	point operator-(const point& other) const
	{
		return point(x - other.x, y - other.y, z - other.z);
	}
	point operator+(const point& other) const
	{
		return point(x + other.x, y + other.y, z + other.z);
	}
	point operator*(double k) const
	{
		return point(x * k, y * k, z * k);
	}
	point operator/(double k) const
	{
		return point(x / k, y / k, z / k);
	}
};

//Попадание в оболочку и выход из неё
void point::step() {
	if (prev->next == this) {
		prev->next = next;
		next->prev = prev;
	}
	else {
		next->prev = this;
		prev->next = this;
	}
}

//Поворот радиус вектора точки на заданный угл
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

//Грань (состоящая из трёх точек)
struct face {
	point* first_p;
	point* second_p;
	point* third_p;

	face(point* a, point* b, point* c) { first_p = a; second_p = b; third_p = c; }
	void index_order();
	bool operator<(const face& other);
};

//Выстраивание порядка точек по индексам
void face::index_order() {
	if (second_p->index < first_p->index && second_p->index < third_p->index) {
		point* time_p = first_p;
		first_p = second_p;
		second_p = third_p;
		third_p = time_p;
	}
	else if (third_p->index < first_p->index && third_p->index < second_p->index) {
		point* time_p = third_p;
		third_p = second_p;
		second_p = first_p;
		first_p = time_p;
	}
}

//Лексикографический порядок
bool face::operator<(const face& other) {
	if (first_p->index >= other.first_p->index) {
		if (other.first_p->index == first_p->index && second_p->index <= other.second_p->index) {
			if (other.second_p->index == second_p->index && third_p->index >= other.third_p->index) {
				return false;
			}
			return true;
		}
		return false;
	}
	return true;
}

/*Векторное произведение двух векторов, определённых тремя точками, в проекции на плоскость xy
или знак векторного произведения в проекции Чана при t = -INF*/
double initial_vector_product(const point* first_p, const point* second_p, const point* third_p) {
	if (first_p == nullptr || second_p == nullptr || third_p == nullptr || 
        first_p->index == -1 || second_p->index == -1 || third_p->index == -1) {
		return 1;
	}
	point first_vector = *second_p - *first_p; 
	point second_vector = *third_p - *first_p;
	return first_vector.x * second_vector.y - second_vector.x * first_vector.y;
}

/*Векторное произведение двух векторов в проекции на плоскость xy
или знак векторного произведения в проекции Чана при t = -INF*/
double initial_vector_product(const point* first_vector, const point* second_vector) {
	return first_vector->x * second_vector->y - second_vector->x * first_vector->y;
}

//Поиск момента, в который вторая точка попадёт в оболочку
double search_intersection_time(const point* first_p, const point* second_p, const point* third_p) {
	if (first_p == nullptr || second_p == nullptr || third_p == nullptr ||
        first_p->index == -1 || second_p->index == -1 || third_p->index == -1) {
		return DBL_MAX;
	}
	point first_vector = *second_p - *first_p;
	point second_vector = *third_p - *first_p;
	return (first_vector.x * second_vector.z - second_vector.x * first_vector.z)
            /initial_vector_product(&first_vector, &second_vector);
}

//Поиск нижней части оболочки
std::vector<point*> search_down_hull(std::vector<point> &vertices,  int size, point* zero, int begin = 0) {
	if (size <= 1) {
		std::vector<point*> entire_hull;
		entire_hull.push_back(zero);
		vertices[begin].next = zero;
		vertices[begin].prev = zero;
		return entire_hull;
	}  

	//Точки, состовляющие мост
	point* u;
	point* v;
	u = &vertices[begin + size / 2 - 1];
	v = &vertices[begin + size / 2];
	point* midle = v;
	//Разбиение оболочки на две части
	std::vector<point*> first_half_hull = search_down_hull(vertices, size / 2, zero, begin);
	std::vector<point*> second_half_hull = search_down_hull(vertices, size - size / 2, zero, begin + size / 2);
	std::vector<point*> entire_hull;

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
			if (first_half_hull[i]->x < u->x) {
				entire_hull.push_back(first_half_hull[i]);
			}
			first_half_hull[i++]->step();
		}
		else if (event_number == 1) {
			if (second_half_hull[j]->x > v->x) {
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
	int real_size = entire_hull.size() - 2;
	u->next = v;
	v->prev = u;
	for (; real_size >= 0; real_size--) {
		if (entire_hull[real_size]->x <= u->x || entire_hull[real_size]->x >= v->x) {
			entire_hull[real_size]->step();
			u = entire_hull[real_size] == u ? u->prev : u;
			v = entire_hull[real_size] == v ? v->next : v;
		} else {
			u->next = entire_hull[real_size];
			entire_hull[real_size]->prev = u;
			v->prev = entire_hull[real_size];
			entire_hull[real_size]->next = v;
			if (entire_hull[real_size]->x < midle->x) {
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
std::vector<face> search_convex_hull(std::vector<point> &vertices, int size, point* zero) {
	std::vector<face> answer;
	std::sort(vertices.begin(), vertices.end());
	std::vector<point*> half_hull = search_down_hull(vertices, size, zero);

	int k = 0;
	int answer_size = 0;
	while (half_hull[k] != zero) {
		if (half_hull[k]->prev->next != half_hull[k]) {
			answer.push_back(face(half_hull[k]->prev, half_hull[k], half_hull[k]->next));
		}
		else {
			answer.push_back(face(half_hull[k], half_hull[k]->prev, half_hull[k]->next));
		}
		half_hull[k++]->step();
		answer[answer_size].index_order();
		answer_size++;
	}

	for (int i = 0; i < vertices.size(); i++) {
		vertices[i].z = -vertices[i].z;
	}
	zero->prev = nullptr;
	zero->next = nullptr;

	half_hull = search_down_hull(vertices, size, zero);

	k = 0;
	while (half_hull[k] != zero) {
		if (half_hull[k]->prev->next == half_hull[k]) {
			answer.push_back(face(half_hull[k]->prev, half_hull[k], half_hull[k]->next));
		}
		else {
			answer.push_back(face(half_hull[k], half_hull[k]->prev, half_hull[k]->next));
		}
		half_hull[k++]->step();
		answer[answer_size].index_order();
		answer_size++;
	}
	zero->prev = nullptr;
	zero->next = nullptr;

	std::sort(answer.begin(), answer.end());
	return answer;
}

int main() {
	point zero;
	int m = 0;
	std::cin >> m;
	for (int j = 0; j < m; j++) {
		int n = 0;
		std::cin >> n;
		std::vector<point> vertices(n);
		for (int i = 0; i < n; i++) {
			std::cin >> vertices[i].x >> vertices[i].y >> vertices[i].z;
			vertices[i].rotate(base_angle);
			vertices[i].index = i;
		}

		std::vector<face> answer = search_convex_hull(vertices, n, &zero);
		std::cout << answer.size() << std::endl;
		for (int i = 0; i < answer.size(); i++) {
			std::cout << 3 << " " << answer[i].first_p->index << " " << answer[i].second_p->index << " " << answer[i].third_p->index << std::endl;
		}
	}

	return 0;
}

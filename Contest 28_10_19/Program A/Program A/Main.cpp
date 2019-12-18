#include <iostream>
#include <vector>
#include <math.h>

const double epsilon = pow(10, -9);



//Точка(вектор) в пространстве
struct point {
	double x = 0;
	double y = 0;
	double z = 0;

	point& operator=(const point& other) = default;
	point operator-(const point& other) const
	{
		return point{ x - other.x, y - other.y, z - other.z };
	}
	point operator+(const point& other) const
	{
		return point{ x + other.x, y + other.y, z + other.z };
	}
	//Скалярное деление вектора на переменную
	point operator/(double k) const
	{
		return point{ x / k, y / k, z/k};
	}
};

//Расстояние между точками
double distance(const point& first, const point& second) {
	return sqrt(pow(first.x - second.x, 2) + pow(first.y - second.y, 2) + pow(first.z - second.z, 2));
}

//Поиск минимального расстояния между отрезком, заданным парой точек, и объектом
template<typename T, typename F>
double search_min_distance(const T& from, const std::pair<point, point> &line, F (find_distance)(const point&, const T&)) {
	point first_to = line.first;
	point second_to = line.second;
	while (distance(first_to, second_to) > epsilon) {
		point vec_of_line = (second_to - first_to) / 3; //Вектор отрезка
		point first = first_to + vec_of_line; //1/3 отрезка
		point second = second_to - vec_of_line; //2/3 отрезка

		double first_distance = find_distance(first, from);
		double second_distance = find_distance(second, from);
		if (first_distance < second_distance) {
			second_to = second;
		}
		else {
			first_to = first;
		}
	}

	return find_distance(first_to, from);
}

//Поиск расстояния между точкой и прямой
double search_distance_line_point(const point& from, const std::pair<point, point>& line) {
	return search_min_distance(from, line, distance);
}

int main() {
	std::vector<point> vertices(4);
	for (int i = 0; i < 4; i++) {
		std::cin >> vertices[i].x >> vertices[i].y >> vertices[i].z;
	}

	printf("%.8lf", search_min_distance({ vertices[0], vertices[1]}, { vertices[2], vertices[3] }, search_distance_line_point));
	return 0;
}

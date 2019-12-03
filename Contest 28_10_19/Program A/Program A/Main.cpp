#include <iostream>
#include <vector>
#include <math.h>

const double epsilon = pow(10, -8);



//Точка(вектор) в пространстве
struct point {
	double x = 0;
	double y = 0;
	double z = 0;

	point(double a, double b, double c) { x = a; y = b; z = c; }
	point() = default;

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

//Расстояние между точками
double distance(const point& first, const point& second) {
	return sqrt(pow(first.x - second.x, 2) + pow(first.y - second.y, 2) + pow(first.z - second.z, 2));
}

//Поиск минимального расстояния между точкой и отрезком
double search_min_distance(const point& from, std::vector<point>& vertices) {
	point first_to = vertices[2];
	point second_to = vertices[3];
	while (distance(first_to, second_to) > epsilon) {
		point vec_of_line = (second_to - first_to) / 3; //Вектор отрезка
		point first = first_to + vec_of_line; //1/3 отрезка
		point second = second_to - vec_of_line; //2/3 отрезка

		double first_distance = distance(first, from);
		double second_distance = distance(second, from);
		if (first_distance < second_distance) {
			second_to = second;
		}
		else {
			first_to = first;
		}
	}

	return distance(first_to, from);
}

//Рассточние между отрезками
double search_distance_line(std::vector<point>& vertices) {
	while (distance(vertices[0], vertices[1]) > epsilon) {
		point vec_of_line = (vertices[1] - vertices[0])/3; //Вектор отрезка
		point first = vertices[0] + vec_of_line; //1/3 отрезка
		point second = vertices[1] - vec_of_line; //2/3 отрезка

		double first_distance = search_min_distance(first, vertices);
		double second_distance = search_min_distance(second, vertices);
		if (first_distance < second_distance) {
			vertices[1] = second;
		}
		else {
			vertices[0] = first;
		}
	}

	return search_min_distance(vertices[0], vertices);
}

int main() {
	std::vector<point> vertices(4);
	for (int i = 0; i < 4; i++) {
		std::cin >> vertices[i].x >> vertices[i].y >> vertices[i].z;
	}

	printf("%.8lf", search_distance_line(vertices));
	return 0;
}
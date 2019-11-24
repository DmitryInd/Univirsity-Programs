#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>
#include <complex>
#include <vector>



struct point {
	double x = 0;
	double y = 0;

	point(double a, double b) { x = a; y = b; }
	point(const point& other) = default;
	point() = default;

	point& operator=(const point& other) = default;
	point operator-(const point& other) const
	{
		return point(x - other.x, y - other.y);
	}
	point operator+(const point& other) const
	{
		return point(x + other.x, y + other.y);
	}
	point operator*(double k) const
	{
		return point(x * k, y * k);
	}
	point operator/(double k) const
	{
		return point(x / k, y / k);
	}
};

//Сравнение полярных углов
bool greater_point (point first_line, point second_line) {
	std::complex<double> first_vec(first_line.x, first_line.y);
	std::complex<double> second_vec(second_line.x, second_line.y);

	double first_angle = std::arg(first_vec);
	first_angle = (first_angle >= 0) ? first_angle : first_angle + 2 * M_PI;
	double second_angle = std::arg(second_vec);
	second_angle = (second_angle >= 0) ? second_angle : second_angle + 2 * M_PI;

	return second_angle > first_angle;
}

//Получение индекса
int index(int num, int size, int start) {
	num = start - num;
	while (num < 0) {
		num = size + num;
	}

	return num;
}

//Поиск самой левой и нижней точки
int search_edge(std::vector<point>& figure) {
	int min_id = 0;
	for (int i = 1; i < figure.size(); i++) {
		if (figure[i].y <= figure[min_id].y) {
			if (figure[i].y < figure[min_id].y) {
				min_id = i;
			}
			else if (figure[i].x < figure[min_id].x) {
				min_id = i;
			}
		}
	}

	return min_id;
}

//Сумма Минковского
std::vector<point> mincovsky_sum(std::vector<point>& first_figure, std::vector<point>& second_figure) {
	int i = 0;
	int j = 0;
	int first_index = search_edge(first_figure);
	int second_index = search_edge(second_figure);
	std::vector<point> figure_sum;
	while (i < first_figure.size() || j < second_figure.size()) {
		int a = index(i, first_figure.size(), first_index);
		int b = index(j, second_figure.size(), second_index);
		int c = index(i + 1, first_figure.size(), first_index);
		int d = index(j + 1, second_figure.size(), second_index);
		figure_sum.push_back(first_figure[a] + second_figure[b]);
		if (greater_point(first_figure[c] - first_figure[a], second_figure[d] - second_figure[b])
            && i < first_figure.size())
			i++;
		else if (greater_point(second_figure[d] - second_figure[b], first_figure[c] - first_figure[a])
                 && j < second_figure.size())
			j++;
		else {
			i++;
			j++;
		}
	}

	return figure_sum;
}

//Произведение векторов и возвращение значения по оси z
double product_vec(point& first_p, point& second_p) {
	point third_vec = second_p - first_p;
	return first_p.x * third_vec.y - first_p.y * third_vec.x;
}

//Проверка на пересечение фигур
bool intersection(std::vector<point>& first_figure, std::vector<point>& second_figure) {
	for (int i = 0; i < second_figure.size(); i++) {
		second_figure[i] = point() - second_figure[i];
	}
	std::vector<point> figure_sum = mincovsky_sum(first_figure, second_figure);
	double sign = 0;
	for (int i = 1; i < figure_sum.size(); i++) {
		double new_sign = product_vec(figure_sum[i - 1], figure_sum[i]);
		if (new_sign * sign < 0) {
			return false;
		}
		sign = new_sign;
	}

	return true;
}

int main() {
	int n, m;
	double a, b;
	std::cin >> n;
	std::vector<point> first_figure(n);
	for (int i = 0; i < n; i++) {
		std::cin >> a >> b;
		first_figure[i] = point(a, b);
	}
	std::cin >> m;
	std::vector<point> second_figure(m);
	for (int i = 0; i < m; i++) {
		std::cin >> a >> b;
		second_figure[i] = point(a, b);
	}

	std::cout << (intersection(first_figure, second_figure) ? "YES" : "NO");
	return 0;
}
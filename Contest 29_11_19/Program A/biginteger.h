#include<iostream>
#include<string>
#include<vector>

constexpr int base = 10000;



class BigInteger {
public:
	BigInteger() = default;
	BigInteger(int prototype);
	friend BigInteger operator+(const BigInteger& left, const BigInteger& right);
	friend BigInteger operator-(const BigInteger& left, const BigInteger& right);
	friend BigInteger operator*(const BigInteger& left, const BigInteger& right);
	friend BigInteger operator/(BigInteger dividend, BigInteger divider);
	friend BigInteger operator%(const BigInteger& left, const BigInteger& right);

	BigInteger& operator+=(const BigInteger& obj);
	BigInteger& operator-=(const BigInteger& obj);
	BigInteger& operator*=(const BigInteger& obj);
	BigInteger& operator/=(const BigInteger& obj);
	BigInteger& operator%=(const BigInteger& obj);

	const BigInteger operator-() const;
	BigInteger& operator++();
	BigInteger& operator--();
	const BigInteger operator++(int);
	const BigInteger operator--(int);

	bool operator==(const BigInteger& other);
	bool operator!=(const BigInteger& other);
	bool operator<(const BigInteger& other);
	bool operator>(const BigInteger& other);
	bool operator<=(const BigInteger& other);
	bool operator>=(const BigInteger& other);

	friend std::ostream& operator<<(std::ostream& stream, const BigInteger& obj);
	friend std::istream& operator>>(std::istream& stream, BigInteger& obj);

	std::string toString() const;
	explicit operator bool() const;

private:
	std::vector<int> value;
	short int sign = 1;

	friend BigInteger sum(const BigInteger& left, const BigInteger& right);
	friend BigInteger sub(const BigInteger& left, const BigInteger& right);
	friend BigInteger simple_product(const BigInteger& left, const BigInteger& right);
	friend BigInteger simple_divide(const BigInteger& left, const BigInteger& right);
	friend BigInteger separate_first(const BigInteger& obj, unsigned int length);
	friend BigInteger separate_second(const BigInteger& obj, unsigned int length);
	friend BigInteger shift(const BigInteger& obj, int num_digit);
	friend bool compare_abs(BigInteger left, BigInteger right);
	void norm_zero();
};

BigInteger::operator bool() const {
	return (value.size() != 0);
}

BigInteger::BigInteger(int prototype) {
	if (prototype < 0) {
		sign = -1;
		prototype *= -1;
	}
	while (prototype > 0) {
		value.push_back(prototype % base);
		prototype /= base;
	}
}

std::string BigInteger::toString() const {
	if (value.size() == 0) {
		return "0";
	}
	std::string int_str;
	for (unsigned int i = 0; i < value.size(); i++) {
		int num = value[i];
		int digit = base;
		while (digit > 1 && ((i != value.size() - 1) || num > 0)) {
			int_str = (char)(num % 10 + 48) + int_str;
			num /= 10;
			digit /= 10;
		}
	}
	if (sign == -1) {
		int_str = '-' + int_str;
	}
	return int_str;
}

//Убирает лишние нули в начале числа
void BigInteger::norm_zero() {
	for (int i = value.size() - 1; i >= 0; i--) {
		if (value[i] != 0) {
			return;
		}
		value.pop_back();
	}
	sign = 1;
}

std::istream& operator>>(std::istream& stream, BigInteger& obj) {
	obj.sign = 1;
	obj.value.clear();
	std::string input;
	stream >> input;
	int index = input.size() - 1;
	while (index >= 0) {
		int new_value = 0;
		int mult = 1;
		while (mult < base && index >= 0) {
			if (input[index] == '-') {
				obj.sign = -1;
				index--;
				break;
			}
			new_value += mult * (input[index] - 48);
			mult *= 10;
			index--;
		}
		obj.value.push_back(new_value);
	}
	obj.norm_zero();
	return stream;
}

std::ostream& operator<<(std::ostream& stream, const BigInteger& obj) {
	stream << obj.toString();
	return stream;
}

bool BigInteger::operator==(const BigInteger& other) {
	if (value.size() != other.value.size() || sign != other.sign) {
		return false;
	}
	for (int i = value.size() - 1; i >= 0; i--) {
		if (value[i] != other.value[i]) {
			return false;
		}
	}
	return true;
}

bool BigInteger::operator!=(const BigInteger& other) {
	return !operator==(other);
}

bool BigInteger::operator>=(const BigInteger& other) {
	if (value.size() != other.value.size()) {
		return (value.size() > other.value.size());
	}
	if (sign != other.sign) {
		return (sign > other.sign);
	}
	for (int i = value.size() - 1; i >= 0; i--) {
		if (value[i] > other.value[i]) {
			return true;
		}
		else if (value[i] < other.value[i]) {
			return false;
		}
	}
	return true;
}

bool BigInteger::operator<(const BigInteger& other) {
	return !operator>=(other);
}

bool BigInteger::operator<=(const BigInteger& other) {
	if (value.size() != other.value.size()) {
		return (value.size() < other.value.size());
	}
	if (sign != other.sign) {
		return (sign < other.sign);
	}
	for (int i = value.size() - 1; i >= 0; i--) {
		if (value[i] > other.value[i]) {
			return false;
		}
		else if (value[i] < other.value[i]) {
			return true;
		}
	}
	return true;
}

bool BigInteger::operator>(const BigInteger& other) {
	return !operator<=(other);
}

//Сравнение >= по модулю
bool compare_abs(BigInteger left, BigInteger right) {
	left.sign = 1;
	right.sign = 1;
	return left >= right;
}

//Сумма без учёта знака, в левом число больше или равно правому
BigInteger sum(const BigInteger& left, const BigInteger& right) {
	BigInteger sum_int;
	int carry = 0;
	for (unsigned int i = 0; i < right.value.size(); i++) {
		sum_int.value.push_back(left.value[i] + right.value[i] + carry);
		if (sum_int.value[i] >= base) {
			sum_int.value[i] -= base;
			carry = 1;
		}
		else {
			carry = 0;
		}
	}
	for (unsigned int i = right.value.size(); i < left.value.size(); i++) {
		sum_int.value.push_back(left.value[i] + carry);
		if (sum_int.value[i] >= base) {
			sum_int.value[i] -= base;
			carry = 1;
		}
		else {
			carry = 0;
		}
	}
	if (carry == 1) {
		sum_int.value.push_back(1);
	}

	return sum_int;
}

//Разность без учёта знака, в левом число больше или равно правому
BigInteger sub(const BigInteger& left, const BigInteger& right) {
	BigInteger sub_int;
	int carry = 0;
	for (unsigned int i = 0; i < right.value.size(); i++) {
		sub_int.value.push_back(left.value[i] - right.value[i] - carry);
		if (sub_int.value[i] < 0) {
			sub_int.value[i] += base;
			carry = 1;
		}
		else {
			carry = 0;
		}
	}
	for (unsigned int i = right.value.size(); i < left.value.size(); i++) {
		sub_int.value.push_back(left.value[i] - carry);
		if (sub_int.value[i] < 0) {
			sub_int.value[i] += base;
			carry = 1;
		}
		else {
			carry = 0;
		}
	}

	return sub_int;
}

BigInteger operator+(const BigInteger& left, const BigInteger& right) {
	BigInteger sum_int;
	if (compare_abs(left, right)) {
		if (left.sign == right.sign) {
			sum_int = sum(left, right);
			if (left.sign == -1) {
				sum_int.sign *= -1;
			}
		}
		else {
			sum_int = sub(left, right);
			if (left.sign == -1) {
				sum_int.sign *= -1;
			}
		}
	}
	else {
		if (left.sign == right.sign) {
			sum_int = sum(right, left);
			if (left.sign == -1) {
				sum_int.sign *= -1;
			}
		}
		else {
			sum_int = sub(right, left);
			if (left.sign == 1) {
				sum_int.sign *= -1;
			}
		}
	}
	sum_int.norm_zero();

	return sum_int;
}

BigInteger operator-(const BigInteger& left, const BigInteger& right) {
	BigInteger sub_int;
	if (compare_abs(left, right)) {
		if (left.sign == right.sign) {
			sub_int = sub(left, right);
			if (left.sign == -1) {
				sub_int.sign *= -1;
			}
		}
		else {
			sub_int = sum(left, right);
			if (left.sign == -1) {
				sub_int.sign *= -1;
			}
		}
	}
	else {
		if (left.sign == right.sign) {
			sub_int = sub(right, left);
			if (left.sign == 1) {
				sub_int.sign *= -1;
			}
		}
		else {
			sub_int = sum(right, left);
			if (left.sign == -1) {
				sub_int.sign *= -1;
			}
		}
	}
	sub_int.norm_zero();

	return sub_int;
}

BigInteger& BigInteger::operator+=(const BigInteger& obj) {
	*this = *this + obj;
	return *this;
}

const BigInteger BigInteger::operator++(int) {
	BigInteger temp = *this;
	*this += 1;
	return temp;
}

BigInteger& BigInteger::operator++() {
	*this += 1;
	return *this;
}

BigInteger& BigInteger::operator-=(const BigInteger& obj) {
	*this = *this - obj;
	return *this;
}

const BigInteger BigInteger::operator--(int) {
	BigInteger temp = *this;
	*this -= 1;
	return temp;
}

BigInteger& BigInteger::operator--() {
	*this -= 1;
	return *this;
}

const BigInteger BigInteger::operator-() const {
	BigInteger copy_int = *this;
	copy_int.sign *= -1;
	return copy_int;
}

//Произведение с помощью стандартного оператора двух чисел с одним разрадом
BigInteger simple_product(const BigInteger& left, const BigInteger& right) {
	BigInteger product;
	if (left.value.size() == 0 || right.value.size() == 0)
		return product;
	product.value.push_back(left.value[0] * right.value[0]);
	if (product.value[0] > base) {
		product.value.push_back(product.value[0] / base);
		product.value[0] %= base;
	}
	product.sign = left.sign * right.sign;
	return product;
}

//Сдвиг на num_digit разрядов
BigInteger shift(const BigInteger& obj, int num_digit) {
	std::vector<int> new_value(num_digit);
	for (unsigned int i = 0; i < obj.value.size(); i++) {
		new_value.push_back(obj.value[i]);
	}
	BigInteger shifted;
	shifted.sign = obj.sign;
	shifted.value = new_value;
	return shifted;
}

//Возвращает правую часть числа (до разрада length невключительно)
BigInteger separate_first(const BigInteger& obj, unsigned int length) {
	BigInteger part;
	length = (length > obj.value.size()) ? obj.value.size() : length;
	for (unsigned int i = 0; i < length; i++) {
		part.value.push_back(obj.value[i]);
	}
	part.norm_zero();
	part.sign = obj.sign;
	return part;
}

//Возвращает левую часть числа (до разрада length включительно)
BigInteger separate_second(const BigInteger& obj, unsigned int length) {
	BigInteger part;
	for (unsigned int i = length; i < obj.value.size(); i++) {
		part.value.push_back(obj.value[i]);
	}
	part.norm_zero();
	part.sign = obj.sign;
	return part;
}

BigInteger operator*(const BigInteger& left, const BigInteger& right) {
	unsigned int half_length = left.value.size() / 2;
	half_length = (right.value.size() / 2 > half_length) ? right.value.size() / 2 : half_length;
	BigInteger first_left = separate_first(left, half_length);
	first_left.sign = 1;
	BigInteger second_left = separate_second(left, half_length);
	second_left.sign = 1;
	BigInteger first_right = separate_first(right, half_length);
	first_right.sign = 1;
	BigInteger second_right = separate_second(right, half_length);
	second_right.sign = 1;
	//Правая часть числа left, перемноженная на правую часть right
	BigInteger first_cross_first = (first_left.value.size() <= 1 && first_right.value.size() <= 1) ?
		simple_product(first_left, first_right) : (first_left * first_right);
	//Левая часть числа left, перемноженная на левую часть right
	BigInteger second_cross_second = (second_left.value.size() <= 1 && second_right.value.size() <= 1) ?
		simple_product(second_left, second_right) : (second_left * second_right);
	//Теперь в first будет храниться его сумма с second
	first_left += second_left;
	first_right += second_right;
	//Перемноженные суммы
	BigInteger sum_cross_sum = (first_left.value.size() <= 1 && first_right.value.size() <= 1) ?
		simple_product(first_left, first_right) : (first_left * first_right);

	BigInteger product = first_cross_first + shift(second_cross_second, half_length * 2) + shift(sum_cross_sum - first_cross_first - second_cross_second, half_length);
	if (product.value.size() != 0) {
		product.sign = left.sign * right.sign;
	}
	return product;
}

BigInteger& BigInteger::operator*=(const BigInteger& obj) {
	*this = *this * obj;
	return *this;
}

//Безнаковое деление двухразрядных чисел
BigInteger simple_divide(const BigInteger& left, const BigInteger& right) {
	int left_value = left.value[0];
	if (left.value.size() == 2) {
		left_value += base * left.value[1];
	}
	int right_value = right.value[0];
	if (right.value.size() == 2) {
		right_value += base * right.value[1];
	}
	BigInteger div;
	div.value.push_back(left_value / right_value);
	return div;
}

BigInteger operator/(BigInteger dividend, BigInteger divider) {
	int dividend_sign = dividend.sign;
	int divider_sign = divider.sign;
	dividend.sign = 1;
	divider.sign = 1;
	BigInteger answer;
	if (divider == BigInteger()) {
		throw "Dividing by zero is forbidden";
	}
	while (compare_abs(dividend, divider)) {
		//Верхние разряды чисел
		BigInteger up_left;
		BigInteger up_right;
		//Сдвиг нижнией оценки частного
		int digit_shift = dividend.value.size() - divider.value.size();
		//Число в последнем разряде
		int last_dividend = dividend.value[dividend.value.size() - 1];
		int last_divider = divider.value[divider.value.size() - 1];
		up_right.value.push_back(last_divider);
		if (dividend.value.size() != divider.value.size() || last_dividend != last_divider) {
			if (last_dividend <= last_divider) {
				up_left.value.push_back(dividend.value[dividend.value.size() - 2]);
				digit_shift--;
			}
			++up_right;
		}
		up_left.value.push_back(last_dividend);
		//Нижняя оценка деления
		BigInteger low_rate = shift(simple_divide(up_left, up_right), digit_shift);
		dividend -= low_rate * divider;
		answer += low_rate;
	}
	if (answer.value.size() != 0) {
		answer.sign = dividend_sign * divider_sign;
	}
	return answer;
}

BigInteger operator%(const BigInteger& left, const BigInteger& right) {
	return (left - (left / right) * right);
}

BigInteger& BigInteger::operator/=(const BigInteger& obj) {
	*this = *this / obj;
	return *this;
}

BigInteger& BigInteger::operator%=(const BigInteger& obj) {
	*this = *this % obj;
	return *this;
}

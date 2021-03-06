//LABKA 2-4 v.1 Class implementation
#include <iostream>
#include <fstream>
#include "classes.h"

constexpr double eps = 1e-7;

Polynom::Polynom(const Polynom& p) {
	for (auto i = p.begin(); i != p.end(); ++i)
		push(*i);
}

Polynom::Polynom(Polynom&& p) noexcept {
	first = p.first;
	p.first = nullptr;
}

Polynom& Polynom::operator=(const Polynom& p) {
	if (this != &p) {
		dispose();
		for (auto i = p.begin(); i != p.end(); ++i)
			push(*i);
	}
	return *this;
}

Polynom& Polynom::operator=(Polynom&& p) noexcept {
	if (this != &p) {
		dispose();
		first = p.first;
		p.first = nullptr;
	}
	return *this;
}

Polynom Polynom::operator+(const Polynom& p) const {
	double resultCoef = 0;
	Polynom resultP;
	auto iA = begin();
	auto iB = p.begin();
	while (true) {
		if (iA != nullptr and iB == nullptr) {
			resultP.push(*iA); ++iA;
		} else if (iA == nullptr and iB != nullptr) {
			resultP.push(*iB); ++iB;
		} else if (iA != nullptr and iB != nullptr) {
			Monom& currA = *iA;
			Monom& currB = *iB;
			if (abs(currA.pow - currB.pow) < eps) { // A and B operands have the same power
				resultCoef = currA.coef + currB.coef;
				Monom resultM{ resultCoef, currA.pow };
				resultP.push(resultM);
				++iA; ++iB;
			} else if (currA.pow < currB.pow) {
				resultP.push(currA); ++iA;
			} else {
				resultP.push(currB); ++iB;
			}
		} else
			break;
	}
	return resultP;
}

Polynom Polynom::operator-(const Polynom& p) const {
	double resultCoef = 0;
	Polynom resultP;
	auto iA = begin();
	auto iB = p.begin();
	while (true) {
		if (iA != nullptr and iB == nullptr) {
			resultP.push(*iA); ++iA;
		} else if (iA == nullptr and iB != nullptr) {
			resultP.push(*iB); ++iB;
		} else if (iA != nullptr and iB != nullptr) {
			Monom& currA = *iA;
			Monom& currB = *iB;
			if (abs(currA.pow - currB.pow) < eps) { // A and B operands have the same power
				resultCoef = currA.coef - currB.coef;
				Monom resultM{ resultCoef, currA.pow };
				resultP.push(resultM);
				++iA; ++iB;
			} else if (currA.pow < currB.pow) {
				resultP.push(currA); ++iA;
			} else {
				resultP.push(currB); ++iB;
			}
		} else
			break;
	}
	return resultP;
}

Polynom& Polynom::operator+=(const Polynom& p) {
	auto iA = begin();
	auto iB = p.begin();
	while (true) {
		if (iA != nullptr and iB == nullptr)
			break;
		else if (iA == nullptr and iB != nullptr) {
			push(*iB); ++iB;
		} else if (iA != nullptr and iB != nullptr) {
			Monom& currA = *iA;
			Monom& currB = *iB;
			if (abs(currA.pow - currB.pow) < eps) { // A and B operands have the same power
				currA.coef += currB.coef; ++iA; ++iB;
			} else {
				push(currB); ++iB;
			}
		} else
			break;
	}
	return *this;
}

Polynom::~Polynom() { dispose(); }

void Polynom::push(const Monom& m) {
	// add element with smaller power to the beginning
	if (first == nullptr or first->data.pow > m.pow) { // add prev
		LE* temp = new LE{ m.coef, m.pow, first };
		first = temp;
	} else {
		LE* seek = first;
		while (seek->next != nullptr and seek->next->data.pow <= m.pow)
			seek = seek->next;
		if (seek->data.pow < m.pow) {
			LE* temp = new LE{ m.coef, m.pow, seek->next }; // add after
			seek->next = temp;
		} else // seek->data.pow == m.pow
			seek->data.coef += m.coef;
	}
}

bool Polynom::pop(Monom& m) {
	if (first == nullptr)
		return false;
	m.coef = first->data.coef;
	m.pow = first->data.pow;
	LE* tempDel = first;
	first = first->next;
	delete tempDel;
	return true;
}

void Polynom::dispose() {
	Monom temp;
	while (pop(temp));
}

std::istream& operator >>(std::istream& ifs, Polynom& p) {
	double tempCoef, tempPow;
	if (&ifs == &std::cin) {
		while (true) {
			std::cout << "Coef: ";
			if (!(ifs >> tempCoef))
				break;
			else {
				std::cout << "Pow: ";
				if (!(ifs >> tempPow))
					break;
			}
			Monom tempMonom{ tempCoef, tempPow };
			p.push(tempMonom);
		}
	} else
		while (ifs.peek() != std::ifstream::traits_type::eof()) {
			ifs >> tempCoef >> tempPow;
			Monom tempMonom{ tempCoef, tempPow };
			p.push(tempMonom);
		}
	close(ifs);
	return ifs;
}

std::ostream& operator <<(std::ostream& ofs, const Polynom& p) {
	if (&ofs == &std::cout) {
		short amountOfMonoms = 0;
		for (auto i = p.begin(); i != p.end(); ++i) {
			Monom& tempM = *i;
			if (tempM.coef > 0 and amountOfMonoms != 0) //dont put '+' on the start of polynom
				ofs << " + ";
			else if (tempM.coef < 0)
				ofs << " - ";

			if (abs(tempM.coef - 0.) < eps) // coefficient equal to 0
				continue;
			else if (abs(tempM.coef - 1.) < eps and abs(tempM.pow - 0.) > eps) // coefficient equal to 1 and power is not equal to zero
				ofs << "x^" << tempM.pow;
			else if (abs(tempM.pow - 0.) < eps) // power equal to zero
				ofs << abs(tempM.coef);
			else if (abs(tempM.pow - 1.) < eps) // power equa to one
				ofs << abs(tempM.coef) << "*x";
			else
				ofs << abs(tempM.coef) << "*x^" << tempM.pow;
			++amountOfMonoms;
		}
		ofs << std::endl;
	} else
		for (auto i = p.begin(); i != p.end(); ++i) {
			Monom& tempM = *i;
			if (abs(tempM.coef - 0.) > eps) // coefficient is not equal to zero
				ofs << tempM.coef << ' ' << tempM.pow << std::endl;
		}
	close(ofs);
	return ofs;
}

void close(std::istream& s) {
	if (&s == &std::cin) {
		s.sync();
		s.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	} else
		((std::ifstream&)s).close();
}

void close(std::ostream& s) {
	if (&s != &std::cout)
		((std::ofstream&)s).close();
}

Polynom::Iter::Iter(Polynom::LE* init) : current{ init } {}

Monom& Polynom::Iter::operator*() {
	if (current != nullptr)
		return current->data;
	else
		throw std::out_of_range("Memory access violation due to * operator");
}

Polynom::Iter& Polynom::Iter::operator++() {
	if (current != nullptr)
		current = current->next;
	else
		throw std::out_of_range("Memory access violation due to ++i operator");
	return *this;
}

Polynom::Iter& Polynom::Iter::operator++(int) {
	Iter temp = *this;
	if (current != nullptr)
		current = current->next;
	else
		throw std::out_of_range("Memory access violation due to i++ operator");
	return temp;
}

bool Polynom::Iter::operator==(const Iter& it) { return (current == it.current); }
bool Polynom::Iter::operator!=(const Iter& it) { return (current != it.current); }

Polynom::Iter Polynom::begin() const { return Iter(first); }
Polynom::Iter Polynom::end() const { return Iter(nullptr); }

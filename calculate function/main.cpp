/*Created by Soulike in SSDUT, July 13, 2017*/
/*Tested on Visual Studio 2017. Your translater is required to support C++11*/

#include<iostream>
#include<vector>
#include<string>
#include<algorithm>
#include<exception>

double calculate(std::string equation);
static bool isSignal(char c);

/*Calculating function, based on recursion*/
/*Note: You need to ensure that your formula is correct. It can only detect a few errors.*/
double calculate(std::string formula)
{
	/*4 operators*/
	const char MULTI = '*';
	const char DIV = '/';
	const char PLUS = '+';
	const char MINUS = '-';

	/*Detect whether the formula omits multiplication signs like (1+2)(3+4) or 5(5+5) or (5+5)6*/
	/*If do, it will insert MULTIs*/
	size_t pos = 0;
	while (pos != std::string::npos)
	{
		if (pos > 0)
			if ((formula[pos - 1] >= '0' && formula[pos - 1] <= '9') || formula[pos - 1] == ')')
			{
				formula.insert(formula.begin() + pos, '*');
				pos++;
			}
		pos++;
		pos = formula.find('(', pos);
	}

	pos = 0;
	while (pos != std::string::npos)
	{
		if (pos > 0 && pos<formula.size() - 1)
			if ((formula[pos + 1] >= '0' && formula[pos + 1] <= '9') || formula[pos + 1] == '(')
			{
				formula.insert(formula.begin() + pos + 1, '*');
				pos++;
			}
		pos++;
		pos = formula.find(')', pos);
	}

	/*The last character of the formula must be a number*/
	if (isSignal(formula[formula.size() - 1]))
		throw "Lack numbers.";

	/*Detect whether the formula contains brackets*/
	auto first_left = formula.find_first_of('(');
	auto last_right = formula.find_last_of(')');

	if (first_left == last_right)//If there is no bracket
	{
		double result, temp_result;//result is the calculation result. temp_result is used to tempararily save the results of partial formulas with higher priorities.
		double num1, num2;
		std::vector<char> operators;
		std::vector<double> nums;
		std::string current_num;

		for (auto i : formula)
			if (i == MULTI || i == DIV || i == PLUS || i == MINUS)//If it's an operator
			{
				if (current_num.size() == 0)//If the first character after an operator is an operator, it must be + or -
				{
					if (i == PLUS || i == MINUS)
						current_num.push_back(i);
					else
						throw std::exception("Wrong operator usage.");
				}
				else//It's a plain operator
				{
					operators.push_back(i);
					nums.push_back(std::stod(current_num));//Save the latest number
					current_num.clear();
				}
			}
			else if ((i >= '0'&&i <= '9') || i == '.')//Else if it's a part of a number
				current_num += i;
		nums.push_back(std::stod(current_num));//Save the last number

		for (size_t i = 0; i < operators.size(); i++)
		{
			/*Multiplication and division have higher priorities. So calculate them first.*/
			/*Remove them and replace their coresponding opertating numbers with their calculation results*/
			if (operators[i] == MULTI)
			{
				num1 = nums[i];
				num2 = nums[i + 1];
				temp_result = num1*num2;
				nums.erase(nums.begin() + i, nums.begin() + i + 2);
				operators.erase(operators.begin() + i);
				nums.insert(nums.begin() + i, temp_result);
				i--;
			}
			else if (operators[i] == DIV)
			{
				num1 = nums[i];
				num2 = nums[i + 1];
				temp_result = num1 / num2;
				nums.erase(nums.begin() + i, nums.begin() + i + 2);
				operators.erase(operators.begin() + i);
				nums.insert(nums.begin() + i, temp_result);
				i--;
			}
		}
		result = nums[0];
		/*Addition and subtraction*/
		for (size_t i = 0; i < nums.size() - 1; i++)
			if (operators[i] == PLUS)
				result += nums[i + 1];
			else if (operators[i] == MINUS)
				result -= nums[i + 1];
		return result;
	}
	else if (first_left == std::string::npos || last_right == std::string::npos)//If brackets do not match
	{
		throw std::exception("Brackets do not match.");
	}
	else//If there are brackets
	{
		size_t left, right, left_2, right_2;
		left = formula.find_first_of('(');
		right = formula.find_last_of(')');
		left_2 = formula.find_last_of('(');
		right_2 = formula.find_first_of(')');
		if (left_2 >= left && right_2 <= right && left_2 < right_2)//Situation like (1+(2+3))
		{
			double sub_result = calculate(formula.substr(first_left + 1, last_right - first_left - 1));//Remove the brackets and calculate the formula in them
			formula.replace(first_left, last_right - first_left + 1, std::to_string(sub_result));//Replace the brackets with calculating result
			return calculate(formula);//Recursion
		}
		else//(1+2)+(3+4)
		{
			left = formula.find_last_of('(');//Find the rightmost left bracket
			right = formula.substr(left, formula.size()).find_first_of(')') + left;////Find the leftmost right bracket after the rightmost left bracket
			//We can get a formula in the brackets
			double sub_result = calculate(formula.substr(left + 1, right - left - 1));//figure it out
			formula.replace(left, right - left + 1, std::to_string(sub_result));//replace it
			return calculate(formula);//Recursion
		}
	}
}

static bool isSignal(char c)
{
	return (c == '+' || c == '-' || c == '*' || c == '/');
}

#ifndef UTILS_HPP
# define UTILS_HPP

# include <sstream>
template <typename T>
T ato_all(std::string const& s)
{
	std::stringstream stream;
	T num;

	stream << s;
	stream >> num;

	return num;
}

template <typename T>
std::string all_toa(T num)
{
	std::stringstream stream;
	stream << num;
	return stream.str();
}

#endif //UTILS_HPP

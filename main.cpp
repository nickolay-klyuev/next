#include <cstdlib>
#include <string>
#include <iostream>
#include <fstream>
#include <memory>
#include <filesystem>

#define DATA_NAME "data.next"
#define TEMP_DATA_NAME "tempdata.next"
#define SEPARATOR ':'

using ofstream_ptr = std::unique_ptr<std::ofstream>;
using ifstream_ptr = std::unique_ptr<std::ifstream>;

ofstream_ptr p_of;
ifstream_ptr p_if;

enum class option_t : uint8_t
{
    next,
    set,
    remove
};

int Next(std::string name, option_t option = option_t::next, int set_amount = 0)
{
    p_if = std::make_unique<std::ifstream>(DATA_NAME);
    p_of = std::make_unique<std::ofstream>(TEMP_DATA_NAME);

    if (p_if->is_open() && p_of->is_open())
    {
        std::string line;
        bool is_new_line = option != option_t::remove;

        while (std::getline(*p_if, line))
        {
            size_t separator_index = line.find(SEPARATOR);
            std::string line_name = line.substr(0, separator_index);

            if (line_name == name)
            {
                if (option == option_t::remove)
                {
                    std::cout << line_name + " removed" << '\n';
                    continue;
                }

                is_new_line = false;

                int next_count;
                if (option == option_t::set)
                {
                    next_count = set_amount;
                }
                else
                {
                    next_count = std::stoi(line.substr(separator_index + 1));
                    ++next_count;
                }

                std::string new_line = line_name + SEPARATOR + std::to_string(next_count) + '\n';

                *p_of << new_line;
                std::cout << new_line;
            }
            else
            {
                *p_of << line + '\n';
            }
        }

        if (is_new_line)
        {
            std::string new_line;
            if (option == option_t::set)
            {
                new_line = name + SEPARATOR + std::to_string(set_amount) + '\n';
            }
            else
            {
                new_line = name + SEPARATOR + '1' + '\n';
            }

            *p_of << new_line;
            std::cout << new_line;
        }
        
        p_if->close();
        p_of->close();

        std::remove(DATA_NAME);
        std::rename(TEMP_DATA_NAME, DATA_NAME);

        return EXIT_SUCCESS;
    }

    std::cerr << "Could NOT open " << DATA_NAME << '\n';
    return EXIT_FAILURE;
}

int Show()
{
    p_if = std::make_unique<std::ifstream>(DATA_NAME);

    if (p_if->is_open())
    {
        std::string line;

        while (std::getline(*p_if, line))
        {
            std::cout << line << '\n';
        }

        p_if->close();

        return EXIT_SUCCESS;
    }

    std::cerr << "Could NOT open " << DATA_NAME << '\n';
    return EXIT_FAILURE;
}

int main(int argc, char const *argv[])
{
    if (!std::filesystem::exists(DATA_NAME))
    {
        p_of = std::make_unique<std::ofstream>(DATA_NAME);
        *p_of << "";
        p_of->close();
        p_of.release();
    }

    if (argc == 1)
    {
        return Show();
    }

    if (argc != 3 && argc != 4)
    {
        std::cerr << "You need to pass 2 or 3 args!" << '\n';
        return EXIT_FAILURE;
    }

    std::string next_name(argv[1]);
    std::string command(argv[2]);

    if (command == "++")
    {
        return Next(next_name);
    }

    if (command == "set")
    {
        if (argc != 4)
        {
            std::cerr << "You need to pass extra arg with set command!" << '\n';
            return EXIT_FAILURE;
        }

        return Next(next_name, option_t::set, std::stoi(argv[3]));
    }

    if (command == "remove")
    {
        return Next(next_name, option_t::remove);
    }

    std::cerr << "Do NOT understand any command!" << '\n';
    return EXIT_FAILURE;
}
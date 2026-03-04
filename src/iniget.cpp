// =============================================================================
// File:            iniget.cpp
// Description:  IniGet / IniSet is simple INI File parser to get and set values in such files.
// Author:        Christian Hartmann
// Date:           02.09.2025
// =============================================================================

//  TODO
// ! handle lines with just a delimeter and those without a key or value (are these invalid?)
// - define class that match section lines line.is_section_head
// - or define class, that represents a section (with a name and key value pairs)
// - define class that reads value from line

// more TODO
// --comment-char (other than '#') - seems ';' is also common for that
// Multi-line values

// autodetect comment char from first non-empty line (and if this starts with a non-blank
// char that is either # or ;

#include <fstream>
#include <iostream>
// using namespace std;
#include <string>
#include <regex>
#include <strings.h> // lets us make use of C function strcasecmp

// from: https://www.ibm.com/docs/en/zos/2.4.0?topic=functions-strcasecmp-case-insensitive-string-comparison
// info: https://stackoverflow.com/questions/5378778/what-does-d-xopen-source-do-mean
#define  _XOPEN_SOURCE_EXTENDED 1

// of: https://gist.github.com/RabaDabaDoba/145049536f815903c79944599c6f952a
#define RED "\e[0;31m"
#define GRN "\e[0;32m"
#define YEL "\e[0;33m"
#define BLU "\e[0;34m"
#define RST "\e[0m"

#define SPC " "

// char *me;
std::string me;
// char *filepath;

void error_exit(std::string msg, int ret = 1)
{
    std::cerr << "[ERROR]" << SPC << msg << std::endl;
    std::exit(ret);
}



// credit: https://cppscripts.com/trim-in-cpp
std::string trim_right(const std::string &str) {
    size_t end = str.find_last_not_of(" \t\n");
    return (end == std::string::npos) ? "" : str.substr(0, end + 1);
}

// credit: https://www.delftstack.com/howto/cpp/how-to-trim-a-string-cpp/
// std::string trim(const std::string& str) {
//     auto start = str.begin();
//     while (start != str.end() && std::isspace(*start)) {
//         ++start;
//     }
//     auto end = str.end();
//     do {
//         --end;
//     } while (end != start && std::isspace(*end));
//     return std::string(start, end + 1);
// }

// credit: https://en.cppreference.com/w/cpp/string/byte/tolower
// std::string str_tolower(std::string s)
// {
//     std::transform(s.begin(), s.end(), s.begin(),
//                    // static_cast<int(*)(int)>(std::tolower)         // wrong
//                    // [](int c){ return std::tolower(c); }           // wrong
//                    // [](char c){ return std::tolower(c); }          // wrong
//                    [](unsigned char c){ return std::tolower(c); } // correct
//     );
//     return s;
// }

// my attempt to create a [non] sensitive string comparison
int strcompic ( std::string test, std::string with, bool ic)
{
    if (!ic)
        return test.compare(with);
    else
        return strcasecmp(test.c_str(), with.c_str());
}

bool match_section_and_key (
        std::string st,
        std::string sw,
        std::string kt,
        std::string kw,
        bool ic
    )
{
    return (
        strcompic(st, sw, ic) == 0 &&
        strcompic(kt, kw, ic) == 0
    );
}

int main (int argc, char *argv[])
{
    // command line options
    bool VERBOSE = false;
    std::string delim = "=";
    bool ic = false; // ignore case
    bool validate = false;
    bool match_all = false;

    // arguments of command line
    std::string filepath;
    std::string search_section = "";
    std::string search_key;

    // internals
    std::string line;
    std::string section_match;
    // std::string last_section = "";
    std::string key_match;
    std::string value_match;
    std::string get_value;

    std::string token;
    std::string option;

    std::smatch match;

    int line_count = 0;
    bool get = false;
    // bool new_section = false;
    int i;

    int exit = 0; // 1 on no match or invalid lines in validation mode

    // std::cerr << YEL << "[0] argc" << ":" << argc << RST << std::endl;
    // for (i = 1; i < argc; i++)
    // {
    //     std::cerr << YEL << "[0] argv[" << i << "]:" << argv[i] << RST << std::endl;
    // }

    me = argv[0];
    // if (argc < 4) error_exit("usage:" SPC + me + SPC + "[options] <file> <section> <key>", 1); // add [<value>] if set mode

    // allow short and long gnu style options
    std::regex option_pattern("^--?([a-z-]*)$");

    // std::string first_token = argv[1];
    // if ( (first_token == "--validate" || first_token == "-V") && argc != 3 )
    if (argc < 2)
        error_exit("usage:" SPC + me + SPC + "[options] <inifile> <section> <key>", 1); // add [<value>] if set mode

    if ( (std::string(argv[1]) == "--validate" || std::string(argv[1]) == "-V" ) && argc < 3 )
        error_exit("usage:" SPC + me + SPC + "--validate <inifile>", 1);

    if ( (std::string(argv[1]) != "--validate" && std::string(argv[1]) != "-V" ) && argc < 4 )
        error_exit("usage:" SPC + me + SPC + "[options] <inifile> <section> <key>", 1); // add [<value>] if set mode

    ////////////////////////////


    // TODO option validate does not have to be first or only option (think --verbose)
    //      from: https://thelinuxcode.com/retrieve-command-line-arguments-cpp/  for: std::vector
    std::vector<std::string> args;
    // Reserve vector capacity
    args.reserve(argc);
    // Insert from index 1 to end
    for (i = 1; i < argc; ++i) {
        args.emplace_back(argv[i]);
    }
    // Find argument by name
    auto result = std::find(args.begin(), args.end(), "--validate");
    if (result != args.end()) {
        // std::cerr << YEL << "[-] validate:" << SPC << "on" << RST << std::endl;
        // TODO put above command line validation code here
    }

////////////////////////////


    // read options and params
    for ( i = 1; i < argc; i++ )
    {
        token = argv[i];
        if (std::regex_search(token, match, option_pattern)) {
            option =  match[1];
            if (option == "verbose" || option == "v")
            {
                VERBOSE = true;
                VERBOSE && std::cerr << YEL << "[0] verbose:" << SPC << "on" << RST << std::endl;
                continue;
            }
            if (option == "all" || option == "a")
            {
                match_all = true;
                VERBOSE && std::cerr << YEL << "[0] match all:" << SPC << "on" << RST << std::endl;
                continue;
            }
            if (option == "delimiter" || option == "delim" || option == "d")
            {
                delim = argv[++i];
                VERBOSE && std::cerr << YEL << "[0] delim:" << SPC << delim << RST << std::endl;
                continue;
            }
            if (option == "ignore-case" || option == "i")
            {
                ic = true;
                VERBOSE && std::cerr << YEL << "[0] ignore case:" << SPC << "on" << RST << std::endl;
                continue;
            }
            if (option == "validate" || option == "V")
            {
                validate = true;
                VERBOSE && std::cerr << YEL << "[0] validate:" << SPC << "on" << RST << std::endl;
                continue;
            }
            VERBOSE && std::cerr << YEL << "[0] argv[" << i << "]:" << argv[i] << RST << std::endl;
        } else {
            filepath = argv[i];
            VERBOSE && std::cerr << YEL << "[0] argv[" << i+0 << "]:" << argv[i+0] << RST << std::endl;
            if (!validate)
            {
                search_section = argv[i+1];
                search_key = argv[i+2];
                VERBOSE && std::cerr << YEL << "[0] argv[" << i+1 << "]:" << argv[i+1] << RST << std::endl;
                VERBOSE && std::cerr << YEL << "[0] argv[" << i+2 << "]:" << argv[i+2] << RST << std::endl;
            }
            break;
        }
    }

    // if ( ! validate )
    // {
    //     VERBOSE && std::cerr << YEL << "[0] file:" << SPC << filepath << RST << std::endl;
    //     VERBOSE && std::cerr << YEL << "[0] ssec:" << SPC << search_section << RST << std::endl;
    //     VERBOSE && std::cerr << YEL << "[0] skey:" << SPC << search_key << RST << std::endl;
    // }
    // else
    // {
    //     VERBOSE && std::cerr << YEL << "[0] file:" << SPC << filepath << RST << std::endl;
    // }


    // some regular expressions matching line types
    std::regex empty_line_pattern("^[ \t]*$");
    std::regex comment_line_pattern("^[ \t]*[#;].*$");
    std::regex section_pattern("^[ \t]*\\[(.*)\\][ \t]*$");
    // std::regex key_value_pattern("^[ \t]*([^ ]*)[ \t]*" + delim + "[ \t]*(.*)[ \t]*$");

		// TODO key matches are too greedy. they struggle with white spaces before delimiter
		// catch keys with spaces (as kde apps use it)
		std::regex key_value_pattern("^[ \t]*(.*)[ \t]*" + delim + "[ \t]*(.*)[ \t]*$");
		// std::regex key_value_pattern_quoted("^[ \t]*([^ ]*)[ \t]*" + delim + "[ \t]*\"([^\"]*)\"[ \t]*$");
    // catch x=""hallali hallala hallalo   "" as "hallali hallala hallalo   "
		// std::regex key_value_pattern_quoted("^[ \t]*([^ ]*)[ \t]*" + delim + "[ \t]*\"(.*)\"[ \t]*$");
		// catch keys with spaces (as kde apps use it)
		// TODO do we hava to allow quoted keys as well?
    std::regex key_value_pattern_quoted("^[ \t]*(.*)[ \t]*" + delim + "[ \t]*\"(.*)\"[ \t]*$");

    // try opening file
    std::ifstream file(filepath);


    // read lines from file if open
    if (!file.is_open())
		{
				error_exit("unable to open filepath: " + filepath, 99);
		}
		else
		{
        while (getline(file, line))
        {
            line_count++;

            if (std::regex_search(line, match, empty_line_pattern)) {
                VERBOSE && std::cerr << GRN << '[' << line_count << ']' << ' ' << "empty line" << RST << std::endl;
                continue;
            }
            else if (std::regex_search(line, match, comment_line_pattern)) {
                VERBOSE && std::cerr << GRN << '[' << line_count << ']' << ' ' << "comment line:" << SPC << line << RST << std::endl;
                continue;
            }
            else if (std::regex_search(line, match, section_pattern)) {
                // std::cout << line << std::endl;
                VERBOSE && std::cerr << GRN << '[' << line_count << ']' << ' ' << "section line:" << SPC << line << RST << std::endl;
                section_match = match[1];
                key_match = ""; // avoid matching with key from last section
                // if ( section_match.compare(last_section) != 0 )
                // {
                //     new_section = true;
                // }
                continue;
            }
            else if (std::regex_search(line, match, key_value_pattern_quoted)) {
                VERBOSE && std::cerr << GRN << '[' << line_count << ']' << ' ' << "key value line quoted:" << SPC << line << RST << std::endl;
                key_match = match[1];
                value_match = match[2];
                VERBOSE && std::cerr << BLU << '[' << line_count << ']' << SPC << "'" << key_match << "'" << ";" << SPC << "'" << value_match << "'" << RST << std::endl;
            }
            else if (std::regex_search(line, match, key_value_pattern)) {
                VERBOSE && std::cerr << GRN << '[' << line_count << ']' << ' ' << "key value line:" << SPC << line << RST << std::endl;
								// trim right for now as the regexp is greedy and catches white spaces before delimiter and at the end of line
								key_match = trim_right(match[1]);
								value_match = trim_right(match[2]);
                VERBOSE && std::cerr << BLU << '[' << line_count << ']' << SPC << "'" << key_match << "'" << ";" << SPC << "'" << value_match << "'" << RST << std::endl;
            }
            else
            {
                VERBOSE && std::cerr << RED << '[' << line_count << ']' << ' ' << "invalid line:" << SPC << line << RST << std::endl;
                if (validate)
                {
                    std::cerr << '[' << line_count << ']' << ' ' << "invalid line:" << SPC << line << std::endl;
                    exit = 1;
                }
                continue;
            }

            // keep value if we have a match
            // TODO this also runs on invalid lines. does not harm much, but is just not smart
            //    > continue on invlaid line match?
            // TODO following condition should go into a separate function, that we can set breakpoint on

            // if (strcompic(key_match, search_key, ic) == 0
            //            && strcompic(section_match, search_section, ic) == 0)
            if (match_section_and_key(section_match, search_section, key_match, search_key, ic))
            {
                VERBOSE && std::cerr << BLU << '[' << line_count << ']' << ' ' << '[' << "MATCH" << ']' << ' ' << key_match << ':' << ' ' << value_match << RST << std::endl;
                get = true;
                get_value = value_match;

                // print out here if we want all matches
                if (match_all)
                {
                    std::cout << value_match << std::endl;
                }
            }
        }
        file.close();
    }

    // print out if key in section found
    if (!validate && !match_all)
    {
        if (get)
            std::cout << get_value << std::endl;
        else
            exit = 1;
    }

    std::exit(exit);
}

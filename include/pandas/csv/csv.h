#include <fstream>
#include <string>
#include <vector>

#include "pandas/array.h"

namespace pandas {
namespace csv {

    std::vector<std::string> read_row(std::ifstream& in, char delimiter = ',')
    {
        std::vector<std::string> cells;
        bool in_quote = false;
        std::stringstream ss;
        char a, b;
        while (in.get(a)) {
            if (a == '"') {
                if (in_quote) {
                    if (in.eof()) {
                        cells.push_back(ss.str());
                        ss.str("");
                        break;
                    }
                    in.get(b);
                    if (b == delimiter || b == '\n') {
                        in_quote = false;
                        cells.push_back(ss.str());
                        ss.str("");
                    } else {
                        ss << b;
                    }

                } else {
                    in_quote = true;
                }

            } else if (a == delimiter) {
                if (in_quote) {
                    ss << a;

                } else {
                    cells.push_back(ss.str());
                    ss.str("");
                }

            } else if (a == '\n') {
                if (in_quote) {
                    ss << a;
                } else {
                    cells.push_back(ss.str());
                    ss.str("");
                    break;
                }
            } else {
                ss << a;
            }
        }

        if (in.eof() && a != '\n') {
            cells.push_back(ss.str());
        }

        return cells;
    }

    std::vector<Array<std::string, std::string>> read_csv(const std::string& filename, bool has_header, char delimiter = ',')
    {
        std::vector<Array<std::string, std::string>> res;
        std::ifstream in(filename);

        if (!in.is_open()) {
            throw std::format("failed open file {}", filename);
        }

        std::vector<std::string> headers = read_row(in, delimiter);

        std::cout << headers.size() << std::endl;

        int col_cnt = headers.size();
        for (int j = 0; j < col_cnt; j++) {
            std::string col_name = std::to_string(j);
            if (has_header) {
                col_name = headers[j];
            }
            res.push_back(Array<std::string, std::string>(col_name));
        }

        if (!has_header) {
            for (int j = 0; j < col_cnt; j++) {
                res[j]._append(headers[j]);
            }
        }

        while (!in.eof()) {
            std::vector<std::string> row = read_row(in, delimiter);
            if (row.size() != col_cnt) {
                throw std::format("csv format error: {}", row.size());
            }
            for (int j = 0; j < col_cnt; j++) {
                res[j]._append(row[j]);
            }
        }

        return res;
    }

}
}
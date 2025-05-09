#include <fstream>
#include <string>
#include <vector>

#include "pandas/csv/csv.h"

namespace pandas {
namespace csv {

    std::vector<std::string> read_row(std::istream& in, char delimiter)
    {
        std::vector<std::string> cells;
        bool in_quote = false;
        std::stringstream ss;
        char a, b;
        char end = '\n';
        while (in.get(a)) {
            end = a;
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
                        if (b == '\n') {
                            break;
                        }
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

        if (in.eof() && end != '\n') {
            cells.push_back(ss.str());
        }

        return cells;
    }

    std::vector<Array<std::string, std::string>> read_csv(const std::string& filename, bool has_header, char delimiter)
    {
        std::vector<Array<std::string, std::string>> res;
        std::ifstream in(filename);

        if (!in.is_open()) {
            PANDAS_THROW(std::format("failed open file {}", filename));
        }

        std::vector<std::string> headers = read_row(in, delimiter);

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

        int c = 0;
        while (!in.eof()) {
            std::vector<std::string> row(read_row(in, delimiter));
            if (row.size() == 0) {
                continue;
            }
            if (row.size() != col_cnt) {
                PANDAS_THROW(std::format("csv format error, row.size()!=col_cnt: {}!={}", row.size(), col_cnt));
            }
            for (int j = 0; j < col_cnt; j++) {
                res[j]._append(row[j]);
            }
            c++;
        }

        return res;
    }

    template <>
    std::string format_csv_string(const std::string& s)
    {
        std::stringstream ss;
        ss << '"';
        for (int i = 0; i < s.size(); i++) {
            char a = s[i];
            ss << a;
            if (a == '"') {
                ss << '"';
            }
        }
        ss << '"';
        return ss.str();
    }

    void write_csv(const std::string& filename, const std::vector<std::string>& cols, char delimiter)
    {
        std::ofstream out(filename);

        if (!out.is_open()) {
            PANDAS_THROW(std::format("failed open file {}", filename));
        }

        if (cols.size() == 0) {
            out.close();
            return;
        }

        int R = cols[0].size(), C = cols.size();
        for (int i = 0; i < R; i++) {
            for (int j = 0; j < C; j++) {
                out << cols[j][i];
                if (j + 1 < C) {
                    out << delimiter;
                } else {
                    out << '\n';
                }
            }
        }

        out.close();
    }

}
}
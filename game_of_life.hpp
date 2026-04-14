#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

inline int row = -1, col = -1;
inline std::vector<uint32_t> live_cells;

inline void Initialize() {
    std::cin >> col >> row;
    live_cells.clear();
    
    int curr_r = 0;
    int curr_c = 0;
    int count = 0;
    
    char ch;
    while (std::cin >> ch) {
        if (ch == '!') {
            break;
        }
        if (ch >= '0' && ch <= '9') {
            count = count * 10 + (ch - '0');
        } else {
            if (count == 0) count = 1;
            
            if (ch == 'b') {
                curr_c += count;
            } else if (ch == 'o') {
                for (int i = 0; i < count; ++i) {
                    live_cells.push_back(curr_r * col + curr_c);
                    curr_c++;
                }
            } else if (ch == '$') {
                curr_r += count;
                curr_c = 0;
            }
            count = 0;
        }
    }
    std::sort(live_cells.begin(), live_cells.end());
    live_cells.erase(std::unique(live_cells.begin(), live_cells.end()), live_cells.end());
}

inline void Tick() {
    std::vector<uint32_t> neighbors;
    neighbors.reserve(live_cells.size() * 8);
    
    for (uint32_t idx : live_cells) {
        int r = idx / col;
        int c = idx % col;
        
        for (int dr = -1; dr <= 1; ++dr) {
            for (int dc = -1; dc <= 1; ++dc) {
                if (dr == 0 && dc == 0) continue;
                int nr = r + dr;
                int nc = c + dc;
                if (nr >= 0 && nr < row && nc >= 0 && nc < col) {
                    neighbors.push_back(nr * col + nc);
                }
            }
        }
    }
    
    std::sort(neighbors.begin(), neighbors.end());
    
    std::vector<uint32_t> next_live_cells;
    int ptr = 0;
    int n_live = live_cells.size();
    
    for (size_t i = 0; i < neighbors.size(); ) {
        uint32_t idx = neighbors[i];
        int cnt = 0;
        while (i < neighbors.size() && neighbors[i] == idx) {
            cnt++;
            i++;
        }
        
        while (ptr < n_live && live_cells[ptr] < idx) {
            ptr++;
        }
        
        bool is_alive = (ptr < n_live && live_cells[ptr] == idx);
        
        if (cnt == 3 || (cnt == 2 && is_alive)) {
            next_live_cells.push_back(idx);
        }
    }
    
    live_cells = std::move(next_live_cells);
}

inline void PrintGame() {
    std::cout << col << " " << row << "\n";
    std::string rle = "";
    int pending_b = 0;
    int pending_o = 0;
    int pending_dollar = 0;

    auto flush_b = [&]() {
        if (pending_b > 0) {
            if (pending_b == 1) rle += "b";
            else rle += std::to_string(pending_b) + "b";
            pending_b = 0;
        }
    };

    auto flush_o = [&]() {
        if (pending_o > 0) {
            if (pending_o == 1) rle += "o";
            else rle += std::to_string(pending_o) + "o";
            pending_o = 0;
        }
    };

    auto flush_dollar = [&]() {
        if (pending_dollar > 0) {
            if (pending_dollar == 1) rle += "$";
            else rle += std::to_string(pending_dollar) + "$";
            pending_dollar = 0;
        }
    };

    auto add_dollar = [&](int count) {
        if (count > 0) {
            flush_o();
            pending_b = 0;
            pending_dollar += count;
        }
    };

    auto add_b = [&](int count) {
        if (count > 0) {
            flush_o();
            flush_dollar();
            pending_b += count;
        }
    };

    auto add_o = [&](int count) {
        if (count > 0) {
            flush_b();
            flush_dollar();
            pending_o += count;
        }
    };

    int curr_r = 0;
    int curr_c = 0;

    for (uint32_t idx : live_cells) {
        int r = idx / col;
        int c = idx % col;

        if (r > curr_r) {
            add_dollar(r - curr_r);
            curr_r = r;
            curr_c = 0;
        }

        if (c > curr_c) {
            add_b(c - curr_c);
            curr_c = c;
        }

        add_o(1);
        curr_c++;
    }

    flush_o();
    rle += "!";
    std::cout << rle << "\n";
}

inline int GetLiveCell() {
    return live_cells.size();
}

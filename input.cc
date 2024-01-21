#pragma once
#include <string>
#include <iostream>
#include <queue>
#include <unistd.h>
#include "events.hh"
#include "vterminal.cc"
#include "keys.cc"

#define READ_CHUNK_SIZE 32

class Input {
    std::queue<Event> event_list;
public:
    inline size_t size() const {
        return event_list.size();
    }

    inline bool empty() const {
        return event_list.empty();
    }

    Event next() {
        Event event(event_list.front());
        event_list.pop(); 

        return event;
    }

    void poll() {
        std::string raw_input;
        char chunk[READ_CHUNK_SIZE];

        while (true) {
            size_t readno = read(STDIN_FILENO, &chunk, READ_CHUNK_SIZE);
            if (readno == -1 || readno == 0) {
                break;
            }

            raw_input.append(chunk, readno);
        }

        if (raw_input.empty()) {
            return;
        }

        KeyParser parser(raw_input);

        while (parser.has_next()) {
            Event event = parser.parse();
            event_list.push(event);
        }
    }
};

int main() {
    terminal::enableRaw();
    Input input;

    while (true) {
        input.poll();
        while(!input.empty()) {
            Event ev = input.next();
            if (ev.type == CHAR) {
                std::cout << ev.type << ": " << ev.chars << " len: " << ev.chars.length() << '\r' << std::endl;
            } else {
                std::cout << ev.type << '\r' << std::endl;
            }
        }
    }
    return 0;
}
#include <list>
#include "gapbuffer.cc"

typedef std::list<GapBuffer>::iterator LineIter;

/* Idea: 
    - Represent lines as double linked lists.
    - Each line is gap buffer.
    Pros:
    - new line insertion/deletion in the middle is fast (arrays would require shifting of all content).
    - no reallocations for whole text buffer (zero copying old array to new array).
    Cons:
    - jumping between lines will be O(n). 
    - very long minified code in single line might be a problem for single gap buffer.
*/
class TextBuffer {
    std::list<GapBuffer> lines;
    LineIter current;
public:
    // new empty file constructor
    TextBuffer() {
        lines.emplace_front(100);
        current = lines.begin();
    }

    void print_debug() {
        for (auto it = lines.begin(); it != lines.end(); ++it) {
            std::cout << it->to_str() << std::endl;
        }
    }

    void insert(const std::string &str) {
        current->insert(str);
    }

    void insert(const char *str) {
        current->insert(str);
    }

    void set_cursor(const size_t pos) {
        current->move_gap_to(pos);
    }

    void up(const signed long n) {
        std::advance(current, -n);
    }

    void down(const size_t n) {
        std::advance(current, n);
    }

    void append_line() {
        std::string substr = current->split();
        lines.emplace(std::next(current), substr);
        current++;
    }
};

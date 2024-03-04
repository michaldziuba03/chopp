#include <list>
#include "gapbuffer.cc"

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
};

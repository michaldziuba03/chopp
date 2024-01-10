> ⚠️ Early stage of development

# Chopp

Simple TUI text editor written in C++. Experimental project with messy code - I plan to rewrite this project very soon and implement concepts:
- parse syntax tree for highlighting
- use rope data structure or gap buffer for text

![image](https://github.com/michaldziuba03/chopp/assets/43048524/150d891f-67d0-4970-b55a-27d508c38876)

> Current state of the project

## Rules and goals of the project

- don't use TUI libraries like ncurses, curses etc. I want to learn how more complex TUI is handled by terminals. Project follows widely supported [vt100](https://www.vt100.net/) standard
- support searching in text
- support syntax highlighting
- support UTF-8
- make it usable for daily use

## License

This program is licensed under the GPLv3 License.

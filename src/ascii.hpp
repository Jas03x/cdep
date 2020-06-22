#ifndef ASCII_HPP
#define ASCII_HPP

inline bool is_space(char c) { return (c == ' ') || (c == '\t'); }
inline bool is_text (char c) { return ((c >= '0') && (c <= '9')) || ((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z')) || (c == '_'); }
inline bool is_path (char c) { return is_text(c) || (c == ' ') || (c == '/'); }

#endif // ASCII_HPP
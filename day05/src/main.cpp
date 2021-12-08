#include "tbox/tbox.h"
#include "tbox/stream/stream.h"
#include "parser.h"

struct textslice_t {
    textslice_t() : m_sos(nullptr), m_str(nullptr), m_end(nullptr), m_eos(nullptr) {}
    textslice_t(tb_byte_t const* str, tb_byte_t const* end) : m_sos(str), m_str(str), m_end(end), m_eos(end) {}
    textslice_t(tb_byte_t const* sos, tb_byte_t const* str, tb_byte_t const* end, tb_byte_t const* eos) : m_sos(sos), m_str(str), m_end(end), m_eos(eos) {}
    textslice_t(textslice_t const& c) : m_sos(c.m_sos), m_str(c.m_str), m_end(c.m_end), m_eos(c.m_eos) {}
    
    bool is_empty() const { return m_str == nullptr && m_end == nullptr; }
    void clear() { m_sos = nullptr; m_str = nullptr; m_end = nullptr; m_eos = nullptr; }
    void reset() { m_str = m_sos; m_end = m_eos; }

    textslice_t& operator=(const textslice_t& c) {
        if (&c != this) {
            m_sos = c.m_sos;
            m_sos = c.m_str;
            m_sos = c.m_end;
            m_sos = c.m_eos;
        }
        return *this;
    }

    tb_byte_t const* m_sos;
    tb_byte_t const* m_str;
    tb_byte_t const* m_end;
    tb_byte_t const* m_eos;
};

textslice_t compare_select(textslice_t& text, const char* str) {
    tb_byte_t const* iter = text.m_str;
    while (iter < text.m_end && *str != '\0') {
        if (*iter != *str) 
            break;
        iter++;
        str++; 
    }
    if (*str == '\0') {
        return textslice_t(text.m_sos, text.m_str, iter, text.m_eos);
    }
    return textslice_t();
}

textslice_t select_after(textslice_t const& text, textslice_t const& str) {
    return textslice_t(text.m_sos, str.m_end, text.m_end, text.m_eos);
}

textslice_t find_select_until(textslice_t const& text, const char* pivot) {
    textslice_t iter = text;
    while (iter.m_str < iter.m_end) {
        tb_byte_t const* str = iter.m_str;
        textslice_t found = compare_select(iter, pivot);
        if (!found.is_empty()) {
            return textslice_t(text.m_sos, str, iter.m_str, text.m_eos);
        }
        iter.m_str++;
    }
    return textslice_t();
}


struct textfilereader_t {
    tb_byte_t* m_start;
    tb_byte_t* m_iter;
    tb_byte_t* m_end;

    textfilereader_t() : m_start(nullptr), m_iter(nullptr), m_end(nullptr) {}

    void init(tb_buffer_t& textbuffer)
    {
        m_start = tb_buffer_data(&textbuffer);
        m_end = m_start + tb_buffer_size(&textbuffer);
        m_iter = m_start;
    }

    bool readline(textslice_t& line)
    {
        line.reset();
        if (m_iter < m_end) {
            line.m_str = m_iter;
            while (m_iter < m_end && *m_iter != '\n') {
                m_iter++;
            }
            line.m_end = m_iter;
            m_iter++;
        } else { 
            return false; 
        }
        return true;
    }
};

tb_buffer_t read_text_file(char const* filename)
{
    tb_buffer_t input_buffer;
    tb_buffer_init(&input_buffer);

    tb_stream_ref_t input_stream = tb_stream_init_from_file(filename, 0);
    if (input_stream) {
        if (tb_stream_open(input_stream)) {
            tb_long_t num_data_read = 0;
            tb_byte_t data[256];

            do {
                num_data_read = tb_stream_read(input_stream, data, 256);
                if (num_data_read > 0)
                    tb_buffer_memncat(&input_buffer, (tb_byte_t const*)data, num_data_read);
            } while (num_data_read > 0);

            num_data_read = 1;
            data[0] = '\n';
            tb_buffer_memncat(&input_buffer, (tb_byte_t const*)data, num_data_read);
        }
        tb_stream_exit(input_stream);
    }
    return input_buffer;
}

struct Line_t
{
    s16 x1;
    s16 y1;
    s16 x2;
    s16 y2;
};

tb_int_t main(tb_int_t argc, tb_char_t** argv)
{
    if (!tb_init(tb_null, tb_null)) return -1;
    
    tb_buffer_t input_buffer = read_text_file("input.txt");
    
    // example: 223,805 -> 223,548
    s64 x1,y1,x2,y2;
    xcombparser::Integer    px1(&x1), py1(&y1), px2(&x2), py2(&y2);
    xcombparser::Is         comma(',');
    xcombparser::Exact      arrow(" -> ", 4);
    xcombparser::Sequence   pp1(xcombparser::Sequence(px1, comma), py1);
    xcombparser::Sequence   pp2(xcombparser::Sequence(px2, comma), py2);
    xcombparser::Sequence   pline(xcombparser::Sequence(pp1, arrow), pp2);

    textslice_t textline;
    textfilereader_t textfile_reader;
    textfile_reader.init(input_buffer);
    while (textfile_reader.readline(textline)) {
        xcombparser::textreader_t tr((const char*)textline.m_str, (const char*)textline.m_end);
        pline.check(tr);

        Line_t line;
        line.x1 = (s16)x1;
        line.y1 = (s16)y1;
        line.x2 = (s16)x2;
        line.y2 = (s16)y2;

        tb_trace_tail("%d,%d -> %d,%d\n",x1,y1,x2,y2);
    }

    tb_buffer_exit(&input_buffer);            
    tb_exit();
    return 0;
}

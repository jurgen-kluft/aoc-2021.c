#include "parser.h"

namespace xcombparser
{
    textreader_t::textreader_t()
        : m_sos(nullptr)
        , m_str(nullptr)
        , m_end(nullptr)
    {
    }

    textreader_t::textreader_t(const char* str, const char* end)
        : m_sos(str)
        , m_str(str)
        , m_end(end)
    {
    }

    textreader_t::textreader_t(textreader_t const& t)
        : m_sos(t.m_str)
        , m_str(t.m_str)
        , m_end(t.m_end)
    {
    }

    void textreader_t::reset()
    {
        m_str = m_sos;
    }

    uchar32 textreader_t::peek()
    {
        if (m_str < m_end)
            return *m_str;
        return 0;
    }

    uchar32 textreader_t::read()
    {
        if (m_str < m_end)
            return *m_str++;
        return 0;
    }

    void textreader_t::skip()
    {
        if (m_str < m_end)
            m_str++;
    }

    bool textreader_t::valid() const
    {
        return (m_str < m_end);
    }

    textreader_t  textreader_t::select(const textreader_t& begin, const textreader_t& end)
    {
        return textreader_t();
    }

    textreader_t& textreader_t::operator =(const textreader_t& t)
    {
        m_sos = t.m_sos;
        m_str = t.m_str;
        m_end = t.m_end;
        return *this;
    }

    bool textreader_t::operator<(const textreader_t& t) const { return (t.m_str < m_str); }
    bool textreader_t::operator>(const textreader_t& t) const { return (t.m_str > m_str); }
    bool textreader_t::operator<=(const textreader_t& t) const { return (t.m_str <= m_str); }
    bool textreader_t::operator>=(const textreader_t& t) const { return (t.m_str >= m_str); }
    bool textreader_t::operator==(const textreader_t& t) const { return (t.m_str == m_str); }
    bool textreader_t::operator!=(const textreader_t& t) const { return (t.m_str != m_str); }

    uchar32 to_lower(uchar32 c) {
        if (c>='A' && c<='Z')
            return 'a' + (c - 'A');
        return c;
    }

    s32 to_digit(uchar32 c) {
        if (c>='0' && c<='9')
            return s32(c - '0');
        return 0;
    }
    bool is_digit(uchar32 c) {return (c>='0' && c<='9');}

    Any          sAny;
    Alphabet     sAlphabet;
    AlphaNumeric sAlphaNumeric;
    Digit        sDigit;
    Decimal      sDecimal;
    EndOfLine    sEOL;
    EndOfText    sEOT;
    Hex          sHex;
    WhiteSpace   sWhitespace;
    Word         sWord;
    Email          sEmail;
    Host           sHost;
    IPv4           sIPv4;
    Phone          sPhone;
    ServerAddress  sServerAddress;
    Uri            sURI;

    bool Not::check(textreader_t& _reader)
    {
        textreader_t start = _reader;
        if (!m_tokenizer_a.check(_reader))
        {
            return true;
        }
        _reader = start;
        return false;
    }

    bool Or::check(textreader_t& _reader)
    {
        textreader_t start = _reader;
        if (!m_tokenizer_a.check(_reader))
        {
            _reader = (start);
            if (!m_tokenizer_b.check(_reader))
            {
                _reader = (start);
                return false;
            }
        }
        return true;
    }

    bool And::check(textreader_t& _reader)
    {
        textreader_t c1 = _reader;
        if (!m_tokenizer_a.check(_reader))
        {
            _reader=(c1);
            return false;
        }
        c1 = _reader;

        textreader_t c2 = _reader;
        if (!m_tokenizer_b.check(_reader))
        {
            _reader=(c2);
            return false;
        }
        c2 = _reader;

        textreader_t c = ((c2 < c1) ? c2 : c1);
        _reader=(c);
        return true;
    }

    bool Sequence::check(textreader_t& _reader)
    {
        textreader_t start = _reader;

        if (!m_tokenizer_a.check(_reader))
        {
            _reader=(start);
            return false;
        }

        if (!m_tokenizer_b.check(_reader))
        {
            _reader=(start);
            return false;
        }

        return true;
    }

    bool Sequence3::check(textreader_t& _reader)
    {
        textreader_t start = _reader;
        if (!m_tokenizer_a.check(_reader))
        {
            _reader = start;
            return false;
        }
        if (!m_tokenizer_b.check(_reader))
        {
            _reader = start;
            return false;
        }
        if (!m_tokenizer_c.check(_reader))
        {
            _reader = start;
            return false;
        }

        return true;
    }

    bool Within::check(textreader_t& _reader)
    {
        textreader_t start = _reader;

        u64 i = 0;
        for (; i < m_max; i++)
        {
            if (!m_tokenizer_a.check(_reader))
                break;
        }

        if (i >= m_min && i <= m_max)
        {
            return true;
        }

        _reader = start;
        return false;
    }

    bool Times::check(textreader_t& _reader) { return Within(m_max, m_max, m_tokenizer_a).check(_reader); }
    bool OneOrMore::check(textreader_t& _reader) { return Within(1, -1, m_tokenizer_a).check(_reader); }
    bool ZeroOrOne::check(textreader_t& _reader) { return Within(0, 1, m_tokenizer_a).check(_reader); }
    bool While::check(textreader_t& _reader) { return Within(0, -1, m_tokenizer_a).check(_reader); }
    bool Until::check(textreader_t& _reader)
    {
        Not n(m_tokenizer_a);
        return (While(n)).check(_reader);
    }

    bool Extract::check(textreader_t& _reader)
    {
        textreader_t start  = _reader;
        bool            result = m_tokenizer_a.check(_reader);
        if (result)
        {
            textreader_t end = _reader;
            m_selection         = _reader.select(start, end);
            return result;
        }
        _reader = start;
        return result;
    }

    bool ReturnToCallback::check(textreader_t& _reader)
    {
        textreader_t start  = _reader;
        bool            result = m_tokenizer_a.check(_reader);
        if (result)
        {
            m_cb(_reader, start);
        }
        _reader = start;
        return result;
    }

    bool Enclosed::check(textreader_t& _reader)
    {
        Exact open(m_open);
        Exact close(m_close);
        Sequence        a(open, m_tokenizer_a);
        Sequence        b(a, close);
        return b.check(_reader);
    }

    bool Any::check(textreader_t& _reader)
    {
        if (!_reader.valid())
            return false;
        _reader.skip();
        return true;
    } 

    bool In::check(textreader_t& _reader)
    {
        if (!_reader.valid())
            return false;

        m_input.reset();
        textreader_t inputcursor = m_input;
        uchar32 const   s           = _reader.peek();
        while (m_input.valid())
        {
            uchar32 const c = m_input.read();
            if (c == s)
            {
                _reader.skip();
                return true;
            }
        }
        return false;
    }

    bool Between::check(textreader_t& _reader)
    {
        uchar32 c = _reader.peek();
        if (c >= m_lower && c < m_upper)
        {
            _reader.skip();
            return true;
        }
        return false;
    }

    bool Alphabet::check(textreader_t& _reader) { return (m_lower_case.check(_reader) | m_upper_case.check(_reader)); }
    bool Digit::check(textreader_t& _reader) { return m_digit.check(_reader); }
    bool Hex::check(textreader_t& _reader)
    {
        if (!m_digit.check(_reader))
        {
            if (!m_lower_case.check(_reader))
            {
                if (!m_upper_case.check(_reader))
                {
                    return false;
                }
            }
        }
        return true;
    }

    bool AlphaNumeric::check(textreader_t& _reader)
    {
        Or r(sAlphabet, sDigit);
        return r.check(_reader);
    }

    bool Exact::check(textreader_t& _reader)
    {
        textreader_t rcursor = _reader;
        textreader_t rinput = m_input;
        while (rinput.valid())
        {
            uchar32 a = _reader.peek();
            uchar32 b = rinput.peek();
            if (a != b)
            {
                _reader = rcursor;
                return false;
            }
			rinput.skip();
            _reader.skip();
        }
        return true;
    }

    bool Like::check(textreader_t& _reader)
    {
        textreader_t rcursor = _reader;
        textreader_t icursor = m_input;
        while (m_input.valid())
        {
            uchar32 a = _reader.peek();
            uchar32 b = m_input.peek();
            if (a != b && (to_lower(a) != to_lower(b)))
            {
                _reader = rcursor;
                return false;
            }
            m_input.skip();
            _reader.skip();
        }
        return true;
    }

    bool WhiteSpace::check(textreader_t& _reader) { return m_whitespace.check(_reader); }

    bool Is::check(textreader_t& _reader)
    {
        if (_reader.peek() == m_char)
        {
            _reader.skip();
            return true;
        }
        return false;
    }

    bool Decimal::check(textreader_t& _reader) { return OneOrMore(sDigit).check(_reader); }
    bool Word::check(textreader_t& _reader) { return OneOrMore(sAlphabet).check(_reader); }
    bool EndOfText::check(textreader_t& _reader) { return (_reader.peek() == ('\0')); }

#if defined(PLATFORM_WINDOWS)
    bool EndOfLine::check(textreader_t& _reader) { return Exact("\r\n", 2).check(_reader); }
#else
    bool EndOfLine::check(textreader_t& _reader) { return Exact("\n", 1).check(_reader); }
#endif

    bool Integer::check(textreader_t& _reader)
    {
        s64             value       = 0;
        textreader_t start      = _reader;
        uchar32         c           = _reader.peek();
        bool            is_negative = (c == '-');
        if (is_negative)
            _reader.skip();
        while (_reader.valid())
        {
            c = _reader.peek();
            if (!is_digit(c))
                break;
            value = (value * 10) + to_digit(c);
            _reader.skip();
        }
        if (start == _reader)
            return false;
        if (is_negative)
            value = -value;
        if (value >= m_min && value <= m_max)
        {
            if (m_out != nullptr)
                *m_out = value;
            return true;
        }
        _reader = start;
        return false;
    }

    bool Float::check(textreader_t& _reader)
    {
        f32             value       = 0.0f;
        textreader_t start      = _reader;
        uchar32         c           = _reader.peek();
        bool            is_negative = c == '-';
        if (is_negative)
            _reader.skip();
        while (_reader.valid())
        {
            c = _reader.peek();
            if (!is_digit(c))
                break;
            value = (value * 10) + to_digit(c);
        }
        if (c == '.')
        {
            _reader.skip();
            f32 mantissa = 10.0f;
            while (_reader.valid())
            {
                c = _reader.peek();
                if (!is_digit(c))
                    break;
                value = value + f32(to_digit(c)) / mantissa;
                mantissa *= 10.0f;
            }
        }
        if (start == _reader)
            return false;
        if (is_negative)
            value = -value;
        if (value >= m_min && value <= m_max)
        {
            return true;
        }
        _reader = start;
        return false;
    }


    bool IPv4::check(textreader_t& _reader) { return m_ipv4.check(_reader); }

    bool Host::check(textreader_t& _reader)
    {
        if (sIPv4.check(_reader))
        {
            return true;
        }
        OneOrMore  oom_an(sAlphaNumeric);
        Is         dash('-');
        Is         dot('.');
        Sequence   dash_oom_an(dash, oom_an);
        Sequence   dot_oom_an(dot, oom_an);
        ZeroOrMore zom_dash_oom_an(dash_oom_an);
        Sequence   dot_oom_an_zom_dash_oom_an(dot_oom_an, zom_dash_oom_an);
        ZeroOrMore host_tail(dot_oom_an_zom_dash_oom_an);
        Sequence   host_head(oom_an, host_tail);
        Sequence   host(host_head, host_tail);
        return host.check(_reader);
    }

    static const char* sValidEmailUriChars = "!#$%&'*+/=?^_`{|}~-";

    bool Email::check(textreader_t& _reader)
    {
        In         validchars(sValidEmailUriChars, 19);
        Or         valid(sAlphaNumeric, validchars);
        OneOrMore  oom_valid(valid);
        Is         dot('.');
        Sequence   dot_valid(dot, valid);
        ZeroOrMore zom_dot_valid(dot_valid);
        Is         mt('@');
        Sequence   a(oom_valid, zom_dot_valid);
        Sequence   b(mt, sHost);
        Sequence   email(a, b);
        return email.check(_reader);
    }
    bool Phone::check(textreader_t& _reader)
    {
        Is         plus('+');
        ZeroOrMore zom_plus(plus);
        Is         open('(');
        Is         close('(');
        OneOrMore  oomdigit(sDigit);
        Sequence3  open_oomdigit_close(open, oomdigit, close);
        ZeroOrMore zom_open_oomdigit_close(open_oomdigit_close);
        ZeroOrMore zom_whitespace(sWhitespace);
        In         spaceordash(" -", 2);
        Sequence   spaceordash_oomdigit(spaceordash, oomdigit);
        ZeroOrMore zom_spaceordash_oomdigit(spaceordash_oomdigit);

        Sequence zom_open_oomdigit_close_whitespace(zom_open_oomdigit_close, zom_whitespace);
        return zom_open_oomdigit_close_whitespace.check(_reader);
    }

    bool ServerAddress::check(textreader_t& _reader) { return false; }
    bool Uri::check(textreader_t& _reader) { return false; }
}

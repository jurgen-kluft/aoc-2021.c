#ifndef __XTEXT_XPARSER_H__
#define __XTEXT_XPARSER_H__
#pragma once

#include "tbox/tbox.h"

typedef tb_uint8_t u8;
typedef tb_sint8_t s8;

typedef tb_uint16_t u16;
typedef tb_sint16_t s16;

typedef tb_uint32_t u32;
typedef tb_sint32_t s32;

typedef tb_uint64_t u64;
typedef tb_sint64_t s64;

typedef tb_uint32_t uchar32;

typedef tb_float_t f32;
typedef tb_double_t f64;

typedef tb_byte_t byte;

namespace xcombparser
{
    class textreader_t
    {
    public:
        textreader_t();
        textreader_t(const char* str, const char* end);
        textreader_t(textreader_t const&);

        void reset();
        uchar32 peek();
        uchar32 read();
        void skip();
        bool valid() const;

        textreader_t  select(const textreader_t& begin, const textreader_t& end);
        textreader_t& operator =(const textreader_t&);
  
        bool operator<(const textreader_t&) const;
        bool operator>(const textreader_t&) const;
        bool operator<=(const textreader_t&) const;
        bool operator>=(const textreader_t&) const;
        bool operator==(const textreader_t&) const;
        bool operator!=(const textreader_t&) const;

        const char* m_sos;
        const char* m_str;
        const char* m_end;
    };

    class tokenizer_t
    {
    public:
        virtual bool check(textreader_t&) = 0;
    };

    class tokenizer_1_t : public tokenizer_t
    {
    public:
        tokenizer_1_t(tokenizer_t& toka)
            : m_tokenizer_a(toka)
        {
        }

    protected:
        tokenizer_t& m_tokenizer_a;
    };

    class tokenizer_2_t : public tokenizer_t
    {
    public:
        tokenizer_2_t(tokenizer_t& toka, tokenizer_t& tokb)
            : m_tokenizer_a(toka)
            , m_tokenizer_b(tokb)
        {
        }

    protected:
        tokenizer_t& m_tokenizer_a;
        tokenizer_t& m_tokenizer_b;
    };

    class Not : public tokenizer_1_t
    {
    public:
        inline Not(tokenizer_t& toka)
            : tokenizer_1_t(toka)
        {
        }
        virtual bool check(textreader_t&);
    };

    class Or : public tokenizer_2_t
    {
    public:
        inline Or(tokenizer_t& toka, tokenizer_t& tokb)
            : tokenizer_2_t(toka, tokb)
        {
        }
        virtual bool check(textreader_t&);
    };

    class And : public tokenizer_2_t
    {
    public:
        And(tokenizer_t& toka, tokenizer_t& tokb)
            : tokenizer_2_t(toka, tokb)
        {
        }
        virtual bool check(textreader_t&);
    };

    class Sequence : public tokenizer_2_t
    {
    public:
        inline Sequence(tokenizer_t& toka, tokenizer_t& tokb)
            : tokenizer_2_t(toka, tokb)
        {
        }
        virtual bool check(textreader_t&);
    };

    class Sequence3 : public tokenizer_2_t
    {
        tokenizer_t& m_tokenizer_c;

    public:
        inline Sequence3(tokenizer_t& toka, tokenizer_t& tokb, tokenizer_t& tokc)
            : tokenizer_2_t(toka, tokb)
            , m_tokenizer_c(tokc)
        {
        }
        virtual bool check(textreader_t&);
    };

    class Within : public tokenizer_1_t
    {
        u64 m_min, m_max;

    public:
        Within(u64 min, u64 max, tokenizer_t& toka)
            : tokenizer_1_t(toka)
            , m_min(min)
            , m_max(max)
        {
        }
        Within(u64 max, tokenizer_t& toka)
            : tokenizer_1_t(toka)
            , m_min(0)
            , m_max(max)
        {
        }
        Within(tokenizer_t& toka)
            : tokenizer_1_t(toka)
            , m_min(0)
            , m_max(0xffffffffffffffffUL)
        {
        }
        virtual bool check(textreader_t&);
    };

    class Times : public tokenizer_1_t
    {
        s32 m_max;

    public:
        inline Times(s32 max, tokenizer_t& toka)
            : tokenizer_1_t(toka)
            , m_max(max)
        {
        }
        virtual bool check(textreader_t&);
    };

    class OneOrMore : public tokenizer_1_t
    {
    public:
        inline OneOrMore(tokenizer_t& toka)
            : tokenizer_1_t(toka)
        {
        }
        virtual bool check(textreader_t&);
    };

    class ZeroOrOne : public tokenizer_1_t
    {
    public:
        inline ZeroOrOne(tokenizer_t& toka)
            : tokenizer_1_t(toka)
        {
        }
        virtual bool check(textreader_t&);
    };
    typedef ZeroOrOne Optional;
    typedef ZeroOrOne _0Or1;

    class While : public tokenizer_1_t
    {
    public:
        inline While(tokenizer_t& toka)
            : tokenizer_1_t(toka)
        {
        }
        virtual bool check(textreader_t&);
    };
    typedef While ZeroOrMore;

    class Until : public tokenizer_1_t
    {
    public:
        inline Until(tokenizer_t& toka)
            : tokenizer_1_t(toka)
        {
        }
        virtual bool check(textreader_t&);
    };

    class Extract : public tokenizer_1_t
    {
        textreader_t& m_selection;

    public:
        inline Extract(textreader_t& m1, tokenizer_t& toka)
            : tokenizer_1_t(toka)
            , m_selection(m1)
        {
        }
        virtual bool check(textreader_t&);
    };

    typedef void (*CallBack)(textreader_t&, textreader_t&);
    class ReturnToCallback : public tokenizer_1_t
    {
        CallBack m_cb;

    public:
        inline ReturnToCallback(CallBack cb, tokenizer_t& toka)
            : tokenizer_1_t(toka)
            , m_cb(cb)
        {
        }
        virtual bool check(textreader_t&);
    };

    class Enclosed : public tokenizer_1_t
    {
        textreader_t m_open;
        textreader_t m_close;

    public:
        inline Enclosed(textreader_t open, textreader_t close, tokenizer_t& toka)
            : tokenizer_1_t(toka)
            , m_open(open)
            , m_close(close)
        {
        }
        virtual bool check(textreader_t&);
    };


    class Any : public tokenizer_t
    {
    public:
        Any() {}
        virtual bool check(textreader_t&);
    };
    extern Any sAny;

    class In : public tokenizer_t
    {
        textreader_t m_input;

    public:
        In() {}
        In(const char* str, u32 len) : m_input(str,str+len) {}
        In(textreader_t input)
            : m_input(input)
        {
        }
        virtual bool check(textreader_t&);
    };

    class Between : public tokenizer_t
    {
        uchar32 m_lower;
        uchar32 m_upper;

    public:
        Between()
            : m_lower('a')
            , m_upper('z')
        {
        }
        Between(uchar32 lower, uchar32 upper)
            : m_lower(lower)
            , m_upper(upper)
        {
        }
        virtual bool check(textreader_t&);
    };

    // TODO:class SmallLetter;
    // TODO:class CapitalLetter;
    // TODO:Class Letter;
    // TODO:Class Punctuation

    class Alphabet : public tokenizer_t
    {
        Between m_lower_case;
        Between m_upper_case;

    public:
        Alphabet()
            : m_lower_case('a', 'z')
            , m_upper_case('A', 'Z')
        {
        }
        virtual bool check(textreader_t&);
    };
    extern Alphabet sAlphabet;

    class Digit : public tokenizer_t
    {
        Between m_digit; //@TODO; make static
    public:
        Digit()
            : m_digit('0', '9')
        {
        }
        virtual bool check(textreader_t&);
    };
    extern Digit sDigit;

    class Hex : public tokenizer_t
    {
        Digit   m_digit;
        Between m_lower_case;
        Between m_upper_case;

    public:
        Hex()
            : m_lower_case('a', 'f')
            , m_upper_case('A', 'F')
        {
        }
        virtual bool check(textreader_t&);
    };
    extern Hex sHex;

    class AlphaNumeric : public tokenizer_t
    {
    public:
        AlphaNumeric() {}
        virtual bool check(textreader_t&);
    };
    extern AlphaNumeric sAlphaNumeric;

    class Exact : public tokenizer_t
    {
        textreader_t m_input;

    public:
        Exact() {}
        Exact(const char* str, u32 len) : m_input(str, str+len) {}
        Exact(const char* str, const char* end) : m_input(str, end) {}
        Exact(textreader_t input)
            : m_input(input)
        {
        }
        virtual bool check(textreader_t&);
    };

    class Like : public tokenizer_t
    {
        textreader_t  m_input;

    public:
        Like() {}
        Like(textreader_t input)
            : m_input(input)
        {
        }
        virtual bool check(textreader_t&);
    };

    class WhiteSpace : public tokenizer_t
    {
        In m_whitespace;

    public:
        WhiteSpace() : m_whitespace(" \t\n\r", 4)
        {
        }
        virtual bool check(textreader_t&);
    };
    extern WhiteSpace sWhitespace;

    class Is : public tokenizer_t
    {
        uchar32 m_char;

    public:
        Is()
            : m_char(' ')
        {
        }
        Is(uchar32 c)
            : m_char(c)
        {
        }
        virtual bool check(textreader_t&);
    };

    class Decimal : public tokenizer_t
    {
    public:
        Decimal() {}
        virtual bool check(textreader_t&);
    };
    extern Decimal sDecimal;

    class Word : public tokenizer_t
    {
    public:
        Word() {}
        virtual bool check(textreader_t&);
    };
    extern Word sWord;

    class EndOfText : public tokenizer_t
    {
    public:
        EndOfText() {}
        virtual bool check(textreader_t&);
    };
    extern EndOfText sEOT;

    class EndOfLine : public tokenizer_t
    {
    public:
        EndOfLine() {}
        virtual bool check(textreader_t&);
    };
    extern EndOfLine sEOL;

    class Integer : public tokenizer_t
    {
        s64 m_min;
        s64 m_max;
        s64* m_out;

    public:
        Integer()
            : m_min(0)
            , m_max(0x7fffffffffffffffL)
            , m_out(nullptr)
        {
        }
        Integer(s64* out)
            : m_min(0)
            , m_max(0x7fffffffffffffffL)
            , m_out(out)
        {
        }
        Integer(s64 max)
            : m_min(0)
            , m_max(max)
            , m_out(nullptr)
        {
        }
        Integer(s64 min, s64 max)
            : m_min(min)
            , m_max(max)
            , m_out(nullptr)
        {
        }
        void set_out(s64* integer) { m_out = integer; }

        virtual bool check(textreader_t&);
    };

    class Float : public tokenizer_t
    {
        f32 m_min;
        f32 m_max;

    public:
        Float()
            : m_min(0.0f)
            , m_max(3.402823e+38f)
        {
        }
        Float(f32 max)
            : m_min(0.0f)
            , m_max(max)
        {
        }
        Float(f32 min, f32 max)
            : m_min(min)
            , m_max(max)
        {
        }
        virtual bool check(textreader_t&);
    };

    class IPv4 : public tokenizer_t
    {
        Within   m_d3;
        Integer  m_b8;
        And      m_sub;
        Is       m_dot;
        Sequence m_bad;
        Times    m_domain;
        Sequence m_ipv4;

    public:
        IPv4()
            : m_d3(1, 3, sDigit)
            , m_b8(255)
            , m_sub(m_d3, m_b8)
            , m_dot('.')
            , m_bad(m_sub, m_dot)
            , m_domain(3, m_bad)
            , m_ipv4(m_domain, m_sub)
        {
        }
        virtual bool check(textreader_t&);
    };
    extern IPv4 sIPv4;

    class Host : public tokenizer_t
    {
    public:
        virtual bool check(textreader_t&);
    };
    extern Host sHost;

    class Email : public tokenizer_t
    {
    public:
        virtual bool check(textreader_t&);
    };
    extern Email sEmail;

    class Phone : public tokenizer_t
    {
    public:
        virtual bool check(textreader_t&);
    };
    extern Phone sPhone;

    class ServerAddress : public tokenizer_t
    {
    public:
        virtual bool check(textreader_t&);
    };
    extern ServerAddress sServerAddress;

    class Uri : public tokenizer_t
    {
    public:
        virtual bool check(textreader_t&);
    };
    extern Uri sURI;

}

#endif
#include "tbox/tbox.h"
#include "tbox/stream/stream.h"

tb_int_t main(tb_int_t argc, tb_char_t** argv)
{
    // init tbox
    if (!tb_init(tb_null, tb_null)) return -1;

    // trace
    tb_trace_i("hello tbox!");

    tb_buffer_t input_buffer;
    tb_buffer_init(&input_buffer);

    tb_stream_ref_t input_stream = tb_stream_init_from_file("input.txt", 0);
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

            tb_trace_i("%d", tb_buffer_size(&input_buffer));
            
            tb_byte_t* text_start = tb_buffer_data(&input_buffer);
            tb_byte_t* text_iter = text_start;
            tb_byte_t* text_end = text_start + tb_buffer_size(&input_buffer);
            while (text_iter < text_end) {

                tb_byte_t* line = text_iter;
                while (text_iter < text_end && *text_iter != '\n') {
                    text_iter++;
                }
                if (*text_iter == '\n') {
                    *text_iter = '\0';
                }
                text_iter++;

                tb_trace_i("%s", line);
            }

        }
        tb_stream_exit(input_stream);
    }

    // exit tbox
    tb_buffer_exit(&input_buffer);            
    tb_exit();
    return 0;
}

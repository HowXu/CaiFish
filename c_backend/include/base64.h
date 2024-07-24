#include <Windows.h>
#include <iostream>
#include <string>
#include <fstream>
#include <cstring>

// base64解码函数 https://blog.csdn.net/qq_31681017/article/details/93161285

bool is_readable(const std::string &file)
{
    wchar_t szBuffer[1024 * 10];
    OemToCharW(file.c_str(), szBuffer);
    std::ifstream fichier(szBuffer);
    return !fichier.fail();
}

// Base 64

static const std::string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static const int bufferSize = 1024 * 10;

static inline bool is_base64(unsigned char c)
{
    return (isalnum(c) || (c == '+') || (c == '/'));
}

std::string base64_encode(unsigned char const *bytes_to_encode, unsigned int in_len)
{
    std::string ret;
    int i = 0;
    int j = 0;
    unsigned char char_array_3[3];
    unsigned char char_array_4[4];

    while (in_len--)
    {
        char_array_3[i++] = *(bytes_to_encode++);
        if (i == 3)
        {
            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;

            for (i = 0; (i < 4); i++)
                ret += base64_chars[char_array_4[i]];
            i = 0;
        }
    }

    if (i)
    {
        for (j = i; j < 3; j++)
            char_array_3[j] = '\0';

        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);

        for (j = 0; (j < i + 1); j++)
            ret += base64_chars[char_array_4[j]];

        while ((i++ < 3))
            ret += '=';
    }

    return ret;
}

std::string hex_string(std::string hexstr)
{
    std::string str = "";
    str.resize((hexstr.size() + 1) / 2);
    for (size_t i = 0, j = 0; i < str.size(); i++, j++)
    {
        char at = '@';
        str[i] = (hexstr[j] & at ? hexstr[j] + 9 : hexstr[j]) << 4, j++;
        str[i] |= (hexstr[j] & at ? hexstr[j] + 9 : hexstr[j]) & 0xF;
    }
    return str;
}

std::string string_hex(std::string str, const bool capital = false)
{
    std::string hexstr = "";
    hexstr.resize(str.size() * 2);
    static const char a = capital ? 0x40 : 0x60;
    for (size_t i = 0; i < str.size(); i++)
    {
        char c = (str[i] >> 4) & 0xF;
        hexstr[i * 2] = c > 9 ? (c - 9) | a : c | '0';
        hexstr[i * 2 + 1] = (str[i] & 0xF) > 9 ? (str[i] - 9) & 0xF | a : str[i] & 0xF | '0';
    }
    return hexstr;
}

std::string char_to_string(char x[], int size_recv)
{
    int num_car = 0;
    int stop_while = size_recv;
    std::string output = "";

    while (stop_while > 0)
    {
        output += x[num_car];
        if (num_car < size_recv)
        {
            num_car++;
        }

        stop_while--;
    }

    return output;
}

std::string str_base64_encode(std::string str)
{
    return base64_encode(reinterpret_cast<const unsigned char *>(str.c_str()), str.length());
}

std::string base64_decode(std::string const &encoded_string)
{
    int in_len = encoded_string.size();
    int i = 0;
    int j = 0;
    int in_ = 0;
    unsigned char char_array_4[4], char_array_3[3];
    std::string ret;

    while (in_len-- && (encoded_string[in_] != '=') && is_base64(encoded_string[in_]))
    {
        char_array_4[i++] = encoded_string[in_];
        in_++;
        if (i == 4)
        {
            for (i = 0; i < 4; i++)
                char_array_4[i] = base64_chars.find(char_array_4[i]);

            char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
            char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

            for (i = 0; (i < 3); i++)
                ret += char_array_3[i];
            i = 0;
        }
    }

    if (i)
    {
        for (j = 0; j < i; j++)
            char_array_4[j] = base64_chars.find(char_array_4[j]);

        char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
        char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);

        for (j = 0; (j < i - 1); j++)
            ret += char_array_3[j];
    }

    return ret;
}

// Decode file

int decode_file_base64(std::string path_in, std::string path_out)
{
    std::string path_file = path_in;

    if (is_readable(path_file))
    {
        FILE *file_input;
        FILE *file_output;
        fopen_s(&file_input, path_file.c_str(), "ab+");
        fopen_s(&file_output, path_out.c_str(), "ab+");

        char buff_read[bufferSize];

        if (file_input != NULL)
        {

            std::string str_encode_buff = "";
            bool lb_read = true;
            while (lb_read)
            {
                lb_read = fread(buff_read, sizeof(buff_read), 1, file_input);
                str_encode_buff += char_to_string(buff_read, sizeof(buff_read));
                memset(buff_read, 0, sizeof(buff_read));
            }
            str_encode_buff = base64_decode(str_encode_buff);

            fwrite(str_encode_buff.c_str(), strlen(string_hex(str_encode_buff).c_str()) / 2, 1, file_output);

            fclose(file_input);
            fclose(file_output);
        }
        else
        {
            return -1;
        }
    }
    else
    {
        return -2;
    }
    return 0;
}

int decode_form_text(std::string base64, std::string path_out)
{
    FILE *file_out;
    fopen_s(&file_out, path_out.c_str(), "ab+");
    std::string str_encode_buff = base64_decode(base64);
    fwrite(str_encode_buff.c_str(), strlen(string_hex(str_encode_buff).c_str()) / 2, 1, file_out);
    fclose(file_out);
    return 0;
}

int decode_string_base64_file(std::string path_in, std::string path_out)
{
    std::string path_file = path_in;

    // if (is_readable(path_file))
    //{
    // FILE *file_input;
    FILE *file_output;
    // fopen_s(&file_input, path_file.c_str(), "ab+");
    fopen_s(&file_output, path_out.c_str(), "ab+");

    // char buff_read[1];

    if (!path_file.empty())
    {
        // std::string str_encode_buff = "";
        // while (fread(buff_read, sizeof(buff_read), 1, file_input))
        //{
        // str_encode_buff += char_to_string(buff_read, sizeof(buff_read));
        // memset(buff_read, 0, sizeof(buff_read));
        //}
        path_file = base64_decode(path_file);

        fwrite(path_file.c_str(), strlen(string_hex(path_file).c_str()) / 2, 1, file_output);

        // fclose(file_input);
        fclose(file_output);
    }
    else
    {
        return -1;
    }
    //}
    return 0;
}

int encode_file_base64(std::string path_in, std::string path_out)
{
    std::string path_file = path_in;

    if (is_readable(path_file))
    {
        FILE *file_input;
        FILE *file_output;

        fopen_s(&file_input, path_file.c_str(), "ab+");
        fopen_s(&file_output, path_out.c_str(), "ab+");

        char buff_read[bufferSize];

        if (file_input != NULL)
        {
            std::string str_encode_buff = "";
            bool lb_read = true;
            while (lb_read)
            {
                lb_read = fread(buff_read, sizeof(buff_read), 1, file_input);
                str_encode_buff += char_to_string(buff_read, sizeof(buff_read));
                memset(buff_read, 0, sizeof(buff_read));
            }

            str_encode_buff = str_base64_encode(str_encode_buff);
            fwrite(str_encode_buff.c_str(), strlen(string_hex(str_encode_buff).c_str()) / 2, 1, file_output);

            fclose(file_input);
            fclose(file_output);
        }
        else
        {
            return -1;
        }
    }
    else
    {
        return -2;
    }

    return 0;
}

int encode_file_base64_string(std::string path_in, OUT char *content)
{
    std::string path_file = path_in;

    if (is_readable(path_file))
    {
        FILE *file_input;
        // FILE *file_output;

        fopen_s(&file_input, path_file.c_str(), "ab+");
        // fopen_s(&file_output, path_out.c_str(), "ab+");

        char buff_read[bufferSize];

        if (file_input != NULL)
        {
            std::string str_encode_buff = "";
            bool lb_read = true;
            while (lb_read)
            {
                lb_read = fread(buff_read, sizeof(buff_read), 1, file_input);
                str_encode_buff += char_to_string(buff_read, sizeof(buff_read));
                memset(buff_read, 0, sizeof(buff_read));
            }

            str_encode_buff = str_base64_encode(str_encode_buff);
            // fwrite(str_encode_buff.c_str(), strlen(string_hex(str_encode_buff).c_str()) / 2, 1, file_output);

            fclose(file_input);
            // fclose(file_output);
            strcpy_s(content, strlen(str_encode_buff.c_str()) + 1, str_encode_buff.c_str());
            // strcpy(content, str_encode_buff.c_str);
            // content = str_encode_buff;
        }
        else
        {
            return -1;
        }
    }
    else
    {
        return -2;
    }

    return 0;
}

std::string encode_file_base64_string(std::string path_in)
{
    std::string path_file = path_in;
    std::string str_encode_buff = "";
    if (is_readable(path_file))
    {
        FILE *file_input;
        // FILE *file_output;

        fopen_s(&file_input, path_file.c_str(), "ab+");
        // fopen_s(&file_output, path_out.c_str(), "ab+");

        char buff_read[bufferSize];

        if (file_input != NULL)
        {
            bool lb_read = true;
            while (lb_read)
            {
                lb_read = fread(buff_read, sizeof(buff_read), 1, file_input);
                str_encode_buff += char_to_string(buff_read, sizeof(buff_read));
                memset(buff_read, 0, sizeof(buff_read));
            }

            str_encode_buff = str_base64_encode(str_encode_buff);
            // fwrite(str_encode_buff.c_str(), strlen(string_hex(str_encode_buff).c_str()) / 2, 1, file_output);

            fclose(file_input);
            // fclose(file_output);
            str_encode_buff;
        }
        else
        {
            str_encode_buff = "-1";
        }
    }
    else
    {
        str_encode_buff = "-2";
    }

    return str_encode_buff;
}
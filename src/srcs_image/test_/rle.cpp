#include <iostream>
#include <string>
#include <array>
#include <vector>
#include <cmath>
#include <cassert>
#include <fstream>
#include <cstdint>

class Run_length
{
    private:
        std::string file_content;
        std::size_t size_of_input_file;

        std::size_t size_of_rle_compressed_string;
        std::string file_output_string;

    public: 
        void read_input_file(std::string file_name)
        {
            std::ifstream input_file{file_name};
            
            if(input_file.is_open())
            {
                input_file.seekg(0,std::ios::end);
                size_of_input_file = input_file.tellg();
                input_file.seekg(0, std::ios::beg);
                file_content.resize(size_of_input_file);
                input_file.read(&file_content[0], size_of_input_file);
                input_file.close();
            }
            else
            {
                std::cout<<"FILE NOT FOUND";
                std::exit;
            }

        }

        void rle_compress()
        {
            size_t file_traverse_index{0}; 
            std::string rle_compressed_string;
            std::string character_repeat_list;
            size_t& i = file_traverse_index;
            while(i < size_of_input_file)
            {   
                char current_character = file_content.at(i);
                rle_compressed_string += current_character;
                if(i+1 == size_of_input_file) break;
                if(file_content.at(i+1) == current_character)
                {
                    std::uint8_t count =2;
                    rle_compressed_string += current_character;
                    i++;
                    if(i + 1 == size_of_input_file) 
                    {
                        character_repeat_list += count;
                        break;
                    }
                    while(file_content.at(i+1) == current_character)
                    {
                        if(count == 255)    break;
                        count++;
                        i++;
                        if(i + 1 >= size_of_input_file) break;
                    }
                    character_repeat_list += count;
                }
                i++;
            }
            file_content.clear();
            size_of_rle_compressed_string = rle_compressed_string.size();
            rle_compressed_string += character_repeat_list;

            character_repeat_list.clear();

            file_output_string.assign( reinterpret_cast<char*>(&size_of_rle_compressed_string)
                                        , sizeof(size_of_rle_compressed_string));
            file_output_string += rle_compressed_string;
        }
        void write_to_file(std::string file_name)
        {
            std::ofstream output_file{file_name};
            if(output_file.is_open())
            {
                output_file.write(file_output_string.c_str(), file_output_string.size());
                output_file.close();
            }
            else
            {
                std::cout<<"FILE NOT FOUND";
                std::exit;
            }
        }

};

int main(int argc, char* argv[])
{
     if(argc != 2)
    {
        std::cout <<"\n Please provide file to compress\n";
        return -2;
    }
    std::string file_name{argv[1]};
    
    Run_length compress_rle;
    compress_rle.read_input_file(file_name);
    compress_rle.rle_compress();
    compress_rle.write_to_file(file_name);
    return 0;   
    
}
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

        std::size_t size_of_rle_compressed_string;
        std::string list_of_runs;

        std::string compressed_string;
        std::string file_output_string;

        std::size_t size_of_input_file;
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
        
        void decode_file_structure()
        {
            size_t traverse_index =0;
            //1st part = 8bytes of data
            size_of_rle_compressed_string = *reinterpret_cast<size_t*>(file_content.substr(0,sizeof(size_t)).data());
            traverse_index = sizeof(size_t);
            //2nd part of file content
            compressed_string = file_content.substr(traverse_index, size_of_rle_compressed_string);
            traverse_index += size_of_rle_compressed_string;

            std::size_t number_of_characters_in_list = size_of_input_file - traverse_index + 1;
            list_of_runs = file_content.substr(traverse_index,number_of_characters_in_list);
            file_content.clear();
        }
        void rle_decompress()
        {
            std::size_t list_traverse_index{0};//TBM1 use of stack to just pop
            for(size_t i = 0;i < size_of_rle_compressed_string;)
            {
                char current_char = compressed_string[i];
                file_output_string += current_char;

                if( i + 1 >= size_of_rle_compressed_string) break;
                
                if(compressed_string[i+1] == current_char)
                {
                    std::uint8_t count = list_of_runs.at(list_traverse_index);
                    for(std::uint8_t i =0; i<count-1 ; i++)
                    {
                        file_output_string += current_char;
                    }
                    list_traverse_index++;
                    i++;
                }
                i++;

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
    Run_length decompress_rle;
    decompress_rle.read_input_file(file_name);
    decompress_rle.decode_file_structure();
    decompress_rle.rle_decompress();
    decompress_rle.write_to_file(file_name);
    return 0;   
    
}
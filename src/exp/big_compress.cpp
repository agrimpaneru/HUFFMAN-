#include <iostream>
#include <string>
#include <array>
#include <vector>
#include <cmath>
#include <cassert>
#include <fstream>
#include <cstdint>

const int MODULO = 256;
struct symbol_id
{
    std::size_t frequency=0;
    std::string sym;
    std::string bit_encoding;
    symbol_id* left=nullptr;
    symbol_id* right=nullptr;
};

class Symbol_manager
{
    private:
        std::vector<symbol_id> list;
        std::array<symbol_id, 256> initial_list;
        std::size_t list_SIZE;
        symbol_id node;
    public:
        Symbol_manager(): list(256){list_SIZE = list.size();}
        
        void frequency_counter(std::string_view input_string)
        {
            for(auto i: input_string)
            {
                uint32_t j = (static_cast<uint>(i)%MODULO);
                if(initial_list.at(j).frequency == 0)    initial_list.at(j).sym = i;
                initial_list.at(j).frequency++;
            }
            create_final_list();
        }
        void create_final_list()
        {
            uint32_t j = 0;
            for(auto i: initial_list)
            {
                if(i.frequency != 0) list.at(j++) = i;
            }
            list.resize(j);
            list.shrink_to_fit();
            list_SIZE = list.size();
            heapify(0);
        }
       
        void heapify(size_t index)
        {
            if(index * 2 + 1 >= list_SIZE)
            {
                return;
            }
            heapify(index+1);
            min_heapify(index);
        }
        void min_heapify(size_t index)
        {
            size_t left_index = 2*index +1;
            size_t right_index{};

            if(left_index >= list_SIZE)
            {
                return;
            }
            // assert(index*2+2 < list.size() && "out of bounds of the LIST");
            size_t left_value = list.at(left_index).frequency;
            size_t right_value{};

            size_t minimum_child_index{};
            size_t minimum_child_value{};


            //to cover the condition when there is no right child
            if(left_index+1 >= list_SIZE)
            {
                minimum_child_index = left_index;
                minimum_child_value = left_value;
            }
            else
            {
                right_index = left_index+1;
                right_value = list.at(right_index).frequency;
                minimum_child_value = (left_value < right_value)?left_value:right_value;
                minimum_child_index = (minimum_child_value == left_value)?left_index:right_index;
            }

            if(list.at(index).frequency > minimum_child_value)
            {
                //swap operation- any better way?
                symbol_id temp = list.at(index);
                list.at(index) = list.at(minimum_child_index);
                list.at(minimum_child_index) = temp;
                min_heapify(minimum_child_index);
            }
        }
        void display_heaped()
        {
            std::cout <<std::endl;

            for(auto i : list)
            {
                std::cout << i.frequency <<'\t';
            }
        }
        void deletion_min()
        {
            size_t index = 0;
            symbol_id temp = list.at(index);
            list.at(index) = list.at(list_SIZE-1);//list_SIZE-1 because 2 elements means 0 1 index but list_SIZE=2
            list.at(list_SIZE-1) = temp;
            list_SIZE--;
            min_heapify(index);
        }

        //THIS insert assumes that you have already inserted element at index, right now lazy to fix
        void insert_in_heap( size_t index)
        {
            //here assign it at the create_huffman function
            if(index == 0)
            {
                return;
            }
            assert(index !=0 &&"index is less than or equal to zero");
            size_t parent_index = floor((index-1)/2);
            if(list.at(index).frequency < list.at(parent_index).frequency)
            {
                swap(index, parent_index);
                insert_in_heap(parent_index);
            }
        }
        void create_huffman_tree()
        {
            if(list_SIZE <= 1)
            {
                return;
            }
            //parent node
            //set left
            deletion_min();
            //here manage the storage duration of left , as function returns this will result in UB
            symbol_id* left = new symbol_id;
            *left = list.at(list_SIZE);
            node.left = left;//for 0 indexed system we dont need to list_SIZE+1
            node.sym = list.at(list_SIZE).sym;
            node.frequency = list.at(list_SIZE).frequency;
            //set right
            deletion_min();
            symbol_id* right = new symbol_id;
            *right = list.at(list_SIZE);
            node.right = right;
            node.sym = node.sym + list.at(list_SIZE).sym;
            node.frequency += list.at(list_SIZE).frequency;
            //give back to list
            
            list.at(list_SIZE) = node;    
            insert_in_heap(list_SIZE);
            list_SIZE++;
            create_huffman_tree();
        }
        void assign_encoding(symbol_id* node, std::string encoding)
        {
            if(node->left)
            {
                assign_encoding(node->left, encoding+'0');
            }
            if(node->right)
            {
                assign_encoding(node->right, encoding+'1');
            }
            if(!node->left && !node->right)
            {
                node->bit_encoding = encoding;
                initial_list.at(static_cast<uint8_t>(node->sym[0])).bit_encoding = encoding;
                //std::cout << "\n"<<node->sym << "==" << node->bit_encoding<<std::flush;
            }
        }
        void call_encoder()
        {
            assign_encoding(&list.at(0),"");
        }
        void swap(size_t index1, size_t index2)
        {
            symbol_id temp = list.at(index1);
            list.at(index1) = list.at(index2);
            list.at(index2)= temp;
        }
        void reverse_sort()
        {
            if(list_SIZE ==1)
            {
                return;
            }
            deletion_min();
            reverse_sort();
        }
       
       //Read file serially and change to proper encoding
        void create_compressed(std::string_view input_text, std::string file_name)
        {
            std::string encoded_string{};
            for(auto c: input_text)
            {
                encoded_string += initial_list.at(static_cast<uint8_t>(c)).bit_encoding;
            }
            // std::cout<<"\n"<<encoded_string;
            save_encoded_string(encoded_string ,file_name);
        }

        void convert_and_store_in_bits(std::string_view encoded_string, std::string& encoding_string_to_bits)
        {
            uint8_t temp=0;
            size_t i;
            for( i=0; i<encoded_string.size(); i++)
            {
                temp <<=1;
                if(encoded_string[i] == '1')    temp |=1;
                else{ temp |=0;}
                
                if((i+1) % 8 == 0 && i!=0)
                {
                    encoding_string_to_bits += temp;    
                }
            }
            if(encoded_string.size() % 8 !=0 && i!=0)
            {
                //7- ans because for a single bit one shift would have already been done at top
               temp <<=(8-(encoded_string.size()%8));
               encoding_string_to_bits += temp;
            }
        }
        //changes encoding char to acutal bits
        void save_encoded_string(std::string_view encoded_string, std::string file_name)
        {
            std::string encoding_string_to_bits{};
            size_t length_of_bits = encoded_string.size();
            //HEADER 1st PART total number of bits occupies 8bytes
            encoding_string_to_bits.assign(reinterpret_cast<char*>(&length_of_bits), sizeof(length_of_bits));
            // uint16_t number_of_uniquie_symbols = list.size();//is there something wrong with assignment of size_t to uint16_t?
           
            assert(list.size() != 0 && "list size if zero");
           //HEADER 2nd BYTE = NUMBER OF UNQUIE SYMBOLS
           //here 1 to 256 to store it -1 is subtracted
            uint8_t number_of_uniquie_symbols = list.size()-1;
            encoding_string_to_bits += number_of_uniquie_symbols;
            //HEADER part 3
            /*
            1ST PART SYMBOL -> 1 BYTE
            2ND PART LENGTH OF BITS IN ENCODING OF SYMBOL -> ? BYTE 
            3RD PART ENCODING = LENGTH OF BITS + (8- LENGTH % 8) ;+ ON PART THAT LENGTH %8 != 0
            */
           //this for loop gets unique characters we have in our input
           for(auto i: list.at(0).sym)
           {
                encoding_string_to_bits += i;
                encoding_string_to_bits += static_cast<uint8_t>((initial_list.at(static_cast<uint8_t>(i)).bit_encoding.length()));
                convert_and_store_in_bits(initial_list.at(static_cast<uint8_t>(i)).bit_encoding, encoding_string_to_bits);
           }
           
            convert_and_store_in_bits(encoded_string, encoding_string_to_bits);
            std::ofstream output_file(file_name);
            if(output_file.is_open())
            {
                output_file.write(encoding_string_to_bits.c_str(),encoding_string_to_bits.size());
                output_file.close();
            }
        }

};

int main(int argc, char* argv[])
{
    if(argc != 2)
    {
        std::cout <<"\n Please provide file to compress";
        return -2;
    }
    std::string file_name{argv[1]};
    // std::string file_name{"input.txt"};
    std::ifstream input_file{file_name};
    if(!input_file)
    {
        std::cout << "\n no file found";
        return -1;
    }
    //take input from a test file
    std::string input_text{};
    input_file.seekg(0, std::ios::end);
    size_t length_of_file = input_file.tellg();
    input_file.seekg(0, std::ios::beg);
    input_text.resize(length_of_file);
    input_file.read(&input_text[0], length_of_file);

    input_file.close();
    Symbol_manager huff;
    huff.frequency_counter(input_text);
    // huff.display_heaped();
    // huff.reverse_sort();
    // huff.display_heaped();
    huff.create_huffman_tree();
    huff.call_encoder();
    huff.create_compressed(input_text, file_name);
    return 0;
}

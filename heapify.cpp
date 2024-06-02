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
                initial_list.at(static_cast<int>(node->sym[0])).bit_encoding = encoding;
                // std::cout << "\n"<<node->sym << "==" << node->bit_encoding;
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
        void create_compressed(std::string_view input_text)
        {
            std::string encoded_string{};
            for(auto c: input_text)
            {
                encoded_string += initial_list.at(static_cast<int>(c)).bit_encoding;
            }
            std::cout<<"\n"<<encoded_string;
            save_encoded_string(encoded_string);
        }

        void convert_and_store_in_bits(std::string_view encoded_string, std::string& encoding_string_to_bits)
        {
            uint8_t temp=0;
            int i;
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
        void save_encoded_string(std::string_view encoded_string)
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
                encoding_string_to_bits += static_cast<uint8_t>((initial_list.at(static_cast<uint>(i)).bit_encoding.length()));
                convert_and_store_in_bits(initial_list.at(static_cast<uint>(i)).bit_encoding, encoding_string_to_bits);
           }

            convert_and_store_in_bits(encoded_string, encoding_string_to_bits);
            std::ofstream output_file("compressss.txt");
            if(output_file.is_open())
            {
                output_file.write(encoding_string_to_bits.c_str(),encoding_string_to_bits.size());
                output_file.close();
            }
        }

};


//FILE DECOMPRESS PART
struct Tree_Node
{
    Tree_Node* left=nullptr;
    Tree_Node* right=nullptr;
    char symbol;
};
struct encoding_scheme
{
    char symbol;
    std::string encoding_bits;
};
class Decompress
{
    private:
        std::string file_content;
        Tree_Node* root_node;
        size_t number_of_bits;
        uint8_t number_of_unique_symbols;
        std::string compressed_string;
        std::vector<encoding_scheme> map_of_symbols;
        std::string decoded_string;
    public:
    Decompress(): map_of_symbols(256){}
    void read_compressed_file()
    {
        // std::string file_content;
        std::ifstream input_file{"compressss.txt"};
        if (input_file.is_open())
        {
            input_file.seekg(0, std::ios::end);
            size_t length_of_file = input_file.tellg();
            input_file.seekg(0, std::ios::beg);
            file_content.resize(length_of_file);
            input_file.read(&file_content[0], length_of_file);
            input_file.close();
        }
    }
    
    void decode_file_structure()
    {
        size_t traverse_index=0;
        number_of_bits = *reinterpret_cast<size_t*>(file_content.substr(0,sizeof(size_t)).data());
        traverse_index = sizeof(size_t);
        number_of_unique_symbols = file_content.at(traverse_index++);

        //Read symbol and their encoding
    
        for(int i=0;  i< static_cast<int>(number_of_unique_symbols)+1 ; i++)
        {
            map_of_symbols.at(i).symbol = file_content.at(traverse_index++);
            uint8_t length_of_bits = file_content.at(traverse_index++);
            size_t length_of_byte = ceil((float)length_of_bits/8.0);
            std::string encoded_text = file_content.substr(traverse_index,length_of_byte);
            traverse_index += length_of_byte;
            convert_bits_to_bytes(encoded_text, map_of_symbols.at(i).encoding_bits, length_of_bits, length_of_byte);
            // std::cout << length_of_byte;
        }
        map_of_symbols.resize(static_cast<size_t>(number_of_unique_symbols)+1);
        
        //read encoded stuff to byte representation for each bit and store in comprressed string
        size_t length_of_byte = ceil((float)number_of_bits/8.0);
        std::string encoded_text = file_content.substr(traverse_index,length_of_byte);
        convert_bits_to_bytes(encoded_text,  compressed_string, number_of_bits, length_of_byte);
    }


    void convert_bits_to_bytes(std::string_view encoded_text, std::string& encoded_text_in_byte, size_t length_of_bits, size_t length_of_byte)
    {
        constexpr std::uint8_t mask7{ 1 << 7 }; // 1000 0000

        for(size_t i = 0 ; i < length_of_byte ; i++)
        {
            uint8_t temp = encoded_text[i];
            for(int j=0;  j< 8; j++)
            {
                if(i*8 + j == length_of_bits)
                {
                    break;
                }
                if( temp & mask7)
                {
                    encoded_text_in_byte += '1';
                }
                else
                {
                    encoded_text_in_byte += '0';
                }
                temp <<= 1;
                
            }
        }
    }
    

    void create_tree()
    {
        root_node = new Tree_Node;
        for(auto i : map_of_symbols)
        {
            insert_node(root_node, i.encoding_bits,0, i.symbol);
        }
        decode(0, root_node);
        std::cout << std::endl<<compressed_string;
        std::cout << std::endl<<decoded_string<<std::endl;
    }
    void insert_node(Tree_Node* node, std::string_view path, size_t index, char sym)
    {
        if(index == path.size())
        {
            node->symbol = sym;
            return;
        }
        if(path.at(index) == '0')
        {
            if(node->left == nullptr) node->left = new Tree_Node;
            insert_node(node->left, path, index + 1, sym);
        }
        else
        {
            if(node->right == nullptr) node->right = new Tree_Node;
            insert_node(node->right, path, index + 1, sym);
        }
    }
    
    void decode(size_t traverse_index, Tree_Node* node)
    {
        if(!node->left && !node->right)
        {
            decoded_string += node->symbol;
            decode(traverse_index, root_node);
            return;
        }
        if(traverse_index == compressed_string.size())
        {
            return;
        }
        
        if(compressed_string.at(traverse_index) == '0')
        {
            decode(traverse_index + 1, node->left);
        }
        else
        {
            decode(traverse_index + 1, node->right);
        }
    }
    
    void display()
    {
        for(auto i: map_of_symbols)
        {
            std::cout << "\n" << i.symbol << "==" << i.encoding_bits<<"\n";
        }
        std::cout << decoded_string;
    }
};
int main()
{
    std::ifstream input_file{"input.txt"};
    if(!input_file)
    {
        std::cout << "\n no file found";
        return -1;
    }
    //take input from a test file
    input_file.seekg(0, std::ios::end);
    int length_of_file = input_file.tellg();
    char *buffer = new char[length_of_file];
    input_file.seekg(0, std::ios::beg);
    input_file.read(buffer, length_of_file);

    std::string test = static_cast<std::string>(buffer);
    input_file.close();
    Symbol_manager huff;
    huff.frequency_counter(test);
    huff.display_heaped();
    // huff.reverse_sort();
    // huff.display_heaped();
    huff.create_huffman_tree();
    huff.call_encoder();
    huff.create_compressed(test);

    Decompress decom;
    decom.read_compressed_file();
    decom.decode_file_structure();
    decom.create_tree();
    // decom.display();
    return 0;
}
//here frequency_counter's parameter can be string from a text file 
/*then frequency counter calls create_final_list, 
create_final_list ->(creates or manages) std::vector list is the list that stores only present symbols and 
their frequency.
then create_final_list calls heapify
heapify() finds youngest parent(at first then works for previous parents after subsequent return)
for a parent heapify() calls min_heapify that parent's family
min_heapify() does parent swap if child min thing and after swap calls itself with position of child(in which parent is swapped)
*/
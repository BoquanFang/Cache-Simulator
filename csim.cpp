#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <math.h>
#include <map>
#include <queue>


unsigned bin_to_decimal (std::vector<unsigned> input) {
    unsigned temp = 0;
    for (int i = int(input.size() - 1); i >= 0 ; i --){
        temp += input[i] * pow(2, input.size() - i - 1);
    }
    return temp;
}

std::vector<unsigned> dec_to_binary (unsigned input) {
    unsigned temp = input;
    std::vector<unsigned> output;
    while (temp != 0) {
        output.insert(output.begin(),temp % 2);
        temp = temp / 2;
    }
    return output;
}

std::vector<unsigned> hex_to_binary (std::string input) {
    unsigned temp = 0;
    std::vector<unsigned> output;
    for (int i = int(input.length() - 1); i >= 0 ; i --) {
        if (isdigit(input[i])) {            
            temp += (input[i] - '0') * pow(16, input.length() - i - 1);
        } else {
            if (input[i] == 'a') {
                temp += 10 * pow(16, input.length() - i - 1);
            } else if (input[i] == 'b') {
                temp += 11 * pow(16, input.length() - i - 1);
            } else if (input[i] == 'c') {
                temp += 12 * pow(16, input.length() - i - 1);
            } else if (input[i] == 'd') {
                temp += 13 * pow(16, input.length() - i - 1);
            } else if (input[i] == 'e') {
                temp += 14 * pow(16, input.length() - i - 1);
            }  else {
                temp += 15 * pow(16, input.length() - i - 1);
            }
        }
    }
    output = dec_to_binary(temp);
    while (output.size() != 32) {
        output.insert(output.begin(), 0);
    }
    return output;
}


struct way_block {
    unsigned valid;
    unsigned tag;
};


void do_print ( std::vector<std::vector<way_block>> cache_content, std::string address, unsigned BlkOfs, unsigned set, unsigned tag, bool Hit, int section) {
    if (section == 1) {
        std::cout << "Address:" << address << '\t';
        std::cout << "BlkOfs:" << BlkOfs << '\t';
        std::cout << "Set:" << set << '\t';
        std::cout << "Tag:" << tag << '\t';
        std::cout << "Hit:";
        if (Hit == true) {
            std::cout << "true" << '\t';
        } else {
            std::cout << "false" << '\t';
        }
        std::cout << "Cache:";
        //need accomodations for later part
        for (int i = 0; i < (int) cache_content.size(); i ++) {
            int valid_elements = 0;
            std::cout << "[";
            for (int k = 0; k < (int) cache_content[i].size(); k ++) {
                if (cache_content[i][k].valid == 1) {
                    valid_elements ++;
                }
            }
            
            for (int j = 0; j < valid_elements; j ++) {

                if (j == (int) (valid_elements - 1)) {
                    if (cache_content[i][j].valid == 1) {
                        std::cout << cache_content[i][j].tag;
                    }
                } else {
                    if (cache_content[i][j].valid == 1) {
                        std::cout << cache_content[i][j].tag << ", ";
                    }              
                }
            }

            std::cout << "]";
        }

        std::cout << std::endl;
    }

}

class Cache
{
public:
    //constructor
    Cache (unsigned input_b, unsigned input_s, unsigned input_n, unsigned input_num_block_offset, unsigned input_num_set_bit) {
        block_size = input_b;
        sets = input_s;
        ways = input_n;
        num_block_offset = input_num_block_offset;
        num_set_bit = input_num_set_bit;
        way_block block = {0,0};
        std::vector<way_block> empty_block_setup;
        for (int i = 0; i < (int) input_n; i ++) {
            empty_block_setup.push_back(block);
        }
        for (int i = 0; i < (int) input_s; i ++) {
            cache_content.push_back(empty_block_setup);
        }
        std::queue<unsigned> empty_queue;
        for (int i = 0; i < (int) input_s; i ++) {
            LRU.push_back(empty_queue);
        }        
    }

    void do_memory_access (std::vector<std::vector<unsigned>> address_binary,  std::vector<std::string> address_hex, int pass_time, int section) {
        unsigned num_of_hit = 0;
        for (int i = 0; i < (int) address_binary.size(); i ++) {
            bool hit = false;
            std::string address = address_hex[i];
            unsigned block_off_set = 0;
            unsigned set_output = 0;
            unsigned tag_in_memory = 0; 
            std::vector<unsigned> record_LRU;
            if (this-> block_size == 1) {
                block_off_set = 0;
                std::vector<unsigned> binary_for_set;
                std::vector<unsigned> binary_for_tag;
                for (int j = 0 ; j < (int) this-> num_set_bit; j ++) {
                    binary_for_set.insert(binary_for_set.begin(),address_binary[i][29- j]);
                }
                for (int z = 0; z <= (int) (29 - this->num_set_bit) ;z ++) {
                    binary_for_tag.push_back(address_binary[i][z]);
                }
                set_output = bin_to_decimal(binary_for_set);
                tag_in_memory = bin_to_decimal(binary_for_tag);
            } else {
                std::vector<unsigned> binary_for_set;
                std::vector<unsigned> binary_for_block;
                std::vector<unsigned> binary_for_tag;
                for (int j = 0; j < (int) this -> num_block_offset; j ++) {
                    binary_for_block.insert(binary_for_block.begin(), address_binary[i][29 - j]);
                }
                for (int k = 0; k < (int) this -> num_set_bit; k ++) {
                    binary_for_set.insert(binary_for_set.begin(), address_binary[i][29 - this->num_block_offset - k]);
                }
                for (int z = 0; z <= (int) (29 - this->num_set_bit - this-> num_block_offset) ;z ++) {
                    binary_for_tag.push_back(address_binary[i][z]);
                }
                block_off_set = bin_to_decimal(binary_for_block);
                set_output = bin_to_decimal(binary_for_set);
                tag_in_memory = bin_to_decimal(binary_for_tag);
            }

            //access memory
            // cache hit
            for (int a = 0; a < (int) this -> ways; a ++) {
                if (cache_content[set_output][a].valid == 1 && cache_content[set_output][a].tag == tag_in_memory) {
                    hit = true;
                    num_of_hit ++;
                    std::vector<unsigned> copy;
                    //update the queue
                    while (!this-> LRU[set_output].empty()) {
                        if ((int) this->LRU[set_output].front() != a) {
                            copy.push_back(this->LRU[set_output].front());
                            this->LRU[set_output].pop();
                        } else {
                            break;
                        }
                    }
                    this->LRU[set_output].pop();
                    while (!this-> LRU[set_output].empty()) {
                        copy.push_back(this->LRU[set_output].front());
                        this->LRU[set_output].pop();
                    }
                    for (int temp = 0; temp < (int) copy.size(); temp ++) {
                        this->LRU[set_output].push(copy[temp]);
                    }
                    this->LRU[set_output].push(a);
                    break;
                }
            }
            // cache miss
            bool filled = false;
            way_block temp = {1, tag_in_memory};
            if (hit == false) {
                for (int b = 0; b < (int) this -> ways; b ++) {
                    if (cache_content[set_output][b].valid != 1) {
                        this -> cache_content[set_output][b] = temp;
                        filled = true;
                        this -> LRU[set_output].push(b);
                        break;
                    }  
                }
                if (filled == false) {
                    unsigned replace = LRU[set_output].front();
                    this -> LRU[set_output].pop();
                    this -> cache_content[set_output][replace] = temp;
                    this -> LRU[set_output].push(replace);
                }
            }

            do_print(this-> cache_content, address, block_off_set, set_output, tag_in_memory, hit, section);
        }
        std::ofstream Output;
        // print miss rate
        float hit_rate = 100 * (num_of_hit / (float) address_binary.size()); 
        int miss_rate = 100 - (int) hit_rate;
        std::cout << "Pass " << pass_time << " hits:" << num_of_hit << " = " << (int) hit_rate << "%";
        std::cout << std::endl;
        std::cout << "Pass " << pass_time << " misses:" << (int) address_binary.size() - num_of_hit << " = " << (int) miss_rate << "%";
        std::cout << std::endl;
        std::cout << std::endl;
    }
private:
    unsigned block_size, sets, ways, num_block_offset, num_set_bit;
    std::vector<std::vector<way_block>> cache_content;
    std::vector<std::queue<unsigned>> LRU;
};


int main () {
    // Convert Address to Binary
    std::vector<std::string> address_hex;
    std::vector<std::vector<unsigned>> address_binary;    
    address_hex.push_back("40");
    address_hex.push_back("44");
    address_hex.push_back("48");
    address_hex.push_back("4c");
    address_hex.push_back("70");
    address_hex.push_back("74");
    address_hex.push_back("78");
    address_hex.push_back("7c");
    address_hex.push_back("80");
    address_hex.push_back("84");
    address_hex.push_back("88");
    address_hex.push_back("8c");
    address_hex.push_back("90");
    address_hex.push_back("94");
    address_hex.push_back("98");
    address_hex.push_back("9c");
    address_hex.push_back("0");
    address_hex.push_back("4"); 
    address_hex.push_back("8");
    address_hex.push_back("c");
    address_hex.push_back("10");
    address_hex.push_back("14");
    address_hex.push_back("18");
    address_hex.push_back("1c");
    address_hex.push_back("20");
    // get binary address  
    for (int i = 0; i < (int) address_hex.size(); i ++) {
        address_binary.push_back(hex_to_binary(address_hex[i]));
    }
    // 8.9
    std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
    std::cout << "Problem 8.9a" << std::endl;
    // 8.9A
    Cache chache_for_8_9_A (1,16,1,0,4);
    chache_for_8_9_A.do_memory_access(address_binary,address_hex, 1, 1);
    chache_for_8_9_A.do_memory_access(address_binary,address_hex, 2, 1);
    std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
    std::cout << "Problem 8.9b" << std::endl;
    // 8.9B
    Cache chache_for_8_9_B (1,1,16,0,0);
    chache_for_8_9_B.do_memory_access(address_binary,address_hex, 1, 1);
    chache_for_8_9_B.do_memory_access(address_binary,address_hex, 2, 1);
    std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
    std::cout << "Problem 8.9c" << std::endl;
    // 8.9C
    Cache chache_for_8_9_C (1,8,2,0,3);
    chache_for_8_9_C.do_memory_access(address_binary,address_hex, 1, 1);
    chache_for_8_9_C.do_memory_access(address_binary,address_hex, 2, 1);
    std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
    std::cout << "Problem 8.9d" << std::endl;
    // 8.9D
    Cache chache_for_8_9_D (2,8,1,1,3);
    chache_for_8_9_D.do_memory_access(address_binary,address_hex, 1, 1);
    chache_for_8_9_D.do_memory_access(address_binary,address_hex, 2, 1);
    
    // 8.10
    address_hex.clear();
    address_binary.clear();
    address_hex.push_back("74");
    address_hex.push_back("a0");
    address_hex.push_back("78");
    address_hex.push_back("38c");
    address_hex.push_back("ac");
    address_hex.push_back("84");
    address_hex.push_back("88");
    address_hex.push_back("8c");
    address_hex.push_back("7c");
    address_hex.push_back("34");
    address_hex.push_back("38");
    address_hex.push_back("13c");
    address_hex.push_back("388");
    address_hex.push_back("18c");
    for (int i = 0; i < (int) address_hex.size(); i ++) {
        address_binary.push_back(hex_to_binary(address_hex[i]));
    }
    std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
    std::cout << "Problem 8.10a" << std::endl;
    // 8.10A
    Cache chache_for_8_10_A (1,16,1,0,4);
    chache_for_8_10_A.do_memory_access(address_binary,address_hex, 1, 1);
    chache_for_8_10_A.do_memory_access(address_binary,address_hex, 2, 1);    
    std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
    std::cout << "Problem 8.10b" << std::endl;
    // 8.10B
    Cache chache_for_8_10_B (2,1,8,1,0);
    chache_for_8_10_B.do_memory_access(address_binary,address_hex, 1, 1);
    chache_for_8_10_B.do_memory_access(address_binary,address_hex, 2, 1);  
    std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
    std::cout << "Problem 8.10c" << std::endl;
    //8.10C
    Cache chache_for_8_10_C (2,4,2,1,2);
    chache_for_8_10_C.do_memory_access(address_binary,address_hex, 1, 1);
    chache_for_8_10_C.do_memory_access(address_binary,address_hex, 2, 1); 
    std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
    std::cout << "Problem 8.10d" << std::endl;
    // 8.10D
    Cache chache_for_8_10_D (4,4,1,2,2);
    chache_for_8_10_D.do_memory_access(address_binary,address_hex, 1, 1);
    chache_for_8_10_D.do_memory_access(address_binary,address_hex, 2, 1);
    return 0;
}
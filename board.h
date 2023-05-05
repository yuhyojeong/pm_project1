#include <fstream>
#include <list>

#include "page.h"

using std::endl;
using std::ofstream;
using std::pair;

class Board {
    public:
        Board(int num_jobs, int width, int height, ofstream& output_stream);
        ~Board();

        void print_board();
        void print_job(int job_idx, char job_type, int id);
        void set_board(); // boardlst의 각 list의 마지막 원소를 board로 set

        //job functions
        void insert_page(int x, int y, int width, int height, int id, char content);
        void delete_page(int id);
        void modify_content(int id, char content);
        void modify_position(int id, int x, int y);

    private:
        int num_jobs, width, height;
        ofstream& output; 
        char* board;
        list <pair<int, Page>> pages; //page 넣은 순서대로 list에 저장
        list<char>* boardlst; //겹치는 content를 list로 board에 넣음
};


Board::Board(int num_jobs, int width, int height, ofstream& output_stream): output(output_stream) {
    this->width = width;
    this->height = height;
    this->num_jobs = num_jobs;

    board = new char[width*height];
    boardlst = new list<char>[width * height];

    for (int h = 0; h < height; h++) {
        for (int w = 0; w < width; w++) {
            board[h*width + w] = ' ';
            boardlst[h*width + w].push_back(' ');
        }
    }

    

}

Board::~Board() {
    delete board;
    
}


void Board::print_board() {
    int h, w;
    for (w = 0; w < width+2; w++) output << "- ";
    output << endl;
    
    for (h = 0; h < height; h++) {
        output << "| ";
        for (w = 0; w < width; w++) {
            output << board[h*width + w] << " ";
        }
        output << "| " << endl;
    }

    for (w = 0; w < width+2; w++) output << "- ";
    output << endl;
}

void Board::print_job(int job_idx, char job_type, int id) {
    output << ">> (" << job_idx <<") ";
    switch(job_type) {
        
        case 'i':
            output << "Insert ";
            break;
        case 'd':
            output << "Delete ";
            break;
        case 'm':
            output << "Modify ";
            break;
    }

    output << id << endl;
}


void Board::insert_page(int x, int y, int width, int height, int id, char content) {
    Page newpage = Page(x, y, width, height, content); // 해당 page 저장
    pages.push_back(pair<int, Page> (id, newpage)); //list의 뒤에 id, page pair 삽입
    for (int h = y; h < y + height; y++){
        for (int w = x; w < x + width; w++){
            boardlst[h * width + w].push_back(content);
        }
    }
    set_board();
    print_board();
}

void Board::delete_page(int id) {
    for (auto itr = pages.end() - 1; itr >= pages.begin(); itr--){
        if ((*itr).first == id){
            pages.erase(itr);
        }
    }
}

void Board::modify_content(int id, char content) {
   

}
void Board::modify_position(int id, int x, int y) {
   
    
}

void Board::set_board(){
    for (int h = 0; h < height; h++) {
        for (int w = 0; w < width; w++) {
            board[h*width + w] = boardlst[h*width + w].back();
        }
    }
}

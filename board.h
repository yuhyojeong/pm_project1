#include <fstream>
#include <vector>
#include <map>
#include <algorithm>

#include "page.h"

using std::endl;
using std::ofstream;
using std::vector;
using std::map;
using std::find;

class Board {
    public:
        Board(int num_jobs, int width, int height, ofstream& output_stream);
        ~Board();
        void print_board();
        void print_job(int job_idx, char job_type, int id);
        //job functions
        void insert_page(int x, int y, int pagewidth, int pageheight, int id, char content);
        void delete_page(int id);
        void modify_content(int id, char content);
        void modify_position(int id, int x, int y);
        void set_board(); // boardlst의 각 list의 마지막 원소를 board로 set
        void board_insert(int x, int y, int pagewidth, int pageheight, char content); //insert content to boardlst
        void board_delete(int x, int y, int pagewidth, int pageheight); //delete content from boardlst
        void deleteseq(int id); //page를 순차적으로 삭제
        int findidx(int id); //해당 page 위의 page 중에 가장 index가 작은 것 return

    private:
        int num_jobs, width, height;
        ofstream& output; 
        char* board;
        map <int, Page> pagemap; //page의 id와 page를 저장
        vector<char>* boardlst; //겹치는 content를 boardlst에 넣음
};


Board::Board(int num_jobs, int width, int height, ofstream& output_stream): output(output_stream) {
    this->width = width;
    this->height = height;
    this->num_jobs = num_jobs;

    board = new char[width*height];
    boardlst = new vector<char>[width * height];

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


void Board::insert_page(int x, int y, int pagewidth, int pageheight, int id, char content) {
    Page newpage = Page(x, y, pagewidth, pageheight, id, content); // 해당 page 저장
    pagemap.insert({id, newpage}); // map에 id, page pair 삽입
    board_insert(x, y, pagewidth, pageheight, content);
    set_board();
    print_board();
}

void Board::delete_page(int id) {
    vector<int> prior;

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

void Board::board_insert(int x, int y, int pagewidth, int pageheight, char content){
    for (int h = y; h < y + pageheight; h++){
        for (int w = x; w < x + pagewidth; w++){
            boardlst[h * width + w].push_back(content); //boardlst의 끝에 content 삽입
        }
    }
}

void Board::board_delete(int x, int y, int pagewidth, int pageheight){
    for (int h = y; h < y + pageheight; h++){
        for (int w = x; w < x + pagewidth; w++){
            boardlst[h * width + w].pop_back(); //boardlst의 마지막 원소 삭제
        }
    }
}

void Board::deleteseq(int id){
    
}

int Board::findidx(int id){
    int x = pagemap[id].getx();
    int y = pagemap[id].gety();
    int pagewidth = pagemap[id].get_width();
    int pageheight = pagemap[id].get_height();
    char cont = pagemap[id].get_content();
    int idmin = 32768;
    for (int h = y; h < y + pageheight; h++){
        for (int w = x; w < x + pagewidth; w++){
            int i = h * width + w;
            if (boardlst[i].back() != cont){ //cont 바로 위의 페이지 탐색
                int j = boardlst[i][find(boardlst[i].begin(), boardlst[i].end(), cont) - boardlst[i].begin() + 1];
                if (j < idmin){ //후보 페이지 중 가장 index 작은 것 return
                    idmin = j;
                }
            }
        }
    }
    return idmin; // target page가 last page이면 idmin == 32768
}
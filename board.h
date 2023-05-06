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
        void set_board(int x, int y, int pagewidth, int pageheight, char cont); // board set
        void board_insert(int x, int y, int pagewidth, int pageheight, int id); //insert id to boardlst
        void board_delete(int x, int y, int pagewidth, int pageheight, vector<char>* blist); //delete content from boardlst
        void deleteseq(int id, int idcop); //page를 순차적으로 삭제
        int findidx(int id); //해당 page 위의 page 중에 가장 index가 작은 것 return
        void copy(vector<char>* copyb, vector<char>* orb); //boardlst 복제
        void insertseq(int id, int idcop, vector<char>* copyb); // page를 순차적으로 insert
        int findidx(int id, vector<char>* copyb); // 해당 page 위의 page 중 가장 index 큰 것 return

    private:
        int num_jobs, width, height;
        ofstream& output; 
        char* board;
        map <int, Page> pagemap; //page의 id와 page를 저장
        vector<int>* boardlst; //겹치는 content의 index를 boardlst에 넣음
};


Board::Board(int num_jobs, int width, int height, ofstream& output_stream): output(output_stream) {
    this->width = width;
    this->height = height;
    this->num_jobs = num_jobs;

    board = new char[width*height];
    boardlst = new vector<int>[width * height];
    Page def = Page(' ');
    pagemap.insert({-1, def});

    for (int h = 0; h < height; h++) {
        for (int w = 0; w < width; w++) {
            board[h*width + w] = ' ';
            boardlst[h*width + w].push_back(-1);
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
    board_insert(x, y, pagewidth, pageheight, id);
    set_board(x, y, pagewidth, pageheight, content);
    print_board();
}

void Board::delete_page(int id) {
    int idcop = id;
    deleteseq(id, idcop); // 페이지 삭제
    pagemap.erase(id);
}

void Board::modify_content(int id, char content) {
   

}
void Board::modify_position(int id, int x, int y) {
   
    
}

void Board::set_board(int x, int y, int pagewidth, int pageheight, char cont){
    for (int h = y; h < y + pageheight; h++) {
        for (int w = x; w < x + pagewidth; w++) {
            board[h*width + w] = cont;
        }
    }
}

void Board::board_insert(int x, int y, int pagewidth, int pageheight, int id){
    for (int h = y; h < y + pageheight; h++){
        for (int w = x; w < x + pagewidth; w++){
            boardlst[h * width + w].push_back(id); //boardlst의 끝에 id 삽입
        }
    }
}

void Board::board_delete(int x, int y, int pagewidth, int pageheight, vector<char>* blist){
    for (int h = y; h < y + pageheight; h++){
        for (int w = x; w < x + pagewidth; w++){
            blist[h * width + w].pop_back(); //boardlst의 마지막 원소 삭제
        }
    }
}

void Board::deleteseq(int id, int idcop){
    int i = findidx(id);
    if (i != 32768){
        deleteseq(i, idcop);
        deleteseq(id, idcop);
    } else{
        int x = pagemap[id].getx();
        int y = pagemap[id].gety();
        int pageheight = pagemap[id].get_height();
        int pagewidth = pagemap[id].get_width();
        for (int h = y; h < y + pageheight; h++) {
            for (int w = x; w < x + pagewidth; w++) {
                int j = h * width + w;
                board[j] = pagemap[boardlst[j][find(boardlst[j].begin(), boardlst[j].end(), id) - boardlst[j].begin() - 1]].get_content();
            }
        }
        print_board();
    }
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
            int k = h * width + w;
            if (board[k] != cont){ //cont 바로 위의 페이지 탐색
                int j = boardlst[k][find(boardlst[k].begin(), boardlst[k].end(), id) - boardlst[k].begin() + 1];
                if (j < idmin){ //후보 페이지 중 가장 index 작은 것 return
                    idmin = j;
                }
            }
        }
    }
    return idmin; // target page가 last page이면 idmin == 32768
}

void Board::copy(vector<char>* copyb, vector<char>* orb){
    for (int h = 0; h < height; h++) {
        for (int w = 0; w < width; w++) {
            int i = h * width + w;
            for (int j = 0; j < boardlst[i].size(); j++){
                copyb[i][j] = boardlst[i][j];
            }
        }
    }
}

void Board::insertseq(int id, int idcop, vector<char>* copyb){
    int i = findidx(id, copyb);
    if (i == -1){
        if (id != idcop){
            board_delete(pagemap[id].getx(), pagemap[id].gety(), pagemap[id].get_width(), pagemap[id].get_height(), copyb);
            for (auto elem : boardlst[pagemap[id].getx()]){
                if (elem != pagemap[id].get_content()){
                    board_insert(pagemap[id].getx(), pagemap[id].gety(), pagemap[id].get_width(), pagemap[id].get_height(), pagemap[id].get_content());
                    //set_board();
                    print_board();
                }
            }
        }
    } else{
        if (id != idcop){
            board_insert(pagemap[id].getx(), pagemap[id].gety(), pagemap[id].get_width(), pagemap[id].get_height(), pagemap[id].get_content());
            //set_board();
            print_board();
        }
        insertseq(i, idcop, copyb);
        insertseq(id, idcop, copyb);
    }
}

int Board::findidx(int id, vector<char>* copyb){
    int x = pagemap[id].getx();
    int y = pagemap[id].gety();
    int pagewidth = pagemap[id].get_width();
    int pageheight = pagemap[id].get_height();
    char cont = pagemap[id].get_content();
    int idmax = -1;
    for (int h = y; h < y + pageheight; h++){
        for (int w = x; w < x + pagewidth; w++){
            int i = h * width + w;
            if (copyb[i].back() != cont){ //cont 바로 위의 페이지 탐색
                char j = copyb[i][find(copyb[i].begin(), copyb[i].end(), cont) - copyb[i].begin() + 1];
                for (auto elem : pagemap){
                    if (elem.second.get_content() == j){
                        if (elem.first > idmax){
                            idmax = elem.first; //후보 페이지 중 가장 index 큰 것 return
                        }
                    }
                }
            }
        }
    }
    return idmax; // target page가 last page이면 idmax = -1
}
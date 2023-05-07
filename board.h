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
using std::remove;

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
        void board_delete(int x, int y, int pagewidth, int pageheight, int id); //delete id from boardlst
        void deleteseq(int id, int idcop); //page를 순차적으로 삭제
        int findminidx(int id); //해당 page 위의 page 중에 가장 index가 작은 것 return
        void insertseq(int id, int idcop); // page를 순차적으로 insert
        int findmaxidx(int id); // 해당 page 위의 page 중 가장 index 큰 것 return

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
    deleteseq(id, idcop); // 페이지 순차적 삭제
    insertseq(id, idcop); // 페이지 순차적 삽입
    board_delete(pagemap[id].getx(), pagemap[id].gety(), pagemap[id].get_width(), pagemap[id].get_height(), id);
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

void Board::board_delete(int x, int y, int pagewidth, int pageheight, int id){
    for (int h = y; h < y + pageheight; h++){
        for (int w = x; w < x + pagewidth; w++){
            int k = h * width + w;
            boardlst[k].erase(find(boardlst[k].begin(), boardlst[k].end(), id)); //boardlst에서 id 삭제
        }
    }
}

void Board::deleteseq(int id, int idcop){
    int i = findminidx(id); // 후보 중 min id 찾기
    if (i != 32768){ // not last page
        deleteseq(i, idcop); // 후보 먼저 delete한 후
        deleteseq(id, idcop); // target delete
    } else{ // last page
        int x = pagemap[id].getx();
        int y = pagemap[id].gety();
        int pageheight = pagemap[id].get_height();
        int pagewidth = pagemap[id].get_width();
        for (int h = y; h < y + pageheight; h++) {
            for (int w = x; w < x + pagewidth; w++) {
                int j = h * width + w;
                board[j] = pagemap[boardlst[j][find(boardlst[j].begin(), boardlst[j].end(), id) - boardlst[j].begin() - 1]].get_content(); // board에서 페이지 한 장 제거
            }
        }
        print_board();
    }
}

int Board::findminidx(int id){
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

void Board::insertseq(int id, int idcop){
    int i = findmaxidx(id); // 후보 중 max id 찾기
    int x = pagemap[id].getx();
    int y = pagemap[id].gety();
    int pageheight = pagemap[id].get_height();
    int pagewidth = pagemap[id].get_width();
    char cont = pagemap[id].get_content();
    if (i == -1){ // last page
        if (id != idcop){ // if page is not target
            for (int h = y; h < y + pageheight; h++){
                for (int w = x; w < x + pagewidth; w++){
                    if (board[h * width + w] == cont){
                        return; // 이미 board에 출력됐으면 skip
                    }
                }
            }
            set_board(x, y, pagewidth, pageheight, cont);
            print_board();
        }
    } else{
        if (id != idcop){ // page is not target
            set_board(x, y, pagewidth, pageheight, cont);
            print_board();
        }
        insertseq(i, idcop);
        insertseq(id, idcop);
    }
}

int Board::findmaxidx(int id){
    int x = pagemap[id].getx();
    int y = pagemap[id].gety();
    int pagewidth = pagemap[id].get_width();
    int pageheight = pagemap[id].get_height();
    char cont = pagemap[id].get_content();
    int idmax = -1;
    vector<int> idcand; // 후보 쌓는 vector
    for (int h = y; h < y + pageheight; h++){
        for (int w = x; w < x + pagewidth; w++){
            int k = h * width + w;
            int j = find(boardlst[k].begin(), boardlst[k].end(), id) - boardlst[k].begin(); // index of target in boardlst
            if (j != boardlst[k].size() - 1){ // not last element
                if (idcand.size() == 0){
                    idcand.push_back(boardlst[k][j + 1]);
                } else{
                    if (find(idcand.begin(), idcand.end(), boardlst[k][j + 1]) == idcand.end()){
                        idcand.push_back(boardlst[k][j + 1]); // 중복 제거
                    }
                }
                for (auto elem : idcand){
                    if (board[k] == pagemap[elem].get_content()){
                        idcand.erase(remove(idcand.begin(), idcand.end(), elem)); // 이미 출력된 값이면 후보에서 제외외
                    }
                }
            }
            
            
        }
    }
    for (auto elem : idcand){
        if (elem > idmax){
            idmax = elem;
        }
    }
    return idmax; // target page가 last page이면 idmax = -1
}
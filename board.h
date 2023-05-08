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
        void board_insert(int x, int y, int pagewidth, int pageheight, int id); //insert page to board
        void board_delete(int x, int y, int pagewidth, int pageheight, int id); //delete one page from board
        void deleteseq(int id, int idcop); //page를 순차적으로 삭제
        vector<int> findidx(int id); //해당 pafge 위의 page 중에 가장 index가 작은 것 return
        void insertseq(int id); // page를 순차적으로 insert
        void boardlst_delete(int x, int y, int pagewidth, int pageheight, int id); // boardlist에서 page delete
        void boardlst_insert(int x, int y, int pagewidth, int pageheight, int id); // boardlist의 끝에 page insert

    private:
        int num_jobs, width, height;
        ofstream& output; 
        char* board;
        map <int, Page> pagemap; //page의 id와 page를 저장
        vector<int>* boardlst; //겹치는 content의 index를 boardlst에 넣음
        vector<int> deletedlst; // 삭제된 id들 저장
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
    boardlst_insert(x, y, pagewidth, pageheight, id);
    set_board(x, y, pagewidth, pageheight, content);
    print_board();
}

void Board::delete_page(int id) {
    int idcop = id;
    deletedlst.clear();
    deleteseq(id, idcop);
    deletedlst.pop_back();
    while(deletedlst.size() != 0){
        insertseq(deletedlst.back());
        deletedlst.pop_back();
    }
    boardlst_delete(pagemap[id].getx(), pagemap[id].gety(), pagemap[id].get_width(), pagemap[id].get_height(), id);
    pagemap.erase(pagemap.find(id));
}
void Board::modify_content(int id, char content) {
   int idcop = id;
   deletedlst.clear();
   deleteseq(id, idcop);
   Page modpage = Page(pagemap[id].getx(), pagemap[id].gety(), pagemap[id].get_width(), pagemap[id].get_height(), id, content);
   pagemap[id] = modpage;
    while(deletedlst.size() != 0){
        insertseq(deletedlst.back());
        deletedlst.pop_back();
    }
}

void Board::modify_position(int id, int x, int y) {
   int idcop = id;
   deletedlst.clear();
   deleteseq(id, idcop);
   int pagewidth = pagemap[id].get_width();
   int pageheight = pagemap[id].get_height();
    boardlst_delete(pagemap[id].getx(), pagemap[id].gety(), pagewidth, pageheight, id); //delete old position
    for (int h = y; h < y + pageheight; h++){
        for (int w = x; w < x + pagewidth; w++){
            int k = h * width + w;
            for (int p = boardlst[k].size() - 1; p >= 0; p--){
                if (pagemap[boardlst[k][p]].get_content() == board[k]){
                    boardlst[k].insert(p + 1 + boardlst[k].begin(), id); //boadlst 적절한 곳에 insert
                }
            }
        }
    }
    set_board(x, y, pagewidth, pageheight, pagemap[id].get_content());
    print_board();
    deletedlst.pop_back();
    while(deletedlst.size() != 0){
        insertseq(deletedlst.back());
        deletedlst.pop_back();
    }
   Page modpage = Page(x, y, pagemap[id].get_width(), pagemap[id].get_height(), id, pagemap[id].get_content());
   pagemap[id] = modpage;
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
            board[h * width + w] = pagemap[id].get_content();
        }
    }
}

void Board::board_delete(int x, int y, int pagewidth, int pageheight, int id){
    for (int h = y; h < y + pageheight; h++){
        for (int w = x; w < x + pagewidth; w++){
            int k = h * width + w;
            int idx = find(boardlst[k].begin(), boardlst[k].end(), id) - boardlst[k].begin();
            board[k] = pagemap[boardlst[k][idx - 1]].get_content();
        }
    }
}

void Board::deleteseq(int id, int idcop){
    vector<int> pr = findidx(id); // 위 페이지를 포인터로 저장
    if (pr.size() == 0){
        board_delete(pagemap[id].getx(), pagemap[id].gety(), pagemap[id].get_width(), pagemap[id].get_height(), id);
        deletedlst.push_back(id);
        print_board();
    } else{
        for (int j = 0; j < pr.size(); j++){
            if (deletedlst.size() == 0){
                deleteseq(pr[j], idcop);
            } else if (find(deletedlst.begin(), deletedlst.end(), pr[j]) == deletedlst.end()){
                deleteseq(pr[j], idcop);
            } else{
                pr.erase(j + pr.begin());
                j--;
            }
        }
        board_delete(pagemap[id].getx(), pagemap[id].gety(), pagemap[id].get_width(), pagemap[id].get_height(), id);
        print_board();
        deletedlst.push_back(id);
    }
}

vector<int> Board::findidx(int id){
    vector<int> prior;
    int x = pagemap[id].getx();
    int y = pagemap[id].gety();
    int pagewidth = pagemap[id].get_width();
    int pageheight = pagemap[id].get_height();
    char cont = pagemap[id].get_content();
    for (int h = y; h < y + pageheight; h++){
        for (int w = x; w < x + pagewidth; w++){
            int k = h * width + w;
            int j = find(boardlst[k].begin(), boardlst[k].end(), id) - boardlst[k].begin();
            if (j != boardlst[k].size() - 1){
                if (prior.size() == 0){
                    prior.push_back(boardlst[k][j + 1]);
                } else{
                    if (find(prior.begin(), prior.end(), boardlst[k][j + 1]) == prior.end()){
                        prior.push_back(boardlst[k][j + 1]); // 중복 제거
                    }
                }
            }
        }
    }
    if (prior.size() != 0){
        sort(prior.begin(), prior.end());
        for (int p = 0; p < prior.size(); p++){
            for (int q = 0; q < deletedlst.size(); q++){
                if (prior[p] == deletedlst[q]){
                    prior.erase(find(prior.begin(), prior.end(), prior[p]));
                }
            }
        }
    }
    
    return prior;
}

void Board::insertseq(int id){
    board_insert(pagemap[id].getx(), pagemap[id].gety(), pagemap[id].get_width(), pagemap[id].get_height(), id);
    print_board();
}

void Board::boardlst_delete(int x, int y, int pagewidth, int pageheight, int id){
    for (int h = y; h < y + pageheight; h++){
        for (int w = x; w < x + pagewidth; w++){
            int k = h * width + w;
            boardlst[k].erase(find(boardlst[k].begin(), boardlst[k].end(), id));
        }
    }
}

void Board::boardlst_insert(int x, int y, int pagewidth, int pageheight, int id){
    for (int h = y; h < y + pageheight; h++){
        for (int w = x; w < x + pagewidth; w++){
            int k = h * width + w;
            boardlst[k].push_back(id);
        }
    }
}
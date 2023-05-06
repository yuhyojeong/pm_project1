class Page {
    public:
        Page(int x, int y, int width, int height, int id, char content): x(x), y(y), width(width), height(height), id(id), content(content) {}
        Page(char content): content(content){}
        Page(){};
        int getx(){return x;} // get x
        int gety(){return y;} // get y
        int get_width(){return width;} // get width
        int get_height(){return height;} // get height
        int get_id(){return id;} // get id
        char get_content(){return content;} // get content
    private:
        int x, y; // position of the page on the board
        int width, height; // width and height of the page 
        int id; // unique id for each page
        char content;
};

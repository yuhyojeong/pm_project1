class Page {
    public:
        Page(int x, int y, int width, int height, char content): x(x), y(y), width(width), height(height), content(content) {}
    private:
        int x, y; // position of the page on the board
        int width, height; // width and height of the page 
        int id; // unique id for each page
        char content;
};

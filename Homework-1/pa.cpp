#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <random>
#include <algorithm>
using namespace std;

const double EPS = 1e-6;

struct Point {
    double x, y;
    
    Point() {}
    Point(double px, double py) : x(px), y(py) {}
    Point(const Point &p) : x(p.x), y(p.y) {}

    friend ostream &operator<<(ostream &os, const Point &p) {
        os << "(" << p.x << ", " << p.y << ")";
        return os;
    }
};

// orientation of the point r w.r.t the line p->q
// 1 - counter-clockwise, -1 - clockwise, 0 - collinear 
int side(const Point &p, const Point &q, const Point &r) {
    double val = (p.x - r.x) * (q.y - r.y) - (p.y - r.y) * (q.x - r.x);
    if(fabs(val) < EPS)
        return 0;
    return ((val > 0) ? 1 : -1);
}

double angle(const Point &p, const Point &q) {
    double ang = atan2(q.y - p.y, q.x - p.x);
    if(ang < 0)
        ang += 2 * M_PI;
    return ang;
}

int find_greater(const vector<Point> &pts, Point &orig, int l, int r, double query_ang) {
    int low = l, high = r, mid, ans = r + 1;
    while(low <= high) {
        mid = (low + high) / 2;
        if(angle(orig, pts[mid]) > query_ang) {
            ans = mid;
            high = mid - 1;
        }
        else {
            low = mid + 1;
        }
    }
    return ans;
}

struct Polygon {
    int n;
    vector<Point> pts;
    Point orig;
    int ind_rot;

    Polygon() {}
    Polygon(const vector<Point> &p) : n((int)p.size()), pts(p) {}

    void preprocess() {
        orig.x = (pts[0].x + pts[1].x + pts[2].x) / 3;
        orig.y = (pts[0].y + pts[1].y + pts[2].y) / 3;

        int low = 0, high = n - 1, mid;
        double ang_0 = angle(orig, pts[0]);
        ind_rot = n;
        while(low <= high) {
            mid = (low + high) / 2;
            if(angle(orig, pts[mid]) < ang_0) {
                ind_rot = mid;
                high = mid - 1;
            }
            else {
                low = mid + 1;
            }
        }
    }

    // 1 - inside, -1 - outside, 0 - on boundary
    int pointInPoly(const Point &p) {
        double query_ang = angle(orig, p);
        int ind_l = find_greater(pts, orig, 0, ind_rot - 1, query_ang);
        int ind_r;

        int v1, v2;
        if(ind_rot == n || ind_l > 0) {
            v2 = ind_l % n;
        }
        else {
            ind_r = find_greater(pts, orig, ind_rot, n - 1, query_ang);
            v2 = ind_r % n;
        }
        v1 = (v2 - 1 + n) % n;
        return side(pts[v1], pts[v2], p);
    }
};

void header(ofstream &file) {
    file << "<?xml version=\"1.0\" standalone=\"no\" ?>" << endl;
    file << "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\" \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">" << endl;
}

void startSVG(ofstream &file, int width, int height) {
    file << "<svg width=\"" << width << "px\" height = \"" << height << "px\" xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">" << endl;
}

void endSVG(ofstream &file) {
    file << "</svg>" << endl;
}

void displayPoint(ofstream &file, const Point &p, string col) {
    file << "\t<circle cx=\"" << p.x << "\" cy=\"" << p.y << "\" r=\"2\" stroke=\"" << col << "\" stroke-width=\"2\" fill=\"" << col <<"\"/>" << endl;
    // file << "\t<text x=\"" << p.x + 2 << "\" y=\"" << p.y << "\" fill=\"black\" font-size=\"10\">" << p.x << "," << p.y << "</text>" << endl;
}

void displayPolygon(ofstream &file, const Polygon &poly) {
    for(int i = 0; i < poly.n; i++) {
        file << "\t<circle cx=\"" << poly.pts[i].x << "\" cy=\"" << poly.pts[i].y << "\" r=\"2\" stroke=\"black\" stroke-width=\"2\"/>" << endl;
        // file << "\t<text x=\"" << poly.pts[i].x << "\" y=\"" << poly.pts[i].y << "\" fill=\"black\" font-size=\"10\">" << poly.pts[i].x << "," << poly.pts[i].y << "</text>" << endl;
    }
    for(int i = 0; i < poly.n; i++) {
        file << "\t<line x1=\"" << poly.pts[i].x << "\" y1=\"" << poly.pts[i].y << "\" x2=\"" << poly.pts[(i + 1) % poly.n].x << "\" y2=\"" << poly.pts[(i + 1) % poly.n].y << "\" stroke=\"black\" stroke-width=\"1\"/>" << endl;
    }
}

Polygon genRandomPoly(int n) {
    int r = 300;
    Point center(400, 300);
    vector<double> theta(n);
    vector<Point> pts(n);
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<double> dis(0, 1);
    for (int i = 0; i < n; i++) {
        theta[i] = dis(gen) * 2 * M_PI;
    }
    sort(theta.begin(), theta.end());
    for (int i = 0; i < n; i++) {
        pts[i].x = center.x + r * cos(theta[i]);
        pts[i].y = center.y + r * sin(theta[i]);
    }
    return Polygon(pts);
}


int main() {
    Polygon poly = genRandomPoly(30);
    poly.preprocess();

    ofstream file("test.svg");
    header(file);
    startSVG(file, 840, 640);
    displayPolygon(file, poly);
    // displayPoint(file, poly.orig, "yellow");

    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<double> xdis(1.0, 839.0);
    uniform_real_distribution<double> ydis(1.0, 639.0);
    for(int i = 0; i < 10000; i++) {
        Point query(xdis(gen), ydis(gen));
        int x = poly.pointInPoly(query);
        if(x == 1) 
            displayPoint(file, query, "red");
        else if(x == -1)
            displayPoint(file, query, "blue");
        else
            displayPoint(file, query, "green");
    }
    endSVG(file);
}
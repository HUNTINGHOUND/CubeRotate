#include <iostream>
#include <cmath>
#include <vector>
#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>
#include <chrono>
#include <thread>
#include <algorithm>
using namespace std;

double sa, ca, sb, cb, sc, cc;
double k2 = 5, k1;

double granity = 0.01;
double smooth = 100;

double A = 0;
double B = 0;
double C = 0;

double ad = 2 * M_PI / smooth;
double bd = 2 * M_PI / smooth;
double cd = 2 * M_PI / smooth;

int screen_width, screen_height;

double calc_x(double x, double y, double z) {
    return y * (sa * sb * cc - ca * sc) + z * (ca * sb * cc + sa * sc) + x * cb * cc;
}

double calc_y(double x, double y, double z) {
    return y * (sa * sb * sc + ca * cc) + z * (ca * sb * sc - sa * cc) + x * cb * sc;
}

double calc_z(double x, double y, double z) {
    return k2 + y * sa * cb + z * ca * cb - x * sb;
}

struct {
    double x = -1, y = -1, z = -1;
    double length = 2;
} cube; // cube is define with 1 corner and length

void display(double change_a, double change_b, double change_c) {
    
    ad = change_a / smooth;
    bd = change_b / smooth;
    cd = change_c / smooth;
    
    for (int _ = 0; _ <= smooth; _++) {
        vector<vector<char>> output(screen_width + 1, vector<char>(screen_height + 1, ' '));
        vector<vector<double>> zbuffer(screen_width + 1, vector<double>(screen_height + 1, 0));
        
        sa = sin(A);
        ca = cos(A);
        sb = sin(B);
        cb = cos(B);
        sc = sin(C);
        cc = cos(C);
        
        double l;
        
        for (double x = cube.x; x <= cube.x + cube.length; x += cube.length) {
            if (x == cube.x) l = -ca * sb * cc - sa * sb * cc + sa * sc - ca * sc - cb * cc;
            else l = ca * sb * cc + sa * sb * cc - sa * sc + ca * sc + cb * cc;
            for (double y = cube.y; y <= cube.y + cube.length; y += granity) {
                for (double z = cube.z; z <= cube.z + cube.length; z += granity) {
                    double dx = calc_x(x, y, z);
                    double dy = calc_y(x, y, z);
                    double dz = calc_z(x, y, z);

                    double ooz = 1.0 / dz;
                    
                    int xp = (int) (screen_width / 2 + k1 * ooz * dx);
                    int yp = (int) (screen_height / 2 - k1 * ooz * dy);
                    
                    if (ooz > zbuffer[xp][yp]) {
                        zbuffer[xp][yp] = ooz;
                        int lum = max(l * 7, 0.0);
                        //output[xp][yp] = ".,-~:;=!*#$@&"[lum];
                        output[xp][yp] = x == cube.x ? '$' : '^';
                    }
                }
            }
        }
        
        for (double y = cube.y; y <= cube.y + cube.length; y += cube.length) {
            if (y == cube.y) l = sa * sb * sc + ca * sb * sc - ca * cc + sa * cc + cb * sc;
            else l = -sa * sb * sc - ca * sb * sc + ca * cc - sa * cc - cb * sc;
            for (double x = cube.x; x <= cube.x + cube.length; x += granity) {
                for (double z = cube.z; z <= cube.z + cube.length; z += granity) {
                    double dx = calc_x(x, y, z);
                    double dy = calc_y(x, y, z);
                    double dz = calc_z(x, y, z);
                    
                    double ooz = 1.0 / dz;
                    
                    int xp = (int) (screen_width / 2 + k1 * ooz * dx);
                    int yp = (int) (screen_height / 2 - k1 * ooz * dy);
                    
                    if (ooz > zbuffer[xp][yp]) {
                        zbuffer[xp][yp] = ooz;
                        int lum = max(l * 7, 0.0);
                        //output[xp][yp] = ".,-~:;=!*#$@&"[lum];
                        output[xp][yp] = y == cube.y ? ';' : '=';
                    }
                }
            }
        }
        
        for (double z = cube.z; z <= cube.z + cube.length; z += cube.length) {
            if (z == cube.z) l = ca * cb + sa * cb - sb;
            else l = -ca * cb - sa * cb + sb;
            for (double y = cube.y; y <= cube.y + cube.length; y += granity) {
                for (double x = cube.x; x <= cube.x + cube.length; x += granity) {
                    double dx = calc_x(x, y, z);
                    double dy = calc_y(x, y, z);
                    double dz = calc_z(x, y, z);

                    double ooz = 1.0 / dz;
                    
                    int xp = (int) (screen_width / 2 + k1 * ooz * dx);
                    int yp = (int) (screen_height / 2 - k1 * ooz * dy);
                    
                    if (ooz > zbuffer[xp][yp]) {
                        zbuffer[xp][yp] = ooz;
                        int lum = max(l * 7, 0.0);
                        //output[xp][yp] = ".,-~:;=!*#$@&"[lum];
                        output[xp][yp] = z == cube.z ? '#' : '*';
                    }
                }
            }
        }
        
        cout << "\x1b[H";
        for (int j = 0; j < screen_height; j++) {
            for (int i = 0; i < screen_width; i++) {
                cout << output[i][j];
            }
            cout << "\n";
        }
        
        A += ad;
        if (abs(A) >= 2 * M_PI) A = A + (A < 0 ? 2 * M_PI : -2 * M_PI);
        
        B += bd;
        if (abs(B) >= 2 * M_PI) B = B + (B < 0 ? 2 * M_PI : -2 * M_PI);
        
        C += cd;
        if (abs(C) >= 2 * M_PI) C = C + (C < 0 ? 2 * M_PI : -2 * M_PI);

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

int main(int argc, const char * argv[]) {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    
    screen_width = w.ws_col;
    screen_height = w.ws_row;

    k1 = screen_width * k2 * 3 / (8 * sqrt(3) * (cube.length));
    
    for (;;) {
        cout << "Enter rotation degrees: ";
        double a_change, b_change, c_change;
        cin >> a_change >> b_change >> c_change >> smooth;

        display(a_change * M_PI / 180, b_change * M_PI / 180, c_change * M_PI / 180);
    }
}
